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
    //trialsDir = core::getUserDataString("trial");
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
    core::Filesystem::Path pathS = core::getApplicationDataString("db/schema/shallowcopy.xml");
    core::Filesystem::Path pathM = core::getApplicationDataString("db/schema/metadata.xml");

    //sorawdzenie przy uruchomieniu czy mamy pliki plytkiej kopii DB
    if(core::Filesystem::pathExists(pathS) == true && core::Filesystem::pathExists(pathM) == true) {
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
    switch(getState()) {
    case DownloadingTrial: {
            try {
                std::vector<wsdl::Trial> serverTrials = queryManager->listSessionContents();
                BOOST_FOREACH(wsdl::Trial& trial, serverTrials) {
                    if(trial.id == entityID) {
                        filesToDownload = trial.trialFiles.size();
                        actualFile = 0;
                        BOOST_FOREACH(int i, trial.trialFiles) {
                            actualFile++;
                            transportManager->downloadFile(i, core::getUserDataString("data/trials"));
                        }
                        break;
                    }
                }
                state = UpdateTrials;
            } catch(std::exception& e) {
                state = Error;
                errorMessage = e.what();
            }
            break;
        }
        case CopyDB: {
                try {
                    LOG_WARNING("Before CopyDB");
                    readDbSchemas(transportManager->getShallowCopy(), transportManager->getMetadata());
                    LOG_WARNING("After CopyDB");
                    state = UpdateTrials;
                } catch(std::exception& e) {
                    LOG_WARNING("CopyDB exception");
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
    LOG_WARNING("In readDbSchemas");
    ShallowCopyParserPtr ptrS = ShallowCopyParserPtr(new ShallowCopyParser());
    ptrS->parseFile(nullptr, shallowCopyDir);
    LOG_WARNING("After parsing file " << shallowCopyDir );
    shallowCopy = ptrS->getShallowCopy();

    LOG_WARNING("After getShallowCopy");
    MetadataParserPtr ptrM = MetadataParserPtr(new MetadataParser());
    ptrM->parseFile(nullptr, metaDataDir);
    LOG_WARNING("After metadata parse file " << metaDataDir);
    metaData = ptrM->getMetadata();
    LOG_WARNING("After get metadata");
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
