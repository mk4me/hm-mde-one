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
#include <OpenThreads/Mutex>
#include <OpenThreads/ScopedLock>
#include <QtCore/QTimer>
#include <QtCore/QThread>

class DownloadDialog : protected QDialog, private Ui::Dialog
{
    Q_OBJECT;

private:
    class LoadFiles : public QThread
    {
    public:
        LoadFiles(DownloadDialog * dialog);
        virtual void run();

    private:
        DownloadDialog * dialog;
    };

    friend class LoadFiles;

public:

    DownloadDialog(communication::CommunicationManager * communicationManager, const std::map<int, std::string> & filesToDownlaod);

    bool doDownloading(std::map<int, std::string> & downloadedFiles);

private slots:
    void refreshProgress();

    void cancelPressed();

private:
    void cancelDownload();

    void startRefreshingProgress();
    void stopRefreshingProgress();

    void onBeginCallback(const communication::CommunicationManager::RequestPtr & request);
    void onEndCallback(const communication::CommunicationManager::RequestPtr & request);
    void onCancelCallback(const communication::CommunicationManager::RequestPtr & request);
    void onErrorCallback(const communication::CommunicationManager::RequestPtr & request, const std::string & error);

private:
    bool dialogStarted;
    communication::CommunicationManager::RequestCallbacks callbacks;
    QTimer timer;
    OpenThreads::Mutex requestMutex;
    bool cancelRequests;
    std::vector<communication::CommunicationManager::RequestPtr> requests;
    std::map<int, std::string> toDownload;
    std::map<int, std::string> * downloaded;

    double currentProgress;

    std::string finalMessage;
    bool error;

    bool refreshingProgress;
    communication::CommunicationManager * communicationManager;
};

#endif HEADER_GUARD___DOWNLOADDIALOGWIDGET_H__