/**
@author Marek Daniluk
*/

#include "CommunicationPCH.h"
#include <core/Filesystem.h>
#include <plugins/newCommunication/CommunicationManager.h>

CommunicationManager* CommunicationManager::instance = nullptr;


CommunicationManager::BasicRequest::~BasicRequest()
{

}

void CommunicationManager::BasicRequest::cancel()
{
	OpenThreads::ScopedLock<OpenThreads::Mutex> lock(cancelLock);
	canceled = true;
}

bool CommunicationManager::BasicRequest::isCancelled() const
{
	OpenThreads::ScopedLock<OpenThreads::Mutex> lock(cancelLock);
	return (isComplete() == false) && canceled;
}

CommunicationManager::Request CommunicationManager::BasicRequest::getType() const
{
	return type;
}

double CommunicationManager::BasicRequest::getProgress() const
{
	return 0;
}

bool CommunicationManager::BasicRequest::isComplete() const
{
	return getProgress() == 100.0;
}

CommunicationManager::BasicRequest::BasicRequest(Request type) : type(type), canceled(false)
{

}

CommunicationManager::MetadataRequest::MetadataRequest(Request type, const std::string & filePath) : BasicRequest(type), filePath(filePath), progress(0)
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

CommunicationManager::FileRequest::FileRequest(const std::string & filePath, unsigned int fileID) : MetadataRequest(DownloadFile, filePath), fileID(fileID)
{

}

unsigned int CommunicationManager::FileRequest::getFileID() const
{
	return fileID;
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

	for(auto it = requests.begin(); it != requests.end(); it++){
		double loc = (*it).request->getProgress();
		if(loc == 0){
			break;
		}

		ret += loc;
	}

	return 100.0 * (ret / (100 * requests.size()));
}

CommunicationManager* CommunicationManager::getInstance()
{
    if(!instance) {
        instance = new CommunicationManager();
    }
    return instance;
}

void CommunicationManager::destoryInstance()
{
    if(instance) {
        delete instance;
    }
    instance = nullptr;
}

CommunicationManager::CommunicationManager()
	: finish(false), state(Ready), pingCurl(nullptr)
{
	pingCurl = curl_easy_init();
	if(pingCurl) {
		curl_easy_setopt(pingCurl, CURLOPT_URL, "127.0.0.1");
		curl_easy_setopt(pingCurl, CURLOPT_CONNECTTIMEOUT, 1);
		curl_easy_setopt(pingCurl, CURLOPT_WRITEFUNCTION, pingDataCallback);
	}else{
		throw std::runtime_error("Error while initializing CURL");
	}
}

CommunicationManager::~CommunicationManager()
{
    if(pingCurl) {
        curl_easy_cleanup(pingCurl);
    }
    if(isRunning()) {
        finish = true;
        join();
    }
}

void CommunicationManager::init()
{
    ScopedLock lock(requestsMutex);

    
    //TODO: dane wpisane na sztywno, dodac zapisywanie ustawien
    /*motionTransportManager->setFTPCredentials("ftps://v21.pjwstk.edu.pl/", "testUser", "testUser");
    motionTransportManager->setWSCredentials("http://v21.pjwstk.edu.pl/Motion/FileStoremanWS.svc?wsdl", "applet_user", "aplet4Motion");

    medicalTransportManager->setFTPCredentials("ftps://v21.pjwstk.edu.pl/", "testUser", "testUser");
    medicalTransportManager->setWSCredentials("http://v21.pjwstk.edu.pl/MotionMed/FileStoremanWS.svc?wsdl", "applet_user", "aplet4Motion");

    motionQueryManager->setWSCredentials("applet_user", "aplet4Motion");
    motionQueryManager->setBasicQueriesServiceUri("http://v21.pjwstk.edu.pl/Motion/res/BasicQueriesWSStandalone.wsdl");
    motionQueryManager->setBasicUpdatesServiceUri("");*/

    /*medicalQueryManager->setWSCredentials("applet_user", "aplet4Motion");
    medicalQueryManager->setBasicQueriesServiceUri("http://v21.pjwstk.edu.pl/Motion/res/BasicQueriesWSStandalone.wsdl");
    medicalQueryManager->setBasicUpdatesServiceUri("");*/
    
    
    /*webservices::SecureWsdlConnection conn;
    conn.setCredentials("https://v21.pjwstk.edu.pl/SecureSample/Service1.svc?wsdl", "hml", "hml");
    conn.setSecurity((core::getPathInterface()->getResourcesPath() / "pjwstk-hm.crt").string(), WsdlPull::CustomSSLWsdlInvoker::HVNone);
    conn.setOperation("CallMe");
    conn.setValue("value", "1");
    try{
        conn.setSecurity((core::getPathInterface()->getResourcesPath() / "pjwstk-hm.crt").string(), WsdlPull::CustomSSLWsdlInvoker::HVNone);
        conn.invokeOperation();
    
    }
    catch(const std::exception & e){
        LOG_DEBUG(e.what());
    }
    catch(...){

    }
    
    auto resp = conn.getXMLResponse();
    LOG_DEBUG("HTTPS Communication:");
    LOG_DEBUG(resp);*/
}

void CommunicationManager::deinit()
{
    
}

void CommunicationManager::setUrlToPingServer(const std::string & urlToPing)
{
	ScopedLock lock(curlMutex);
	curl_easy_setopt(pingCurl, CURLOPT_URL, urlToPing.c_str());
}

void CommunicationManager::setMedicalFtps(const webservices::FtpsConnectionPtr & medicalFtps)
{
	ScopedLock lock(requestsMutex);
	medicalFtps_ = medicalFtps;
	medicalShallowDownloadHelper.configure(medicalFileStoremanService_, medicalFtps_);
	photoDownloadHelper.configure(medicalFileStoremanService_, medicalFtps_);
}

void CommunicationManager::setMotionFtps(const webservices::FtpsConnectionPtr & motionFtps)
{
	ScopedLock lock(requestsMutex);
	motionFtps_ = motionFtps;
	motionShallowDownloadHelper.configure(motionFileStoremanService_, motionFtps);
	fileDownloadHelper.configure(motionFileStoremanService_, motionFtps);
}

void CommunicationManager::setMotionFileStoremanService(const webservices::MotionFileStoremanWSPtr & motionFileStoremanService)
{
	ScopedLock lock(requestsMutex);
	motionFileStoremanService_ = motionFileStoremanService;
	fileDownloadHelper.configure(motionFileStoremanService_, motionFtps_);
	motionShallowDownloadHelper.configure(motionFileStoremanService_, motionFtps_);
}

void CommunicationManager::setMedicalFileStoremanService(const webservices::MedicalFileStoremanWSPtr & medicalFileStoremanService)
{
	ScopedLock lock(requestsMutex);
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
	ScopedLock lock(requestsMutex);
	reuest = requestsQueue.front();
	requestsQueue.pop();
}

void CommunicationManager::pushRequest(const CompleteRequest & request)
{
	ScopedLock lock(requestsMutex);
	requestsQueue.push(request);

	if(isRunning() == false){
		start();
	}
}

void CommunicationManager::setCurrentDownloadHelper(webservices::IDownloadHelper * downloadHelper)
{
	ScopedLock lock(downloadHelperMutex);
	currentDownloadHelper = downloadHelper;
}

bool CommunicationManager::requestsQueueEmpty() const
{
	ScopedLock lock(requestsMutex);
	return requestsQueue.empty();
}

void CommunicationManager::cancelAllPendingRequests()
{
	ScopedLock lock(requestsMutex);
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
    ScopedLock lock(downloadHelperMutex);
    return (currentDownloadHelper == nullptr || currentRequest.request == nullptr) ? 0 : currentRequest.request->getProgress();
}

void CommunicationManager::setState(State state)
{
	ScopedLock lock(trialsMutex);
	this->state = state;    
}

CommunicationManager::State CommunicationManager::getState()
{
	ScopedLock lock(trialsMutex);
	return state;
}

bool CommunicationManager::isServerResponse() const
{
	return serverResponse;
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

    core::shared_ptr<ComplexRequest> complexRequest = core::dynamic_pointer_cast<ComplexRequest>(request.request);
    
    unsigned int s = complexRequest->size();
    unsigned int i = 0;

    for( ; i < s; i++){
        
        if(request.request->isCancelled()){
            ret = webservices::IFtpsConnection::Cancelled;
        }

        //sprawdz czy anulowano lub blad
        //jesli nie to kontynuj
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

            case DownloadPhoto:
                ret = processPhoto(r, message);
                break;

            case CopyMotionShallowCopy:
                ret = processMotionShallowCopy(r, message);
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

        core::shared_ptr<PhotoRequest> photoRequest = core::dynamic_pointer_cast<PhotoRequest>(request.request);
        
		setCurrentDownloadHelper(&photoDownloadHelper);

		photoDownloadHelper.setDownload(photoRequest->getPhotoID(), photoRequest->getFilePath());
		
		{
			ScopedLock lock(requestsMutex);
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

        core::shared_ptr<FileRequest> fileRequest = core::dynamic_pointer_cast<FileRequest>(request.request);
        setCurrentDownloadHelper(&fileDownloadHelper);

		fileDownloadHelper.setDownload(fileRequest->fileID, fileRequest->filePath);
		
		{
			ScopedLock lock(requestsMutex);
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

webservices::IFtpsConnection::OperationStatus CommunicationManager::processMotionShallowCopy(const CompleteRequest & request, std::string & message)
{
    webservices::IFtpsConnection::OperationStatus ret = webservices::IFtpsConnection::Complete;

    if(request.callbacks.onBeginCallback.empty() == false){
        request.callbacks.onBeginCallback(request.request);
    }

    try {
        core::shared_ptr<MetadataRequest> metaRequest = core::dynamic_pointer_cast<MetadataRequest>(request.request);
        setCurrentDownloadHelper(&motionShallowDownloadHelper);
        
		motionShallowDownloadHelper.setDownload(webservices::ShallowDownloadHelper::ShallowData, metaRequest->filePath);
		
		{
			ScopedLock lock(requestsMutex);
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
        core::shared_ptr<MetadataRequest> metaRequest = core::dynamic_pointer_cast<MetadataRequest>(request.request);
		setCurrentDownloadHelper(&motionShallowDownloadHelper);

		motionShallowDownloadHelper.setDownload(webservices::ShallowDownloadHelper::ShallowMetadata, metaRequest->filePath);
		
		{
			ScopedLock lock(requestsMutex);
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
        core::shared_ptr<MetadataRequest> metaRequest = core::dynamic_pointer_cast<MetadataRequest>(request.request);
		setCurrentDownloadHelper(&medicalShallowDownloadHelper);

		medicalShallowDownloadHelper.setDownload(webservices::ShallowDownloadHelper::ShallowData, metaRequest->filePath);
		
		{
			ScopedLock lock(requestsMutex);
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
        core::shared_ptr<MetadataRequest> metaRequest = core::dynamic_pointer_cast<MetadataRequest>(request.request);
		setCurrentDownloadHelper(&medicalShallowDownloadHelper);

		medicalShallowDownloadHelper.setDownload(webservices::ShallowDownloadHelper::ShallowMetadata, metaRequest->filePath);
		
		{
			ScopedLock lock(requestsMutex);
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

		{
			ScopedLock lock(curlMutex);
			pingCurlResult = curl_easy_perform(pingCurl);
		}

        if(pingCurlResult == CURLE_OK) {
            serverResponse = true;
        } else {
            serverResponse = false;
        }

        if(request.callbacks.onEndCallback.empty() == false){
            request.callbacks.onEndCallback(request.request);
        }

    }catch(const std::exception & e){
        serverResponse = false;
        if(request.callbacks.onErrorCallback.empty() == false){
            request.callbacks.onErrorCallback(request.request, e.what());
        }

        ret = webservices::IFtpsConnection::Error;
    }

    return ret;
}

void CommunicationManager::run()
{
    //inicjujemy po³¹czenia dla us³ugi plików i zapytañ
    init();

    while(finish == false){

        if(requestsQueueEmpty() == false){

            cancelDownloading = false;

            popRequest(currentRequest);

            if(currentRequest.request->canceled == false){

                switch(currentRequest.request->type) {

                    case Complex:

                        setState(ProcessingComplex);

                        processComplex(currentRequest);

                        break;

                    case DownloadFile:

                        setState(DownloadingFile);

                        processFile(currentRequest);

                        break;

                    case DownloadPhoto:

                        setState(DownloadingPhoto);

                        processPhoto(currentRequest);

                        break;

                    case CopyMotionShallowCopy:

                        setState(CopyingMotionShallowCopy);

                        processMotionShallowCopy(currentRequest);

                        break;
                
                    case CopyMotionMetadata:

                        setState(CopyingMotionMetadata);

                        processMotionMetadata(currentRequest);

                        break;

                    case CopyMedicalShallowCopy:

                        setState(CopyingMedicalShallowCopy);

                        processMedicalShallowCopy(currentRequest);

                        break;

                    case CopyMedicalMetadata:

                        setState(CopyingMedicalMetadata);

                        processMedicalMetadata(currentRequest);

                        break;

                    case PingServer:

                        setState(PingingServer);
                            
                        processPing(currentRequest);

                        break;
                }

                currentRequest.request.reset();
            }
        }else{
            if(state != Ready){
                setState(Ready);
            }
            microSleep(10000);
        }
    }

    //zwalniamy zasoby po³¹czeñ dla us³ugi plików i prostych zapytañ
    deinit();
}

size_t CommunicationManager::pingDataCallback(void *buffer, size_t size, size_t nmemb, void *stream)
{
    return size*nmemb;
}

//core::IDataManager::LocalTrial CommunicationManager::findLocalTrialsPaths(const core::Filesystem::Path& path)
//{
//    core::IDataManager::LocalTrial trial;
//
//    //przeszukujemy katalog w poszukiwaniu plikow:
//    std::vector<std::string> masks;
//    masks.push_back(".c3d");
//    masks.push_back(".mp");
//    masks.push_back(".vsk");
//    masks.push_back(".zip");
//    masks.push_back(".amc");
//    masks.push_back(".asf");
//    masks.push_back(".avi");
//    masks.push_back(".imgsequence");
//    std::vector<std::string> filesPath = core::Filesystem::listFiles(path.string(), true, masks);
//    BOOST_FOREACH(std::string path, filesPath)
//    {
//        trial.push_back(path);
//    }
//    return trial;
//}

CommunicationManager::ComplexRequestPtr CommunicationManager::createRequestComplex(const std::vector<CompleteRequest> & requests)
{
    return ComplexRequestPtr(new ComplexRequest(requests));
}

CommunicationManager::FileRequestPtr CommunicationManager::createRequestFile(unsigned int fileID, const std::string & filePath)
{
    return FileRequestPtr(new FileRequest(filePath, fileID));
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

CommunicationManager::BasicRequestPtr CommunicationManager::createRequestPing()
{
    return BasicRequestPtr(new BasicRequest(PingServer));
}
