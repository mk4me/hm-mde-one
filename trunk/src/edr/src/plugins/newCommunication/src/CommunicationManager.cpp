/**
@author Marek Daniluk
*/

#include "CommunicationPCH.h"
#include <corelib/Filesystem.h>
#include "CommunicationManager.h"
#include <OpenThreads/Thread>

CommunicationManager::RequestsExecutor::RequestsExecutor(CommunicationManager * manager,
	const time_type sleepTime) : manager(manager), sleepTime_(sleepTime), finish_(false)
{
	if(manager == nullptr){
		throw std::invalid_argument("Uninitialized CommunicationManager");
	}
}

CommunicationManager::RequestsExecutor::~RequestsExecutor()
{
	finish();
}

void CommunicationManager::RequestsExecutor::run()
{
	while(!finish_){
		
		if(manager->requestsQueueEmpty() == false){

			manager->cancelDownloading = false;
			CompleteRequest currentRequest;
			manager->popRequest(currentRequest);
			manager->currentRequest = currentRequest;
			if(currentRequest.request->canceled == false){

				switch(currentRequest.request->type) {

				case Complex:

					manager->setState(ProcessingComplex);

					manager->processComplex(currentRequest);

					break;

				case DownloadFile:

					manager->setState(DownloadingFile);

					manager->processFile(currentRequest);

					break;

                case UploadFile:

                    manager->setState(UploadingFile);

                    manager->processUpload(currentRequest);

                    break;

                case ReplaceFile:

                    manager->setState(ReplacingFile);

                    manager->processReplace(currentRequest);

                    break;

				case DownloadPhoto:

					manager->setState(DownloadingPhoto);

					manager->processPhoto(currentRequest);

					break;

				case CopyMotionShallowCopy:

					manager->setState(CopyingMotionShallowCopy);

					manager->processMotionShallowCopy(currentRequest);

					break;

                case CopyMotionBranchIncrement:

                    manager->setState(CopyingMotionBranchIncrement);

                    manager->processMotionBranchIncrementShallowCopy(currentRequest);
                    break;

				case CopyMotionMetadata:

					manager->setState(CopyingMotionMetadata);

					manager->processMotionMetadata(currentRequest);

					break;

				case CopyMedicalShallowCopy:

					manager->setState(CopyingMedicalShallowCopy);

					manager->processMedicalShallowCopy(currentRequest);

					break;

				case CopyMedicalMetadata:

					manager->setState(CopyingMedicalMetadata);

					manager->processMedicalMetadata(currentRequest);

					break;

				case PingServer:

					manager->setState(PingingServer);

					manager->processPing(currentRequest);

					break;
				}

				currentRequest.request.reset();
			}
		}else{
			if(manager->state != Ready){
				manager->setState(Ready);
			}
			OpenThreads::Thread::microSleep(sleepTime_);
		}
	}
}

void CommunicationManager::RequestsExecutor::finish()
{
	finish_ = true;
}

void CommunicationManager::RequestsExecutor::setSleepTime(const time_type sleepTime)
{
	sleepTime_ = sleepTime;
}

const CommunicationManager::RequestsExecutor::time_type CommunicationManager::RequestsExecutor::sleepTime() const
{
	return sleepTime_;
}


CommunicationManager::BasicRequest::~BasicRequest()
{

}

void CommunicationManager::BasicRequest::cancel()
{
	utils::ScopedLock<utils::StrictSyncPolicy> lock(cancelLock);
	canceled = true;
}

bool CommunicationManager::BasicRequest::isCancelled() const
{
	utils::ScopedLock<utils::StrictSyncPolicy> lock(cancelLock);
	return (isComplete() == false) && canceled;
}

CommunicationManager::Request CommunicationManager::BasicRequest::getType() const
{
	return type;
}

bool CommunicationManager::BasicRequest::isComplete() const
{
	return getProgress() == 100.0;
}

CommunicationManager::BasicRequest::BasicRequest(Request type) : type(type), canceled(false)
{

}

CommunicationManager::PingRequest::PingRequest(const std::string & urlToPing) : BasicRequest(PingServer), urlToPing_(urlToPing), response(false)
{

}

void CommunicationManager::PingRequest::setServerResponse(bool response)
{
	this->response = response;
}

double CommunicationManager::PingRequest::getProgress() const
{
	return response == true ? 100.0 : 0.0;
}

const std::string & CommunicationManager::PingRequest::urlToPing() const
{
	return urlToPing_;
}

CommunicationManager::MetadataRequest::MetadataRequest(Request type, const std::string & filePath, const webservices::DateTime& dateTime) : 
    BasicRequest(type), 
    filePath(filePath),
    progress(0),
    dateTime(dateTime)
{

}

const std::string & CommunicationManager::MetadataRequest::getFilePath() const
{
	return filePath;
}

void CommunicationManager::MetadataRequest::setProgress(double p)
{
	progress = p;
}

double CommunicationManager::MetadataRequest::getProgress() const
{
	return progress;
}

webservices::DateTime CommunicationManager::MetadataRequest::getDateTime() const
{
    return dateTime;
}

CommunicationManager::FileRequest::FileRequest(const std::string & filePath, unsigned int fileID) : MetadataRequest(DownloadFile, filePath), fileID(fileID)
{

}

unsigned int CommunicationManager::FileRequest::getFileID() const
{
	return fileID;
}

CommunicationManager::ReplaceRequest::ReplaceRequest( unsigned int fileID, const std::string & sourcePath, const std::string & filePath) : 
    MetadataRequest(ReplaceFile, filePath), 
    sourcePath(sourcePath),
    fileID(fileID)
{

}

unsigned int CommunicationManager::ReplaceRequest::getFileID() const
{
    return fileID;
}

std::string CommunicationManager::ReplaceRequest::getFileName() const
{
    core::Filesystem::Path p(getFilePath());
    return p.filename().string();
}

std::string CommunicationManager::ReplaceRequest::getSourcePath() const
{
    return sourcePath;
}

CommunicationManager::UploadRequest::UploadRequest(const std::string& sourcePath, const std::string & filePath, unsigned int trialID) : 
    MetadataRequest(UploadFile, filePath), 
    sourcePath(sourcePath),
    trialID(trialID)
{

}

unsigned int CommunicationManager::UploadRequest::getTrialID() const
{
    return trialID;
}

std::string CommunicationManager::UploadRequest::getFileName() const
{
    core::Filesystem::Path p(getFilePath());
    return p.filename().string();
}


CommunicationManager::PhotoRequest::PhotoRequest(const std::string & filePath, unsigned int photoID) : MetadataRequest(DownloadPhoto, filePath), photoID(photoID)
{

}

unsigned int CommunicationManager::PhotoRequest::getPhotoID() const
{
	return photoID;
}

CommunicationManager::ComplexRequest::ComplexRequest(const std::vector<CompleteRequest> & requests) : BasicRequest(Complex), requests(requests)
{

}

unsigned int CommunicationManager::ComplexRequest::size() const
{
	return requests.size();
}

bool CommunicationManager::ComplexRequest::empty() const
{
	return requests.empty();
}

const CommunicationManager::CompleteRequest & CommunicationManager::ComplexRequest::getRequest(unsigned int i) const
{
	return requests.at(i);
}

double CommunicationManager::ComplexRequest::getProgress() const
{
	double ret = 0;

	for(auto it = requests.begin(); it != requests.end(); ++it){
		double loc = (*it).request->getProgress();
		if(loc == 0){
			break;
		}

		ret += loc;
	}

	return 100.0 * (ret / (100 * requests.size()));
}

CommunicationManager::CommunicationManager(core::IThreadPtr executorThread)
	: executorThread(executorThread),	finish(false), state(Ready), pingCurl(nullptr)
{

	if(executorThread == nullptr){
		throw std::invalid_argument("Uninitialized requests processing thread");
	}

	pingCurl = curl_easy_init();
	if(pingCurl) {
		curl_easy_setopt(pingCurl, CURLOPT_URL, "127.0.0.1");
		curl_easy_setopt(pingCurl, CURLOPT_CONNECTTIMEOUT, 1);
		curl_easy_setopt(pingCurl, CURLOPT_WRITEFUNCTION, pingDataCallback);
	}else{
		throw std::runtime_error("Error while initializing CURL");
	}	

	requestsExecutor.reset(new RequestsExecutor(this));
}

CommunicationManager::~CommunicationManager()
{
    if(pingCurl) {
        curl_easy_cleanup(pingCurl);
    }

	if(executorThread->running() == true){    
        requestsExecutor->finish();
        executorThread->join();
    }
}

void CommunicationManager::setMedicalFtps(const webservices::FtpsConnectionPtr & medicalFtps)
{
	utils::ScopedLock<utils::RecursiveSyncPolicy> lock(requestsMutex);
	medicalFtps_ = medicalFtps;
	medicalShallowDownloadHelper.configure(medicalFileStoremanService_, medicalFtps_);
	photoDownloadHelper.configure(medicalFileStoremanService_, medicalFtps_);
}

void CommunicationManager::setMotionFtps(const webservices::FtpsConnectionPtr & motionFtps)
{
	utils::ScopedLock<utils::RecursiveSyncPolicy> lock(requestsMutex);
	motionFtps_ = motionFtps;
	motionShallowDownloadHelper.configure(motionFileStoremanService_, motionFtps);
	fileDownloadHelper.configure(motionFileStoremanService_, motionFtps);
    fileUploadHelper.configure(motionFileStoremanService_, motionFtps_);
}

void CommunicationManager::setMotionFileStoremanService(const webservices::MotionFileStoremanWSPtr & motionFileStoremanService)
{
	utils::ScopedLock<utils::RecursiveSyncPolicy> lock(requestsMutex);
	motionFileStoremanService_ = motionFileStoremanService;
	fileDownloadHelper.configure(motionFileStoremanService_, motionFtps_);
    //fileUploadHelper.configure(motionFileStoremanService_, motionFtps_);
	motionShallowDownloadHelper.configure(motionFileStoremanService_, motionFtps_);
}

void CommunicationManager::setMedicalFileStoremanService(const webservices::MedicalFileStoremanWSPtr & medicalFileStoremanService)
{
	utils::ScopedLock<utils::RecursiveSyncPolicy> lock(requestsMutex);
	medicalFileStoremanService_ = medicalFileStoremanService;
	photoDownloadHelper.configure(medicalFileStoremanService_, medicalFtps_);
	medicalShallowDownloadHelper.configure(medicalFileStoremanService_, medicalFtps_);
}

const webservices::FtpsConnectionPtr & CommunicationManager::medicalFtps()
{
	return medicalFtps_;
}

const webservices::FtpsConnectionPtr & CommunicationManager::motionFtps()
{
	return motionFtps_;
}

void CommunicationManager::popRequest(CompleteRequest & reuest)
{
	utils::ScopedLock<utils::RecursiveSyncPolicy> lock(requestsMutex);
	reuest = requestsQueue.front();
	requestsQueue.pop();
}

void CommunicationManager::pushRequest(const CompleteRequest & request)
{
	utils::ScopedLock<utils::RecursiveSyncPolicy> lock(requestsMutex);
	requestsQueue.push(request);

	if(executorThread->running() == false){
		executorThread->start(requestsExecutor);
	}
}

void CommunicationManager::setCurrentDownloadHelper(webservices::IDownloadHelper * downloadHelper)
{
	utils::ScopedLock<utils::RecursiveSyncPolicy> lock(downloadHelperMutex);
	currentDownloadHelper = downloadHelper;
}

bool CommunicationManager::requestsQueueEmpty() const
{
	utils::ScopedLock<utils::RecursiveSyncPolicy> lock(requestsMutex);
	return requestsQueue.empty();
}

void CommunicationManager::cancelAllPendingRequests()
{
	utils::ScopedLock<utils::RecursiveSyncPolicy> lock(requestsMutex);
	while(requestsQueue.empty() == false)
	{
		if(requestsQueue.front().callbacks.onCancelCallback.empty() == false){
			requestsQueue.front().callbacks.onCancelCallback(requestsQueue.front().request);
		}

		requestsQueue.pop();
	}
}


int CommunicationManager::getProgress() const
{
    utils::ScopedLock<utils::RecursiveSyncPolicy> lock(downloadHelperMutex);
    return (currentDownloadHelper == nullptr || currentRequest.request == nullptr) ? 0 : currentRequest.request->getProgress();
}

void CommunicationManager::setState(State state)
{
	utils::ScopedLock<utils::RecursiveSyncPolicy> lock(trialsMutex);
	this->state = state;    
}

CommunicationManager::State CommunicationManager::getState()
{
	utils::ScopedLock<utils::RecursiveSyncPolicy> lock(trialsMutex);
	return state;
}

void CommunicationManager::cancelRequest(const BasicRequestPtr & request)
{
    request->cancel();
    if(currentRequest.request == request){
        if(currentDownloadHelper != nullptr){
            currentDownloadHelper->abort();
        }
    }
}

webservices::IFtpsConnection::OperationStatus CommunicationManager::processComplex(const CompleteRequest & request, std::string & message)
{
    webservices::IFtpsConnection::OperationStatus ret = webservices::IFtpsConnection::Complete;

    if(request.callbacks.onBeginCallback.empty() == false){
        request.callbacks.onBeginCallback(request.request);
    }

    utils::shared_ptr<ComplexRequest> complexRequest = utils::dynamic_pointer_cast<ComplexRequest>(request.request);
    
    unsigned int s = complexRequest->size();
    unsigned int i = 0;

    for( ; i < s; i++){
        
        if(request.request->isCancelled()){
            ret = webservices::IFtpsConnection::Cancelled;
        }

        //sprawdz czy anulowano lub blad
        //jeśli nie to kontynuj
        if(ret != webservices::IFtpsConnection::Complete){
            break;
        }
  
        const auto & r = complexRequest->getRequest(i);

        switch(r.request->type){

            case Complex:
                ret = processComplex(r, message);
                break;

            case DownloadFile:
                ret = processFile(r, message);
                break;

            case UploadFile:
                ret = processUpload(r, message);
                break;

            case ReplaceFile:
                ret = processReplace(r, message);
                break;

            case DownloadPhoto:
                ret = processPhoto(r, message);
                break;

            case CopyMotionShallowCopy:
                ret = processMotionShallowCopy(r, message);
                break;

            case CopyMotionBranchIncrement:
                ret = processMotionBranchIncrementShallowCopy(r, message);
                break;

            case CopyMotionMetadata:
                ret = processMotionMetadata(r, message);
                break;

            case CopyMedicalShallowCopy:
                ret = processMedicalShallowCopy(r, message);
                break;

            case CopyMedicalMetadata:
                ret = processMedicalMetadata(r, message);
                break;

            case PingServer:
                ret = processPing(r, message);
                break;
        }
    }

    switch(ret){
    case webservices::IFtpsConnection::Error:
        
        if(request.callbacks.onErrorCallback.empty() == false){
            request.callbacks.onErrorCallback(request.request, message);
        }

        break;

    case webservices::IFtpsConnection::Complete:

        if(request.callbacks.onEndCallback.empty() == false){
            request.callbacks.onEndCallback(request.request);
        }

        break;

    case webservices::IFtpsConnection::Cancelled:

        if(request.callbacks.onCancelCallback.empty() == false){
            request.callbacks.onCancelCallback(request.request);
        }

        break;

    }

    return ret;
}

webservices::IFtpsConnection::OperationStatus CommunicationManager::processPhoto(const CompleteRequest & request, std::string & message)
{
    webservices::IFtpsConnection::OperationStatus ret = webservices::IFtpsConnection::Complete;

    if(request.callbacks.onBeginCallback.empty() == false){
        request.callbacks.onBeginCallback(request.request);
    }

    try {

        utils::shared_ptr<PhotoRequest> photoRequest = utils::dynamic_pointer_cast<PhotoRequest>(request.request);
        
		setCurrentDownloadHelper(&photoDownloadHelper);

		photoDownloadHelper.setDownload(photoRequest->getPhotoID(), photoRequest->getFilePath());
		
		{
			utils::ScopedLock<utils::RecursiveSyncPolicy> lock(requestsMutex);
			ret = photoDownloadHelper.get(photoRequest.get());
		}

        //ret = medicalTransportManager->downloadPhoto(photoRequest->getPhotoID(), photoRequest->getFilePath(), photoRequest.get());

        switch(ret){

        case webservices::IFtpsConnection::Complete:
            if(request.callbacks.onEndCallback.empty() == false){
                request.callbacks.onEndCallback(request.request);
            }

            break;

        case webservices::IFtpsConnection::Cancelled:
            if(request.callbacks.onCancelCallback.empty() == false){
                request.callbacks.onCancelCallback(request.request);
            }

            break;

        case webservices::IFtpsConnection::Error:

            message += photoDownloadHelper.errorMessage();

            if(request.callbacks.onErrorCallback.empty() == false){
                request.callbacks.onErrorCallback(request.request, message);
            }

        }
    } catch(std::exception& e) {

        message += e.what();

        if(request.callbacks.onErrorCallback.empty() == false){
            request.callbacks.onErrorCallback(request.request, message);
        }
    }

	if(ret == webservices::IFtpsConnection::Error){
		serverResponse = false;
	}else{
		serverResponse = true;
	}

    return ret;
}

webservices::IFtpsConnection::OperationStatus CommunicationManager::processFile(const CompleteRequest & request, std::string & message)
{
    webservices::IFtpsConnection::OperationStatus ret = webservices::IFtpsConnection::Complete;

    if(request.callbacks.onBeginCallback.empty() == false){
        request.callbacks.onBeginCallback(request.request);
    }

    try {

        utils::shared_ptr<FileRequest> fileRequest = utils::dynamic_pointer_cast<FileRequest>(request.request);
        setCurrentDownloadHelper(&fileDownloadHelper);

		fileDownloadHelper.setDownload(fileRequest->fileID, fileRequest->filePath);
		
		{
			utils::ScopedLock<utils::RecursiveSyncPolicy> lock(requestsMutex);
			ret = fileDownloadHelper.get(fileRequest.get());
		}

        //ret = motionTransportManager->downloadFile(fileRequest->fileID, fileRequest->filePath, fileRequest.get());

        switch(ret){

        case webservices::IFtpsConnection::Complete:
            if(request.callbacks.onEndCallback.empty() == false){
                request.callbacks.onEndCallback(request.request);
            }

            break;

        case webservices::IFtpsConnection::Cancelled:
            if(request.callbacks.onCancelCallback.empty() == false){
                request.callbacks.onCancelCallback(request.request);
            }

            break;

        case webservices::IFtpsConnection::Error:

            message += fileDownloadHelper.errorMessage();

            if(request.callbacks.onErrorCallback.empty() == false){
                request.callbacks.onErrorCallback(request.request, message);
            }

        }
    } catch(std::exception& e) {

        message += e.what();

        if(request.callbacks.onErrorCallback.empty() == false){
            request.callbacks.onErrorCallback(request.request, message);
        }
    }

	if(ret == webservices::IFtpsConnection::Error){
		serverResponse = false;
	}else{
		serverResponse = true;
	}

    return ret;
}


webservices::IFtpsConnection::OperationStatus CommunicationManager::processUpload(const CompleteRequest & request, std::string & message)
{
    webservices::IFtpsConnection::OperationStatus ret = webservices::IFtpsConnection::Complete;

    if(request.callbacks.onBeginCallback.empty() == false){
        request.callbacks.onBeginCallback(request.request);
    }

    try {

        utils::shared_ptr<UploadRequest> uploadRequest = utils::dynamic_pointer_cast<UploadRequest>(request.request);
        
        fileUploadHelper.setFileUpload(uploadRequest->sourcePath, uploadRequest->filePath);

        {
            utils::ScopedLock<utils::RecursiveSyncPolicy> lock(requestsMutex);
            ret = fileUploadHelper.put(uploadRequest.get());
            //if (ret == webservices::IFtpsConnection::Complete) {
                motionFileStoremanService_->storeTrialFile(uploadRequest->trialID,  "/BDR/w", "test xml file", uploadRequest->getFileName());
            //}
        }

        //ret = motionTransportManager->downloadFile(fileRequest->fileID, fileRequest->filePath, fileRequest.get());

        switch(ret){

        case webservices::IFtpsConnection::Complete:
            if(request.callbacks.onEndCallback.empty() == false){
                request.callbacks.onEndCallback(request.request);
            }

            break;

        case webservices::IFtpsConnection::Cancelled:
            if(request.callbacks.onCancelCallback.empty() == false){
                request.callbacks.onCancelCallback(request.request);
            }

            break;

        case webservices::IFtpsConnection::Error:

            message += fileUploadHelper.errorMessage();

            if(request.callbacks.onErrorCallback.empty() == false){
                request.callbacks.onErrorCallback(request.request, message);
            }

        }
    } catch(std::exception& e) {

        message += e.what();

        if(request.callbacks.onErrorCallback.empty() == false){
            request.callbacks.onErrorCallback(request.request, message);
        }
    }

    if(ret == webservices::IFtpsConnection::Error){
        serverResponse = false;
    }else{
        serverResponse = true;
    }

    return ret;
}

webservices::IFtpsConnection::OperationStatus CommunicationManager::processReplace(const CompleteRequest & request, std::string & message)
{
    webservices::IFtpsConnection::OperationStatus ret = webservices::IFtpsConnection::Complete;

    if(request.callbacks.onBeginCallback.empty() == false){
        request.callbacks.onBeginCallback(request.request);
    }

    try {

        utils::shared_ptr<ReplaceRequest> replaceRequest = utils::dynamic_pointer_cast<ReplaceRequest>(request.request);

        fileUploadHelper.setFileUpload(replaceRequest->sourcePath, replaceRequest->filePath);

        {
            utils::ScopedLock<utils::RecursiveSyncPolicy> lock(requestsMutex);
            ret = fileUploadHelper.put(replaceRequest.get());
            //if (ret == webservices::IFtpsConnection::Complete) {
            motionFileStoremanService_->replaceFile(replaceRequest->fileID,  "/BDR/w", replaceRequest->getFileName());
            //}
        }

        //ret = motionTransportManager->downloadFile(fileRequest->fileID, fileRequest->filePath, fileRequest.get());

        switch(ret){

        case webservices::IFtpsConnection::Complete:
            if(request.callbacks.onEndCallback.empty() == false){
                request.callbacks.onEndCallback(request.request);
            }

            break;

        case webservices::IFtpsConnection::Cancelled:
            if(request.callbacks.onCancelCallback.empty() == false){
                request.callbacks.onCancelCallback(request.request);
            }

            break;

        case webservices::IFtpsConnection::Error:

            message += fileUploadHelper.errorMessage();

            if(request.callbacks.onErrorCallback.empty() == false){
                request.callbacks.onErrorCallback(request.request, message);
            }

        }
    } catch(std::exception& e) {

        message += e.what();

        if(request.callbacks.onErrorCallback.empty() == false){
            request.callbacks.onErrorCallback(request.request, message);
        }
    }

    if(ret == webservices::IFtpsConnection::Error){
        serverResponse = false;
    }else{
        serverResponse = true;
    }

    return ret;
}


webservices::IFtpsConnection::OperationStatus CommunicationManager::processMotionShallowCopy(const CompleteRequest & request, std::string & message)
{
    webservices::IFtpsConnection::OperationStatus ret = webservices::IFtpsConnection::Complete;

    if(request.callbacks.onBeginCallback.empty() == false){
        request.callbacks.onBeginCallback(request.request);
    }

    try {
        utils::shared_ptr<MetadataRequest> metaRequest = utils::dynamic_pointer_cast<MetadataRequest>(request.request);
        setCurrentDownloadHelper(&motionShallowDownloadHelper);
        
		motionShallowDownloadHelper.setDownload(webservices::ShallowDownloadHelper::ShallowData, metaRequest->filePath);
		
		{
			utils::ScopedLock<utils::RecursiveSyncPolicy> lock(requestsMutex);
			ret = motionShallowDownloadHelper.get(metaRequest.get());
		}


        //ret = motionTransportManager->getShallowCopy(metaRequest->filePath, metaRequest.get());

        switch(ret){

        case webservices::IFtpsConnection::Complete:
            if(request.callbacks.onEndCallback.empty() == false){
                request.callbacks.onEndCallback(request.request);
            }

            break;

        case webservices::IFtpsConnection::Cancelled:
            if(request.callbacks.onCancelCallback.empty() == false){
                request.callbacks.onCancelCallback(request.request);
            }

            break;

        case webservices::IFtpsConnection::Error:

            message += motionShallowDownloadHelper.errorMessage();

            if(request.callbacks.onErrorCallback.empty() == false){
                request.callbacks.onErrorCallback(request.request, message);
            }
        }
    } catch(std::exception& e) {

        message += e.what();

        if(request.callbacks.onErrorCallback.empty() == false){
            request.callbacks.onErrorCallback(request.request, message);
        }
    }

	if(ret == webservices::IFtpsConnection::Error){
		serverResponse = false;
	}else{
		serverResponse = true;
	}

    return ret;
}

webservices::IFtpsConnection::OperationStatus CommunicationManager::processMotionMetadata(const CompleteRequest & request, std::string & message)
{
    webservices::IFtpsConnection::OperationStatus ret = webservices::IFtpsConnection::Complete;

    if(request.callbacks.onBeginCallback.empty() == false){
        request.callbacks.onBeginCallback(request.request);
    }

    try {
        utils::shared_ptr<MetadataRequest> metaRequest = utils::dynamic_pointer_cast<MetadataRequest>(request.request);
		setCurrentDownloadHelper(&motionShallowDownloadHelper);

		motionShallowDownloadHelper.setDownload(webservices::ShallowDownloadHelper::ShallowMetadata, metaRequest->filePath);
		
		{
			utils::ScopedLock<utils::RecursiveSyncPolicy> lock(requestsMutex);
			ret = motionShallowDownloadHelper.get(metaRequest.get());
		}

        switch(ret){

        case webservices::IFtpsConnection::Complete:
            if(request.callbacks.onEndCallback.empty() == false){
                request.callbacks.onEndCallback(request.request);
            }

            break;

        case webservices::IFtpsConnection::Cancelled:
            if(request.callbacks.onCancelCallback.empty() == false){
                request.callbacks.onCancelCallback(request.request);
            }

            break;

        case webservices::IFtpsConnection::Error:

            message += motionShallowDownloadHelper.errorMessage();

            if(request.callbacks.onErrorCallback.empty() == false){
                request.callbacks.onErrorCallback(request.request, message);
            }

        }
    } catch(std::exception& e) {

        message += e.what();

        if(request.callbacks.onErrorCallback.empty() == false){
            request.callbacks.onErrorCallback(request.request, message);
        }
    }

	if(ret == webservices::IFtpsConnection::Error){
		serverResponse = false;
	}else{
		serverResponse = true;
	}

    return ret;
}

webservices::IFtpsConnection::OperationStatus CommunicationManager::processMedicalShallowCopy(const CompleteRequest & request, std::string & message)
{
    webservices::IFtpsConnection::OperationStatus ret = webservices::IFtpsConnection::Complete;

    if(request.callbacks.onBeginCallback.empty() == false){
        request.callbacks.onBeginCallback(request.request);
    }

    try {
        utils::shared_ptr<MetadataRequest> metaRequest = utils::dynamic_pointer_cast<MetadataRequest>(request.request);
		setCurrentDownloadHelper(&medicalShallowDownloadHelper);

		medicalShallowDownloadHelper.setDownload(webservices::ShallowDownloadHelper::ShallowData, metaRequest->filePath);
		
		{
			utils::ScopedLock<utils::RecursiveSyncPolicy> lock(requestsMutex);
			ret = medicalShallowDownloadHelper.get(metaRequest.get());
		}

        switch(ret){

        case webservices::IFtpsConnection::Complete:
            if(request.callbacks.onEndCallback.empty() == false){
                request.callbacks.onEndCallback(request.request);
            }

            break;

        case webservices::IFtpsConnection::Cancelled:
            if(request.callbacks.onCancelCallback.empty() == false){
                request.callbacks.onCancelCallback(request.request);
            }

            break;

        case webservices::IFtpsConnection::Error:

            message += medicalShallowDownloadHelper.errorMessage();

            if(request.callbacks.onErrorCallback.empty() == false){
                request.callbacks.onErrorCallback(request.request, message);
            }
        }
    } catch(std::exception& e) {

        message += e.what();

        if(request.callbacks.onErrorCallback.empty() == false){
            request.callbacks.onErrorCallback(request.request, message);
        }
    }

	if(ret == webservices::IFtpsConnection::Error){
		serverResponse = false;
	}else{
		serverResponse = true;
	}

    return ret;
}

webservices::IFtpsConnection::OperationStatus CommunicationManager::processMedicalMetadata(const CompleteRequest & request, std::string & message)
{
    webservices::IFtpsConnection::OperationStatus ret = webservices::IFtpsConnection::Complete;

    if(request.callbacks.onBeginCallback.empty() == false){
        request.callbacks.onBeginCallback(request.request);
    }

    try {
        utils::shared_ptr<MetadataRequest> metaRequest = utils::dynamic_pointer_cast<MetadataRequest>(request.request);
		setCurrentDownloadHelper(&medicalShallowDownloadHelper);

		medicalShallowDownloadHelper.setDownload(webservices::ShallowDownloadHelper::ShallowMetadata, metaRequest->filePath);
		
		{
			utils::ScopedLock<utils::RecursiveSyncPolicy> lock(requestsMutex);
			ret = medicalShallowDownloadHelper.get(metaRequest.get());
		}

        switch(ret){

        case webservices::IFtpsConnection::Complete:
            if(request.callbacks.onEndCallback.empty() == false){
                request.callbacks.onEndCallback(request.request);
            }

            break;

        case webservices::IFtpsConnection::Cancelled:
            if(request.callbacks.onCancelCallback.empty() == false){
                request.callbacks.onCancelCallback(request.request);
            }

            break;

        case webservices::IFtpsConnection::Error:

            message += medicalShallowDownloadHelper.errorMessage();

            if(request.callbacks.onErrorCallback.empty() == false){
                request.callbacks.onErrorCallback(request.request, message);
            }
        }
    } catch(std::exception& e) {

        message += e.what();

        if(request.callbacks.onErrorCallback.empty() == false){
            request.callbacks.onErrorCallback(request.request, message);
        }
    }

	if(ret == webservices::IFtpsConnection::Error){
		serverResponse = false;
	}else{
		serverResponse = true;
	}

    return ret;
}

webservices::IFtpsConnection::OperationStatus CommunicationManager::processPing(const CompleteRequest & request, std::string & message)
{
    webservices::IFtpsConnection::OperationStatus ret = webservices::IFtpsConnection::Complete;

    setCurrentDownloadHelper(nullptr);
    
    if(request.callbacks.onBeginCallback.empty() == false){
        request.callbacks.onBeginCallback(request.request);
    }

    try{

		utils::shared_ptr<PingRequest> pingRequest = utils::dynamic_pointer_cast<PingRequest>(request.request);
		CURLcode pingCurlResult;
		{
			curl_easy_setopt(pingCurl, CURLOPT_URL, pingRequest->urlToPing().c_str());
			pingCurlResult = curl_easy_perform(pingCurl);
		}

        if(pingCurlResult == CURLE_OK) {
            pingRequest->setServerResponse(true);
        } else {
            pingRequest->setServerResponse(false);
        }

        if(request.callbacks.onEndCallback.empty() == false){
            request.callbacks.onEndCallback(request.request);
        }

    }catch(const std::exception & e){
        if(request.callbacks.onErrorCallback.empty() == false){
            request.callbacks.onErrorCallback(request.request, e.what());
        }

        ret = webservices::IFtpsConnection::Error;
    }

    return ret;
}

webservices::IFtpsConnection::OperationStatus CommunicationManager::processMotionBranchIncrementShallowCopy(const CompleteRequest & request, std::string & message)
{
    webservices::IFtpsConnection::OperationStatus ret = webservices::IFtpsConnection::Complete;

    if(request.callbacks.onBeginCallback.empty() == false){
        request.callbacks.onBeginCallback(request.request);
    }

    try {
        utils::shared_ptr<MetadataRequest> metaRequest = utils::dynamic_pointer_cast<MetadataRequest>(request.request);
        setCurrentDownloadHelper(&motionShallowDownloadHelper);

        motionShallowDownloadHelper.setIncrementalDownload(metaRequest->filePath, metaRequest->getDateTime());

        {
            utils::ScopedLock<utils::RecursiveSyncPolicy> lock(requestsMutex);
            ret = motionShallowDownloadHelper.get(metaRequest.get());
        }


        //ret = motionTransportManager->getShallowCopy(metaRequest->filePath, metaRequest.get());

        switch(ret){

        case webservices::IFtpsConnection::Complete:
            if(request.callbacks.onEndCallback.empty() == false){
                request.callbacks.onEndCallback(request.request);
            }

            break;

        case webservices::IFtpsConnection::Cancelled:
            if(request.callbacks.onCancelCallback.empty() == false){
                request.callbacks.onCancelCallback(request.request);
            }

            break;

        case webservices::IFtpsConnection::Error:

            message += motionShallowDownloadHelper.errorMessage();

            if(request.callbacks.onErrorCallback.empty() == false){
                request.callbacks.onErrorCallback(request.request, message);
            }
        }
    } catch(std::exception& e) {

        message += e.what();

        if(request.callbacks.onErrorCallback.empty() == false){
            request.callbacks.onErrorCallback(request.request, message);
        }
    }

    if(ret == webservices::IFtpsConnection::Error){
        serverResponse = false;
    }else{
        serverResponse = true;
    }

    return ret;
}


size_t CommunicationManager::pingDataCallback(void *buffer, size_t size, size_t nmemb, void *stream)
{
    return size*nmemb;
}

CommunicationManager::ComplexRequestPtr CommunicationManager::createRequestComplex(const std::vector<CompleteRequest> & requests)
{
    return ComplexRequestPtr(new ComplexRequest(requests));
}

CommunicationManager::FileRequestPtr CommunicationManager::createRequestFile(unsigned int fileID, const std::string & filePath)
{
    return FileRequestPtr(new FileRequest(filePath, fileID));
}


CommunicationManager::UploadRequestPtr CommunicationManager::createRequestUpload( const std::string & sourcePath, const std::string & filePath, unsigned int trialID )
{
    return UploadRequestPtr(new UploadRequest(sourcePath, filePath, trialID));
}


CommunicationManager::PhotoRequestPtr CommunicationManager::createRequestPhoto(unsigned int fileID, const std::string & filePath)
{
    return PhotoRequestPtr(new PhotoRequest(filePath, fileID));
}

CommunicationManager::MetadataRequestPtr CommunicationManager::createRequestMotionShallowCopy(const std::string & filePath)
{
    return MetadataRequestPtr(new MetadataRequest(CopyMotionShallowCopy, filePath));
}

CommunicationManager::MetadataRequestPtr CommunicationManager::createRequestMotionMetadata(const std::string & filePath)
{
    return MetadataRequestPtr(new MetadataRequest(CopyMotionMetadata, filePath));
}

CommunicationManager::MetadataRequestPtr CommunicationManager::createRequestMedicalShallowCopy(const std::string & filePath)
{
    return MetadataRequestPtr(new MetadataRequest(CopyMedicalShallowCopy, filePath));
}

CommunicationManager::MetadataRequestPtr CommunicationManager::createRequestMedicalMetadata(const std::string & filePath)
{
    return MetadataRequestPtr(new MetadataRequest(CopyMedicalMetadata, filePath));    
}

CommunicationManager::PingRequestPtr CommunicationManager::createRequestPing(const std::string & urlToPing)
{
    return PingRequestPtr(new PingRequest(urlToPing));
}

CommunicationManager::ReplaceRequestPtr CommunicationManager::createRequestReplace( const std::string & sourcePath, const std::string & filePath, unsigned int fileID )
{
    return ReplaceRequestPtr(new ReplaceRequest(fileID, sourcePath, filePath));
}

CommunicationManager::MetadataRequestPtr CommunicationManager::createRequestMotionShallowBranchIncrement( const std::string& path, const webservices::DateTime& since )
{
    return MetadataRequestPtr(new MetadataRequest(CopyMotionBranchIncrement, path, since));
}

