/**
@author Marek Daniluk
*/

#include "CommunicationPCH.h"
#include <core/Filesystem.h>
#include <plugins/communication/CommunicationManager.h>

using namespace communication;

typedef std::vector<Trial> ServerTrials;
typedef std::vector<core::IDataManager::LocalTrial> LocalTrials;

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
    pingCurl = curl_easy_init();
    if(pingCurl) {
        curl_easy_setopt(pingCurl, CURLOPT_URL, "http://83.230.112.43/");
        curl_easy_setopt(pingCurl, CURLOPT_CONNECTTIMEOUT, 1);
        curl_easy_setopt(pingCurl, CURLOPT_WRITEFUNCTION, pingDataCallback);
        //curl_easy_setopt(pingCurl, CURLOPT_WRITEDATA, &pingResp);
    }
    transportManager = core::shared_ptr<communication::TransportWSDL_FTPS>(new communication::TransportWSDL_FTPS());
    queryManager = core::shared_ptr<communication::QueryWSDL>(new communication::QueryWSDL());
    //TODO: dane wpisane na sztywno, dodac zapisywanie ustawien
    transportManager->setFTPCredentials("ftps://83.230.112.43/", "testUser", "testUser");
    transportManager->setWSCredentials("http://83.230.112.43/Motion/FileStoremanWS.svc?wsdl", "applet_user", "aplet4Motion");
    queryManager->setWSCredentials("applet_user", "aplet4Motion");
    queryManager->setBasicQueriesServiceUri("http://v21.pjwstk.edu.pl/Motion/res/BasicQueriesWSStandalone.wsdl");
    queryManager->setBasicUpdatesServiceUri("");
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

//void CommunicationManager::saveToXml(const std::string& filename)
//{
//    TiXmlDocument document;
//    TiXmlDeclaration* declaration = new TiXmlDeclaration("1.0", "", "");
//    TiXmlElement* root = new TiXmlElement("Communication");
//    document.LinkEndChild(declaration);
//    document.LinkEndChild(root);
//
//    //zapis daty
//    if(isLastUpdate) {
//        root->SetAttribute("date", lastUpdate.toString());
//    } else {
//        DateTime dt;
//        root->SetAttribute("date", dt.toString());
//    }
//
//    BOOST_FOREACH(Trial& trial, serverTrials) {
//        //<ServerTrial>
//        TiXmlElement* strial_element = new TiXmlElement("ServerTrial");
//        root->LinkEndChild(strial_element);
//        strial_element->SetAttribute("id", trial.id);
//        strial_element->SetAttribute("sessionID", trial.sessionID);
//        strial_element->SetAttribute("trialDescription", trial.trialDescription);
//        BOOST_FOREACH(int id, trial.trialFiles) {
//            //<File>
//            TiXmlElement* file_element = new TiXmlElement("File");
//            strial_element->LinkEndChild(file_element);
//            file_element->SetAttribute("id", id);
//            //</File>
//        }
//    }
//    document.SaveFile(filename);
//}
//
////TODO: trzeba sprawdzic poprawnosc wczytywanych danych
//void CommunicationManager::loadFromXml(const std::string& filename)
//{
//    TiXmlDocument document(filename);
//    if(!document.LoadFile()) {
//        LOG_ERROR(": !Blad wczytania pliku Communication\n");
//        return;
//    }
//    TiXmlHandle hDocument(&document);
//    TiXmlElement* strial_element;
//    TiXmlHandle hParent(0);
//
//    strial_element = hDocument.FirstChildElement().Element();
//    if(!strial_element) {
//        LOG_ERROR(": !Blad czytania z pliku Communication\n");
//        return;
//    }
//    hParent = TiXmlHandle(strial_element);
//
//    //data ostatniej aktualizacji danych z serwera
//    std::string temp;
//    hParent.ToElement()->QueryStringAttribute("date", &temp);
//    isLastUpdate = false;
//    if(!temp.empty()) {
//        try {
//            lastUpdate.setDate(temp);
//            isLastUpdate = true;
//        } catch(std::exception& e)
//        {
//            LOG_ERROR(e.what() << ": !Blad wczytania daty z pliku Communication\n");
//        }
//    }
//    //wczytanie zrzutu danych pomiarowych z serwera
//    this->serverTrials.clear();
//    strial_element = hParent.FirstChild("ServerTrial").ToElement();
//    while(strial_element) {
//        Trial serverTrial;
//        strial_element->QueryIntAttribute("id", &serverTrial.id);
//        strial_element->QueryIntAttribute("sessionID", &serverTrial.sessionID);
//        strial_element->QueryStringAttribute("trialDescription", &serverTrial.trialDescription);
//        TiXmlElement* file_element = strial_element->FirstChildElement("File");
//        while(file_element) {
//            int id;
//            file_element->QueryIntAttribute("id", &id);
//            serverTrial.trialFiles.push_back(id);
//            file_element = file_element->NextSiblingElement();
//        }
//        serverTrials.push_back(serverTrial);
//        strial_element = strial_element->NextSiblingElement("ServerTrial");
//    }
//    notify();
//}

void CommunicationManager::setTrialsDir(const std::string& dir)
{
    this->trialsDir = dir;
}

const std::string& CommunicationManager::getTrialsDir() const
{
    return this->trialsDir;
}

int CommunicationManager::getProgress() const
{
    return transportManager->getProgress();
}

void CommunicationManager::listSessionContents()
{
    setState(UpdatingServerTrials);
    start();
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
    setState(Ready);
    localTrials.clear();
    dataManager->findLocalTrials();
    if(dataManager && dataManager->getLocalTrialsCount() > 0) {
        for(int i = 0; i < dataManager->getLocalTrialsCount(); i++) {
            localTrials.push_back(dataManager->getLocalTrial(i));
        }
    }
    notify();
}

void CommunicationManager::loadTrial(const core::IDataManager::LocalTrial& localTrial)
{
    dataManager->loadTrial(localTrial);
}

void CommunicationManager::run()
{
    switch(getState()) {
        //nie uzywamy na razie
        //case DownloadingFile:
        //    {
        //        try
        //        {
        //            filesToDownload = 1;
        //            actualFile = 1;
        //            this->transportManager->downloadFile(entityID, this->trialsDir);
        //            state = Ready;
        //        }
        //        catch(std::runtime_error& e)
        //        {
        //            state = Error;
        //            errorMessage = e.what();
        //        }
        //        break;
        //    }
    case DownloadingTrial: {
            std::string pathToDownloadingTrial;
            try {
                BOOST_FOREACH(Trial& trial, serverTrials) {
                    if(trial.id == entityID) {
                        pathToDownloadingTrial = trialsDir;
                        pathToDownloadingTrial.append("/").append(trial.trialDescription);
                        filesToDownload = trial.trialFiles.size();
                        actualFile = 0;
                        BOOST_FOREACH(int i, trial.trialFiles) {
                            actualFile++;
                            transportManager->downloadFile(i, trialsDir);
                        }
                        break;
                    }
                }
                state = UpdateTrials;
            } catch(std::runtime_error& e) {
                if(!pathToDownloadingTrial.empty()) {
                    core::Filesystem::deleteDirectory(pathToDownloadingTrial);
                }
                state = Error;
                errorMessage = e.what();
            }
            break;
        }
    case UpdatingServerTrials: {
            try {
                serverTrials.clear();
                serverTrials = queryManager->listSessionContents();
                state = UpdateTrials;
            } catch(std::runtime_error& e) {
                state = Error;
                errorMessage = e.what();
            }
            break;
        }
        case CopyDB: {
                try {
                    std::string path = transportManager->getShallowCopy();
                    ShallowCopyParserPtr ptr = ShallowCopyParserPtr(new ShallowCopyParser());
                    ptr->parseFile(path);
                    shallowCopy = ptr->getShallowCopy();
                } catch(std::runtime_error& e) {
                    state = Error;
                    errorMessage = e.what();
                }
                try {
                    std::string path = transportManager->getMetadata();
                    MetadataParserPtr ptr = MetadataParserPtr(new MetadataParser());
                    ptr->parseFile(path);
                    metaData = ptr->getMetadata();
                state = UpdateTrials;
                } catch(std::runtime_error& e) {
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