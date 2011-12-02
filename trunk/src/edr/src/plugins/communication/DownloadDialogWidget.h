/********************************************************************
    created:  2011/11/09
    created:  9:11:2011   9:47
    filename: DownloadDialogWidget.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___DOWNLOADDIALOGWIDGET_H__
#define HEADER_GUARD___DOWNLOADDIALOGWIDGET_H__

#include "ui_DownloadDialog.h"
#include <plugins/communication/CommunicationManager.h>
#include <QtCore/QTimer>

class DownloadDialog : protected QDialog, private Ui::Dialog
{
    Q_OBJECT;

public:

    DownloadDialog(communication::CommunicationManager * communicationManager, const std::map<int, std::string> & filesToDownlaod);

    bool doDownloading(std::map<int, std::string> & downloadedFiles);

private slots:
    void refreshProgress();

    void cancelPressed();

private:

    void onBeginCallback(const communication::CommunicationManager::RequestPtr & request);
    void onEndCallback(const communication::CommunicationManager::RequestPtr & request);
    void onCancelCallback(const communication::CommunicationManager::RequestPtr & request);
    void onErrorCallback(const communication::CommunicationManager::RequestPtr & request, const std::string & error);

    void onFileEndCallback(const communication::CommunicationManager::RequestPtr & request);

private:

    bool processingStarted;
    bool processingFinished;
    std::string finalMessage;
    bool error;

    communication::CommunicationManager::RequestCallbacks complexCallbacks;
    communication::CommunicationManager::RequestCallbacks fileCallbacks;
    QTimer timer;
    OpenThreads::Mutex requestMutex;
    communication::CommunicationManager::RequestPtr complexRequest;
    std::map<int, std::string> toDownload;
    std::map<int, std::string> * downloaded;
    
    communication::CommunicationManager * communicationManager;
};

#endif HEADER_GUARD___DOWNLOADDIALOGWIDGET_H__