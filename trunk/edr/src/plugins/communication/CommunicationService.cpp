#include "CommunicationPCH.h"
#include <plugins/communication/CommunicationService.h>

CommunicationService::CommunicationService() : name("Communication")
{
    this->transport = new communication::TransportWSDL_FTPS();
    this->query = new communication::QueryWSDL();
    //----------------------------------------------
    //TODO: dane wpisane na sztywno, dodac zapisywanie ustawien
    this->transport->setFTPCredentials("ftps://83.230.112.43/", "testUser", "testUser");
    this->transport->setWSCredentials("http://83.230.112.43/Motion/FileStoremanWS.svc?wsdl", "applet_user", "aplet4Motion");
    this->query->setWSCredentials("applet_user", "aplet4Motion");
    this->query->setBasicQueriesServiceUri("http://v21.pjwstk.edu.pl/Motion/res/BasicQueriesWSStandalone.wsdl");
    this->query->setBasicUpdatesServiceUri("");
    //----------------------------------------------
    this->model = communication::CommunicationManager::getInstance();
    this->model->setTransportManager(this->transport);
    this->model->setQueryManager(this->query);

    this->widget = new CommunicationWidget(this);
    this->model->attach(this->widget);
    try
    {
        this->load();
    }
    catch(std::runtime_error& e)
    {
        LOG_ERROR(e.what());
    }
}

CommunicationService::~CommunicationService()
{
    save();
    delete this->transport;
    this->transport = NULL;
    delete this->query;
    this->query = NULL;
    communication::CommunicationManager::destoryInstance();
    this->model = NULL;
}

const communication::TransportWSDL_FTPS* CommunicationService::getTransportManager() const
{
    return this->transport;
}

const communication::QueryWSDL* CommunicationService::getQueryManager() const
{
    return this->query;
}

void CommunicationService::setQueryCredentials(const std::string& user, const std::string& password, const std::string& bqsUri, const std::string& busUri)
{
    this->query->setWSCredentials(user, password);
    this->query->setBasicQueriesServiceUri(bqsUri);
    this->query->setBasicUpdatesServiceUri(busUri);
}

void CommunicationService::setTransportFTPCredentials(const std::string& user, const std::string& password, const std::string& uri)
{
    this->transport->setFTPCredentials(uri, user, password);
}

void CommunicationService::setTransportWSCredentials(const std::string& user, const std::string& password, const std::string& uri)
{
    this->transport->setWSCredentials(uri, user, password);
}

void CommunicationService::updateSessionContents()
{
    model->listSessionContents();
}

void CommunicationService::downloadFile(unsigned int fileID)
{
    model->downloadFile(fileID);
}

void CommunicationService::downloadTrial(unsigned int trialID)
{
    model->downloadTrial(trialID);
}

void CommunicationService::load()
{
    this->model->loadFromXml(this->name);
}

void CommunicationService::save()
{
    this->model->saveToXml(this->name);
}

AsyncResult CommunicationService::loadData(IServiceManager* serviceManager, IDataManager* dataManager)
{
    this->model->setTrialsDir(dataManager->getTrialsPath());
    return AsyncResult_Complete;
}

AsyncResult CommunicationService::update(double time, double timeDelta)
{
    //pinguj co pol minuty
    if(static_cast<int>(time + 0.5f)%30 == 0 && model->getState() == communication::CommunicationManager::Ready)
    {
        ping();
    }
    switch(model->getState())
    {
    case communication::CommunicationManager::UpdatingServerTrials:
        {
            widget->setInfoLabel("Updating server trials");
            break;
        }
        //nie uzywamy na razie
    //case communication::CommunicationManager::DownloadingFile:
    //    {
    //        widget->setBusy(true);
    //        widget->setInfoLabel("Downloading file");
    //        widget->setProgress(model->getProgress());
    //        break;
    //    }
    case communication::CommunicationManager::DownloadingTrial:
        {
            std::ostringstream stream;
            stream << "Downloading file " << model->getActualDownloadFileNumber() << " from " << model->getFilesToDownloadCount();
            widget->setInfoLabel(stream.str());
            widget->setProgress(model->getProgress());
            break;
        }
    case communication::CommunicationManager::Error:
        {
            //przekaz info
            widget->showErrorMessage(model->getErrorMessage());
            model->setState(communication::CommunicationManager::Ready);
            //przeladuj zasoby
            load();
            break;
        }
    case communication::CommunicationManager::UpdateTrials:
        {
            model->loadLocalTrials();
            break;
        }
    default:
        {
            break;
        }

    }

    return AsyncResult_Complete;
}

AsyncResult CommunicationService::init(IServiceManager* serviceManager, IDataManager* dataManager, osg::Node* sceneRoot, osgViewer::CompositeViewer* viewer)
{
    //model->setServiceManager(serviceManager);
    model->setDataManager(dataManager);
    ping();
    return AsyncResult_Complete;
}

void CommunicationService::loadTrial(const std::string& name)
{
    model->loadTrial(name);
}

void CommunicationService::cancelDownloading()
{
    model->cancelDownloading();
}