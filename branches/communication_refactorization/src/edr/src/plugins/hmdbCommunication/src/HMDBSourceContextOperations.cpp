#include "CommunicationPCH.h"
#include "HMDBSourceContextOperations.h"
#include <corelib/Filesystem.h>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <corelib/IPlugin.h>
#include <corelib/IJobManager.h>
#include <corelib/IThreadPool.h>
#include <hmdbserviceslib/IFileStoremanWS.h>
#include <plugins/hmdbCommunication/IHMDBSession.h>
#include <plugins/hmdbCommunication/IHMDBStorage.h>
#include <hmdbserviceslib/IncrementalBranchShallowCopyParser.h>
#include <hmdbserviceslib/ShallowCopyParser.h>
#include <plugins/hmdbCommunication/IHMDBDataContext.h>
#include <plugins/hmdbCommunication/IHMDBStatusManager.h>
#include <plugins/hmdbCommunication/DataStatus.h>
#include <utils/Utils.h>

using namespace hmdbCommunication;

HMDBStorageProgress::HMDBStorageProgress() : progress_(0), aborted_(false)
{

}

HMDBStorageProgress::~HMDBStorageProgress()
{

}

void HMDBStorageProgress::setProgress(const float progress)
{
	progress_ = progress;
}

void HMDBStorageProgress::setError(const std::string & error)
{
	error_ = error;
}

const bool HMDBStorageProgress::aborted() const
{
	return aborted_;
}

const float HMDBStorageProgress::progress() const
{
	return progress_;
}

const std::string & HMDBStorageProgress::error() const
{
	return error_;
}

void HMDBStorageProgress::abort()
{
	aborted_ = true;
}

CompoundOperation::CompoundOperation(const ExecutionPolicy ep,
	const ExecutionSchema es,
	const Operations & suboperations)
	: operations(suboperations),
	ep(ep), es(es),
	status_(threadingUtils::IOperation::Initialized)
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

const unsigned int CompoundOperation::size() const
{
	return operations.size();
}

const IHMDBRemoteContext::OperationConstPtr CompoundOperation::operation(const unsigned int idx) const
{
	return operations[idx];
}

const IHMDBRemoteContext::OperationPtr CompoundOperation::operation(const unsigned int idx)
{
	return operations[idx];
}

const ICompoundOperation::ExecutionPolicy CompoundOperation::executionPolicy() const
{
	return ep;
}

const ICompoundOperation::ExecutionSchema CompoundOperation::executionSchema() const
{
	return es;
}

void CompoundOperation::start()
{
	if (status_ != threadingUtils::IOperation::Initialized){
		return;
	}

	status_ = threadingUtils::IOperation::Pending;

	if (ep == ICompoundOperation::Any){

		if (es == ICompoundOperation::Serial){
			auto runnable = threadingUtils::IRunnablePtr(new threadingUtils::FunctorRunnable(boost::bind(&CompoundOperation::callAnySerial, this)));
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

		if (es == ICompoundOperation::Serial){
			runnable = threadingUtils::IRunnablePtr(new threadingUtils::FunctorRunnable(boost::bind(&CompoundOperation::callAllSerial, this)));
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
		status_ != threadingUtils::IOperation::Finished){

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
	for (auto it = operations.begin(); it != operations.end() && status_ == threadingUtils::IOperation::Pending; ++it){
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
				errorOperation = (*it);
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
					errorOperation = (*it);
					status_ = threadingUtils::IOperation::Error;
				}
			}
			else if (s == threadingUtils::IOperation::Aborted){
				status_ = threadingUtils::IOperation::Aborted;
			}
			else if (s == threadingUtils::IOperation::Finished){
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
		else if (ss == IOperation::Finished)
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
			s = IOperation::Finished;
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

IHMDBRemoteContext::OperationPtr CompoundOperation::findErrorOperation() const
{
	IHMDBRemoteContext::OperationPtr ret;

	for (auto it = operations.begin(); it != operations.end(); ++it){
		if ((*it)->status() == IOperation::Error){
			ret = (*it);
			break;
		}
	}

	return ret;
}

FunctorOperation::FunctorOperation(threadingUtils::FunctorRunnable::Functor functor)
	: functor_(functor), progress_(0.0),
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
	if (functor_.empty() == false){
		throw std::runtime_error("Functor operation already initialized");
	}	
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
		functor_();
		status_ = threadingUtils::IOperation::Finished;
		progress_ = 1.0;
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
	if (status_ == threadingUtils::IOperation::Initialized ||
		status_ == threadingUtils::IOperation::Pending){

		status_ = threadingUtils::IOperation::Aborted;

	}
}

void FunctorOperation::start()
{
	if (status_ != threadingUtils::IOperation::Initialized){
		return;
	}

	status_ = threadingUtils::IOperation::Pending;
	job = plugin::getJobManager()->addJob("HMDBSourceContext", "FunctorOperation", threadingUtils::IRunnablePtr(new threadingUtils::FunctorRunnable(boost::bind(&FunctorOperation::call, this))));
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

UploadOperation::UploadOperation(const std::string & fileName,
	IHMDBFtp::TransferPtr transfer,
	HMDBStore store,
	const std::string & fileDescription,
	const std::string & subdirectory)
	: fileName(fileName), store(store), transfer(transfer),
	fileDescription(fileDescription),
	subdirectory(subdirectory), fileID_(-1),
	fOp(boost::bind(&UploadOperation::upload, this))
{	

}

UploadOperation::~UploadOperation()
{

}

const hmdbServices::ID UploadOperation::fileID() const
{
	return fileID_;
}

void UploadOperation::upload()
{
	if (status() != threadingUtils::IOperation::Aborted){
		transfer->start();
		transfer->wait();
	}

	if (status() != threadingUtils::IOperation::Aborted){
		fileID_ = store(fileName, fileDescription, subdirectory);
		progress_ = 1.0f;
		status_ = threadingUtils::IOperation::Finished;
	}
}

void UploadOperation::start()
{	
	fOp.start();
}

void UploadOperation::wait()
{
	fOp.wait();
}

void UploadOperation::abort()
{
	status_ = Aborted;
	fOp.abort();
}

const UploadOperation::Status UploadOperation::status() const
{
	if (status_ == Aborted){
		return status_;
	}

	return fOp.status();
}

const std::string UploadOperation::error() const
{
	return fOp.error();
}

const float UploadOperation::progress() const
{
	return (progress_ + transfer->progress()) / 2.0;
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

PrepareHMDB::PrepareHMDB(hmdbServices::IFileStoremanWS * fs,
	const hmdbServices::ID fileID)
	: fs(fs), fileID_(fileID)
{
	
}

PrepareHMDB::~PrepareHMDB()
{
	clearHMDB();
}

const std::string PrepareHMDB::prepareFilePath()
{
	if (fileID_ > -1){
		preparedFilePath_ = retrieveData(fs, fileID_);
	}
	else{
		preparedFilePath_ = retrievePerspective(fs, fileID_);
	}

	return preparedFilePath_;
}

//! \return Czyści baze danych ruchu
void PrepareHMDB::clearHMDB()
{
	if (preparedFilePath_.empty() == false){
		const auto tmpPath = preparedFilePath_;
		std::string().swap(preparedFilePath_);
		if (fileID_ > -1){
			dynamic_cast<hmdbServices::IBasicStoremanWS*>(fs)->downloadComplete(fileID_, tmpPath);
		}
		else{
			dynamic_cast<hmdbServices::IShallowStoremanWS*>(fs)->downloadComplete(tmpPath);
		}
	}
}

void StoreOutput::store(IHMDBStorage::TransactionPtr storageTransaction, const std::string & key,
	IHMDBStorage::IStreamPtr stream, IHMDBStorageProgress * progress)
{
	storageTransaction->set(key, stream, progress);
}

TmpFileTransferIO::TmpFileTransferIO()
{

}

TmpFileTransferIO::~TmpFileTransferIO()
{

}

IHMDBStorage::OStreamPtr TmpFileTransferIO::prepareOutput()
{
	int i = 0;
	while (core::Filesystem::pathExists(tmpFilePath = plugin::getPaths()->getTmpPath() / plugin::getPaths()->generateTempFileName()) == true && i++ < 10) {}

	if (core::Filesystem::pathExists(tmpFilePath) == false){

		//stream.reset(new std::fstream(tmpFilePath.string().c_str(), std::ios_base::in | std::ios_base::out));
		stream.reset(new std::fstream(tmpFilePath.string(), std::ios_base::out));

		if (stream->is_open() == false){
			stream.reset();
		}
		else{
			stream->close();
			stream.reset(new std::fstream(tmpFilePath.string()));
			if (stream->is_open() == false){
				stream.reset();
			}
		}
	}

	return stream;
}

void TmpFileTransferIO::closeOutput()
{
	stream->close();
	stream.reset();
}

IHMDBStorage::IStreamPtr TmpFileTransferIO::openInput()
{
	stream->flush();
	stream->seekg(0, std::ios::beg);
	return stream;
}

void TmpFileTransferIO::closeInput()
{
	stream->flush();
	stream.reset();
}

MemoryTransferIO::MemoryTransferIO()
{

}

MemoryTransferIO::~MemoryTransferIO()
{

}

IHMDBStorage::OStreamPtr MemoryTransferIO::prepareOutput()
{
	stream.reset(new std::stringstream);
	return stream;
}

void MemoryTransferIO::closeOutput()
{
	stream.reset();
}

IHMDBStorage::IStreamPtr MemoryTransferIO::openInput()
{
	stream->seekg(0, std::ios::beg);
	return stream;
}

void MemoryTransferIO::closeInput()
{
	closeOutput();
}

FileDownload::FileDownload(const IHMDBRemoteContext::CompoundID & fileToDownload,
	utils::shared_ptr<PrepareHMDB> prepareHMDB,
	utils::shared_ptr<ITransferIO> transferIO,
	IHMDBFtp * ftp,
	IHMDBStoragePtr storage)
	: prepareHMDB(prepareHMDB), transferIO(transferIO), storage_(storage), ftp(ftp),
	downloaded_(false), fileID_(fileToDownload), fOp(boost::bind(&FileDownload::download, this)),
	progressNormalizer(storage == nullptr ? 5 : 7), progress_(0)
{	
	
}

FileDownload::~FileDownload()
{

}

void FileDownload::start()
{
	storeProgress.reset(new HMDBStorageProgress);
	fOp.start();
}

void FileDownload::wait()
{
	fOp.wait();
}

void FileDownload::abort()
{
	if (status() != FileDownload::Aborted){
		status_ = FileDownload::Aborted;
		fOp.abort();
	}
}

const FileDownload::Status FileDownload::status() const
{
	if (status_ == FileDownload::Aborted){
		return status_;
	}

	return fOp.status();
}

const std::string FileDownload::error() const
{
	return fOp.error();
}

const float FileDownload::progress() const
{
	return (progress_ + (transfer != nullptr ? transfer->progress() : 0) + (storeProgress != nullptr ? storeProgress->progress() : 0)) / (float)progressNormalizer;
}

const IHMDBRemoteContext::CompoundID FileDownload::fileID() const
{
	return fileID_;
}

const bool FileDownload::fileDownloaded() const
{
	return downloaded_;
}

const IHMDBStoragePtr FileDownload::storage()
{
	return storage_;
}

const IHMDBStorageConstPtr FileDownload::storage() const
{
	return storage_;
}

void FileDownload::download()
{
	if (status_ == FileDownload::Aborted){
		return;
	}

	auto const filePath = prepareHMDB->prepareFilePath();

	if (filePath.empty() == true){
		throw std::runtime_error("Empty file path to retrieve");
	}
	++progress_;

	if (status_ == FileDownload::Aborted){
		prepareHMDB->clearHMDB();
		++progress_;
		return;
	}

	auto output = transferIO->prepareOutput();

	if (output == nullptr){
		prepareHMDB->clearHMDB();
		++progress_;
		throw std::runtime_error("Uninitialized output stream");
	}
	++progress_;

	if (status_ == FileDownload::Aborted){
		transferIO->closeOutput();
		++progress_;
		prepareHMDB->clearHMDB();
		++progress_;
		return;
	}

	transfer = ftp->prepareGet(filePath, output, fileID_.fileSize);
	
	if (transfer == nullptr){
		transferIO->closeOutput();
		++progress_;
		prepareHMDB->clearHMDB();
		++progress_;
		throw std::runtime_error("Uninitialized data transfer");
	}

	transfer->start();
	transfer->wait();

	if (status() == FileDownload::Aborted || transfer->size() == 0){
		transferIO->closeOutput();
		++progress_;
		prepareHMDB->clearHMDB();
		++progress_;
		return;
	}

	if (storage_ != nullptr){
		auto input = transferIO->openInput();

		if (input == nullptr){
			transferIO->closeInput();
			++progress_;
			prepareHMDB->clearHMDB();
			++progress_;
			throw std::runtime_error("Failed to open downloaded data for reading");
		}
		++progress_;

		
		const auto begin = input->tellg();
		input->seekg(0, std::ios::end);
		const auto end = input->tellg();
		input->seekg(0, std::ios::beg);
		const auto streamSize = end - begin;

		auto storageTransaction = storage_->transaction();

		if (storageTransaction == nullptr){
			transferIO->closeInput();
			++progress_;
			prepareHMDB->clearHMDB();
			++progress_;
			throw std::runtime_error("Failed to create storage transaction");
		}

		if (storageTransaction->canStore(streamSize) == false){
			transferIO->closeInput();
			++progress_;
			prepareHMDB->clearHMDB();
			++progress_;
			throw std::runtime_error("Storage can not handle downloaded data size");
		}

		StoreOutput::store(storageTransaction, fileID_.fileName, input, storeProgress.get());

		if (storeProgress->error().empty() == false){
			throw std::runtime_error("Saving file in storage failed");
		}

		if (storeProgress->aborted() == false){
			downloaded_ = true;
		}

		transferIO->closeInput();
	}
	else{
		transferIO->closeOutput();
	}

	++progress_;

	prepareHMDB->clearHMDB();
	++progress_;
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

void ExtractShallowcopy::extract(const std::list<IHMDBRemoteContext::DownloadOperationPtr> & downloads, IHMDBStorageProgress * progress)
{
	auto it = downloads.begin();	

	std::map<IHMDBStoragePtr, std::list<IHMDBRemoteContext::DownloadOperationPtr>> byStorage;
	int numDownloadsToProcess = 0;

	std::for_each(downloads.begin(), downloads.end(), [&](IHMDBRemoteContext::DownloadOperationPtr d)
	{
		if (d->fileDownloaded() == true){
			auto s = d->storage();
			if (s != nullptr){
				byStorage[s].push_back(d);
				++numDownloadsToProcess;
			}
		}
	});

	if (numDownloadsToProcess == 0){
		return;
	}

	ShallowCopyPtr locSh(new ShallowCopy);
	IncrementalBranchShallowCopyPtr locIncSh(new hmdbServices::IncrementalBranchShallowCopy);

	const auto progressStep = 1.0 / numDownloadsToProcess;
	int currentStep = 1;
	progress->setProgress(0.0);

	for (auto it = byStorage.begin(); it != byStorage.end() && progress->aborted() == false; ++it){

		auto streamTransaction = it->first->transaction();

		auto IT = it->second.begin();
		while (IT != it->second.end() && progress->aborted() == false){
			
			auto stream = streamTransaction->get((*IT)->fileID().fileName);
			extractShallowCopy((*IT)->fileID().fileID, (*IT)->fileID().dataReference,
					*locSh, *locIncSh, stream.get());

			++IT;
			progress->setProgress(progressStep * currentStep++);
		}
	}

	if (progress->aborted() == false){
		progress->setProgress(1.0);
		shallowCopy_ = locSh;
		incrementalShallowCopy_ = locIncSh;
	}
}

SynchronizeOperation::SynchronizeOperation(const std::list<IHMDBRemoteContext::DownloadOperationPtr> & downloads,
	IHMDBStoragePtr storage)
	: status_(Initialized), storage(storage),
	downloads(downloads), fOp(boost::bind(&SynchronizeOperation::synchronize, this))
{
	
}

SynchronizeOperation::~SynchronizeOperation()
{

}

void SynchronizeOperation::start()
{
	extractProgress.reset(new HMDBStorageProgress);
	fOp.start();
}

void SynchronizeOperation::wait()
{
	fOp.wait();
}

void SynchronizeOperation::abort()
{
	if (status_ == Initialized){
		status_ = Aborted;
		fOp.abort();
	}
}

const SynchronizeOperation::Status SynchronizeOperation::status() const
{
	if (status_ == Aborted){
		return status_;
	}

	return fOp.status();
}

const std::string SynchronizeOperation::error() const
{
	return fOp.error();
}

const float SynchronizeOperation::progress() const
{
	return (downloadsProgress() + ((extractProgress == nullptr) ? 0.0 : extractProgress->progress())) / 2.0;
}

void SynchronizeOperation::synchronize()
{
	std::for_each(downloads.begin(), downloads.end(), [&](IHMDBRemoteContext::DownloadOperationPtr d)
	{
		d->start();
	});

	std::for_each(downloads.begin(), downloads.end(), [&](IHMDBRemoteContext::DownloadOperationPtr d)
	{
		d->wait();
	});

	if (status() == Running){

		ExtractShallowcopy eSc;

		eSc.extract(downloads, extractProgress.get());

		if (extractProgress->aborted() == false && extractProgress->error().empty() == true){
			shallowCopy_ = eSc.shallowCopy();
			incrementalShallowCopy_ = eSc.incrementalBranchShallowCopy();
		}
	}
}

const float SynchronizeOperation::downloadsProgress() const
{
	float p = 0.0;

	std::for_each(downloads.begin(), downloads.end(), [&](IHMDBRemoteContext::DownloadOperationPtr d)
	{
		p += d->progress();
	});

	return p / downloads.size();
}

const ShallowCopyConstPtr SynchronizeOperation::shallowCopy() const
{
	return shallowCopy_;
}

const IncrementalBranchShallowCopyConstPtr SynchronizeOperation::incrementalBranchShallowCopy() const
{
	return incrementalShallowCopy_;
}