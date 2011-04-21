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
    : trialsMutex()
{
    trialsDir = "data/trials/";
    setState(Ready);
    //ping serwera
    pingCurl = curl_easy_init();
    if(pingCurl) {
        curl_easy_setopt(pingCurl, CURLOPT_URL, "http://83.230.112.43/");
        curl_easy_setopt(pingCurl, CURLOPT_CONNECTTIMEOUT, 1);
        curl_easy_setopt(pingCurl, CURLOPT_WRITEFUNCTION, pingDataCallback);
        //curl_easy_setopt(pingCurl, CURLOPT_WRITEDATA, &pingResp);
    }
    //konfiguracja transportu i odpytywania
    transportManager = core::shared_ptr<communication::TransportWSDL_FTPS>(new communication::TransportWSDL_FTPS());
    queryManager = core::shared_ptr<communication::QueryWSDL>(new communication::QueryWSDL());
    //TODO: dane wpisane na sztywno, dodac zapisywanie ustawien
    transportManager->setFTPCredentials("ftps://83.230.112.43/", "testUser", "testUser");
    transportManager->setWSCredentials("http://83.230.112.43/Motion/FileStoremanWS.svc?wsdl", "applet_user", "aplet4Motion");
    queryManager->setWSCredentials("applet_user", "aplet4Motion");
    queryManager->setBasicQueriesServiceUri("http://v21.pjwstk.edu.pl/Motion/res/BasicQueriesWSStandalone.wsdl");
    queryManager->setBasicUpdatesServiceUri("");

    //na razie recznie wpisane sciezki
    boost::filesystem::path pathS = "data/db/schema/shallowcopy.xml";
    boost::filesystem::path pathM = "data/db/schema/metadata.xml";

    //sorawdzenie przy uruchomieniu czy mamy pliki plytkiej kopii DB
    if(boost::filesystem::exists(pathS) && boost::filesystem::exists(pathM)) {
        readDbSchemas(pathS.string(), pathM.string());
    } else {
        LOG_WARNING("Missing DB data from XML files.");
        copyDbData();
    }
    actualFile = 0;
    filesToDownload = 0;
}

CommunicationManager::~CommunicationManager()
{
    if(pingCurl) {
        curl_easy_cleanup(pingCurl);
    }
    if(isRunning()) {
        join();
    }
}

void CommunicationManager::setTrialsDir(const std::string& dir)
{
    this->trialsDir = dir;
}

const boost::filesystem::path& CommunicationManager::getTrialsDir() const
{
    return this->trialsDir;
}

int CommunicationManager::getProgress() const
{
    return transportManager->getProgress();
}

void CommunicationManager::copyDbData()
{
    setState(CopyDB);
    start();
}

void CommunicationManager::downloadTrial(unsigned int trialID)
{
    setState(DownloadingTrial);
    entityID = trialID;
    start();
}

void CommunicationManager::downloadFile(unsigned int fileID)
{
    setState(DownloadingFile);
    entityID = fileID;
    start();
}

void CommunicationManager::ping()
{
    setState(PingServer);
    start();
}

void CommunicationManager::loadLocalTrials()
{
    //TODO: uproszczenie wyszukiwania lokalnych triali
    localTrials.clear();
    //przeszukujemy liste prob pomiarowych, nie plikow
    std::vector<std::string> tempPaths = core::Filesystem::listSubdirectories(dataManager->getTrialsPath().string());
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

void CommunicationManager::loadFiles(const std::vector<core::IDataManager::Path> files)
{
    dataManager->loadFiles(files);
}

void CommunicationManager::removeFiles(const std::vector<core::IDataManager::Path> files)
{
    dataManager->removeFiles(files);
}

void CommunicationManager::run()
{
    switch(getState()) {
    case DownloadingTrial: {
            std::string pathToDownloadingTrial;
            try {
                std::vector<wsdl::Trial> serverTrials = queryManager->listSessionContents();
                BOOST_FOREACH(wsdl::Trial& trial, serverTrials) {
                    if(trial.id == entityID) {
                        pathToDownloadingTrial = trialsDir.string();
                        pathToDownloadingTrial.append("/").append(trial.trialDescription);
                        filesToDownload = trial.trialFiles.size();
                        actualFile = 0;
                        BOOST_FOREACH(int i, trial.trialFiles) {
                            actualFile++;
                            transportManager->downloadFile(i, trialsDir.string());
                        }
                        break;
                    }
                }
                state = UpdateTrials;
            } catch(std::exception& e) {
                if(!pathToDownloadingTrial.empty()) {
                    core::Filesystem::deleteDirectory(pathToDownloadingTrial);
                }
                state = Error;
                errorMessage = e.what();
            }
            break;
        }
        case CopyDB: {
                try {
                    readDbSchemas(transportManager->getShallowCopy(), transportManager->getMetadata());
                    state = UpdateTrials;
                } catch(std::exception& e) {
                    state = Error;
                    errorMessage = e.what();
                }
                break;
            }
    case PingServer: {
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
            state = Ready;
            break;
        }
    default: {
            return;
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

    //
    ////budujemy relacje miedzy encjami bazodanowymi od nowa
    //performers.clear();

    //BOOST_FOREACH(communication::ShallowCopy::Performer performer, ptrS->getShallowCopy().performers)
    //{
    //    communication::PerformerPtr p = PerformerPtr(new communication::Performer(performer));
    //    performers.push_back(p);
    //    std::vector<communication::SessionPtr> sessions;
    //    //sesje polaczone z performerami
    //    BOOST_FOREACH(communication::ShallowCopy::PerformerConf performerC, ptrS->getShallowCopy().performerConfs)
    //    {
    //        BOOST_FOREACH(communication::ShallowCopy::Session session, ptrS->getShallowCopy().sessions)
    //        {
    //            if(performerC.performerID == performer.performerID && performerC.sessionID == session.sessionID) {
    //                communication::SessionPtr s = communication::SessionPtr(new communication::Session(session));
    //                std::vector<communication::TrialPtr> trials;
    //                //kojarzenie sesji z trialami
    //                BOOST_FOREACH(communication::ShallowCopy::Trial trial, shallowCopy.trials)
    //                {
    //                    if(trial.sessionID == session.sessionID) {
    //                        communication::TrialPtr t = communication::TrialPtr(new communication::Trial(trial));
    //                        trials.push_back(t);
    //                    }
    //                }
    //                s->setTrials(trials);
    //                sessions.push_back(s);
    //            }
    //        }
    //    }
    //    p->setSessions(sessions);
    //}
}

core::IDataManager::LocalTrial CommunicationManager::findLocalTrialsPaths(const core::IDataManager::Path& path)
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
