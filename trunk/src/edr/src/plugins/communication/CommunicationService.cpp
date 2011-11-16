#include "CommunicationPCH.h"
#include <plugins/communication/CommunicationService.h>
#include <core/IPath.h>
#include <plugins/dataExplorer/DataExplorerService.h>
#include "CommunicationDataSource.h"

//#include <boost/bind.hpp>

CommunicationService::CommunicationService()
    : name("Communication"), finish(false), sleepTime(500000)//, currentRequestID(-1), updatingRequestID(-1),
    //currentState(communication::CommunicationManager::Ready), previousState(communication::CommunicationManager::Ready)
{
    XmlUtils::TMPFILESDIR = core::getPathInterface()->getTempPath().string();
    WsdlPull::SCHEMADIR = (core::getPathInterface()->getResourcesPath() / "schemas/").string();   
    this->model = communication::CommunicationManager::getInstance();

    //this->widget = new CommunicationWidgetEx(this);
    //this->model->attach(this->widget);

    /*callbacks.onBeginCallback = boost::bind(&CommunicationService::onBeginRequest, this, _1);
    callbacks.onEndCallback = boost::bind(&CommunicationService::onEndRequest, this, _1);
    callbacks.onCancelCallback = boost::bind(&CommunicationService::onCancelRequest, this, _1);
    callbacks.onErrorCallback = boost::bind(&CommunicationService::onRequestError, this, _1, _2);

    resetWidgetState();*/
}

CommunicationService::~CommunicationService()
{
    communication::CommunicationManager::destoryInstance();
    this->model = nullptr;
}

//void CommunicationService::resetWidgetState()
//{
//    currentLabel = "";
//    refreshProgress = false;
//    refreshDBView = false;
//    refreshWidgetState = false;
//    refreshDBConnectionState = false;
//}
//
void CommunicationService::update(double deltaTime)
{
    //ScopedLock lock(currentWidgetStateMutex);

    //bool requestChanged = false;

    //if(currentRequestID != updatingRequestID){
    //    requestChanged = true;
    //    if(previousState != communication::CommunicationManager::PingingServer && previousState != communication::CommunicationManager::Ready){
    //        //model->loadLocalTrials();
    //    }else{
    //        //widget->refreshUI();
    //    }

    //    previousState = model->getState();
    //}

    //updatingRequestID = currentRequestID;

    //if(refreshProgress == true){
    //    std::stringstream s;

    //    s << currentLabel << " files [" << model->getActualDownloadFileNumber() << "/" << model->getFilesToDownloadCount() << "]";
    //    //widget->setInfoLabel(s.str());
    //    //widget->setProgress(model->getProgress());
    //}else{
    //    //widget->setInfoLabel(currentLabel);
    //    //widget->setProgress(100);
    //}
    //
    //if(refreshDBConnectionState == true){
    //    //widget->setOnline(model->isServerResponse());
    //}
}
//
//void CommunicationService::onBeginRequest(const communication::CommunicationManager::BasicRequest & request)
//{
//    ScopedLock lock(currentWidgetStateMutex);
//
//    resetWidgetState();
//
//    currentRequestID++;
//
//    switch(request.type){
//        case communication::CommunicationManager::DownloadFile:
//            currentLabel = "File download";
//            refreshProgress = true;
//            break;
//
//        case communication::CommunicationManager::DownloadTrial:
//            currentLabel = "Trial download";
//            refreshProgress = true;
//            break;
//
//        case communication::CommunicationManager::DownloadSession:
//            currentLabel = "Session download";
//            refreshProgress = true;
//            break;
//
//        case communication::CommunicationManager::DownloadPerformer:
//            currentLabel = "Performer download";
//            refreshProgress = true;
//            break;
//
//        case communication::CommunicationManager::CopyMotionDB:
//            currentLabel = "Motion database synchronization";
//            break;
//    }
//
//    if(currentLabel.empty() == false){
//        LOG_INFO(currentLabel);
//    }
//}
//
//void CommunicationService::onEndRequest(const communication::CommunicationManager::BasicRequest & request)
//{
//    ScopedLock lock(currentWidgetStateMutex);
//
//    switch(request.type){
//        case communication::CommunicationManager::DownloadFile:
//            currentLabel = "File download complete";
//            refreshProgress = false;
//            break;
//
//        case communication::CommunicationManager::DownloadTrial:
//            currentLabel = "Trial download complete";
//            refreshProgress = false;
//            break;
//
//        case communication::CommunicationManager::DownloadSession:
//            currentLabel = "Session download complete";
//            refreshProgress = false;
//            break;
//
//        case communication::CommunicationManager::DownloadPerformer:
//            currentLabel = "Performer download complete";
//            refreshProgress = false;
//            break;
//
//        case communication::CommunicationManager::CopyMotionDB:
//            currentLabel = "Motion database synchronization complete";
//            refreshProgress = false;
//            break;
//
//        case communication::CommunicationManager::PingServer:
//            refreshDBConnectionState = true;
//            break;
//    }
//
//    if(currentLabel.empty() == false){
//        LOG_INFO(currentLabel);
//    }
//}
//
//void CommunicationService::onCancelRequest(const communication::CommunicationManager::BasicRequest & request)
//{
//    ScopedLock lock(currentWidgetStateMutex);
//
//    switch(request.type){
//        case communication::CommunicationManager::DownloadFile:
//            currentLabel = "File download canceled";
//            refreshProgress = false;
//            break;
//
//        case communication::CommunicationManager::DownloadTrial:
//            currentLabel = "Trial download canceled";
//            refreshProgress = false;
//            break;
//
//        case communication::CommunicationManager::DownloadSession:
//            currentLabel = "Session download canceled";
//            refreshProgress = false;
//            break;
//
//        case communication::CommunicationManager::DownloadPerformer:
//            currentLabel = "Performer download canceled";
//            refreshProgress = false;
//            break;
//
//        case communication::CommunicationManager::CopyMotionDB:
//            currentLabel = "Motion database synchronization canceled";
//            break;
//    }
//
//    LOG_INFO(currentLabel);
//}
//
//void CommunicationService::onRequestError(const communication::CommunicationManager::BasicRequest & request, const std::string & error)
//{
//    ScopedLock lock(currentWidgetStateMutex);
//
//    switch(request.type){
//    case communication::CommunicationManager::DownloadFile:
//        currentLabel = "File download error: ";
//        refreshProgress = false;
//        break;
//
//    case communication::CommunicationManager::DownloadTrial:
//        currentLabel = "Trial download error: ";
//        refreshProgress = false;
//        break;
//
//    case communication::CommunicationManager::DownloadSession:
//        currentLabel = "Session download error: ";
//        refreshProgress = false;
//        break;
//
//    case communication::CommunicationManager::DownloadPerformer:
//        currentLabel = "Performer download error: ";
//        refreshProgress = false;
//        break;
//
//    case communication::CommunicationManager::CopyMotionDB:
//        currentLabel = "Motion database synchronization error: ";
//        break;
//    }
//
//    if(error.empty() == false){
//        currentLabel += error;
//    }else{
//        currentLabel += "UNKNOWN";
//    }
//
//    LOG_ERROR(currentLabel);
//}
//
//void CommunicationService::copyMotionDbData()
//{
//    model->copyMotionDbData(callbacks);
//}
//
//void CommunicationService::copyMedicalDbData()
//{
//    model->copyMedicalDbData(callbacks);
//}
//
//void CommunicationService::downloadFile(unsigned int fileID)
//{
//    model->downloadFile(fileID, callbacks);
//}
//
//void CommunicationService::downloadTrial(unsigned int trialID)
//{
//    model->downloadTrial(trialID, callbacks);
//}
//
//void CommunicationService::downloadSession(unsigned int sessionID)
//{
//    model->downloadSession(sessionID, callbacks);
//}
//
//void CommunicationService::downloadPerformer(unsigned int performerID)
//{
//    model->downloadPerformer(performerID, callbacks);
//}

void CommunicationService::init(core::IManagersAccessor * managersAccessor)
{
    this->managersAccessor = managersAccessor;
    //model->setDataManager(core::getDataManager());
    start();

}

void CommunicationService::lateInit()
{
    auto dataExplorer = core::queryServices<DataExplorerService>(managersAccessor->getServiceManager());
    auto subjectService = core::queryServices<ISubjectService>(managersAccessor->getServiceManager());
    if(dataExplorer != nullptr){
        try{
            dataExplorer->registerSource(new CommunicationDataSource(model, subjectService.get()));
        }catch(std::exception & e){
            LOG_ERROR(e.what());
        }
    }else{
        LOG_INFO("CommunicationSource could not be added to DataExplorer - DataExplorer service not found");
    }
}

//void CommunicationService::cancelDownloading()
//{
//    model->cancelDownload();
//}
