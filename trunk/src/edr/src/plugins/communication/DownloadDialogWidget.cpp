#include "CommunicationPCH.h"
#include <boost/bind.hpp>
#include "DownloadDialogWidget.h"
#include <QtGui/QMessageBox>

DownloadDialog::DownloadDialog(communication::CommunicationManager * communicationManager, const std::map<int, std::string> & filesToDownload)
    : communicationManager(communicationManager), toDownload(filesToDownload), error(false), processingStarted(false), processingFinished(false)
{
    setupUi(this);

     connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancelPressed()));

    complexCallbacks.onBeginCallback = boost::bind(&DownloadDialog::onBeginCallback, this, _1);
    complexCallbacks.onEndCallback = boost::bind(&DownloadDialog::onEndCallback, this, _1);
    complexCallbacks.onCancelCallback = boost::bind(&DownloadDialog::onCancelCallback, this, _1);
    complexCallbacks.onErrorCallback = boost::bind(&DownloadDialog::onErrorCallback, this, _1, _2);

    fileCallbacks.onEndCallback = boost::bind(&DownloadDialog::onFileEndCallback, this, _1);

    timer.setInterval(40);
    connect(&timer, SIGNAL(timeout()), this, SLOT(refreshProgress()));

    currentProgressBar->setValue(0);
    totalProgressBar->setValue(0);

    fileTotalLabel->setText(QString::number(1) + QString(" / ") + QString::number(toDownload.size()));
}

bool DownloadDialog::doDownloading(std::map<int, std::string> & downloadedFiles)
{
    downloaded = &downloadedFiles;

    std::vector<communication::CommunicationManager::CompleteRequest> requests;

    for(auto it = toDownload.begin(); it != toDownload.end(); it++){
        communication::CommunicationManager::CompleteRequest request;
        request.callbacks = fileCallbacks;
        request.request = communicationManager->createRequestFile(it->first, it->second);
        requests.push_back(request);
    }

    complexRequest = communicationManager->createRequestComplex(requests);

    communication::CommunicationManager::CompleteRequest request;

    request.request = complexRequest;
    request.callbacks = complexCallbacks;

    communicationManager->pushRequest(request);

    timer.start();
    int ret = exec();

    return ret == QDialog::Accepted;    
}

void DownloadDialog::refreshProgress()
{
    if(processingStarted == true){

        double totalProgress = complexRequest->getProgress();

        totalProgressBar->setValue(totalProgress);
        if(totalProgress == 100.0){
            currentProgressBar->setValue(100);
        }else if(totalProgress == 0.0){
            currentProgressBar->setValue(0);
        }else{
            currentProgressBar->setValue(communicationManager->getProgress());
        }

        fileTotalLabel->setText(QString::number((std::min)(downloaded->size() + 1, toDownload.size())) + QString(" / ") + QString::number(toDownload.size()));

        if(processingFinished == true){

            timer.stop();
            if(error == true){
                //error message
                QMessageBox msgBox;
                msgBox.setText("Error during data download");
                msgBox.setInformativeText(QString(finalMessage.c_str()));
                msgBox.setStandardButtons(QMessageBox::Ok);
                msgBox.setIcon(QMessageBox::Critical);
                msgBox.exec();
                reject();
                return;
            }else if(finalMessage.empty() == false){
                //info message
                QMessageBox msgBox;
                msgBox.setText("Data downloading finished");
                msgBox.setInformativeText(QString(finalMessage.c_str()));
                msgBox.setStandardButtons(QMessageBox::Ok);
                msgBox.setIcon(QMessageBox::Information);
                msgBox.exec();
            }

            if(downloaded->size() == toDownload.size()){
                accept();
            }else{
                reject();
            }
        }
    }
}

void DownloadDialog::cancelPressed()
{
    //complexRequest->cancel();
    communicationManager->cancelRequest(complexRequest);
    if(processingStarted == false){
        processingStarted = true;
        processingFinished = true;
        finalMessage = "Operation cancelled";
    }
}

void DownloadDialog::onBeginCallback(const communication::CommunicationManager::RequestPtr & request)
{
    processingStarted = true;
}

void DownloadDialog::onEndCallback(const communication::CommunicationManager::RequestPtr & request)
{ 
    processingFinished = true;
}

void DownloadDialog::onCancelCallback(const communication::CommunicationManager::RequestPtr & request)
{
    processingFinished = true;
    finalMessage = "Operation cancelled";
}

void DownloadDialog::onErrorCallback(const communication::CommunicationManager::RequestPtr & request, const std::string & error)
{
    processingFinished = true;
    this->error = true;
    finalMessage = error;
}

void DownloadDialog::onFileEndCallback(const communication::CommunicationManager::RequestPtr & request)
{
    auto fileRequest = core::dynamic_pointer_cast<communication::CommunicationManager::FileRequest>(request);
    (*downloaded)[fileRequest->getFileID()] = fileRequest->getFilePath();
}