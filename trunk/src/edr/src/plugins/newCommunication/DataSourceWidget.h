/********************************************************************
    created:  2012/03/09
    created:  9:3:2012   11:06
    filename: DataSourceWidget.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___DATASOURCEWIDGET_H__
#define HEADER_GUARD___DATASOURCEWIDGET_H__

#include "ui_DataSourceWidget.h"
#include "DataSourceContentManager.h"
#include "DataSourcePerspectiveManager.h"
#include "DataSourceFilterManager.h"
#include "PatientCardManager.h"
#include "DataSource.h"
#include "DownloadRequest.h"
#include <QtCore/QTimer>
#include <QtCore/QThread>
#include <QtGui/QPixmap>
#include <QtGui/QIcon>

#include <set>

class DownloadStatusWidget;

class LocalDataLoader : public QThread
{
    Q_OBJECT

public:

    LocalDataLoader(DataSourceWidget * sourceWidget);

    virtual ~LocalDataLoader();

protected:

    virtual void run();

private slots:

    void prepareStatusWidget();

    void updateStatusWidget();

    void showFinalMessage();

private:

    DataSourceWidget * sourceWidget;
    int counter;
    QString info;
};

class DataSourceWidget : public QTabWidget, private Ui::DataSourceWidget, private utils::Observer<communication::IDownloadRequest>
{
    Q_OBJECT;
    friend class LocalDataLoader;

public:

    DataSourceWidget(CommunicationDataSource * dataSource, QWidget * parent = nullptr);

    virtual ~DataSourceWidget();

private slots:

    //! Odswieza status danych
    void refreshStatus();
    //! Od�wie�a status po �ci�ganiu danych
    void refreshStatusAfterDownload();

    //! Zimana aktualnego filtra
    void onFilterChange(int idx);
    //! Edycja filtra
    void onFilterEdit();
    //! Dodanie filtra
    void onFilterAdd();
    //! Usunu�cie filtra
    void onFilterRemove();

    void onLogin();
    void onLoginRecover();
    void onRegistration();

    //! Zmiana perspektywy
    void onPerspectiveChange(int idx);
    //! Zmiana profilu zawarto�ci
    void onContentChange(int idx);

    void perspectiveContextMenu(const QPoint & pos);

    void updateShallowCopy();

    void onDownload();
    void onLoad();
    void onUnload();

    //! Wywo�ywany po stronie w�tku UI update
    void onUpdateDownloadRequest();
    //! Pr�ba schowania widgeta
    void tryHideStatusWidget();
    //! Od�wie�a progress pobierania
    void refreshDownloadProgress();

    void onPerspectiveCurrentItemChanged(QTreeWidgetItem * current, QTreeWidgetItem * previous);

private:

	static bool isItemLoadable(const QTreeWidgetItem * item);

	void loadSubjectHierarchy(const std::map<int, std::vector<core::ObjectWrapperPtr>> & loadedFilesObjects);
	void unloadSubjectHierarchy(const std::set<int> & filesIDs);
	void unloadSubjectHierarchy();

    void refreshStatus(const std::set<int> & filesIDs);
    //! Od�wie�a zawarto�� aktualnej perspektywy dla aktualnego contentu - g��wnie dla status�w
    void refreshCurrentPerspectiveContent();

    static void getItemsFiles(QTreeWidgetItem * item, std::set<int> & filesIDs, const communication::ShallowCopy & shallowCopy);
    static void getPatientAndSubject(QTreeWidgetItem * item, const webservices::MedicalShallowCopy::Patient *& patient,
    const webservices::MotionShallowCopy::Performer *& subject);	

    static QIcon statusIcon(const communication::DataStorage storage, const communication::DataUsage usage);

    //! \param pmL Lewa pixmapa do scalenia
    //! \param pmR Prawa pixmapa do scalenia
    //! \return Scalona pixmapa
    static QPixmap mergePixmaps(const QPixmap & pmL, const QPixmap & pmR);

    void initializeStatusIcons();

    void registerPerspective(communication::IDataSourcePerspective * perspective);
    void registerPatientCard(communication::IPatientCard * patientCard);

    void invalidatePerspectivesContent();

    bool refreshShallowCopy();

    void finishDownloadRequest();

    void saveAndRemoveShallowCopy();

	void performShallowCopyUpdate();

    void processShallowCopyDownload(const CommunicationDataSource::DownloadRequestPtr & request);
    void processDataDownload(QTreeWidgetItem * item, const CommunicationDataSource::DownloadRequestPtr & request);
    void processDownload(const CommunicationDataSource::DownloadRequestPtr & request);

    //! \param reqest Pobieranie kt�rego dotyczy update
    virtual void update(const communication::IDownloadRequest * request);

    //! Resetuje stan widgeta - status pobierania
    void resetDownloadProgressStatus();

private:
    //! Aktualnie ustawiony content perspektyw
    std::map<QTreeWidget*, int> perspectivesContent;

    //! Przefiltrowana p�ytka kopia bazy danych
    communication::ShallowCopy filteredShallowCopy;

    //! Manager status�w przefiltrowanej p�ytkiej kopi
    core::shared_ptr<DataSourceStatusManager> filteredShallowCopyStatus;

    //! Manager karty pacjenta
    PatientCardManager patientCardManager;

    //! Manager filtr�w danych
    DataSourceFilterManager filterManager;

    //! Manager filtr�w danych
    DataSourcePerspectiveManager perspectiveManager;

    QTreeWidgetItem * currentPerspectiveItem;

    //Manager zawarto�ci
    DataSourceContentManager contentManager;

    CommunicationDataSource * dataSource;
    DownloadStatusWidget * downloadStatusWidget;

    //! Aktualne pobieranie
    CommunicationDataSource::DownloadRequestPtr currentDownloadRequest;
    //! Request do synchronizacji p�ytkiej kopii
    CommunicationDataSource::DownloadRequestPtr shallowCopyRequest;	

    std::map<void*, CommunicationDataSource::DownloadRequestPtr> registeredRequests;

    //! Timer do od�wie�ania statusu pobierania
    QTimer downloadRefreshTimer;

    //! Pliki �ci�gni�te dla danego requesta - b�da potem �adowane do localStorage
    std::vector<std::string> downloadedFiles;

    //! Czy pobieranie przerwane
    bool downloadCanceled;
    //! Czy pobieranie zako�czy�o si� niepowodzeniem
    bool downloadCrashed;
    //! Informacja o niepowodzeniu
    QString downloadError;

    core::shared_ptr<LocalDataLoader> localDataLoader;

    //! Pliki zaanga�owane w operacje u�ytkownika - �ci�gnij / �aduj / wy�aduj
    //! Wyznaczmy je przy realizacji menu kontekstowego
    //! Potem musz� zosta� zresetowane!!
    //! Pliki do za�adowania
    std::set<int> filesToLoad;
    //! Pliki do wy�adowania - EXPEREIMENTAL!!
    std::set<int> filesToUnload;
    //! Pliki do �ci�gni�cia
    std::set<int> filesToDownload;

	//! Pliki aktualnie za�adowane do DM
	std::set<int> filesLoadedToDM;

	//! ------------ Logika danych w DM (ObjectWrappers) ----------------
	std::map<int, core::ObjectWrapperPtr> patientsMapping;
	std::map<int, core::ObjectWrapperPtr> subjectsMapping;
	std::map<int, core::ObjectWrapperPtr> sessionsMapping;
	std::map<int, core::ObjectWrapperPtr> motionsMapping;
};

#endif	//	HEADER_GUARD___DATASOURCEWIDGET_H__
