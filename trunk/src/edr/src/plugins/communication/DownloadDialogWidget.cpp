#include "CommunicationPCH.h"
#include <boost/bind.hpp>
#include "DownloadDialogWidget.h"

DownloadDialog::LoadFiles::LoadFiles(DownloadDialog * dialog) : dialog(dialog)
{

}
void DownloadDialog::LoadFiles::run()
{
    for(auto it = dialog->toDownload.begin(); it != dialog->toDownload.end(); it++){
        OpenThreads::ScopedLock<OpenThreads::Mutex> lock(dialog->requestMutex);
        if(dialog->cancelRequests == true){
            break;
        }else{
            dialog->requests.push_back(dialog->communicationManager->requestFile(it->first, it->second, dialog->callbacks));
        }
    }
}

DownloadDialog::DownloadDialog(communication::CommunicationManager * communicationManager, const std::map<int, std::string> & filesToDownload)
    : communicationManager(communicationManager), toDownload(filesToDownload), refreshingProgress(false), cancelRequests(false), dialogStarted(false),
    error(false), currentProgress(0)
{
    setupUi(this);

     //connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancelPressed()));

    callbacks.onBeginCallback = boost::bind(&DownloadDialog::onBeginCallback, this, _1);
    callbacks.onEndCallback = boost::bind(&DownloadDialog::onEndCallback, this, _1);
    callbacks.onCancelCallback = boost::bind(&DownloadDialog::onCancelCallback, this, _1);
    callbacks.onErrorCallback = boost::bind(&DownloadDialog::onErrorCallback, this, _1, _2);

    timer.setInterval(40);
    connect(&timer, SIGNAL(timeout()), this, SLOT(refreshProgress()));

    currentProgressBar->setValue(0);
    totalProgressBar->setValue(0);

    fileTotalLabel->setText(QString::number(1) + QString(" / ") + QString::number(toDownload.size()));
}

bool DownloadDialog::doDownloading(std::map<int, std::string> & downloadedFiles)
{
    downloaded = &downloadedFiles;

    LoadFiles lf(this);
    timer.start();
    lf.start();

    int ret = 0;

    if(cancelRequests == false){
        dialogStarted = true;
        ret = exec();
    }

    if(cancelRequests == true){
        if(error == true){

        }
        return false;
    }

    return true;
}

void DownloadDialog::refreshProgress()
{
    if(refreshingProgress == true){
        currentProgressBar->setValue(communicationManager->getProgress());
        currentProgress = std::max(currentProgress, 100.0 * (downloaded->size() + (float)communicationManager->getProgress() / 100.0) / (float)toDownload.size());
        totalProgressBar->setValue(currentProgress);
    }

    
    fileTotalLabel->setText(QString::number(std::min(downloaded->size() + 1, toDownload.size())) + QString(" / ") + QString::number(toDownload.size()));

    if(downloaded->size() == toDownload.size()){
        timer.stop();
        accept();
    }

    {
        OpenThreads::ScopedLock<OpenThreads::Mutex> lock(requestMutex);
        if(cancelRequests == true){
            timer.stop();
            if(dialogStarted == true){
                this->reject();
            }
        }
    }
}

void DownloadDialog::cancelPressed()
{

    //pokaz message box - jesli potwirdzi konczymy
    timer.stop();
    {
        OpenThreads::ScopedLock<OpenThreads::Mutex> lock(requestMutex);
        cancelRequests = true;
    }
    
    cancelDownload();
    reject();
}

void DownloadDialog::cancelDownload()
{
    // wszystkie dotychczas do³ozone anulujemy 
    for(auto it = requests.begin(); it != requests.end(); it++){
        communicationManager->cancelRequest(*it);
    }
}

void DownloadDialog::startRefreshingProgress()
{
    refreshingProgress = true;
}

void DownloadDialog::stopRefreshingProgress()
{
    refreshingProgress = false;
}

void DownloadDialog::onBeginCallback(const communication::CommunicationManager::RequestPtr & request)
{
    startRefreshingProgress();
}

void DownloadDialog::onEndCallback(const communication::CommunicationManager::RequestPtr & request)
{
    stopRefreshingProgress();
    
    if(request->type == communication::CommunicationManager::DownloadFile){
        core::shared_ptr<communication::CommunicationManager::FileRequest> fileRequest(core::dynamic_pointer_cast<communication::CommunicationManager::FileRequest>(request));
        (*downloaded)[fileRequest->fileID] = fileRequest->filePath;
    }
}

void DownloadDialog::onCancelCallback(const communication::CommunicationManager::RequestPtr & request)
{
    {
        OpenThreads::ScopedLock<OpenThreads::Mutex> lock(requestMutex);
        cancelRequests = true;
    }

    cancelDownload();

    stopRefreshingProgress();

    finalMessage = "Operation cancelled through communication manager";

    //zamykamy nasz dialog z wartoœci¹ canceled
    //wiadomoœc o anulowaniu
}

void DownloadDialog::onErrorCallback(const communication::CommunicationManager::RequestPtr & request, const std::string & error)
{
    {
        OpenThreads::ScopedLock<OpenThreads::Mutex> lock(requestMutex);
        cancelRequests = true;
    }

    cancelDownload();

    stopRefreshingProgress();

    //zamykamy nasz dialog z wartoœci¹ canceled
    //wiadomoœc o anulowaniu
    this->error = true;
    finalMessage = error;
}