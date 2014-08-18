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
#include <numeric>

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

const std::string HMDBStorageProgress::error() const
{
	return error_;
}

void HMDBStorageProgress::abort()
{
	aborted_ = true;
}

DownloadHelper::DownloadHelper(IHMDBStoragePtr storage, const std::list<IHMDBRemoteContext::DownloadOperationPtr> & downloads)
	: downloads(downloads), storage(storage)
{

}

void DownloadHelper::download()
{
	std::for_each(downloads.begin(), downloads.end(), [&](IHMDBRemoteContext::DownloadOperationPtr d)
	{
		d->start();
	});
}

void DownloadHelper::wait()
{
	std::for_each(downloads.begin(), downloads.end(), [&](IHMDBRemoteContext::DownloadOperationPtr d)
	{
		d->wait();
	});
}

void DownloadHelper::abort()
{
	storeProgress.abort();
	std::for_each(downloads.begin(), downloads.end(), [&](IHMDBRemoteContext::DownloadOperationPtr d)
	{
		d->abort();
	});
}

const float DownloadHelper::progress() const
{
	const auto pd = std::accumulate(downloads.begin(), downloads.end(), 0.0, [](float total, IHMDBRemoteContext::DownloadOperationPtr d) { return total + d->progress(); }) / downloads.size();
	return (pd + storeProgress.progress()) / 2.0;
}

const std::list<IHMDBRemoteContext::DownloadOperationPtr> filterCompleteDownloads(const std::list<IHMDBRemoteContext::DownloadOperationPtr> & downloads)
{
	std::list<IHMDBRemoteContext::DownloadOperationPtr> ret;

	std::for_each(downloads.begin(), downloads.end(), [&](IHMDBRemoteContext::DownloadOperationPtr d)
	{
		if (d->fileDownloaded() == true){
			ret.push_back(d);
		}
	});

	return ret;
}

void DownloadHelper::filterDownloaded()
{
	downloaded_ = filterCompleteDownloads(downloads);
}

const std::list<IHMDBRemoteContext::DownloadOperationPtr> & DownloadHelper::downloaded() const
{
	return downloaded_;
}

void DownloadHelper::store()
{
	if (downloaded_.empty() == true){
		throw std::runtime_error("Downloads failed. Could not synchronize");
	}

	auto t = storage->transaction();

	const auto s = downloaded_.size();
	std::for_each(downloaded_.begin(), downloaded_.end(), [&](IHMDBRemoteContext::DownloadOperationPtr d)
	{
		if (storeProgress.aborted() == true){
			return;
		}

		try{
			t->set(d->fileID().fileName, d->stream(), &storeProgress, s);
		}
		catch (...){

		}
	});
}

DownloadHelper::~DownloadHelper()
{
	std::for_each(downloads.begin(), downloads.end(), [&](IHMDBRemoteContext::DownloadOperationPtr d)
	{
		try{
			d->release();
		}
		catch (...){

		}
	});
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

TmpFileTransferIO::TmpFileTransferIO()
{

}

TmpFileTransferIO::~TmpFileTransferIO()
{
	release();
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
	stream->flush();
}

void TmpFileTransferIO::release()
{
	if (tmpFilePath.empty() == false && stream != nullptr){
		stream->flush();
		stream->close();
		stream.reset();
		core::Filesystem::deleteFile(tmpFilePath);
		tmpFilePath = core::Filesystem::Path();
	}
}

IHMDBStorage::IStreamPtr TmpFileTransferIO::openInput()
{	
	stream->seekg(0, std::ios::beg);
	return stream;
}

void TmpFileTransferIO::closeInput()
{
	release();
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
	stream->flush();
}

IHMDBStorage::IStreamPtr MemoryTransferIO::openInput()
{
	stream->seekg(0, std::ios::beg);
	return stream;
}

void MemoryTransferIO::closeInput()
{
	release();
}

void MemoryTransferIO::release()
{
	stream.reset();
}

FileDownload::FileDownload(const IHMDBRemoteContext::FileDescriptor & fileToDownload,
	utils::shared_ptr<PrepareHMDB> prepareHMDB,
	utils::shared_ptr<ITransferIO> transferIO,
	IHMDBFtp * ftp)
	: prepareHMDB(prepareHMDB), transferIO(transferIO), ftp(ftp),
	downloaded_(false), fileID_(fileToDownload),
	fOp(boost::bind(&FileDownload::download, this)),
	progress_(0)
{	
	
}

FileDownload::~FileDownload()
{
	transferIO->release();
}

void FileDownload::start()
{	
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
	return (progress_ + (transfer != nullptr ? transfer->progress() : 0)) / 5;
}

const IHMDBRemoteContext::FileDescriptor FileDownload::fileID() const
{
	return fileID_;
}

const bool FileDownload::fileDownloaded() const
{
	return downloaded_;
}

const IHMDBStorage::IStreamPtr FileDownload::stream() const
{
	return transferIO->openInput();
}

void FileDownload::release()
{
	transferIO->release();
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
	
	transferIO->closeOutput();

	if (transfer->status() == threadingUtils::IOperation::Finished){
		downloaded_ = true;
	}

	++progress_;
	prepareHMDB->clearHMDB();
	++progress_;
}

MultipleFilesDownloadAndStore::MultipleFilesDownloadAndStore(const std::list<IHMDBRemoteContext::DownloadOperationPtr> & downloads,
	IHMDBStoragePtr storage) : status_(Initialized), downloadHelper(storage, downloads)
{

}

MultipleFilesDownloadAndStore::~MultipleFilesDownloadAndStore()
{

}

void MultipleFilesDownloadAndStore::start()
{
	if (status_ != Initialized){
		return;
	}

	try{
		core::IThreadPool::Threads t;
		plugin::getThreadPool()->getThreads("HMDBCommunication", t, 1);
		thread_ = t.front();
		threadingUtils::IRunnablePtr runnable(new threadingUtils::FunctorRunnable(boost::bind(&MultipleFilesDownloadAndStore::download, this)));

		thread_->start(runnable);
		status_ = Pending;
	}
	catch (std::exception & e){
		error_ = e.what();
		status_ = Error;
		throw;
	}
	catch (...){
		error_ = "UNKNOWN error while launching multiple files download";
		status_ = Error;
		throw;
	}
}

void MultipleFilesDownloadAndStore::wait()
{
	if (thread_ != nullptr){
		thread_->join();
	}
}

void MultipleFilesDownloadAndStore::abort()
{
	if (status_ != Error && status_ != Aborted){
		status_ = Aborted;
		downloadHelper.abort();
	}
}

const MultipleFilesDownloadAndStore::Status MultipleFilesDownloadAndStore::status() const
{
	return status_;
}

const std::string MultipleFilesDownloadAndStore::error() const
{
	return error_;
}

const float MultipleFilesDownloadAndStore::progress() const
{
	return downloadHelper.progress();
}

void MultipleFilesDownloadAndStore::download()
{
	try{

		if (status_ == Aborted){
			return;
		}

		downloadHelper.download();
		downloadHelper.wait();

		if (status_ == Aborted){
			return;
		}

		downloadHelper.filterDownloaded();

		downloadHelper.store();

		if (status_ == Aborted){
			return;
		}

		downloadFinished(downloadHelper.downloaded());

		status_ = Finished;
	}
	catch (std::exception & e){
		error_ = e.what();
		status_ = Error;
	}
	catch (...){
		error_ = "UNKNOWN error while downloading multiple files";
		status_ = Error;
	}
}

MultipleFilesDownloadStoreAndStatusUpdate::MultipleFilesDownloadStoreAndStatusUpdate(const std::list<IHMDBRemoteContext::DownloadOperationPtr> & downloads,
	IHMDBStoragePtr storage, IHMDBStatusManagerPtr statusManager, ShallowCopyConstPtr shallowCopy)
	: MultipleFilesDownloadAndStore(downloads, storage), statusManager(statusManager), shallowCopy(shallowCopy)
{

}

MultipleFilesDownloadStoreAndStatusUpdate::~MultipleFilesDownloadStoreAndStatusUpdate()
{

}

void MultipleFilesDownloadStoreAndStatusUpdate::downloadFinished(const std::list<IHMDBRemoteContext::DownloadOperationPtr> & finishedDownloads)
{
	if (finishedDownloads.empty() == true){
		return;
	}

	auto t = statusManager->transaction();

	std::for_each(finishedDownloads.begin(), finishedDownloads.end(), [&](IHMDBRemoteContext::DownloadOperationPtr d)
	{		
		t->updateDataStatus(FileType, d->fileID().id.fileID, DataStatus(DataStatus::Local, DataStatus::UnknownUsage, DataStatus::Valid));
	});

	if (shallowCopy != nullptr){
		statusManager->tryUpdate(shallowCopy);
	}
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
	ShallowCopyPtr locSh(new ShallowCopy);
	IncrementalBranchShallowCopyPtr locIncSh(new hmdbServices::IncrementalBranchShallowCopy);	

	const auto progressStep = 1.0 / (float)downloads.size();
	int currentStep = 1;
	progress->setProgress(0.0);

	std::for_each(downloads.begin(), downloads.end(), [&](IHMDBRemoteContext::DownloadOperationPtr d)
	{
		if (progress->aborted() == true){
			return;
		}

		auto stream = d->stream();		
		extractShallowCopy(d->fileID().id.fileID, d->fileID().id.dataReference,
			*locSh, *locIncSh, stream.get());		

		progress->setProgress(progressStep * currentStep++);
	});

	if (progress->aborted() == false){
		progress->setProgress(1.0);
		shallowCopy_ = locSh;
		incrementalShallowCopy_ = locIncSh;
	}
}

SynchronizeOperationImpl::SynchronizeOperationImpl(const std::list<IHMDBRemoteContext::DownloadOperationPtr> & downloads,
		IHMDBStoragePtr storage) : MultipleFilesDownloadAndStore(downloads, storage)
{

}

SynchronizeOperationImpl::~SynchronizeOperationImpl()
{

}

void SynchronizeOperationImpl::abort()
{
	extractProgress.abort();
	MultipleFilesDownloadAndStore::abort();
}

const float SynchronizeOperationImpl::progress() const
{
	return (MultipleFilesDownloadAndStore::progress() + extractProgress.progress()) / 2.0;
}

const ShallowCopyConstPtr SynchronizeOperationImpl::shallowCopy() const
{
	return shallowCopy_;
}

const IncrementalBranchShallowCopyConstPtr SynchronizeOperationImpl::incrementalBranchShallowCopy() const
{
	return incrementalShallowCopy_;
}

void SynchronizeOperationImpl::downloadFinished(const std::list<IHMDBRemoteContext::DownloadOperationPtr> & finishedDownloads)
{
	ExtractShallowcopy eSc;

	eSc.extract(finishedDownloads, &extractProgress);

	if (extractProgress.aborted() == false && extractProgress.error().empty() == true){
		shallowCopy_ = eSc.shallowCopy();
		incrementalShallowCopy_ = eSc.incrementalBranchShallowCopy();
	}
}

SynchronizeOperation::SynchronizeOperation(const std::list<IHMDBRemoteContext::DownloadOperationPtr> & downloads,
	IHMDBStoragePtr storage) : op(downloads, storage)
{
	
}

SynchronizeOperation::~SynchronizeOperation()
{

}

void SynchronizeOperation::start()
{
	op.start();
}

void SynchronizeOperation::wait()
{
	op.wait();
}

void SynchronizeOperation::abort()
{
	op.abort();
}

const SynchronizeOperation::Status SynchronizeOperation::status() const
{
	return op.status();
}

const std::string SynchronizeOperation::error() const
{
	return op.error();
}

const float SynchronizeOperation::progress() const
{
	return op.progress();
}

const ShallowCopyConstPtr SynchronizeOperation::shallowCopy() const
{
	return op.shallowCopy();
}

const IncrementalBranchShallowCopyConstPtr SynchronizeOperation::incrementalBranchShallowCopy() const
{
	return op.incrementalBranchShallowCopy();
}