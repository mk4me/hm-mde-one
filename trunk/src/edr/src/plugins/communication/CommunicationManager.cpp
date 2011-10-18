/**
@author Marek Daniluk
*/

#include "CommunicationPCH.h"
#include <core/Filesystem.h>
#include <plugins/communication/CommunicationManager.h>
#include <boost/regex.hpp>

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
    : finish(false), filesToDownload(0), actualDownloadFileNumer(0)
{
    setState(Ready);

    //na razie recznie wpisane sciezki
    shallowCopyPath = core::getApplicationDataString("db/schema/shallowcopy.xml");
    metadataPath = core::getApplicationDataString("db/schema/metadata.xml");

    localDataPath = core::getUserDataString("data/trials");

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

void CommunicationManager::init()
{
    transportManager.reset(new communication::TransportWSDL_FTPS());
    queryManager.reset(new communication::QueryWSDL());

    //ping serwera
    pingCurl = curl_easy_init();
    if(pingCurl) {
        curl_easy_setopt(pingCurl, CURLOPT_URL, "http://v21.pjwstk.edu.pl/");
        curl_easy_setopt(pingCurl, CURLOPT_CONNECTTIMEOUT, 1);
        curl_easy_setopt(pingCurl, CURLOPT_WRITEFUNCTION, pingDataCallback);
    }
    //MotionMed
    //TODO: dane wpisane na sztywno, dodac zapisywanie ustawien
    transportManager->setFTPCredentials("ftps://v21.pjwstk.edu.pl/", "testUser", "testUser");
    transportManager->setWSCredentials("http://v21.pjwstk.edu.pl/Motion/FileStoremanWS.svc?wsdl", "applet_user", "aplet4Motion");
    queryManager->setWSCredentials("applet_user", "aplet4Motion");
    queryManager->setBasicQueriesServiceUri("http://v21.pjwstk.edu.pl/Motion/res/BasicQueriesWSStandalone.wsdl");
    queryManager->setBasicUpdatesServiceUri("");
}

void CommunicationManager::deinit()
{
    transportManager.reset();
    queryManager.reset();
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

const communication::ShallowCopy::Performer * CommunicationManager::getPerformer(int performerID)
{
    auto it = constShallowCopy->performers.find(performerID);

    if(it == constShallowCopy->performers.end()){
        throw std::runtime_error("Could not find performer with given id - try refreshing shallow copy");
    }

    return it->second;
}

const communication::ShallowCopy::Trial * CommunicationManager::getTrial(int trialID)
{
    auto it = constShallowCopy->trials.find(trialID);

    if(it == constShallowCopy->trials.end()){
        throw std::runtime_error("Could not find trial with given id - try refreshing shallow copy");
    }
    
    return it->second;
}

const communication::ShallowCopy::Session * CommunicationManager::getSession(int sessionID)
{
    auto it = constShallowCopy->sessions.find(sessionID);

    if(it == constShallowCopy->sessions.end()){
        throw std::runtime_error("Could not find session with given id - try refreshing shallow copy");
    }

    return it->second;
}

const communication::ShallowCopy::File * CommunicationManager::getFile(int fileID)
{
    auto it = constShallowCopy->files.find(fileID);

    if(it == constShallowCopy->files.end()){
        throw std::runtime_error("Could not find file with given id - try refreshing shallow copy");
    }

    return it->second;
}

const std::string & CommunicationManager::getTrialName(int trialID)
{
    return getTrial(trialID)->trialName;
}

const std::string & CommunicationManager::getSessionName(int sessionID)
{
    return getSession(sessionID)->sessionName;
}

const std::string & CommunicationManager::getFileName(int fileID)
{
    return getFile(fileID)->fileName;
}

void CommunicationManager::downloadPerformer(unsigned int performerID, const RequestCallbacks & callbacks)
{
    CompleteRequest request;
    request.request.type = DownloadPerformer;
    request.request.id = performerID;
    request.callbacks = callbacks;
    pushRequest(request);
}

void CommunicationManager::downloadSession(unsigned int sessionID, const RequestCallbacks & callbacks)
{
    CompleteRequest request;
    request.request.type = DownloadSession;
    request.request.id = sessionID;
    request.callbacks = callbacks;
    pushRequest(request);
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
    localTrials.clear();

    std::map<std::string, std::vector<std::string> > trials;

    //przeszukujemy liste prob pomiarowych, nie plikow
    std::vector<std::string> sessionPaths = core::Filesystem::listSubdirectories(localDataPath);
    BOOST_FOREACH(std::string path, sessionPaths)
    {
        core::Filesystem::Path p(path);
        std::string dirName(p.filename().string());

        boost::cmatch matches;
        boost::regex e("(.*)(\\d{4}-\\d{2}-\\d{2}-B\\d{4,}-S\\d{2,})(.*)");
        //sprawdzamy, czy zgadza sie nazwa folderu z nazwa sesji
        if(dirName.size() > 0 && boost::regex_search(dirName.c_str(), matches, e)) {

            std::string sessionID(matches[0].first, matches[0].second);

            std::vector<std::string> masks;

            masks.push_back(".c3d");
            masks.push_back(".avi");
            masks.push_back(".amc");
            masks.push_back(".imgsequence");

            try{

                std::vector<std::string> trialFiles = core::Filesystem::listFiles(path, false, masks);

                //iterujemy po plikach, grupujemy je wg triali
                for(auto fileIT = trialFiles.begin(); fileIT != trialFiles.end(); fileIT++){
                    boost::cmatch matches;
                    boost::regex e("(\\d{4}-\\d{2}-\\d{2}-B\\d{4,}-S\\d{2,}-T\\d{2,})");

                    core::Filesystem::Path p(*fileIT);
                    std::string file(p.filename().string());

                    if(file.size() > 0 && boost::regex_search(file.c_str(), matches, e)) {
                        
                        std::string trialID(matches[0].first, matches[0].second);

                        if(trialID.find(sessionID) != std::string::npos){
                            trials[trialID].push_back(*fileIT);
                        }
                    }
                }
            }catch(std::exception & e){
                LOG_WARNING("Error during session exploration: " << path << " Error: " << e.what());
            }catch(...){
                LOG_WARNING("Error during session exploration: " << path << " Error: UNKNOWN");
            }
        }
    }

    for(auto it = trials.begin(); it != trials.end(); it++){
        core::IDataManager::LocalTrial trial;
        for(auto iT = it->second.begin(); iT != it->second.end(); iT++){
            trial.push_back(*iT);
        }
        localTrials.push_back(trial);
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
    //inicjujemy po³¹czenia dla us³ugi plików i zapytañ
    init();

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

                        const communication::ShallowCopy::File * file = getFile(request.request.id);

                        path = core::Filesystem::Path(localDataPath) / core::Filesystem::Path(getSessionName(file->session->sessionID));

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
                        std::map<int, std::string> toDownload;

                        try {

                            auto trial = getTrial(request.request.id);

                            //sprawdz czy sesja juz istnieje
                            auto sessionPath = core::Filesystem::Path(localDataPath) / core::Filesystem::Path(trial->session->sessionName);

                            if(core::Filesystem::pathExists(sessionPath) == false){
                                core::Filesystem::createDirectory(sessionPath);
                            }

                            //sprawdz czy wszystkie pliki sesji juz sa - jesli nie kaz dosciagac brakujace pliki
                            auto localSessionFiles = core::Filesystem::listFiles(sessionPath);

                            std::set<std::string> localFileNamesOnly;

                            for(auto it = localSessionFiles.begin(); it != localSessionFiles.end(); it++){
                                localFileNamesOnly.insert(core::Filesystem::Path(*it).filename().string());
                            }

                            for(auto it = trial->session->files.begin(); it != trial->session->files.end(); it++){
                                if(localFileNamesOnly.find(it->second->fileName) == localFileNamesOnly.end()){
                                    toDownload[it->second->fileID] = (sessionPath / core::Filesystem::Path(it->second->fileName)).string();
                                }
                            }

                            //dodajemy pliki triala
                            for(auto it = trial->files.begin(); it != trial->files.end(); it++){
                                if(localFileNamesOnly.find(it->second->fileName) == localFileNamesOnly.end()){
                                    //brakuje nam pliku - trzeba go sciagnac
                                    toDownload[it->second->fileID] = (sessionPath / core::Filesystem::Path(it->second->fileName)).string();
                                }
                            }

                            if(toDownload.empty() == false){
                                actualDownloadFileNumer = 1;
                                filesToDownload = toDownload.size();
                            }

                            for(auto it = toDownload.begin(); it != toDownload.end(); it++){

                                if(cancelDownloading == true){
                                    break;
                                }

                                transportManager->downloadFile(it->first, it->second);
                                actualDownloadFileNumer++;
                            }

                            if(cancelDownloading == false || actualDownloadFileNumer == filesToDownload){
                                cancelDownloading = false;
                                if(request.callbacks.onEndCallback.empty() == false){
                                    request.callbacks.onEndCallback(request.request);
                                }
                            }else{

                                //czyœcimy jesli przerwano
                                for(auto it = toDownload.begin(); it != toDownload.end(); it++){
                                    if(core::Filesystem::pathExists(it->second) == true){
                                        core::Filesystem::deleteFile(it->second);
                                    }else{
                                        break;
                                    }
                                }

                                if(request.callbacks.onCancelCallback.empty() == false){
                                    request.callbacks.onCancelCallback(request.request);
                                }
                            }
                        } catch(std::exception& e) {
                            //czyœcimy jesli nie uda³o siê utworzyæ zasobów w ca³oœci
                            for(auto it = toDownload.begin(); it != toDownload.end(); it++){
                                if(core::Filesystem::pathExists(it->second) == true){
                                    core::Filesystem::deleteFile(it->second);
                                }else{
                                    break;
                                }
                            }

                            if(request.callbacks.onErrorCallback.empty() == false){
                                request.callbacks.onErrorCallback(request.request, e.what());
                            }
                        }

                        actualDownloadFileNumer = 0;
                        filesToDownload = 0;

                        break;
                    }

                case DownloadSession: {

                    cancelDownloading = false;    

                    setState(DownloadingSession);

                    if(request.callbacks.onBeginCallback.empty() == false){
                        request.callbacks.onBeginCallback(request.request);
                    }

                    core::Filesystem::Path path;
                    std::map<int, std::string> toDownload;

                    try {

                        auto session = getSession(request.request.id);
                        //sprawdz czy sesja juz istnieje
                        auto sessionPath = core::Filesystem::Path(localDataPath) / core::Filesystem::Path(session->sessionName);

                        if(core::Filesystem::pathExists(sessionPath) == false){
                            core::Filesystem::createDirectory(sessionPath);
                        }

                        //sprawdz czy wszystkie pliki sesji juz sa - jesli nie kaz dosciagac brakujace pliki
                        auto localSessionFiles = core::Filesystem::listFiles(sessionPath);

                        for(auto sessionFilesIT = session->files.begin(); sessionFilesIT != session->files.end(); sessionFilesIT++){
                            if( std::find(localSessionFiles.begin(), localSessionFiles.end(), sessionFilesIT->second->fileName) == localSessionFiles.end()){
                                //brakuje nam pliku - trzeba go sciagnac
                                toDownload[sessionFilesIT->second->fileID] = (sessionPath / core::Filesystem::Path(sessionFilesIT->second->fileName)).string();
                            } 
                        }

                        //dodajemy pliki triali
                        for(auto trialsIT = session->trials.begin(); trialsIT != session->trials.end(); trialsIT++){
                            //weryfikujemy pliki triali, jeœli brakuje dodajemy je

                            for(auto filesIT = trialsIT->second->files.begin(); filesIT != trialsIT->second->files.end(); filesIT++){
                                if( std::find(localSessionFiles.begin(), localSessionFiles.end(), filesIT->second->fileName) == localSessionFiles.end()){
                                    //brakuje nam pliku - trzeba go sciagnac
                                    toDownload[filesIT->second->fileID] = (sessionPath / core::Filesystem::Path(filesIT->second->fileName)).string();
                                }
                            }
                        }

                        actualDownloadFileNumer = 1;
                        filesToDownload = toDownload.size();

                        for(auto it = toDownload.begin(); it != toDownload.end(); it++){

                            if(cancelDownloading == true){
                                break;
                            }

                            transportManager->downloadFile(it->first, it->second);
                            actualDownloadFileNumer++;
                        }

                        if(cancelDownloading == false){
                            if(request.callbacks.onEndCallback.empty() == false){
                                request.callbacks.onEndCallback(request.request);
                            }
                        }else{

                            //czyœcimy jesli przerwano
                            for(auto it = toDownload.begin(); it != toDownload.end(); it++){
                                if(core::Filesystem::pathExists(it->second) == true){
                                    core::Filesystem::deleteFile(it->second);
                                }else{
                                    break;
                                }
                            }

                            if(request.callbacks.onCancelCallback.empty() == false){
                                request.callbacks.onCancelCallback(request.request);
                            }
                        }
                    } catch(std::exception& e) {
                        //czyœcimy jesli nie uda³o siê utworzyæ zasobów w ca³oœci
                        for(auto it = toDownload.begin(); it != toDownload.end(); it++){
                            if(core::Filesystem::pathExists(it->second) == true){
                                core::Filesystem::deleteFile(it->second);
                            }else{
                                break;
                            }
                        }

                        if(request.callbacks.onErrorCallback.empty() == false){
                            request.callbacks.onErrorCallback(request.request, e.what());
                        }
                    }

                    actualDownloadFileNumer = 0;
                    filesToDownload = 0;

                    break;
                    }

                case DownloadPerformer: {
                    cancelDownloading = false;    

                    setState(DownloadingPerformer);

                    if(request.callbacks.onBeginCallback.empty() == false){
                        request.callbacks.onBeginCallback(request.request);
                    }

                    core::Filesystem::Path path;
                    std::map<int, std::string> toDownload;

                    try {

                        auto performer = getPerformer(request.request.id);

                        for(auto perfConfIT = performer->performerConfs.begin(); perfConfIT != performer->performerConfs.end(); perfConfIT++){
                            //sprawdz czy sesja juz istnieje
                            auto sessionPath = core::Filesystem::Path(localDataPath) / core::Filesystem::Path(perfConfIT->second->session->sessionName);

                            if(core::Filesystem::pathExists(sessionPath) == false){
                                core::Filesystem::createDirectory(sessionPath);
                            }

                            //sprawdz czy wszystkie pliki sesji juz sa - jesli nie kaz dosciagac brakujace pliki
                            auto localSessionFiles = core::Filesystem::listFiles(sessionPath);

                            for(auto sessionFilesIT = perfConfIT->second->session->files.begin(); sessionFilesIT != perfConfIT->second->session->files.end(); sessionFilesIT++){
                                if( std::find(localSessionFiles.begin(), localSessionFiles.end(), sessionFilesIT->second->fileName) == localSessionFiles.end()){
                                    //brakuje nam pliku - trzeba go sciagnac
                                    toDownload[sessionFilesIT->second->fileID] = (sessionPath / core::Filesystem::Path(sessionFilesIT->second->fileName)).string();
                                } 
                            }

                            //dodajemy pliki triali
                            for(auto trialsIT = perfConfIT->second->session->trials.begin(); trialsIT != perfConfIT->second->session->trials.end(); trialsIT++){

                                //weryfikujemy pliki triali, jeœli brakuje dodajemy je

                                for(auto filesIT = trialsIT->second->files.begin(); filesIT != trialsIT->second->files.end(); filesIT++){
                                    if( std::find(localSessionFiles.begin(), localSessionFiles.end(), filesIT->second->fileName) == localSessionFiles.end()){
                                        //brakuje nam pliku - trzeba go sciagnac
                                        toDownload[filesIT->second->fileID] = (sessionPath / core::Filesystem::Path(filesIT->second->fileName)).string();
                                    }
                                }
                            }
                        }                        

                        actualDownloadFileNumer = 1;
                        filesToDownload = toDownload.size();

                        for(auto it = toDownload.begin(); it != toDownload.end(); it++){

                            if(cancelDownloading == true){
                                break;
                            }

                            transportManager->downloadFile(it->first, it->second);
                            actualDownloadFileNumer++;
                        }

                        if(cancelDownloading == false){
                            if(request.callbacks.onEndCallback.empty() == false){
                                request.callbacks.onEndCallback(request.request);
                            }
                        }else{

                            //czyœcimy jesli przerwano
                            for(auto it = toDownload.begin(); it != toDownload.end(); it++){
                                if(core::Filesystem::pathExists(it->second) == true){
                                    core::Filesystem::deleteFile(it->second);
                                }else{
                                    break;
                                }
                            }

                            if(request.callbacks.onCancelCallback.empty() == false){
                                request.callbacks.onCancelCallback(request.request);
                            }
                        }
                    } catch(std::exception& e) {
                        //czyœcimy jesli nie uda³o siê utworzyæ zasobów w ca³oœci
                        for(auto it = toDownload.begin(); it != toDownload.end(); it++){
                            if(core::Filesystem::pathExists(it->second) == true){
                                core::Filesystem::deleteFile(it->second);
                            }else{
                                break;
                            }
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

    //zwalniamy zasoby po³¹czeñ dla us³ugi plików i prostych zapytañ
    deinit();
}

size_t CommunicationManager::pingDataCallback(void *buffer, size_t size, size_t nmemb, void *stream)
{
    return size*nmemb;
}

void CommunicationManager::readDbSchemas(const std::string& shallowCopyDir, const std::string& metaDataDir)
{
    ShallowCopyParserPtr ptrS = ShallowCopyParserPtr(new ShallowCopyParser());
    ptrS->parseFile(nullptr, shallowCopyDir);
    constShallowCopy = ptrS->getShallowCopy();

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
    masks.push_back(".mp");
    masks.push_back(".vsk");
    masks.push_back(".zip");
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
