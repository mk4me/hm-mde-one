/**
@author Marek Daniluk
*/

#include "CommunicationPCH.h"
#include <core/Filesystem.h>
#include <plugins/communication/CommunicationManager.h>

using namespace communication;

CommunicationManager* CommunicationManager::instance = nullptr;

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

    motionTransportManager.reset(new communication::MotionTransportWSDL_FTPS());
    medicalTransportManager.reset(new communication::MedicalTransportWSDL_FTPS());
    motionQueryManager.reset(new communication::MotionQueryWSDL());
    medicalQueryManager.reset(new communication::MedicalQueryWSDL());

    //ping serwera
    pingCurl = curl_easy_init();
    if(pingCurl) {
        curl_easy_setopt(pingCurl, CURLOPT_URL, "http://v21.pjwstk.edu.pl/");
        curl_easy_setopt(pingCurl, CURLOPT_CONNECTTIMEOUT, 1);
        curl_easy_setopt(pingCurl, CURLOPT_WRITEFUNCTION, pingDataCallback);
    }
    //TODO: dane wpisane na sztywno, dodac zapisywanie ustawien
    motionTransportManager->setFTPCredentials("ftps://v21.pjwstk.edu.pl/", "testUser", "testUser");
    motionTransportManager->setWSCredentials("http://v21.pjwstk.edu.pl/Motion/FileStoremanWS.svc?wsdl", "applet_user", "aplet4Motion");

    medicalTransportManager->setFTPCredentials("ftps://v21.pjwstk.edu.pl/", "testUser", "testUser");
    medicalTransportManager->setWSCredentials("http://v21.pjwstk.edu.pl/MotionMed/FileStoremanWS.svc?wsdl", "applet_user", "aplet4Motion");

    motionQueryManager->setWSCredentials("applet_user", "aplet4Motion");
    motionQueryManager->setBasicQueriesServiceUri("http://v21.pjwstk.edu.pl/Motion/res/BasicQueriesWSStandalone.wsdl");
    motionQueryManager->setBasicUpdatesServiceUri("");

    /*medicalQueryManager->setWSCredentials("applet_user", "aplet4Motion");
    medicalQueryManager->setBasicQueriesServiceUri("http://v21.pjwstk.edu.pl/Motion/res/BasicQueriesWSStandalone.wsdl");
    medicalQueryManager->setBasicUpdatesServiceUri("");*/
    
    /*
    WsdlPull::WsdlParser::useLocalSchema_ = false;
    communication::WsdlConnection conn;
    conn.setCredentials("https://v21.pjwstk.edu.pl/SecureSample/Service1.svc?wsdl", "hml", "hml");
    conn.setOperation("CallMe");
    conn.setValue("value", "1");
    try{
    conn.setSecurity(true, (core::getPathInterface()->getResourcesPath() / "pjwstk-hm.crt").string(), WsdlPull::WsdlInvoker::CNAny);
    //conn.setSecurity(true, "c:\\ca.crt", WsdlPull::WsdlInvoker::CNAny);
    conn.invokeOperation();
    
    }
    catch(const std::exception & e){
        LOG_DEBUG(e.what());
    }
    catch(...){

    }
    
    auto resp = conn.getXMLResponse();
    LOG_DEBUG("HTTPS Communication:");
    LOG_DEBUG(resp);
    */
}

void CommunicationManager::deinit()
{
    motionTransportManager.reset();
    motionQueryManager.reset();

    medicalTransportManager.reset();
    medicalQueryManager.reset();
}

int CommunicationManager::getProgress() const
{
    ScopedLock lock(downloadHelperMutex);
    return currentDownloadHelper == nullptr ? 0 : currentDownloadHelper->getProgress();
}

void CommunicationManager::cancelRequest(const RequestPtr & request)
{
    request->cancel();
    if(currentRequest.request == request){
        if(currentDownloadHelper != nullptr){
            currentDownloadHelper->abort();
        }
    }
}

FtpsConnection::OperationStatus CommunicationManager::processComplex(const CompleteRequest & request, std::string & message)
{

    FtpsConnection::OperationStatus ret = FtpsConnection::Complete;

    if(request.callbacks.onBeginCallback.empty() == false){
        request.callbacks.onBeginCallback(request.request);
    }

    core::shared_ptr<ComplexRequest> complexRequest = core::dynamic_pointer_cast<ComplexRequest>(request.request);
    
    unsigned int s = complexRequest->size();
    unsigned int i = 0;

    for( ; i < s; i++){
        
        if(request.request->isCancelled()){
            ret = FtpsConnection::Cancelled;
        }

        //sprawdz czy anulowano lub blad
        //jesli nie to kontynuj
        if(ret != FtpsConnection::Complete){
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
    case FtpsConnection::Error:
        
        if(request.callbacks.onErrorCallback.empty() == false){
            request.callbacks.onErrorCallback(request.request, message);
        }

        break;

    case FtpsConnection::Complete:

        if(request.callbacks.onEndCallback.empty() == false){
            request.callbacks.onEndCallback(request.request);
        }

        break;

    case FtpsConnection::Cancelled:

        if(request.callbacks.onCancelCallback.empty() == false){
            request.callbacks.onCancelCallback(request.request);
        }

        break;

    }

    return ret;
}

FtpsConnection::OperationStatus CommunicationManager::processPhoto(const CompleteRequest & request, std::string & message)
{
    FtpsConnection::OperationStatus ret = FtpsConnection::Complete;

    if(request.callbacks.onBeginCallback.empty() == false){
        request.callbacks.onBeginCallback(request.request);
    }

    try {

        core::shared_ptr<PhotoRequest> photoRequest = core::dynamic_pointer_cast<PhotoRequest>(request.request);
        setCurrentDownloadHelper(medicalTransportManager);

        ret = medicalTransportManager->downloadPhoto(photoRequest->getPhotoID(), photoRequest->getFilePath(), photoRequest.get());

        switch(ret){

        case FtpsConnection::Complete:
            if(request.callbacks.onEndCallback.empty() == false){
                request.callbacks.onEndCallback(request.request);
            }

            break;

        case FtpsConnection::Cancelled:
            if(request.callbacks.onCancelCallback.empty() == false){
                request.callbacks.onCancelCallback(request.request);
            }

            break;

        case FtpsConnection::Error:

            message += medicalTransportManager->getLastError();

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

    return ret;
}

FtpsConnection::OperationStatus CommunicationManager::processFile(const CompleteRequest & request, std::string & message)
{
    FtpsConnection::OperationStatus ret = FtpsConnection::Complete;

    if(request.callbacks.onBeginCallback.empty() == false){
        request.callbacks.onBeginCallback(request.request);
    }

    try {

        core::shared_ptr<FileRequest> fileRequest = core::dynamic_pointer_cast<FileRequest>(request.request);
        setCurrentDownloadHelper(motionTransportManager);

        ret = motionTransportManager->downloadFile(fileRequest->fileID, fileRequest->filePath, fileRequest.get());

        switch(ret){

        case FtpsConnection::Complete:
            if(request.callbacks.onEndCallback.empty() == false){
                request.callbacks.onEndCallback(request.request);
            }

            break;

        case FtpsConnection::Cancelled:
            if(request.callbacks.onCancelCallback.empty() == false){
                request.callbacks.onCancelCallback(request.request);
            }

            break;

        case FtpsConnection::Error:

            message += medicalTransportManager->getLastError();

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

    return ret;
}

FtpsConnection::OperationStatus CommunicationManager::processMotionShallowCopy(const CompleteRequest & request, std::string & message)
{
    FtpsConnection::OperationStatus ret = FtpsConnection::Complete;

    if(request.callbacks.onBeginCallback.empty() == false){
        request.callbacks.onBeginCallback(request.request);
    }

    try {
        core::shared_ptr<MetadataRequest> metaRequest = core::dynamic_pointer_cast<MetadataRequest>(request.request);
        setCurrentDownloadHelper(motionTransportManager);
        
        ret = motionTransportManager->getShallowCopy(metaRequest->filePath, metaRequest.get());

        switch(ret){

        case FtpsConnection::Complete:
            if(request.callbacks.onEndCallback.empty() == false){
                request.callbacks.onEndCallback(request.request);
            }

            break;

        case FtpsConnection::Cancelled:
            if(request.callbacks.onCancelCallback.empty() == false){
                request.callbacks.onCancelCallback(request.request);
            }

            break;

        case FtpsConnection::Error:

            message += medicalTransportManager->getLastError();

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

    return ret;
}

FtpsConnection::OperationStatus CommunicationManager::processMotionMetadata(const CompleteRequest & request, std::string & message)
{
    FtpsConnection::OperationStatus ret = FtpsConnection::Complete;

    if(request.callbacks.onBeginCallback.empty() == false){
        request.callbacks.onBeginCallback(request.request);
    }

    try {
        core::shared_ptr<MetadataRequest> metaRequest = core::dynamic_pointer_cast<MetadataRequest>(request.request);
        setCurrentDownloadHelper(motionTransportManager);

        ret = motionTransportManager->getMetadata(metaRequest->filePath, metaRequest.get());

        switch(ret){

        case FtpsConnection::Complete:
            if(request.callbacks.onEndCallback.empty() == false){
                request.callbacks.onEndCallback(request.request);
            }

            break;

        case FtpsConnection::Cancelled:
            if(request.callbacks.onCancelCallback.empty() == false){
                request.callbacks.onCancelCallback(request.request);
            }

            break;

        case FtpsConnection::Error:

            message += medicalTransportManager->getLastError();

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

    return ret;
}

FtpsConnection::OperationStatus CommunicationManager::processMedicalShallowCopy(const CompleteRequest & request, std::string & message)
{
    FtpsConnection::OperationStatus ret = FtpsConnection::Complete;

    if(request.callbacks.onBeginCallback.empty() == false){
        request.callbacks.onBeginCallback(request.request);
    }

    try {
        core::shared_ptr<MetadataRequest> metaRequest = core::dynamic_pointer_cast<MetadataRequest>(request.request);
        setCurrentDownloadHelper(medicalTransportManager);
        medicalTransportManager->getShallowCopy(metaRequest->filePath);

        ret = medicalTransportManager->getShallowCopy(metaRequest->filePath, metaRequest.get());

        switch(ret){

        case FtpsConnection::Complete:
            if(request.callbacks.onEndCallback.empty() == false){
                request.callbacks.onEndCallback(request.request);
            }

            break;

        case FtpsConnection::Cancelled:
            if(request.callbacks.onCancelCallback.empty() == false){
                request.callbacks.onCancelCallback(request.request);
            }

            break;

        case FtpsConnection::Error:

            message += medicalTransportManager->getLastError();

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

    return ret;
}

FtpsConnection::OperationStatus CommunicationManager::processMedicalMetadata(const CompleteRequest & request, std::string & message)
{
    FtpsConnection::OperationStatus ret = FtpsConnection::Complete;

    if(request.callbacks.onBeginCallback.empty() == false){
        request.callbacks.onBeginCallback(request.request);
    }

    try {
        core::shared_ptr<MetadataRequest> metaRequest = core::dynamic_pointer_cast<MetadataRequest>(request.request);
        setCurrentDownloadHelper(medicalTransportManager);
        
        ret = medicalTransportManager->getMetadata(metaRequest->filePath, metaRequest.get());

        switch(ret){

        case FtpsConnection::Complete:
            if(request.callbacks.onEndCallback.empty() == false){
                request.callbacks.onEndCallback(request.request);
            }

            break;

        case FtpsConnection::Cancelled:
            if(request.callbacks.onCancelCallback.empty() == false){
                request.callbacks.onCancelCallback(request.request);
            }

            break;

        case FtpsConnection::Error:

            message += medicalTransportManager->getLastError();

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

    return ret;
}

FtpsConnection::OperationStatus CommunicationManager::processPing(const CompleteRequest & request, std::string & message)
{
    FtpsConnection::OperationStatus ret = FtpsConnection::Complete;

    setCurrentDownloadHelper(core::shared_ptr<communication::TransportWSDL_FTPSBase>());
    
    if(request.callbacks.onBeginCallback.empty() == false){
        request.callbacks.onBeginCallback(request.request);
    }

    try{

        if(pingCurl) {
            pingCurlResult = curl_easy_perform(pingCurl);
            if(pingCurlResult) {
                serverResponse = false;
            } else {
                serverResponse = true;
            }
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

        ret = FtpsConnection::Error;
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

            if(currentRequest.request->cancelled == false){

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

CommunicationManager::RequestPtr CommunicationManager::createRequestComplex(const std::vector<CompleteRequest> & requests)
{
    return RequestPtr(new ComplexRequest(requests));
}

CommunicationManager::RequestPtr CommunicationManager::createRequestFile(unsigned int fileID, const std::string & filePath)
{
    return RequestPtr(new FileRequest(filePath, fileID));
}

CommunicationManager::RequestPtr CommunicationManager::createRequestPhoto(unsigned int fileID, const std::string & filePath)
{
    return RequestPtr(new PhotoRequest(filePath, fileID));
}

CommunicationManager::RequestPtr CommunicationManager::createRequestMotionShallowCopy(const std::string & filePath)
{
    return RequestPtr(new MetadataRequest(CopyMotionShallowCopy, filePath));
}

CommunicationManager::RequestPtr CommunicationManager::createRequestMotionMetadata(const std::string & filePath)
{
    return RequestPtr(new MetadataRequest(CopyMotionMetadata, filePath));
}

CommunicationManager::RequestPtr CommunicationManager::createRequestMedicalShallowCopy(const std::string & filePath)
{
    return RequestPtr(new MetadataRequest(CopyMedicalShallowCopy, filePath));
}

CommunicationManager::RequestPtr CommunicationManager::createRequestMedicalMetadata(const std::string & filePath)
{
    return RequestPtr(new MetadataRequest(CopyMedicalMetadata, filePath));    
}

CommunicationManager::RequestPtr CommunicationManager::createRequestPing()
{
    return RequestPtr(new BasicRequest(PingServer));
}
