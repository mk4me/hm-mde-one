#include "CommunicationPCH.h"
#include <plugins/communication/CommunicationService.h>
#include <core/IPath.h>
#include <boost/bind.hpp>

CommunicationService::CommunicationService()
    : name("Communication"), finish(false)
{
    XmlUtils::TMPFILESDIR = core::getPathInterface()->getTempPath().string();
    WsdlPull::SCHEMADIR = (core::getPathInterface()->getResourcesPath() / "schemas/").string();   
    this->model = communication::CommunicationManager::getInstance();

    this->widget = new CommunicationWidgetEx(this);
    this->model->attach(this->widget);

    callbacks.onBeginCallback = boost::bind(&CommunicationService::onBeginRequestInvoker, this, _1);
    callbacks.onEndCallback = boost::bind(&CommunicationService::onEndRequestInvoker, this, _1);
    callbacks.onCancelCallback = boost::bind(&CommunicationService::onCancelRequestInvoker, this, _1);
    callbacks.onErrorCallback = boost::bind(&CommunicationService::onRequestErrorInvoker, this, _1, _2);

    qRegisterMetaType<communication::CommunicationManager::BasicRequest>("communication::CommunicationManager::BasicRequest");
    qRegisterMetaType<std::string>("std::string");

    turnOffProgressRefresh();
}

CommunicationService::~CommunicationService()
{
    communication::CommunicationManager::destoryInstance();
    this->model = nullptr;
}

void CommunicationService::turnOnProgressRefresh()
{
    refreshProgress = true;
    sleepTime = 20000;
}

void CommunicationService::turnOffProgressRefresh()
{
    refreshProgress = false;
    sleepTime = 500000;
}

void CommunicationService::updateProgress()
{
    if(model->getFilesToDownloadCount() > 0){
        std::ostringstream stream;
        stream << "Downloading file " << model->getActualDownloadFileNumber() << " from " << model->getFilesToDownloadCount();
        widget->setInfoLabel(stream.str());
        widget->setProgress(model->getProgress());
    }
}

void CommunicationService::onBeginRequestInvoker(const communication::CommunicationManager::BasicRequest & request)
{
    QMetaObject::invokeMethod(this, "onBeginRequest", Qt::BlockingQueuedConnection, Q_ARG(communication::CommunicationManager::BasicRequest, request));
}

void CommunicationService::onEndRequestInvoker(const communication::CommunicationManager::BasicRequest & request)
{
    QMetaObject::invokeMethod(this, "onEndRequest", Qt::BlockingQueuedConnection, Q_ARG(communication::CommunicationManager::BasicRequest, request));
}

void CommunicationService::onCancelRequestInvoker(const communication::CommunicationManager::BasicRequest & request)
{
    QMetaObject::invokeMethod(this, "onCancelRequest", Qt::BlockingQueuedConnection, Q_ARG(communication::CommunicationManager::BasicRequest, request));
}

void CommunicationService::onRequestErrorInvoker(const communication::CommunicationManager::BasicRequest & request, const std::string & error)
{
    QMetaObject::invokeMethod(this, "onCancelRequest", Qt::BlockingQueuedConnection, Q_ARG(communication::CommunicationManager::BasicRequest, request),
        Q_ARG(std::string, error));
}

void CommunicationService::onBeginRequest(const communication::CommunicationManager::BasicRequest & request)
{
    std::string info;

    bool refreshDownloadProgress = false;

    switch(request.type){
        case communication::CommunicationManager::DownloadFile:
            info = "File download";
            refreshDownloadProgress = true;
            break;

        case communication::CommunicationManager::DownloadTrial:
            info = "Trial download";
            refreshDownloadProgress = true;
            break;

        case communication::CommunicationManager::CopyDB:
            info = "Database synchronization";
            break;
    }

    widget->setInfoLabel(info);
    widget->setProgress(100);

    if(refreshDownloadProgress == true){
        turnOnProgressRefresh();
    }
}

void CommunicationService::onEndRequest(const communication::CommunicationManager::BasicRequest & request)
{
    std::string info;

    bool refreshTrials = false;
    bool refreshDownloadProgress = false;

    switch(request.type){
        case communication::CommunicationManager::DownloadFile:
            info = "File download complete";
            refreshTrials = true;
            refreshDownloadProgress = true;
            break;

        case communication::CommunicationManager::DownloadTrial:
            info = "Trial download complete";
            refreshTrials = true;
            refreshDownloadProgress = true;
            break;

        case communication::CommunicationManager::CopyDB:
            info = "Database synchronization complete";
            refreshTrials = true;
            break;

        case communication::CommunicationManager::PingServer:
            if(model->requestsQueueEmpty() == true){
                widget->setOnline(model->isServerResponse());
            }
            break;
    }

    if(refreshDownloadProgress == true){
        turnOffProgressRefresh();
    }

    widget->setInfoLabel(info);
    widget->setProgress(100);

    if(refreshTrials == true){
        model->loadLocalTrials();
    }
}

void CommunicationService::onCancelRequest(const communication::CommunicationManager::BasicRequest & request)
{
    std::string info;
    bool refreshDownloadProgress = false;

    switch(request.type){
        case communication::CommunicationManager::DownloadFile:
            info = "File download canceled";
            refreshDownloadProgress = true;
            break;

        case communication::CommunicationManager::DownloadTrial:
            info = "Trial download canceled";
            refreshDownloadProgress = true;
            break;

        case communication::CommunicationManager::CopyDB:
            info = "Database synchronization canceled";
            break;
    }

    if(refreshDownloadProgress == true){
        turnOffProgressRefresh();
    }

    widget->setInfoLabel(info);
    widget->setProgress(100);

    widget->refreshUI();
}

void CommunicationService::onRequestError(const communication::CommunicationManager::BasicRequest & request, const std::string & error)
{
    std::string info;

    bool refreshDownloadProgress = false;

    switch(request.type){
    case communication::CommunicationManager::DownloadFile:
        info = "File download error: ";
        refreshDownloadProgress = true;
        break;

    case communication::CommunicationManager::DownloadTrial:
        info = "Trial download error: ";
        refreshDownloadProgress = true;
        break;

    case communication::CommunicationManager::CopyDB:
        info = "Database synchronization error: ";
        break;
    }

    if(refreshDownloadProgress == true){
        turnOffProgressRefresh();
    }

    if(error.empty() == false){
        info += error;
    }else{
        info += "UNKNOWN";
    }

    LOG_ERROR(info);

    widget->setInfoLabel(info);
    widget->setProgress(100);

    widget->refreshUI();
}

void CommunicationService::copyDbData()
{
    model->copyDbData(callbacks);
}

void CommunicationService::downloadFile(unsigned int fileID)
{
    model->downloadFile(fileID, callbacks);
}

void CommunicationService::downloadTrial(unsigned int trialID)
{
    model->downloadTrial(trialID, callbacks);
}

void CommunicationService::init()
{
    model->setDataManager(core::getDataManager());
    start();
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
    model->cancelDownload();
}
