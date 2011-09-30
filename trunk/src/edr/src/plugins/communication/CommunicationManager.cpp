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
    : transportManager(new communication::TransportWSDL_FTPS()),
    queryManager(new communication::QueryWSDL()), finish(false),
    filesToDownload(0), actualDownloadFileNumer(0)
{
    setState(Ready);
    //ping serwera
    pingCurl = curl_easy_init();
    if(pingCurl) {
        curl_easy_setopt(pingCurl, CURLOPT_URL, "http://83.230.112.43/");
        curl_easy_setopt(pingCurl, CURLOPT_CONNECTTIMEOUT, 1);
        curl_easy_setopt(pingCurl, CURLOPT_WRITEFUNCTION, pingDataCallback);
    }
    
    //TODO: dane wpisane na sztywno, dodac zapisywanie ustawien
    transportManager->setFTPCredentials("ftps://83.230.112.43/", "testUser", "testUser");
    transportManager->setWSCredentials("http://83.230.112.43/Motion/FileStoremanWS.svc?wsdl", "applet_user", "aplet4Motion");
    queryManager->setWSCredentials("applet_user", "aplet4Motion");
    queryManager->setBasicQueriesServiceUri("http://v21.pjwstk.edu.pl/Motion/res/BasicQueriesWSStandalone.wsdl");
    queryManager->setBasicUpdatesServiceUri("");

    //na razie recznie wpisane sciezki
    shallowCopyPath = core::getApplicationDataString("db/schema/shallowcopy.xml");
    metadataPath = core::getApplicationDataString("db/schema/metadata.xml");

    //sorawdzenie przy uruchomieniu czy mamy pliki plytkiej kopii DB
    if(core::Filesystem::pathExists(shallowCopyPath) == true && core::Filesystem::pathExists(metadataPath) == true) {
        readDbSchemas(shallowCopyPath, metadataPath);
    } else {
        LOG_WARNING("Missing DB data from XML files.");
        copyDbData();
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

int CommunicationManager::getProgress() const
{
    return transportManager->getProgress();
}

int CommunicationManager::getTotalProgress() const
{
    int count = getFilesToDownloadCount();

    int ret = getProgress();

    if(count > 0){
        ret = (transportManager->getDownloadedFilesCount() + ret / 100.0) / (float)count;
    }

    return ret;
}

void CommunicationManager::copyDbData(const RequestCallbacks & callbacks)
{
    CompleteRequest request;
    request.request.type = CopyDB;
    request.request.id = 0;
    request.callbacks = callbacks;
    pushRequest(request);
}

std::string CommunicationManager::getTrialDirectoryName(int trialID)
{
    auto vec = queryManager->listFiles(trialID, "trial");
    if(vec.empty() == true){
        throw std::runtime_error("Could not retrieve trial directory name - empty trial or error");
    }

    for(auto it = vec.begin(); it != vec.end(); it++){
        if((*it).fileName.find(".c3d") != std::string::npos || (*it).fileName.find(".asf") != std::string::npos ||
            (*it).fileName.find(".amc") != std::string::npos){

            return (*it).fileName.substr(0, (*it).fileName.find("."));
        }
    }

    throw std::runtime_error("Could not deduce trial directory name from assigned files");
}

void CommunicationManager::downloadTrial(unsigned int trialID, const RequestCallbacks & callbacks)
{
    CompleteRequest request;
    request.request.type = DownloadTrial;
    request.request.id = trialID;
    request.callbacks = callbacks;
    pushRequest(request);
}

void CommunicationManager::downloadFile(unsigned int fileID, const RequestCallbacks & callbacks)
{
    CompleteRequest request;
    request.request.type = DownloadFile;
    request.request.id = fileID;
    request.callbacks = callbacks;
    pushRequest(request);
}

void CommunicationManager::ping(const RequestCallbacks & callbacks)
{
    CompleteRequest request;
    request.request.type = PingServer;
    request.request.id = 0;
    request.callbacks = callbacks;
    pushRequest(request);
}

void CommunicationManager::loadLocalTrials()
{
    //TODO: uproszczenie wyszukiwania lokalnych triali
    localTrials.clear();
    //przeszukujemy liste prob pomiarowych, nie plikow
    std::vector<std::string> tempPaths = core::Filesystem::listSubdirectories(std::string(core::getUserDataString("data/trials")));
    BOOST_FOREACH(std::string path, tempPaths)
    {
        try {
            core::IDataManager::LocalTrial trial = findLocalTrialsPaths(path);
            localTrials.push_back(trial);
        } catch(std::exception& e) {
            LOG_INFO("Loading trial exception: " << e.what());
        }
    }
    
    notify();
}

void CommunicationManager::loadFiles(const std::vector<core::Filesystem::Path> files)
{
    dataManager->loadFiles(files);
}

void CommunicationManager::removeFiles(const std::vector<core::Filesystem::Path> files)
{
    dataManager->removeFiles(files);
}

void CommunicationManager::run()
{
    while(finish == false){

        if(requestsQueueEmpty() == false){

            cancelDownloading = false;

            CompleteRequest request;

            popRequest(request);
            
            switch(request.request.type) {
                case DownloadFile: {

                    cancelDownloading = false;

                    setState(DownloadingFile);

                    if(request.callbacks.onBeginCallback.empty() == false){
                        request.callbacks.onBeginCallback(request.request);
                    }

                    core::Filesystem::Path path;

                    try {

                        std::vector<wsdl::Trial> serverTrials = queryManager->listSessionContents();

                        int trialID = -1;

                        BOOST_FOREACH(wsdl::Trial& trial, serverTrials) {
                            BOOST_FOREACH(int i, trial.trialFiles) {
                                if(i == request.request.id){
                                    trialID = trial.id;
                                    break;
                                }
                            }
                            if(trialID != -1){
                                break;
                            }
                        }

                        if(trialID == -1 ){
                            //error request - nieznany trial pliku
                            throw std::runtime_error("Requested file does not belong to any known trial");
                        }

                        path = core::Filesystem::Path(core::getUserDataString("data/trials")) / core::Filesystem::Path(getTrialDirectoryName(trialID));

                        core::Filesystem::createDirectory(path);
                        actualDownloadFileNumer = 1;
                        filesToDownload = 1;
                        transportManager->downloadFile(request.request.id, path.string());

                        if(cancelDownloading == false){
                            if(request.callbacks.onEndCallback.empty() == false){
                                request.callbacks.onEndCallback(request.request);
                            }
                        }else{
                            if(request.callbacks.onCancelCallback.empty() == false){
                                request.callbacks.onCancelCallback(request.request);
                            }
                        }
                    } catch(std::exception& e) {
                        //czyœcimy jesli nie uda³o siê utworzyæ zasobów w ca³oœci
                        if(path.empty() == false){
                            core::Filesystem::deleteDirectory(path);
                        }

                        if(request.callbacks.onErrorCallback.empty() == false){
                            request.callbacks.onErrorCallback(request.request, e.what());
                        }
                    }

                    actualDownloadFileNumer = 0;
                    filesToDownload = 0;

                    break;
                    }

                case DownloadTrial: {

                        cancelDownloading = false;    

                        setState(DownloadingTrial);

                        if(request.callbacks.onBeginCallback.empty() == false){
                            request.callbacks.onBeginCallback(request.request);
                        }

                        core::Filesystem::Path path;

                        try {
                            std::vector<wsdl::Trial> serverTrials = queryManager->listSessionContents();
                            BOOST_FOREACH(wsdl::Trial& trial, serverTrials) {
                                if(trial.id == request.request.id) {

                                    path = core::Filesystem::Path(core::getUserDataString("data/trials")) / core::Filesystem::Path(getTrialDirectoryName(trial.id));

                                    core::Filesystem::createDirectory(path);

                                    actualDownloadFileNumer = 1;
                                    filesToDownload = trial.trialFiles.size();

                                    BOOST_FOREACH(int i, trial.trialFiles) {

                                        if(cancelDownloading == true){
                                            break;
                                        }

                                        transportManager->downloadFile(i, path.string());
                                        actualDownloadFileNumer++;
                                    }

                                    break;
                                }
                            }

                            if(cancelDownloading == false){
                                if(request.callbacks.onEndCallback.empty() == false){
                                    request.callbacks.onEndCallback(request.request);
                                }
                            }else{

                                //czyœcimy jesli przerwano
                                core::Filesystem::deleteDirectory(path);

                                if(request.callbacks.onCancelCallback.empty() == false){
                                    request.callbacks.onCancelCallback(request.request);
                                }
                            }
                        } catch(std::exception& e) {
                            //czyœcimy jesli nie uda³o siê utworzyæ zasobów w ca³oœci
                            if(path.empty() == false){
                                core::Filesystem::deleteDirectory(path);
                            }

                            if(request.callbacks.onErrorCallback.empty() == false){
                                request.callbacks.onErrorCallback(request.request, e.what());
                            }
                        }

                        actualDownloadFileNumer = 0;
                        filesToDownload = 0;

                        break;
                    }
                case CopyDB: {

                        setState(CopyingDB);

                        if(request.callbacks.onBeginCallback.empty() == false){
                            request.callbacks.onBeginCallback(request.request);
                        }

                        try {

                            actualDownloadFileNumer = 1;
                            filesToDownload = 2;

                            transportManager->getShallowCopy(shallowCopyPath);
                            actualDownloadFileNumer = 2;
                            transportManager->getMetadata(metadataPath);

                            readDbSchemas(shallowCopyPath, metadataPath);
                            
                            if(cancelDownloading == false){
                                if(request.callbacks.onEndCallback.empty() == false){
                                    request.callbacks.onEndCallback(request.request);
                                }
                            }else{
                                if(request.callbacks.onCancelCallback.empty() == false){
                                    request.callbacks.onCancelCallback(request.request);
                                }
                            }
                        } catch(std::exception& e) {
                            if(request.callbacks.onErrorCallback.empty() == false){
                                request.callbacks.onErrorCallback(request.request, e.what());
                            }
                        }

                        actualDownloadFileNumer = 0;
                        filesToDownload = 0;

                        break;
                    }
                case PingServer: {

                        setState(PingingServer);

                        if(request.callbacks.onBeginCallback.empty() == false){
                            request.callbacks.onBeginCallback(request.request);
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

                        if(request.callbacks.onEndCallback.empty() == false){
                            request.callbacks.onEndCallback(request.request);
                        }

                        break;
                    }
            }
        }else{
            if(state != Ready){
                setState(Ready);
            }
            microSleep(10000);
        }
    }
}

size_t CommunicationManager::pingDataCallback(void *buffer, size_t size, size_t nmemb, void *stream)
{
    return size*nmemb;
}

void CommunicationManager::readDbSchemas(const std::string& shallowCopyDir, const std::string& metaDataDir)
{
    ShallowCopyParserPtr ptrS = ShallowCopyParserPtr(new ShallowCopyParser());
    ptrS->parseFile(nullptr, shallowCopyDir);
    shallowCopy = ptrS->getShallowCopy();

    MetadataParserPtr ptrM = MetadataParserPtr(new MetadataParser());
    ptrM->parseFile(nullptr, metaDataDir);
    metaData = ptrM->getMetadata();
}

core::IDataManager::LocalTrial CommunicationManager::findLocalTrialsPaths(const core::Filesystem::Path& path)
{
    core::IDataManager::LocalTrial trial;

    //przeszukujemy katalog w poszukiwaniu plikow:
    std::vector<std::string> masks;
    masks.push_back(".c3d");
    masks.push_back(".amc");
    masks.push_back(".asf");
    masks.push_back(".avi");
    masks.push_back(".imgsequence");
    std::vector<std::string> filesPath = core::Filesystem::listFiles(path.string(), true, masks);
    BOOST_FOREACH(std::string path, filesPath)
    {
        trial.push_back(path);
    }
    return trial;
}
