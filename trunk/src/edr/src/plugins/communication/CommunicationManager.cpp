/**
@author Marek Daniluk
*/

#include "CommunicationPCH.h"
#include <core/Filesystem.h>
#include <plugins/communication/CommunicationManager.h>
//#include <boost/regex.hpp>

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
    : finish(false), state(Ready), pingCurl(nullptr)//, filesToDownload(0), actualDownloadFileNumer(0),
    //localDBImagesPath(core::getApplicationDataString("db/schema/")), localDataPath(core::getUserDataString("data/sessions"))
{
    ////na razie recznie wpisane sciezki
    //motionShallowCopyPath = (localDBImagesPath / "motion" / "shallowcopy.xml").string();
    //motionMetadataPath = (localDBImagesPath / "motion" / "metadata.xml").string();

    //medicalShallowCopyPath = (localDBImagesPath / "medical" / "shallowcopy.xml").string();
    //medicalMetadataPath = (localDBImagesPath / "medical" / "metadata.xml").string();    

    //if(core::Filesystem::pathExists(localDBImagesPath / "medical") == false){
    //    core::Filesystem::createDirectory(localDBImagesPath / "medical");
    //}

    //if(core::Filesystem::pathExists(localDBImagesPath / "motion") == false){
    //    core::Filesystem::createDirectory(localDBImagesPath / "motion");
    //}

    ////sorawdzenie przy uruchomieniu czy mamy pliki plytkiej kopii DB
    //if(core::Filesystem::pathExists(motionShallowCopyPath) == true && core::Filesystem::pathExists(motionMetadataPath) == true) {
    //    readMotionDbSchemas(motionShallowCopyPath, motionMetadataPath);
    //} else {
    //    LOG_WARNING("Missing motion DB data from XML files.");
    //    copyMotionDbData();
    //}

    ////sorawdzenie przy uruchomieniu czy mamy pliki plytkiej kopii DB
    //if(core::Filesystem::pathExists(medicalShallowCopyPath) == true && core::Filesystem::pathExists(medicalMetadataPath) == true) {
    //    readMedicalDbSchemas(medicalShallowCopyPath, medicalMetadataPath);
    //} else {
    //    LOG_WARNING("Missing medical DB data from XML files.");
    //    copyMedicalDbData();
    //}

    ////czekamy a¿ skonczy sci¹gaæ
    //while(requestsQueueEmpty() == false){
    //    OpenThreads::Thread::microSleep(10000);
    //}
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
    ScopedLock lock(transportManagerMutex);
    return currentTransportManager == nullptr ? 0 : currentTransportManager->getProgress();
}

//int CommunicationManager::getTotalProgress() const
//{
//    int count = getFilesToDownloadCount();
//
//    int ret = getProgress();
//
//    if(count > 0){
//        ret = (getActualDownloadFileNumber() + ret / 100.0) / (float)count;
//    }
//
//    return ret;
//}
//
//void CommunicationManager::copyMotionDbData(const RequestCallbacks & callbacks)
//{
//    CompleteRequest request;
//    request.request.type = CopyMotionDB;
//    request.request.id = 0;
//    request.callbacks = callbacks;
//    pushRequest(request);
//}
//
//void CommunicationManager::copyMedicalDbData(const RequestCallbacks & callbacks)
//{
//    CompleteRequest request;
//    request.request.type = CopyMedicalDB;
//    request.request.id = 0;
//    request.callbacks = callbacks;
//    pushRequest(request);
//}
//
//const communication::MotionShallowCopy::Performer * CommunicationManager::getPerformer(int performerID)
//{
//    auto it = constMotionShallowCopy->performers.find(performerID);
//
//    if(it == constMotionShallowCopy->performers.end()){
//        throw std::runtime_error("Could not find performer with given id - try refreshing shallow copy");
//    }
//
//    return it->second;
//}
//
//const communication::MotionShallowCopy::Trial * CommunicationManager::getTrial(int trialID)
//{
//    auto it = constMotionShallowCopy->trials.find(trialID);
//
//    if(it == constMotionShallowCopy->trials.end()){
//        throw std::runtime_error("Could not find trial with given id - try refreshing shallow copy");
//    }
//    
//    return it->second;
//}
//
//const communication::MotionShallowCopy::Session * CommunicationManager::getSession(int sessionID)
//{
//    auto it = constMotionShallowCopy->sessions.find(sessionID);
//
//    if(it == constMotionShallowCopy->sessions.end()){
//        throw std::runtime_error("Could not find session with given id - try refreshing shallow copy");
//    }
//
//    return it->second;
//}
//
//const communication::MotionShallowCopy::File * CommunicationManager::getFile(int fileID)
//{
//    auto it = constMotionShallowCopy->files.find(fileID);
//
//    if(it == constMotionShallowCopy->files.end()){
//        throw std::runtime_error("Could not find file with given id - try refreshing shallow copy");
//    }
//
//    return it->second;
//}
//
//const std::string & CommunicationManager::getTrialName(int trialID)
//{
//    return getTrial(trialID)->trialName;
//}
//
//const std::string & CommunicationManager::getSessionName(int sessionID)
//{
//    return getSession(sessionID)->sessionName;
//}
//
//const std::string & CommunicationManager::getFileName(int fileID)
//{
//    return getFile(fileID)->fileName;
//}
//
//void CommunicationManager::downloadPerformer(unsigned int performerID, const RequestCallbacks & callbacks)
//{
//    CompleteRequest request;
//    request.request.type = DownloadPerformer;
//    request.request.id = performerID;
//    request.callbacks = callbacks;
//    pushRequest(request);
//}
//
//void CommunicationManager::downloadSession(unsigned int sessionID, const RequestCallbacks & callbacks)
//{
//    CompleteRequest request;
//    request.request.type = DownloadSession;
//    request.request.id = sessionID;
//    request.callbacks = callbacks;
//    pushRequest(request);
//}
//
//void CommunicationManager::downloadTrial(unsigned int trialID, const RequestCallbacks & callbacks)
//{
//    CompleteRequest request;
//    request.request.type = DownloadTrial;
//    request.request.id = trialID;
//    request.callbacks = callbacks;
//    pushRequest(request);
//}
//
//void CommunicationManager::downloadFile(unsigned int fileID, const RequestCallbacks & callbacks)
//{
//    CompleteRequest request;
//    request.request.type = DownloadFile;
//    request.request.id = fileID;
//    request.callbacks = callbacks;
//    pushRequest(request);
//}
//
//void CommunicationManager::ping(const RequestCallbacks & callbacks)
//{
//    CompleteRequest request;
//    request.request.type = PingServer;
//    request.request.id = 0;
//    request.callbacks = callbacks;
//    pushRequest(request);
//}

void CommunicationManager::cancelRequest(const RequestPtr & request)
{
    request->cancelled = true;
    if(currentRequest.request == request){
        if(currentTransportManager != nullptr){
            currentTransportManager->abort();
            cancelDownloading = true;
        }

        if(currentRequest.callbacks.onCancelCallback.empty() == false){
            currentRequest.callbacks.onCancelCallback(currentRequest.request);
        }
    }
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
                    case DownloadFile: {

                        cancelDownloading = false;

                        setState(DownloadingFile);

                        if(currentRequest.callbacks.onBeginCallback.empty() == false){
                            currentRequest.callbacks.onBeginCallback(currentRequest.request);
                        }

                        //core::Filesystem::Path path;

                        try {

                            core::shared_ptr<FileRequest> fileRequest = core::dynamic_pointer_cast<FileRequest>(currentRequest.request);
                            setCurrentTransportManager(motionTransportManager);
                            /*const communication::MotionShallowCopy::File * file = getFile(request.request.id);

                            path = core::Filesystem::Path(localDataPath) / core::Filesystem::Path(getSessionName(file->session->sessionID));

                            core::Filesystem::createDirectory(path);
                            actualDownloadFileNumer = 1;
                            filesToDownload = 1;*/
                            motionTransportManager->downloadFile(fileRequest->fileID, fileRequest->filePath);

                            if(cancelDownloading == false){
                                if(currentRequest.callbacks.onEndCallback.empty() == false){
                                    currentRequest.callbacks.onEndCallback(currentRequest.request);
                                }
                            }else{
                                if(currentRequest.callbacks.onCancelCallback.empty() == false){
                                    currentRequest.callbacks.onCancelCallback(currentRequest.request);
                                }
                            }
                        } catch(std::exception& e) {
                            //czyœcimy jesli nie uda³o siê utworzyæ zasobów w ca³oœci
                            /*if(path.empty() == false){
                                core::Filesystem::deleteDirectory(path);
                            }*/

                            if(currentRequest.callbacks.onErrorCallback.empty() == false){
                                currentRequest.callbacks.onErrorCallback(currentRequest.request, e.what());
                            }
                        }

                        //actualDownloadFileNumer = 0;
                        //filesToDownload = 0;

                        break;
                        }

                    case DownloadPhoto: {

                        cancelDownloading = false;

                        setState(DownloadingPhoto);

                        if(currentRequest.callbacks.onBeginCallback.empty() == false){
                            currentRequest.callbacks.onBeginCallback(currentRequest.request);
                        }

                        //core::Filesystem::Path path;

                        try {
                            core::shared_ptr<PhotoRequest> photoRequest = core::dynamic_pointer_cast<PhotoRequest>(currentRequest.request);
                            setCurrentTransportManager(motionTransportManager);
                            /*const communication::MotionShallowCopy::File * file = getFile(request.request.id);

                            path = core::Filesystem::Path(localDataPath) / core::Filesystem::Path(getSessionName(file->session->sessionID));

                            core::Filesystem::createDirectory(path);
                            actualDownloadFileNumer = 1;
                            filesToDownload = 1;*/
                            motionTransportManager->downloadPhoto(photoRequest->photoID, photoRequest->filePath);

                            if(cancelDownloading == false){
                                if(currentRequest.callbacks.onEndCallback.empty() == false){
                                    currentRequest.callbacks.onEndCallback(currentRequest.request);
                                }
                            }else{
                                if(currentRequest.callbacks.onCancelCallback.empty() == false){
                                    currentRequest.callbacks.onCancelCallback(currentRequest.request);
                                }
                            }
                        } catch(std::exception& e) {
                            //czyœcimy jesli nie uda³o siê utworzyæ zasobów w ca³oœci
                            /*if(path.empty() == false){
                                core::Filesystem::deleteDirectory(path);
                            }*/

                            if(currentRequest.callbacks.onErrorCallback.empty() == false){
                                currentRequest.callbacks.onErrorCallback(currentRequest.request, e.what());
                            }
                        }

                        //actualDownloadFileNumer = 0;
                        //filesToDownload = 0;

                        break;
                        }

                    //case DownloadTrial: {

                    //        cancelDownloading = false;    

                    //        setState(DownloadingTrial);

                    //        if(request.callbacks.onBeginCallback.empty() == false){
                    //            request.callbacks.onBeginCallback(request.request);
                    //        }

                    //        core::Filesystem::Path path;
                    //        std::map<int, std::string> toDownload;

                    //        try {

                    //            auto trial = getTrial(request.request.id);

                    //            //sprawdz czy sesja juz istnieje
                    //            auto sessionPath = core::Filesystem::Path(localDataPath) / core::Filesystem::Path(trial->session->sessionName);

                    //            if(core::Filesystem::pathExists(sessionPath) == false){
                    //                core::Filesystem::createDirectory(sessionPath);
                    //            }

                    //            //sprawdz czy wszystkie pliki sesji juz sa - jesli nie kaz dosciagac brakujace pliki
                    //            auto localSessionFiles = core::Filesystem::listFiles(sessionPath);

                    //            std::set<std::string> localFileNamesOnly;

                    //            for(auto it = localSessionFiles.begin(); it != localSessionFiles.end(); it++){
                    //                localFileNamesOnly.insert(core::Filesystem::Path(*it).filename().string());
                    //            }

                    //            for(auto it = trial->session->files.begin(); it != trial->session->files.end(); it++){
                    //                if(localFileNamesOnly.find(it->second->fileName) == localFileNamesOnly.end()){
                    //                    toDownload[it->second->fileID] = (sessionPath / core::Filesystem::Path(it->second->fileName)).string();
                    //                }
                    //            }

                    //            //dodajemy pliki triala
                    //            for(auto it = trial->files.begin(); it != trial->files.end(); it++){
                    //                if(localFileNamesOnly.find(it->second->fileName) == localFileNamesOnly.end()){
                    //                    //brakuje nam pliku - trzeba go sciagnac
                    //                    toDownload[it->second->fileID] = (sessionPath / core::Filesystem::Path(it->second->fileName)).string();
                    //                }
                    //            }

                    //            if(toDownload.empty() == false){
                    //                actualDownloadFileNumer = 1;
                    //                filesToDownload = toDownload.size();
                    //            }

                    //            for(auto it = toDownload.begin(); it != toDownload.end(); it++){

                    //                if(cancelDownloading == true){
                    //                    break;
                    //                }

                    //                motionTransportManager->downloadFile(it->first, it->second);
                    //                actualDownloadFileNumer++;
                    //            }

                    //            if(cancelDownloading == false || actualDownloadFileNumer == filesToDownload){
                    //                cancelDownloading = false;
                    //                if(request.callbacks.onEndCallback.empty() == false){
                    //                    request.callbacks.onEndCallback(request.request);
                    //                }
                    //            }else{

                    //                //czyœcimy jesli przerwano
                    //                for(auto it = toDownload.begin(); it != toDownload.end(); it++){
                    //                    if(core::Filesystem::pathExists(it->second) == true){
                    //                        core::Filesystem::deleteFile(it->second);
                    //                    }else{
                    //                        break;
                    //                    }
                    //                }

                    //                if(request.callbacks.onCancelCallback.empty() == false){
                    //                    request.callbacks.onCancelCallback(request.request);
                    //                }
                    //            }
                    //        } catch(std::exception& e) {
                    //            //czyœcimy jesli nie uda³o siê utworzyæ zasobów w ca³oœci
                    //            for(auto it = toDownload.begin(); it != toDownload.end(); it++){
                    //                if(core::Filesystem::pathExists(it->second) == true){
                    //                    core::Filesystem::deleteFile(it->second);
                    //                }else{
                    //                    break;
                    //                }
                    //            }

                    //            if(request.callbacks.onErrorCallback.empty() == false){
                    //                request.callbacks.onErrorCallback(request.request, e.what());
                    //            }
                    //        }

                    //        actualDownloadFileNumer = 0;
                    //        filesToDownload = 0;

                    //        break;
                    //    }

                    //case DownloadSession: {

                    //    cancelDownloading = false;    

                    //    setState(DownloadingSession);

                    //    if(request.callbacks.onBeginCallback.empty() == false){
                    //        request.callbacks.onBeginCallback(request.request);
                    //    }

                    //    core::Filesystem::Path path;
                    //    std::map<int, std::string> toDownload;

                    //    try {

                    //        auto session = getSession(request.request.id);
                    //        //sprawdz czy sesja juz istnieje
                    //        auto sessionPath = core::Filesystem::Path(localDataPath) / core::Filesystem::Path(session->sessionName);

                    //        if(core::Filesystem::pathExists(sessionPath) == false){
                    //            core::Filesystem::createDirectory(sessionPath);
                    //        }

                    //        //sprawdz czy wszystkie pliki sesji juz sa - jesli nie kaz dosciagac brakujace pliki
                    //        auto localSessionFiles = core::Filesystem::listFiles(sessionPath);

                    //        for(auto sessionFilesIT = session->files.begin(); sessionFilesIT != session->files.end(); sessionFilesIT++){
                    //            if( std::find(localSessionFiles.begin(), localSessionFiles.end(), sessionFilesIT->second->fileName) == localSessionFiles.end()){
                    //                //brakuje nam pliku - trzeba go sciagnac
                    //                toDownload[sessionFilesIT->second->fileID] = (sessionPath / core::Filesystem::Path(sessionFilesIT->second->fileName)).string();
                    //            } 
                    //        }

                    //        //dodajemy pliki triali
                    //        for(auto trialsIT = session->trials.begin(); trialsIT != session->trials.end(); trialsIT++){
                    //            //weryfikujemy pliki triali, jeœli brakuje dodajemy je

                    //            for(auto filesIT = trialsIT->second->files.begin(); filesIT != trialsIT->second->files.end(); filesIT++){
                    //                if( std::find(localSessionFiles.begin(), localSessionFiles.end(), filesIT->second->fileName) == localSessionFiles.end()){
                    //                    //brakuje nam pliku - trzeba go sciagnac
                    //                    toDownload[filesIT->second->fileID] = (sessionPath / core::Filesystem::Path(filesIT->second->fileName)).string();
                    //                }
                    //            }
                    //        }

                    //        actualDownloadFileNumer = 1;
                    //        filesToDownload = toDownload.size();

                    //        for(auto it = toDownload.begin(); it != toDownload.end(); it++){

                    //            if(cancelDownloading == true){
                    //                break;
                    //            }

                    //            motionTransportManager->downloadFile(it->first, it->second);
                    //            actualDownloadFileNumer++;
                    //        }

                    //        if(cancelDownloading == false){
                    //            if(request.callbacks.onEndCallback.empty() == false){
                    //                request.callbacks.onEndCallback(request.request);
                    //            }
                    //        }else{

                    //            //czyœcimy jesli przerwano
                    //            for(auto it = toDownload.begin(); it != toDownload.end(); it++){
                    //                if(core::Filesystem::pathExists(it->second) == true){
                    //                    core::Filesystem::deleteFile(it->second);
                    //                }else{
                    //                    break;
                    //                }
                    //            }

                    //            if(request.callbacks.onCancelCallback.empty() == false){
                    //                request.callbacks.onCancelCallback(request.request);
                    //            }
                    //        }
                    //    } catch(std::exception& e) {
                    //        //czyœcimy jesli nie uda³o siê utworzyæ zasobów w ca³oœci
                    //        for(auto it = toDownload.begin(); it != toDownload.end(); it++){
                    //            if(core::Filesystem::pathExists(it->second) == true){
                    //                core::Filesystem::deleteFile(it->second);
                    //            }else{
                    //                break;
                    //            }
                    //        }

                    //        if(request.callbacks.onErrorCallback.empty() == false){
                    //            request.callbacks.onErrorCallback(request.request, e.what());
                    //        }
                    //    }

                    //    actualDownloadFileNumer = 0;
                    //    filesToDownload = 0;

                    //    break;
                    //    }

                    //case DownloadPerformer: {
                    //    cancelDownloading = false;    

                    //    setState(DownloadingPerformer);

                    //    if(request.callbacks.onBeginCallback.empty() == false){
                    //        request.callbacks.onBeginCallback(request.request);
                    //    }

                    //    core::Filesystem::Path path;
                    //    std::map<int, std::string> toDownload;

                    //    try {

                    //        auto performer = getPerformer(request.request.id);

                    //        for(auto perfConfIT = performer->performerConfs.begin(); perfConfIT != performer->performerConfs.end(); perfConfIT++){
                    //            //sprawdz czy sesja juz istnieje
                    //            auto sessionPath = core::Filesystem::Path(localDataPath) / core::Filesystem::Path(perfConfIT->second->session->sessionName);

                    //            if(core::Filesystem::pathExists(sessionPath) == false){
                    //                core::Filesystem::createDirectory(sessionPath);
                    //            }

                    //            //sprawdz czy wszystkie pliki sesji juz sa - jesli nie kaz dosciagac brakujace pliki
                    //            auto localSessionFiles = core::Filesystem::listFiles(sessionPath);

                    //            for(auto sessionFilesIT = perfConfIT->second->session->files.begin(); sessionFilesIT != perfConfIT->second->session->files.end(); sessionFilesIT++){
                    //                if( std::find(localSessionFiles.begin(), localSessionFiles.end(), sessionFilesIT->second->fileName) == localSessionFiles.end()){
                    //                    //brakuje nam pliku - trzeba go sciagnac
                    //                    toDownload[sessionFilesIT->second->fileID] = (sessionPath / core::Filesystem::Path(sessionFilesIT->second->fileName)).string();
                    //                } 
                    //            }

                    //            //dodajemy pliki triali
                    //            for(auto trialsIT = perfConfIT->second->session->trials.begin(); trialsIT != perfConfIT->second->session->trials.end(); trialsIT++){

                    //                //weryfikujemy pliki triali, jeœli brakuje dodajemy je

                    //                for(auto filesIT = trialsIT->second->files.begin(); filesIT != trialsIT->second->files.end(); filesIT++){
                    //                    if( std::find(localSessionFiles.begin(), localSessionFiles.end(), filesIT->second->fileName) == localSessionFiles.end()){
                    //                        //brakuje nam pliku - trzeba go sciagnac
                    //                        toDownload[filesIT->second->fileID] = (sessionPath / core::Filesystem::Path(filesIT->second->fileName)).string();
                    //                    }
                    //                }
                    //            }
                    //        }                        

                    //        actualDownloadFileNumer = 1;
                    //        filesToDownload = toDownload.size();

                    //        for(auto it = toDownload.begin(); it != toDownload.end(); it++){

                    //            if(cancelDownloading == true){
                    //                break;
                    //            }

                    //            motionTransportManager->downloadFile(it->first, it->second);
                    //            actualDownloadFileNumer++;
                    //        }

                    //        if(cancelDownloading == false){
                    //            if(request.callbacks.onEndCallback.empty() == false){
                    //                request.callbacks.onEndCallback(request.request);
                    //            }
                    //        }else{

                    //            //czyœcimy jesli przerwano
                    //            for(auto it = toDownload.begin(); it != toDownload.end(); it++){
                    //                if(core::Filesystem::pathExists(it->second) == true){
                    //                    core::Filesystem::deleteFile(it->second);
                    //                }else{
                    //                    break;
                    //                }
                    //            }

                    //            if(request.callbacks.onCancelCallback.empty() == false){
                    //                request.callbacks.onCancelCallback(request.request);
                    //            }
                    //        }
                    //    } catch(std::exception& e) {
                    //        //czyœcimy jesli nie uda³o siê utworzyæ zasobów w ca³oœci
                    //        for(auto it = toDownload.begin(); it != toDownload.end(); it++){
                    //            if(core::Filesystem::pathExists(it->second) == true){
                    //                core::Filesystem::deleteFile(it->second);
                    //            }else{
                    //                break;
                    //            }
                    //        }

                    //        if(request.callbacks.onErrorCallback.empty() == false){
                    //            request.callbacks.onErrorCallback(request.request, e.what());
                    //        }
                    //    }

                    //    actualDownloadFileNumer = 0;
                    //    filesToDownload = 0;

                    //    break;
                    //}

                    case CopyMotionShallowCopy: {

                            setState(CopyingMotionShallowCopy);

                            if(currentRequest.callbacks.onBeginCallback.empty() == false){
                                currentRequest.callbacks.onBeginCallback(currentRequest.request);
                            }

                            try {
                                core::shared_ptr<MetadataRequest> metaRequest = core::dynamic_pointer_cast<MetadataRequest>(currentRequest.request);
                                setCurrentTransportManager(motionTransportManager);
                                motionTransportManager->getShallowCopy(metaRequest->filePath);
                            
                                if(cancelDownloading == false){
                                    if(currentRequest.callbacks.onEndCallback.empty() == false){
                                        currentRequest.callbacks.onEndCallback(currentRequest.request);
                                    }
                                }else{
                                    if(currentRequest.callbacks.onCancelCallback.empty() == false){
                                        currentRequest.callbacks.onCancelCallback(currentRequest.request);
                                    }
                                }
                            } catch(std::exception& e) {
                                if(currentRequest.callbacks.onErrorCallback.empty() == false){
                                    currentRequest.callbacks.onErrorCallback(currentRequest.request, e.what());
                                }
                            }

                            break;
                        }
                
                    case CopyMotionMetadata: {

                        setState(CopyingMotionMetadata);

                        if(currentRequest.callbacks.onBeginCallback.empty() == false){
                            currentRequest.callbacks.onBeginCallback(currentRequest.request);
                        }

                        try {
                            core::shared_ptr<MetadataRequest> metaRequest = core::dynamic_pointer_cast<MetadataRequest>(currentRequest.request);
                            setCurrentTransportManager(motionTransportManager);
                            motionTransportManager->getMetadata(metaRequest->filePath);

                            if(cancelDownloading == false){
                                if(currentRequest.callbacks.onEndCallback.empty() == false){
                                    currentRequest.callbacks.onEndCallback(currentRequest.request);
                                }
                            }else{
                                if(currentRequest.callbacks.onCancelCallback.empty() == false){
                                    currentRequest.callbacks.onCancelCallback(currentRequest.request);
                                }
                            }
                        } catch(std::exception& e) {
                            if(currentRequest.callbacks.onErrorCallback.empty() == false){
                                currentRequest.callbacks.onErrorCallback(currentRequest.request, e.what());
                            }
                        }

                        break;
                    }

                    case CopyMedicalShallowCopy: {

                        setState(CopyingMedicalShallowCopy);

                        if(currentRequest.callbacks.onBeginCallback.empty() == false){
                            currentRequest.callbacks.onBeginCallback(currentRequest.request);
                        }

                        try {
                            core::shared_ptr<MetadataRequest> metaRequest = core::dynamic_pointer_cast<MetadataRequest>(currentRequest.request);
                            setCurrentTransportManager(medicalTransportManager);
                            medicalTransportManager->getShallowCopy(metaRequest->filePath);

                            if(cancelDownloading == false){
                                if(currentRequest.callbacks.onEndCallback.empty() == false){
                                    currentRequest.callbacks.onEndCallback(currentRequest.request);
                                }
                            }else{
                                if(currentRequest.callbacks.onCancelCallback.empty() == false){
                                    currentRequest.callbacks.onCancelCallback(currentRequest.request);
                                }
                            }
                        } catch(std::exception& e) {
                            if(currentRequest.callbacks.onErrorCallback.empty() == false){
                                currentRequest.callbacks.onErrorCallback(currentRequest.request, e.what());
                            }
                        }

                        break;
                    }

                    case CopyMedicalMetadata: {

                        setState(CopyingMedicalMetadata);

                        if(currentRequest.callbacks.onBeginCallback.empty() == false){
                            currentRequest.callbacks.onBeginCallback(currentRequest.request);
                        }

                        try {
                            core::shared_ptr<MetadataRequest> metaRequest = core::dynamic_pointer_cast<MetadataRequest>(currentRequest.request);
                            setCurrentTransportManager(medicalTransportManager);
                            medicalTransportManager->getMetadata(metaRequest->filePath);

                            if(cancelDownloading == false){
                                if(currentRequest.callbacks.onEndCallback.empty() == false){
                                    currentRequest.callbacks.onEndCallback(currentRequest.request);
                                }
                            }else{
                                if(currentRequest.callbacks.onCancelCallback.empty() == false){
                                    currentRequest.callbacks.onCancelCallback(currentRequest.request);
                                }
                            }
                        } catch(std::exception& e) {
                            if(currentRequest.callbacks.onErrorCallback.empty() == false){
                                currentRequest.callbacks.onErrorCallback(currentRequest.request, e.what());
                            }
                        }

                        break;
                    }


                    case PingServer: {

                            setState(PingingServer);
                            setCurrentTransportManager(core::shared_ptr<communication::TransportWSDL_FTPSBase>());
                            if(currentRequest.callbacks.onBeginCallback.empty() == false){
                                currentRequest.callbacks.onBeginCallback(currentRequest.request);
                            }

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

                            if(currentRequest.callbacks.onEndCallback.empty() == false){
                                currentRequest.callbacks.onEndCallback(currentRequest.request);
                            }

                            break;
                        }
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

//void CommunicationManager::readMotionDbSchemas(const std::string& shallowCopyPath, const std::string& metaDataPath)
//{
//    MotionShallowCopyParserPtr ptrS = MotionShallowCopyParserPtr(new MotionShallowCopyParser());
//    ptrS->parseFile(shallowCopyPath);
//    constMotionShallowCopy = ptrS->getShallowCopy();
//
//    MotionMetadataParserPtr ptrM = MotionMetadataParserPtr(new MotionMetadataParser());
//    ptrM->parseFile(metaDataPath);
//    motionMetaData = ptrM->getMetadata();
//}
//
//void CommunicationManager::readMedicalDbSchemas(const std::string& shallowCopyPath, const std::string& metaDataPath)
//{
//    MedicalShallowCopyParserPtr ptrS = MedicalShallowCopyParserPtr(new MedicalShallowCopyParser());
//    ptrS->parseFile(shallowCopyPath);
//    constMedicalShallowCopy = ptrS->getShallowCopy();
//
//    MedicalMetadataParserPtr ptrM = MedicalMetadataParserPtr(new MedicalMetadataParser());
//    ptrM->parseFile(metaDataPath);
//    medicalMetaData = ptrM->getMetadata();
//}

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

CommunicationManager::RequestPtr CommunicationManager::requestFile(unsigned int fileID, const std::string filePath, const RequestCallbacks & callbacks)
{
    RequestPtr ret(new FileRequest(false, DownloadFile, filePath, fileID));

    CompleteRequest request;
    request.request = ret;
    request.callbacks = callbacks;

    pushRequest(request);

    return ret;
}

CommunicationManager::RequestPtr CommunicationManager::requestPhoto(unsigned int fileID, const std::string filePath, const RequestCallbacks & callbacks)
{
    RequestPtr ret;
    return ret;
}

CommunicationManager::RequestPtr CommunicationManager::requestMotionShallowCopy(const std::string filePath, const RequestCallbacks & callbacks)
{
    RequestPtr ret;
    return ret;
}

CommunicationManager::RequestPtr CommunicationManager::requestMotionMetadata(const std::string filePath, const RequestCallbacks & callbacks)
{
    RequestPtr ret;
    return ret;
}

CommunicationManager::RequestPtr CommunicationManager::requestMedicalShallowCopy(const std::string filePath, const RequestCallbacks & callbacks)
{
    RequestPtr ret;
    return ret;
}

CommunicationManager::RequestPtr CommunicationManager::requestMedicalMetadata(const std::string filePath, const RequestCallbacks & callbacks)
{
    RequestPtr ret;
    return ret;
}

CommunicationManager::RequestPtr CommunicationManager::requestPing(const RequestCallbacks & callbacks)
{
    RequestPtr ret;
    return ret;
}
