#include "CommunicationPCH.h"
#include "HMDBSourceContextOperations.h"
#include <corelib/Filesystem.h>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <corelib/IPlugin.h>
#include <corelib/IJobManager.h>
#include <corelib/IThreadPool.h>
#include <boost/thread.hpp>
#include <hmdbserviceslib/IFileStoremanWS.h>
#include <plugins/hmdbCommunication/IHMDBSession.h>
#include <QThread>
#include <plugins/hmdbCommunication/IHMDBStorage.h>
#include <hmdbserviceslib/IncrementalBranchShallowCopyParser.h>
#include <hmdbserviceslib/ShallowCopyParser.h>
#include <plugins/hmdbCommunication/IHMDBDataContext.h>
#include <boost/lexical_cast.hpp>
#include <plugins/hmdbCommunication/IHMDBStatusManager.h>
#include <plugins/hmdbCommunication/DataStatus.h>
#include <utils/Utils.h>

using namespace hmdbCommunication;


CompoundOperation::CompoundOperation(const IHMDBRemoteContext::OperationType opt,
	const ExecutionPolicy ep, const ExecutionSchema es,
	const Operations & suboperations)
	: opt(opt), operations(suboperations),
	ep(ep), es(es), status_(threadingUtils::IOperation::Initialized)
{

}

CompoundOperation::~CompoundOperation()
{
	abort();
	wait();
}

void CompoundOperation::setOperations(const Operations & suboperations)
{
	if (status_ == IOperation::Initialized){
		operations = suboperations;
	}
	else{
		throw std::runtime_error("Compound operation already started");
	}
}

const IHMDBRemoteContext::OperationType CompoundOperation::operationType() const
{
	return opt;
}

const unsigned int CompoundOperation::size() const
{
	return operations.size();
}

const IHMDBRemoteContext::IOperation * CompoundOperation::operation(const unsigned int idx) const
{
	return operations[idx].get();
}

IHMDBRemoteContext::IOperation * CompoundOperation::operation(const unsigned int idx)
{
	return operations[idx].get();
}

const IHMDBRemoteContext::ICompoundOperation::ExecutionPolicy CompoundOperation::executionPolicy() const
{
	return ep;
}

const IHMDBRemoteContext::ICompoundOperation::ExecutionSchema CompoundOperation::executionSchema() const
{
	return es;
}

void CompoundOperation::start()
{
	if (status_ != threadingUtils::IOperation::Initialized){
		return;
	}

	status_ = threadingUtils::IOperation::Pending;

	if (ep == IHMDBRemoteContext::ICompoundOperation::Any){

		if (es == IHMDBRemoteContext::ICompoundOperation::Serial){
			auto runnable = threadingUtils::IRunnablePtr(new threadingUtils::FunctorRunnable(boost::bind(&CompoundOperation::callAnySerial, this)));
			//job = plugin::getJobManager()->addJob("HMDBSourceContext", "CompoundOperation: " + boost::lexical_cast<std::string>(opt), runnable);
			core::IThreadPool::Threads t;
			plugin::getThreadPool()->getThreads("HMDBSourceContext", t, 1);
			workerThread = t.front();
			workerThread->start(runnable);
		}
		else{
			callAnyParallel();			
		}

	}
	else{

		core::IThreadPool::Threads t;
		plugin::getThreadPool()->getThreads("HMDBSourceContext", t, 1);
		workerThread = t.front();

		threadingUtils::IRunnablePtr runnable;

		if (es == IHMDBRemoteContext::ICompoundOperation::Serial){
			runnable = threadingUtils::IRunnablePtr(new threadingUtils::FunctorRunnable(boost::bind(&CompoundOperation::callAllSerial, this)));
			//job = plugin::getJobManager()->addJob("HMDBSourceContext", "CompoundOperation: " + boost::lexical_cast<std::string>(opt), threadingUtils::IRunnablePtr(new threadingUtils::FunctorRunnable(boost::bind(&CompoundOperation::callAllSerial, this))));
		}
		else{
			runnable = threadingUtils::IRunnablePtr(new threadingUtils::FunctorRunnable(boost::bind(&CompoundOperation::callAllParallel, this)));
		}

		workerThread->start(runnable);
	}
}

void CompoundOperation::abort()
{
	if (status_ != threadingUtils::IOperation::Aborted &&
		status_ != threadingUtils::IOperation::Error &&
		status_ != threadingUtils::IOperation::Finieshed){

		status_ = threadingUtils::IOperation::Aborted;

		for (auto it = operations.begin(); it != operations.end(); ++it){
			(*it)->abort();			
		}
	}
}

void CompoundOperation::callAnySerial()
{
	if (status_ == threadingUtils::IOperation::Pending){
		status_ = threadingUtils::IOperation::Running;

		for (auto it = operations.begin(); it != operations.end() && status_ == threadingUtils::IOperation::Running; ++it){
			(*it)->start();
			(*it)->wait();
		}
	}
}

void CompoundOperation::callAnyParallel()
{
	for (auto it = operations.begin(); it != operations.end() && status_ == threadingUtils::IOperation::Running; ++it){
		(*it)->start();
	}

	status_ = threadingUtils::IOperation::Running;
}

void CompoundOperation::callAllSerial()
{
	if (status_ == threadingUtils::IOperation::Pending){
		status_ = threadingUtils::IOperation::Running;

		for (auto it = operations.begin(); it != operations.end() && status_ == threadingUtils::IOperation::Running; ++it){
			(*it)->start();
			(*it)->wait();

			if ((*it)->status() == threadingUtils::IOperation::Error){
				status_ = threadingUtils::IOperation::Error;
				errorOperation = (*it).get();
				break;
			}
		}
	}
}

void CompoundOperation::observe()
{
	bool finish = false;
	while (finish == false){		

		int opFinished = 0;

		for (auto it = operations.begin(); it != operations.end() && status_ == threadingUtils::IOperation::Running; ++it){
			auto s = (*it)->status();

			if (s == threadingUtils::IOperation::Error){
				if (errorOperation == nullptr){
					errorOperation = (*it).get();
					status_ = threadingUtils::IOperation::Error;
				}
			}
			else if (s == threadingUtils::IOperation::Aborted){
				status_ = threadingUtils::IOperation::Aborted;
			}
			else if (s == threadingUtils::IOperation::Finieshed){
				++opFinished;
			}
		}

		if (status_ != threadingUtils::IOperation::Running || opFinished == operations.size()){
			finish = true;
		} else {
			boost::this_thread::sleep_for(boost::chrono::milliseconds(50));
		}
	}

	if (status_ == threadingUtils::IOperation::Error){
		for (auto it = operations.begin(); it != operations.end(); ++it){
			(*it)->abort();
		}
	}
}

void CompoundOperation::callAllParallel()
{
	if (status_ == threadingUtils::IOperation::Pending){
		callAnyParallel();
		observe();
	}
}

void CompoundOperation::wait()
{
	if (status_ != threadingUtils::IOperation::Pending &&
		status_ != threadingUtils::IOperation::Running){
		return;
	}

	for (auto it = operations.begin(); it != operations.end(); ++it){
		(*it)->wait();
	}

	if (workerThread != nullptr){
		workerThread->join();
		workerThread.reset();
	}
}

const threadingUtils::IOperation::Status CompoundOperation::compoundStatus() const
{
	threadingUtils::IOperation::Status s = IOperation::Initialized;

	int pi = 0;
	int ri = 0;
	int fi = 0;

	for (auto it = operations.begin(); it != operations.end(); ++it)
	{
		auto ss = (*it)->status();
		if (ss == IOperation::Aborted || ss == IOperation::Error){
			s = ss;
			break;
		}
		else if (ss == IOperation::Finieshed)
		{
			++fi;
		}
		else if (ss == IOperation::Running){
			++ri;
		}
		else if (ss == IOperation::Pending){
			++pi;
		}
	}

	if (s == IOperation::Initialized){
		if (pi == operations.size()){
			s = IOperation::Pending;
		}else if (fi == operations.size()){
			s = IOperation::Finieshed;
		}else if(ri > 0){	
			s = IOperation::Running;	
		}
	}

	return s;
}

const threadingUtils::IOperation::Status CompoundOperation::status() const
{
	if (status_ == threadingUtils::IOperation::Running){
		status_ = compoundStatus();		
	}

	return status_;
}

const std::string CompoundOperation::error() const
{
	if (errorOperation == nullptr){
		errorOperation = findErrorOperation();
	}


	return errorOperation == nullptr ? std::string() : errorOperation->error();
}

IHMDBRemoteContext::IOperation * CompoundOperation::findErrorOperation() const
{
	IOperation * ret = nullptr;

	for (auto it = operations.begin(); it != operations.end(); ++it){
		if ((*it)->status() == IOperation::Error){
			ret = (*it).get();
			break;
		}
	}

	return ret;
}

FunctorOperation::FunctorOperation(const IHMDBRemoteContext::OperationType opt,
	threadingUtils::FunctorRunnable::Functor functor)
	: opt(opt), functor(functor), progress_(0.0),
	status_(threadingUtils::IOperation::Initialized)
{

}

FunctorOperation::~FunctorOperation()
{
	abort();
	wait();
}

void FunctorOperation::setFunctor(threadingUtils::FunctorRunnable::Functor functor)
{
	if (status_ == IOperation::Initialized){
		this->functor = functor;
	}
	else{
		throw std::runtime_error("Functor operation already started");
	}
}

const IHMDBRemoteContext::OperationType FunctorOperation::operationType() const
{
	return opt;
}

const float FunctorOperation::progress() const
{
	return progress_;
}

void FunctorOperation::call()
{
	if (status_ == threadingUtils::IOperation::Aborted){
		return;
	}

	status_ = threadingUtils::IOperation::Running;
	try{
		functor();

		if (status_ == threadingUtils::IOperation::Running){
			status_ = threadingUtils::IOperation::Finieshed;
			progress_ = 1.0;
		}
	}
	catch (std::exception & e){
		error_ = e.what();
		status_ = threadingUtils::IOperation::Error;
	}
	catch (...){
		error_ = "Unknown functor call error";
		status_ = threadingUtils::IOperation::Error;
	}
}

void FunctorOperation::abort()
{
	if (status_ != threadingUtils::IOperation::Aborted &&
		status_ != threadingUtils::IOperation::Error &&
		status_ != threadingUtils::IOperation::Finieshed){

		status_ = threadingUtils::IOperation::Aborted;

	}
}

void FunctorOperation::start()
{
	if (status_ != threadingUtils::IOperation::Initialized){
		return;
	}

	status_ = threadingUtils::IOperation::Pending;
	job = plugin::getJobManager()->addJob("HMDBSourceContext", "FunctorOperation: " + boost::lexical_cast<std::string>(opt), threadingUtils::IRunnablePtr(new threadingUtils::FunctorRunnable(boost::bind(&FunctorOperation::call, this))));
}

void FunctorOperation::wait()
{
	if (job != nullptr){
		job->wait();
		job.reset();
	}
}

const threadingUtils::IOperation::Status FunctorOperation::status() const
{
	return status_;
}

const std::string FunctorOperation::error() const
{
	return error_;
}

TransferOperation::TransferOperation()
	: status_(threadingUtils::IOperation::Initialized)
{

}

TransferOperation::~TransferOperation()
{
	abort();
	wait();
}

void TransferOperation::setTransfer(IHMDBFtp::TransferPtr transfer)
{
	transfer_ = transfer;
}

IHMDBFtp::ITransfer * TransferOperation::transfer()
{
	return transfer_.get();
}

const IHMDBFtp::ITransfer * TransferOperation::transfer() const
{
	return transfer_.get();
}

const float TransferOperation::progress() const
{
	return transfer_ != nullptr ? transfer_->progress() : 0.0;
}

void TransferOperation::abort()
{
	if (transfer_ != nullptr){
		transfer_->abort();
	}
	else{
		status_ = threadingUtils::IOperation::Aborted;
	}
}

void TransferOperation::start()
{
	if (transfer_ != nullptr){
		transfer_->start();
	}
}

void TransferOperation::wait()
{
	if (transfer_ != nullptr){
		transfer_->wait();
	}
}

const threadingUtils::IOperation::Status TransferOperation::status() const
{
	return transfer_ != nullptr ? transfer_->status() : status_;
}

const std::string TransferOperation::error() const
{
	return transfer_ != nullptr ? transfer_->error() : std::string();
}

const IHMDBRemoteContext::OperationType TransferOperation::operationType() const
{
	return IHMDBRemoteContext::TransferingData;
}

UploadOperation::UploadOperation(const std::string & fileName,
	std::istream * source,
	IHMDBFtp * ftp, HMDBStore store,
	const unsigned long long size,
	const std::string & fileDescription,
	const std::string & subdirectory)
	: CompoundOperation(IHMDBRemoteContext::Uploading, IHMDBRemoteContext::ICompoundOperation::AllOrNothing, IHMDBRemoteContext::ICompoundOperation::Serial),
	fileName(fileName), source(source),
	ftp(ftp), store(store), fileDescription(fileDescription),
	subdirectory(subdirectory), fileID_(-1)
{
	std::auto_ptr<TransferOperation> t(new TransferOperation());
	t->setTransfer(ftp->put(fileName, source, size));	
	IHMDBRemoteContext::OperationPtr funcOp(new FunctorOperation(IHMDBRemoteContext::WebServiceCall,
		boost::bind(&UploadOperation::getFileID, this)));

	Operations op;
	op.push_back(IHMDBRemoteContext::OperationPtr(t.release()));
	op.push_back(funcOp);
	setOperations(op);	
}

UploadOperation::~UploadOperation()
{

}

const int UploadOperation::fileID() const
{
	return fileID_;
}

void UploadOperation::getFileID()
{
	fileID_ = store(fileName, fileDescription, subdirectory);
}


StoreOutputOperation::StoreOutputOperation(IHMDBStorage * storage, const std::string & key)
	: FunctorOperation(IHMDBRemoteContext::StoringData), storage(storage), key(key)
{

}

StoreOutputOperation::~StoreOutputOperation()
{

}

void StoreOutputOperation::setFileInput(std::istream * stream)
{
	stream_ = stream;
}

std::istream * StoreOutputOperation::stream()
{
	return storage->get(key);
}

void StoreOutputOperation::store()
{
	storage->set(key, stream_);
}

const std::string retrieveData(hmdbServices::IBasicStoremanWS * service, const int id)
{
	return service->retrieve(id).fileLocation;
}

const std::string retrievePerspective(hmdbServices::IShallowStoremanWS * service, const int id)
{
	std::string ret;
	switch (id)
	{
	case ShallowCopyUtils::ShallowCopyFileID:
		ret = service->getShallowCopy();
		break;

	case ShallowCopyUtils::MetadataFileID:
		ret = service->getMetadata();
		break;

	case ShallowCopyUtils::IncrementalShallowCopyFileID:
		ret = service->getShallowCopyIncrement(hmdbServices::DateTime::now().toString());
		break;

	case ShallowCopyUtils::IncrementalMetadataFileID:
		UTILS_DEBUG((false), "Brak operacji w HMDB");
		break;

	default:
		UTILS_DEBUG((false), "Blad identyfikatora pliku perspektywy danych");
		break;
	}

	return ret;
}

typedef boost::function<void(const std::string &)> StoreFunctor;
typedef boost::function<std::string()> GetPathFunctor;

void setDataPath(StoreFunctor sf, GetPathFunctor gpf)
{
	sf(gpf());
}

class PrepareFileToDownloadOperation::ClearFileToDownloadOperation : public FunctorOperation
{
public:
	ClearFileToDownloadOperation(PrepareFileToDownloadOperation * op)
		: FunctorOperation(IHMDBRemoteContext::CleaningHMDB), op(op)
	{
		setFunctor(boost::bind(&ClearFileToDownloadOperation::clear, this));
	}

private:

	void clear()
	{
		if (op->fileID_ > -1){
			dynamic_cast<hmdbServices::IBasicStoremanWS*>(op->fs)->downloadComplete(op->fileID_, op->preparedFilePath_);
		}
		else{
			dynamic_cast<hmdbServices::IShallowStoremanWS*>(op->fs)->downloadComplete(op->preparedFilePath_);			
		}
	}

private:
	PrepareFileToDownloadOperation * op;
};

PrepareFileToDownloadOperation::PrepareFileToDownloadOperation(hmdbServices::IFileStoremanWS * fs,
	const hmdbServices::ID fileID)
	: FunctorOperation(IHMDBRemoteContext::PreparingHMDB), fs(fs), fileID_(fileID)
{
	threadingUtils::FunctorRunnable::Functor func;

	StoreFunctor sf = boost::bind(&PrepareFileToDownloadOperation::setPath, this, _1);
	GetPathFunctor gpf;

	if (fileID > -1){		
		gpf = boost::bind(&retrieveData, fs, fileID);
	}
	else{		
		gpf = boost::bind(&retrievePerspective, fs, fileID);
	}

	setFunctor(boost::bind(&setDataPath, sf, gpf));
}

PrepareFileToDownloadOperation::~PrepareFileToDownloadOperation()
{

}

const std::string & PrepareFileToDownloadOperation::preparedFilePath() const
{
	return preparedFilePath_;
}

PrepareFileToDownloadOperation::ClearFileToDownloadOperation * PrepareFileToDownloadOperation::clearFileDownloadOperation()
{
	return new ClearFileToDownloadOperation(this);
}

void PrepareFileToDownloadOperation::setPath(const std::string & path)
{
	preparedFilePath_ = path;
}

PrepareTmpFileTransferOutputOperation::OpenTmpFileTransferOutputOperation::OpenTmpFileTransferOutputOperation(PrepareTmpFileTransferOutputOperation * op)
	: FunctorOperation(IHMDBRemoteContext::OpeningInput), op(op)
{
	setFunctor(boost::bind(&OpenTmpFileTransferOutputOperation::openOutput, this));
}

PrepareTmpFileTransferOutputOperation::OpenTmpFileTransferOutputOperation::~OpenTmpFileTransferOutputOperation()
{

}

void PrepareTmpFileTransferOutputOperation::OpenTmpFileTransferOutputOperation::openOutput()
{
	stream_ = new std::ifstream(op->tmpFilePath.string());
}

std::istream * PrepareTmpFileTransferOutputOperation::OpenTmpFileTransferOutputOperation::fileInput()
{
	return stream_;
}

void closeInputFileStream(std::ifstream * stream)
{
	stream->close();
	delete stream;
}

IHMDBRemoteContext::IOperation * PrepareTmpFileTransferOutputOperation::OpenTmpFileTransferOutputOperation::closeInputOperation()
{
	return new FunctorOperation(IHMDBRemoteContext::ClosingInput, boost::bind(&closeInputFileStream, stream_));
}

PrepareTmpFileTransferOutputOperation::PrepareTmpFileTransferOutputOperation()
	: FunctorOperation(IHMDBRemoteContext::PreparingOutput)
{
	setFunctor(boost::bind(&PrepareTmpFileTransferOutputOperation::prepareOutput, this));
}

PrepareTmpFileTransferOutputOperation::~PrepareTmpFileTransferOutputOperation()
{

}

void PrepareTmpFileTransferOutputOperation::prepareOutput()
{
	std::string nameBase(boost::lexical_cast<std::string>(boost::posix_time::microsec_clock::local_time().time_of_day().total_microseconds()) + "_" + boost::lexical_cast<std::string>(QThread::currentThreadId()) + ".tmp");
	tmpFilePath = plugin::getPaths()->getTmpPath() / nameBase;
	stream = new std::ofstream(tmpFilePath.string());
}

std::ostream * PrepareTmpFileTransferOutputOperation::fileOutput()
{
	return stream;
}

void closeOutputFileStream(std::ofstream * stream)
{
	stream->close();
	delete stream;
}

IHMDBRemoteContext::IOperation * PrepareTmpFileTransferOutputOperation::closeOutputOperation()
{
	return new FunctorOperation(IHMDBRemoteContext::ClosingOutput, boost::bind(&closeOutputFileStream, stream));
}

IOpenTransferOutputOperation * PrepareTmpFileTransferOutputOperation::openInputOperation()
{
	return new PrepareTmpFileTransferOutputOperation::OpenTmpFileTransferOutputOperation(this);
}

IHMDBRemoteContext::IOperation * PrepareTmpFileTransferOutputOperation::cleanOutputOperation()
{
	return new FunctorOperation(IHMDBRemoteContext::CleaningOutput,
		boost::bind(static_cast<void(*)(const core::Filesystem::Path&)>(&core::Filesystem::deleteFile),
		tmpFilePath));
}


PrepareMemoryTransferOutputOperation::OpenMemoryTransferOutputOperation::OpenMemoryTransferOutputOperation(PrepareMemoryTransferOutputOperation * op)
	: FunctorOperation(IHMDBRemoteContext::OpeningInput), op(op)
{

}

PrepareMemoryTransferOutputOperation::PrepareMemoryTransferOutputOperation()
	: FunctorOperation(IHMDBRemoteContext::PreparingOutput)
{
	setFunctor(boost::bind(&PrepareMemoryTransferOutputOperation::prepareOutput, this));
}

PrepareMemoryTransferOutputOperation::OpenMemoryTransferOutputOperation::~OpenMemoryTransferOutputOperation()
{

}

void PrepareMemoryTransferOutputOperation::prepareOutput()
{
	stream = new std::stringstream();
}

std::istream * PrepareMemoryTransferOutputOperation::OpenMemoryTransferOutputOperation::fileInput()
{
	return stream;
}

void PrepareMemoryTransferOutputOperation::OpenMemoryTransferOutputOperation::openOutput()
{
	stream = dynamic_cast<std::istream*>(op->stream);
	stream->seekg(0, std::ios::beg);
}

IHMDBRemoteContext::IOperation * PrepareMemoryTransferOutputOperation::OpenMemoryTransferOutputOperation::closeInputOperation()
{
	return nullptr;
}

PrepareMemoryTransferOutputOperation::~PrepareMemoryTransferOutputOperation()
{

}

std::ostream * PrepareMemoryTransferOutputOperation::fileOutput()
{
	return stream;
}

IHMDBRemoteContext::IOperation * PrepareMemoryTransferOutputOperation::closeOutputOperation()
{
	return new FunctorOperation(IHMDBRemoteContext::ClosingOutput, boost::bind(&std::stringstream::flush, stream));
}

IOpenTransferOutputOperation * PrepareMemoryTransferOutputOperation::openInputOperation()
{
	return new OpenMemoryTransferOutputOperation(this);
}

void cleanStringstream(std::stringstream * stream)
{
	delete stream;
}

IHMDBRemoteContext::IOperation * PrepareMemoryTransferOutputOperation::cleanOutputOperation()
{
	return new FunctorOperation(IHMDBRemoteContext::CleaningOutput, boost::bind(&cleanStringstream, stream));
}


FTPDownloadFileOperation::FTPDownloadFileOperation(IHMDBFtp * ftp)
	: ftp(ftp)
{
	
}

FTPDownloadFileOperation::~FTPDownloadFileOperation()
{

}

void FTPDownloadFileOperation::setDownload(const std::string & path, std::ostream * stream, const hmdbServices::FileSize size)
{
	setTransfer(ftp->get(path, stream, size));
}

CompoundFileDownload::CompoundFileDownload(const IHMDBRemoteContext::CompoundID & fileToDownload,
	PrepareFileToDownloadOperation * pfOp,
	IPrepareTransferOutputOperation * ptOp,
	IDownloadFileOperation * dOp,
	IStoreOutputOperation * soOp) : FunctorOperation(IHMDBRemoteContext::Downloading),
	pfOp(pfOp), ptOp(ptOp), dOp(dOp), soOp(soOp),
	downloaded_(false), file_(fileToDownload)
{	
	cfOp.reset(pfOp->clearFileDownloadOperation());
	ctoOp.reset(ptOp->closeOutputOperation());
	oiOp.reset(ptOp->openInputOperation());

	if (oiOp != nullptr){
		ciOp.reset(oiOp->closeInputOperation());
	}

	ptOp->cleanOutputOperation();
	cltoOp.reset(pfOp->clearFileDownloadOperation());

	operations.push_back(pfOp);
	operations.push_back(ptOp);
	operations.push_back(dOp);
	operations.push_back(cfOp.get());
	if (ctoOp != nullptr){
		operations.push_back(ctoOp.get());
	}

	operations.push_back(oiOp.get());
	operations.push_back(soOp);
	if (ciOp != nullptr){
		operations.push_back(ciOp.get());
	}

	if (cltoOp != nullptr){
		operations.push_back(cltoOp.get());
	}

	setFunctor(boost::bind(&CompoundFileDownload::run, this));
}

CompoundFileDownload::~CompoundFileDownload()
{

}

const float CompoundFileDownload::progress() const
{
	float ret = 0.0;

	auto s = operations.size();

	if (s > 0){
		for (auto it = operations.begin(); it != operations.end(); ++it){
			ret += (*it)->progress();
		}

		ret /= s;
	}

	return ret;
}

const unsigned int CompoundFileDownload::size() const
{
	return operations.size();
}

const IHMDBRemoteContext::IOperation * CompoundFileDownload::operation(const unsigned int idx) const
{
	return operations[idx];
}

IHMDBRemoteContext::IOperation * CompoundFileDownload::operation(const unsigned int idx)
{
	return operations[idx];
}

const IHMDBRemoteContext::ICompoundOperation::ExecutionPolicy CompoundFileDownload::executionPolicy() const
{
	return IHMDBRemoteContext::ICompoundOperation::Any;
}

const IHMDBRemoteContext::ICompoundOperation::ExecutionSchema CompoundFileDownload::executionSchema() const
{
	return IHMDBRemoteContext::ICompoundOperation::Serial;
}

const IHMDBRemoteContext::CompoundID CompoundFileDownload::fileID() const
{
	return file_;
}

const bool CompoundFileDownload::fileDownloaded() const
{
	return downloaded_;
}

const IHMDBRemoteContext::OperationType CompoundFileDownload::operationType() const
{
	return IHMDBRemoteContext::Downloading;
}

std::istream * CompoundFileDownload::stream()
{
	if (downloaded_ == true){
		soOp->stream();
	}

	return nullptr;
}

threadingUtils::IOperation::Status launchAndWait(const IHMDBRemoteContext::OperationPtr operation)
{
	operation->start();
	operation->wait();
	return operation->status();
}

void CompoundFileDownload::run()
{
	if (status() == threadingUtils::IOperation::Running && launchAndWait(pfOp) == IOperation::Finieshed){

		auto c = utils::Cleanup(boost::bind(&launchAndWait, cfOp));

		if (status() == threadingUtils::IOperation::Running && launchAndWait(ptOp) == IOperation::Finieshed){

			utils::shared_ptr<utils::Cleanup> c(cltoOp != nullptr ? new utils::Cleanup(boost::bind(&launchAndWait, cltoOp)) : nullptr);

			dOp->setDownload(pfOp->preparedFilePath(), ptOp->fileOutput(), file_.fileSize);
			auto s = launchAndWait(dOp);

			if (ctoOp != nullptr){
				launchAndWait(ctoOp);				
			}

			if (s == IOperation::Finieshed){				

				if (status() == threadingUtils::IOperation::Running && launchAndWait(oiOp) == IOperation::Finieshed){

					utils::shared_ptr<utils::Cleanup> c(ciOp != nullptr ? new utils::Cleanup(boost::bind(&launchAndWait, ciOp)) : nullptr);

					soOp->setFileInput(oiOp->fileInput());

					if (status() == threadingUtils::IOperation::Running && launchAndWait(soOp) == IOperation::Finieshed){
						downloaded_ = true;
					}
				}
			}
		}
	}
}

ExtractShallowcopy::ExtractShallowcopy(const std::list<IHMDBRemoteContext::IDownloadOperation*> & downloads)
	: FunctorOperation(IHMDBRemoteContext::ExtractingShallowCopy), downloads(downloads)
{
	setFunctor(boost::bind(&ExtractShallowcopy::extract, this));
}

ExtractShallowcopy::~ExtractShallowcopy()
{

}

const float ExtractShallowcopy::progress() const
{
	return downloads.size() > 0 ? (float)progress_ / (float)downloads.size() : 0.0;
}

const ShallowCopyConstPtr ExtractShallowcopy::shallowCopy() const
{
	return shallowCopy_;
}

const IncrementalBranchShallowCopyConstPtr ExtractShallowcopy::incrementalBranchShallowCopy() const
{
	return incrementalShallowCopy_;
}


void extractShallowCopy(const hmdbServices::ID shallowID, const IHMDBRemoteContext::DataReference dataReference,
	ShallowCopy & shallowCopy, hmdbServices::IncrementalBranchShallowCopy & incShallowCopy,
	std::istream * stream)
{

	if (dataReference == IHMDBRemoteContext::Motion){

		switch (shallowID)
		{
		case ShallowCopyUtils::IncrementalMetadataFileID:
			UTILS_DEBUG((false), "Incremental metadata not supported for motion database");
			break;

		case ShallowCopyUtils::IncrementalShallowCopyFileID:
			hmdbServices::IncrementalBranchShallowCopyParser::parseFile(stream, incShallowCopy);
			break;

		case ShallowCopyUtils::ShallowCopyFileID:
			hmdbServices::MotionShallowCopyParser::parseFile(stream, shallowCopy.motionShallowCopy);
			break;

		case ShallowCopyUtils::MetadataFileID:
			hmdbServices::MotionMetadataParser::parseFile(stream, shallowCopy.motionMetaData);
			break;
		}
	}
	else{
		switch (shallowID)
		{
		case ShallowCopyUtils::IncrementalMetadataFileID:
			UTILS_DEBUG((false), "Incremental metadata not supported for medical database");
			break;

		case ShallowCopyUtils::IncrementalShallowCopyFileID:
			UTILS_DEBUG((false), "Incremental shallowcopy not supported for medical database");
			break;

		case ShallowCopyUtils::ShallowCopyFileID:
			hmdbServices::MedicalShallowCopyParser::parseFile(stream, shallowCopy.medicalShallowCopy);
			break;

		case ShallowCopyUtils::MetadataFileID:
			hmdbServices::MedicalMetadataParser::parseFile(stream, shallowCopy.medicalMetaData);
			break;
		}
	}
}

void ExtractShallowcopy::extract()
{
	auto it = downloads.begin();

	ShallowCopyPtr locSh(new ShallowCopy);
	IncrementalBranchShallowCopyPtr locIncSh(new hmdbServices::IncrementalBranchShallowCopy);

	while (it != downloads.end() && status() == Running){

		if ((*it)->fileDownloaded() == true){

			extractShallowCopy((*it)->fileID().fileID, (*it)->fileID().dataReference,
				*locSh, *locIncSh, (*it)->stream());
		}

		++it;
		++progress_;
	}
}

SynchronizeOperation::SynchronizeOperation(const std::list<IHMDBRemoteContext::IDownloadOperation*> & downloads)
	: CompoundOperation(IHMDBRemoteContext::Synchronizing, IHMDBRemoteContext::ICompoundOperation::AllOrNothing,
	IHMDBRemoteContext::ICompoundOperation::Serial)
{
	CompoundOperation::Operations downloadOperatins;

	for (auto it = downloads.begin(); it != downloads.end(); ++it){
		downloadOperatins.push_back(IHMDBRemoteContext::OperationPtr(*it));
	}

	IHMDBRemoteContext::OperationPtr dco(new CompoundOperation(IHMDBRemoteContext::Downloading,
		IHMDBRemoteContext::ICompoundOperation::Any,
		IHMDBRemoteContext::ICompoundOperation::Parallel,
		downloadOperatins));

	eOp = new ExtractShallowcopy(downloads);

	CompoundOperation::Operations synchOperatins;
	synchOperatins.push_back(dco);
	synchOperatins.push_back(IHMDBRemoteContext::OperationPtr(eOp));

	setOperations(synchOperatins);
}

SynchronizeOperation::~SynchronizeOperation()
{

}

const ShallowCopyConstPtr SynchronizeOperation::shallowCopy() const
{
	return eOp->shallowCopy();
}

const IncrementalBranchShallowCopyConstPtr SynchronizeOperation::incrementalBranchShallowCopy() const
{
	return eOp->incrementalBranchShallowCopy();
}