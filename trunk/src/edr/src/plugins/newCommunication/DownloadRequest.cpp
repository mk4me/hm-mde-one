#include "CommunicationPCH.h"
#include "DownloadRequest.h"
#include <boost/bind.hpp>

using namespace communication;

//! Prywatny konstruktor
DownloadRequest::DownloadRequest() : state_(Created), currentTransferID(0), totalSize_(0), currentSize(0)
{
	//inicjalizacja callbacków pojedynczego pobierania
	singleTransferCallbacks.onBeginCallback = (CommunicationManager::RequestCallback)boost::bind(&DownloadRequest::onDownloadBegin, this, _1);
	singleTransferCallbacks.onEndCallback = (CommunicationManager::RequestCallback)boost::bind(&DownloadRequest::onDownloadComplete, this, _1);
	singleTransferCallbacks.onCancelCallback = (CommunicationManager::RequestCallback)boost::bind(&DownloadRequest::onDownloadCancel, this, _1);
	singleTransferCallbacks.onErrorCallback = (CommunicationManager::RequestErrorCallback)boost::bind(&DownloadRequest::onDownloadError, this, _1, _2);

	//inicjalizacja callbacków całego pakietu transferów
	complexTransferCallbacks.onBeginCallback = (CommunicationManager::RequestCallback)boost::bind(&DownloadRequest::onFullDownloadStart, this, _1);
	complexTransferCallbacks.onEndCallback = (CommunicationManager::RequestCallback)boost::bind(&DownloadRequest::onFullDownloadComplete, this, _1);
	complexTransferCallbacks.onCancelCallback = singleTransferCallbacks.onCancelCallback;
	complexTransferCallbacks.onErrorCallback = singleTransferCallbacks.onErrorCallback;
}

DownloadRequest * DownloadRequest::createFilesRequest(const std::map<int, FileDescriptor> & filesToDownload)
{
    auto communicationManager = CommunicationManager::getInstance();

    DownloadRequest * ret = new DownloadRequest();

    std::vector<CommunicationManager::CompleteRequest> filesRequests;

    for(auto it = filesToDownload.begin(); it != filesToDownload.end(); ++it){
		CommunicationManager::CompleteRequest completeRequest;
		completeRequest.request = communicationManager->createRequestFile(it->first, it->second.destPath);
		completeRequest.callbacks = ret->singleTransferCallbacks;
        filesRequests.push_back(completeRequest);

		ret->totalSize_ += it->second.size;
        ret->filesSizes[it->first] = it->second.size;
    }

	ret->request_ = communicationManager->createRequestComplex(filesRequests);

    return ret;
}

DownloadRequest * DownloadRequest::createPhotosRequest(const std::map<int, FileDescriptor> & photosToDownload)
{
    return nullptr;
}

DownloadRequest * DownloadRequest::createFilesAndPhotosRequest(const std::map<int, FileDescriptor> & filesToDownload,
    const std::map<int, FileDescriptor> & photosToDownload)
{
    return nullptr;
}

DownloadRequest * DownloadRequest::createShallowCopyRequest(const std::map<CommunicationManager::Request, ShallowCopyDescriptor> & shallowCopiesToDownload)
{
	auto communicationManager = CommunicationManager::getInstance();

	DownloadRequest * ret = new DownloadRequest();

	std::vector<CommunicationManager::CompleteRequest> filesRequests;

	for(auto it = shallowCopiesToDownload.begin(); it != shallowCopiesToDownload.end(); ++it){
		CommunicationManager::CompleteRequest completeRequest;

		switch(it->first){
		case CommunicationManager::CopyMotionShallowCopy:
			completeRequest.request = communicationManager->createRequestMotionShallowCopy(it->second.destPath);
			ret->shallowSizes[CommunicationManager::CopyMotionShallowCopy] = it->second.size;
			break;
		case CommunicationManager::CopyMotionMetadata:
			completeRequest.request = communicationManager->createRequestMotionMetadata(it->second.destPath);
			ret->shallowSizes[CommunicationManager::CopyMotionMetadata] = it->second.size;
			break;
		case CommunicationManager::CopyMedicalShallowCopy:
			completeRequest.request = communicationManager->createRequestMedicalShallowCopy(it->second.destPath);
			ret->shallowSizes[CommunicationManager::CopyMedicalShallowCopy] = it->second.size;
			break;
		case CommunicationManager::CopyMedicalMetadata:
			completeRequest.request = communicationManager->createRequestMedicalMetadata(it->second.destPath);
			ret->shallowSizes[CommunicationManager::CopyMedicalMetadata] = it->second.size;
			break;
		}

		if(completeRequest.request != nullptr){
			completeRequest.callbacks = ret->singleTransferCallbacks;
			filesRequests.push_back(completeRequest);

			if(it->second.size > 0){
				ret->totalSize_ += it->second.size;
				ret->filesSizes[it->first] = it->second.size;
			}
		}
	}

	ret->request_ = communicationManager->createRequestComplex(filesRequests);

	return ret;
}

DownloadRequest::~DownloadRequest()
{

}

const DownloadRequest::State DownloadRequest::state() const
{
    return state_;
}

const std::string & DownloadRequest::error() const
{
	ScopedLock lock(stateMutex);
    return error_;
}

const CommunicationManager::ComplexRequestPtr & DownloadRequest::request() const
{
    return request_;
}

void DownloadRequest::cancel()
{
	ScopedLock lock(cancelMutex);
	if(state_ == FinishedCancel || state_ == FinishedError || state_ == FinishedOK){
		throw std::runtime_error("Download already finished");
	}

    CommunicationManager::getInstance()->cancelRequest(request_);

	state_ = FinishedCancel;

	notify();
}

void DownloadRequest::start()
{
	{
		ScopedLock lock(stateMutex);
		if(state_ != Created){
			throw std::runtime_error("Download already started");
		}

		state_ = Pending;
	}

	notify();

	CommunicationManager::CompleteRequest completeRequest;
	completeRequest.request = request_;
	completeRequest.callbacks = complexTransferCallbacks;

	CommunicationManager::getInstance()->pushRequest(completeRequest);
}

const float DownloadRequest::totalProgress() const
{
    return request_->getProgress();
}

const float DownloadRequest::currentFileProgress() const
{
    ScopedLock lock(stateMutex);
    if(currentTransfer != nullptr){
        return currentTransfer->getProgress();
    }

    return 0;
}

std::string DownloadRequest::currentFileName() const
{
	ScopedLock lock(stateMutex);
	if(currentTransfer != nullptr){
		auto metaTransfer(core::dynamic_pointer_cast<CommunicationManager::MetadataRequest>(currentTransfer));
		if(metaTransfer != nullptr){
			return core::Filesystem::Path(metaTransfer->getFilePath()).filename().string();
		}
	}

	return std::string();
}

std::string DownloadRequest::currentFilePath() const
{
	ScopedLock lock(stateMutex);
	if(currentTransfer != nullptr){
		auto metaTransfer(core::dynamic_pointer_cast<CommunicationManager::MetadataRequest>(currentTransfer));
		if(metaTransfer != nullptr){
			return metaTransfer->getFilePath();
		}
	}

	return std::string();
}

const long DownloadRequest::totalSize() const
{
    return totalSize_;
}

const long DownloadRequest::currentFileSize() const
{
	ScopedLock lock(stateMutex);
    return currentSize;
}

const int DownloadRequest::totalFilesToDownload() const
{
    return request_->size();
}

const int DownloadRequest::filesDownloaded() const
{
	ScopedLock lock(stateMutex);
    return (std::max)(currentTransferID - 1, 0);
}

const int DownloadRequest::currentFile() const
{
    ScopedLock lock(stateMutex);
    return (std::max)(currentTransferID, 1);
}

const long DownloadRequest::getRequestSize(const CommunicationManager::BasicRequestPtr & request) const
{
    long size = 0;
    auto it = shallowSizes.find(request->getType());
    if(it != shallowSizes.end()){
        size = it->second;
    }else {
        CommunicationManager::FileRequestPtr fileRequest(core::dynamic_pointer_cast<CommunicationManager::FileRequest>(request));
        if(fileRequest != nullptr){
            size = filesSizes.find(fileRequest->getFileID())->second;
        }else{
            CommunicationManager::PhotoRequestPtr photoRequest(core::dynamic_pointer_cast<CommunicationManager::PhotoRequest>(request));
            size = photosSizes.find(photoRequest->getPhotoID())->second;
        }
    }

    return size;
}

void DownloadRequest::addFileRequest(int fileID, const std::string & destPath, long fileSize, const CommunicationManager::RequestCallbacks & callbacks)
{

}

void DownloadRequest::addPhotoRequest(int photoID, const std::string & destPath, long fileSize, const CommunicationManager::RequestCallbacks & callbacks)
{

}

void DownloadRequest::addMetadataRequest(CommunicationManager::Request metadataType, const std::string & destPath, long fileSize, const CommunicationManager::RequestCallbacks & callbacks)
{

}

void DownloadRequest::onFullDownloadStart(const CommunicationManager::BasicRequestPtr & request)
{
    state_ = Started;
    notify();
}

void DownloadRequest::onFullDownloadComplete(const CommunicationManager::BasicRequestPtr & request)
{
    state_ = FinishedOK;
    notify();
}

void DownloadRequest::onDownloadBegin(const CommunicationManager::BasicRequestPtr & request)
{
	ScopedLock lock(stateMutex);
    currentTransfer = request;
    currentSize = getRequestSize(request);
	currentTransferID++;
}

void DownloadRequest::onDownloadCancel(const CommunicationManager::BasicRequestPtr & request)
{
	{
		ScopedLock lock(cancelMutex);
		if(state_ == FinishedCancel){
			return;
		}

		if(state_ == Started || state_ == Pending){
			state_ = FinishedCancel;
		}
	}

	if(state_ == FinishedCancel){
		notify();
	}
}

void DownloadRequest::onDownloadComplete(const CommunicationManager::BasicRequestPtr & request)
{
    state_ = SingleFinished;
    notify();
}

void DownloadRequest::onDownloadError(const CommunicationManager::BasicRequestPtr & request, const std::string & error)
{
    if(state_ == Started || state_ == SingleFinished){
        state_ = FinishedError;
        error_ = error;
        notify();
    }
}
