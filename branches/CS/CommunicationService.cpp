#include "CommunicationPCH.h"
#include <plugins/communication/CommunicationService.h>

CommunicationService::CommunicationService()
    : name("Communication")
{
    this->model = communication::CommunicationManager::getInstance();

    this->widget = new CommunicationWidgetEx(this);
    this->model->attach(this->widget);
}

CommunicationService::~CommunicationService()
{
    communication::CommunicationManager::destoryInstance();
    this->model = NULL;
}

void CommunicationService::updateSessionContents()
{
    model->listSessionContents();
}

void CommunicationService::copyDbData()
{
    model->copyDbData();
}

void CommunicationService::downloadFile(unsigned int fileID)
{
    model->downloadFile(fileID);
}

void CommunicationService::downloadTrial(unsigned int trialID)
{
    model->downloadTrial(trialID);
}

AsyncResult CommunicationService::loadData(IServiceManager* serviceManager, core::IDataManager* dataManager)
{
    this->model->setTrialsDir(dataManager->getTrialsPath().string());
    return AsyncResult_Complete;
}

AsyncResult CommunicationService::update(double time, double timeDelta)
{
    //pinguj co pol minuty
    if(static_cast<int>(time + 0.5f)%30 == 0 && model->getState() == communication::CommunicationManager::Ready) {
        ping();
    }
    switch(model->getState()) {
    case communication::CommunicationManager::CopyDB: {
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
    case communication::CommunicationManager::DownloadingTrial: {
            std::ostringstream stream;
            stream << "Downloading file " << model->getActualDownloadFileNumber() << " from " << model->getFilesToDownloadCount();
            widget->setInfoLabel(stream.str());
            widget->setProgress(model->getProgress());
            break;
        }
    case communication::CommunicationManager::Error: {
            //przekaz info
            widget->setInfoLabel(model->getErrorMessage());
            model->setState(communication::CommunicationManager::Ready);
            break;
        }
    case communication::CommunicationManager::UpdateTrials: {
            model->loadLocalTrials();
            break;
        }
    default: {
            break;
        }

    }

    return AsyncResult_Complete;
}

AsyncResult CommunicationService::init(IServiceManager* serviceManager, core::IDataManager* dataManager, osg::Node* sceneRoot, osgViewer::CompositeViewer* viewer)
{
    model->setDataManager(dataManager);
    ping();
    return AsyncResult_Complete;
}

void CommunicationService::loadTrial(const core::IDataManager::LocalTrial& localTrial)
{
    model->loadTrial(localTrial);
}

void CommunicationService::cancelDownloading()
{
    model->cancelDownloading();
}
