#include "CommunicationPCH.h"
#include <plugins/communication/CommunicationService.h>
#include <core/IPath.h>

CommunicationService::CommunicationService()
    : name("Communication")
{
    WsdlPull::SCHEMADIR = (core::getPathInterface()->getResourcesPath() / "schemas/").string();   
    this->model = communication::CommunicationManager::getInstance();

    this->widget = new CommunicationWidgetEx(this);
    this->model->attach(this->widget);
}

CommunicationService::~CommunicationService()
{
    communication::CommunicationManager::destoryInstance();
    this->model = NULL;
}

void CommunicationService::finalize()
{

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

void CommunicationService::update(double time, double timeDelta)
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
    case communication::CommunicationManager::DownloadingTrial: {
            std::ostringstream stream;
            stream << "Downloading file " << model->getActualDownloadFileNumber() << " from " << model->getFilesToDownloadCount();
            widget->setInfoLabel(stream.str());
            widget->setProgress(model->getProgress());
            break;
        }
    case communication::CommunicationManager::Error: {
            //przekaz info
            LOG_ERROR(model->getErrorMessage());
            widget->refreshUI();
            model->setState(communication::CommunicationManager::Ready);
            break;
        }
    case communication::CommunicationManager::UpdateTrials: {
            model->loadLocalTrials();
            model->setState(communication::CommunicationManager::Ready);
            break;
        }
    default: {
        widget->setOnline(model->isServerResponse());
        widget->setInfoLabel("");
        widget->setProgress(100);
        break;
        }

    }
}

void CommunicationService::init(core::IServiceManager* serviceManager, core::IDataManager* dataManager)
{
    model->setDataManager(dataManager);
    ping();
}

void CommunicationService::loadFiles(const std::vector<core::Filesystem::Path> files)
{
    model->loadFiles(files);
}

void CommunicationService::removeFiles(const std::vector<core::Filesystem::Path> files)
{
    model->removeFiles(files);
}

void CommunicationService::cancelDownloading()
{
    model->cancelDownloading();
}
