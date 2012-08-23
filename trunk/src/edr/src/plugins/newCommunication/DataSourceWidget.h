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

class CommunicationDataSource;

class DataSourceWidget : public QTabWidget, private Ui::DataSourceWidget, private utils::Observer<communication::IDownloadRequest>
{
    Q_OBJECT;
    friend class LocalDataLoader;
	friend class CommunicationDataSource;

private:

	//! Obiekt ³api¹cy klawisz ENTER w oknie logowania i wyzwalaj¹cy logowanie/wylogowywanie
	class LoginEventFilter : public QObject
	{
	public:
		//! Konstruktor
		//! \param sourceWidget Widget obs³uguj¹cy Ÿród³o, przez któy loguje/wylogowuje
		//! \param parent Rodzic tego obiektu
		LoginEventFilter(DataSourceWidget * sourceWidget, QObject *parent = nullptr);

		virtual bool eventFilter(QObject * watched, QEvent * event);

	private:
		//! Widget obs³uguj¹cy Ÿród³o danych
		DataSourceWidget * sourceWidget;
	};

	friend class LoginEventFilter;

public:

    DataSourceWidget(CommunicationDataSource * dataSource, QWidget * parent = nullptr);

    virtual ~DataSourceWidget();

public slots:

	void showUserData();
	void showConfiguration();
	void showPatientCard();

	void setPatientCard(webservices::MedicalShallowCopy::Patient * patient, webservices::MotionShallowCopy::Performer * subject);
	
private slots:

	//! Zmieniamy metode dzia³ania - online lub offline
	void connectionModeChanged();

    //! odœwie¿a status danych
    void refreshStatus();
    //! Odœwie¿a status po œci¹ganiu danych
    void refreshStatusAfterDownload();

    //! Zimana aktualnego filtra
    void onFilterChange(int idx);
    //! Edycja filtra
    void onFilterEdit();
    //! Dodanie filtra
    void onFilterAdd();
    //! Usunuêcie filtra
    void onFilterRemove();

    void onLogin();
    void onLoginRecovery();
    void onRegistration();
	void onActivate();

    //! Zmiana perspektywy
    void onPerspectiveChange(int idx);
    //! Zmiana profilu zawartoœci
    void onContentChange(int idx);

    void perspectiveContextMenu(const QPoint & pos);

	void onSaveProject();
	void onDeleteProject();
	void onLoadProject();

    void updateShallowCopy();

    void onDownload();
    void onLoad();
	void onUnload();

    //! Wywo³ywany po stronie w¹tku UI update
    void onUpdateDownloadRequest();
    //! Próba schowania widgeta
    void tryHideStatusWidget();
    //! Odœwie¿a progress pobierania
    void refreshDownloadProgress();

    void onPerspectiveSelectionChanged();

private:

	void onLogin(const QString & user, const QString & password);

	static QString formatFileSize(unsigned long long size);

	void generateItemSpecyficContextMenu(QMenu & menu, QTreeWidget * perspective);
	void generateGeneralContextMenu(QMenu & menu, QTreeWidget * perspective);
	void generateCommonContextMenu(QMenu & menu, QTreeWidget * perspective);


	static core::shared_ptr<communication::AntropometricData> createAntropometricData(const webservices::MotionShallowCopy::Attrs & attrs);
	static float getAntropometricValue(const std::string & attribute, const webservices::MotionShallowCopy::Attrs & attrs, float defValue = 0.0);

	void loadFiles(const std::set<int> & files);
	void unloadFiles(const std::set<int> & files, bool showMessage = true);

	void saveProject(const std::string & projectName, const std::set<int> & projectFiles);
	void loadProject(const std::string & projectName);
	void deleteProject(const std::string & projectName);

	void filteredFiles(std::set<int> & files) const;

	static bool isItemLoadable(const QTreeWidgetItem * item);

	void loadSubjectHierarchy(const std::map<int, std::vector<core::ObjectWrapperPtr>> & loadedFilesObjects);
	void unloadSubjectHierarchy(const std::set<int> & filesIDs);
	void unloadSubjectHierarchy();

    void refreshStatus(const std::set<int> & filesIDs);
    //! Odœwie¿a zawartoœæ aktualnej perspektywy dla aktualnego contentu - g³ównie dla statusów
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

    //! \param reqest Pobieranie którego dotyczy update
    virtual void update(const communication::IDownloadRequest * request);

    //! Resetuje stan widgeta - status pobierania
    void resetDownloadProgressStatus();

	void trySaveProjects();
	void tryLoadProjects();

	void addPatientObject(const webservices::MedicalShallowCopy::Patient * patient, PluginSubject::SubjectID subjectID);

private:
	//! Event filter do obs³ugi ENTER przy logowaniu/wylogownaiu
	LoginEventFilter * loginEventFilter;

    //! Aktualnie ustawiony content perspektyw
    std::map<QTreeWidget*, int> perspectivesContent;

    //! Przefiltrowana p³ytka kopia bazy danych
    communication::ShallowCopy filteredShallowCopy;

    //! Manager statusów przefiltrowanej p³ytkiej kopi
    core::shared_ptr<DataSourceStatusManager> filteredShallowCopyStatus;

    //! Manager karty pacjenta
    PatientCardManager patientCardManager;

    //! Manager filtrów danych
    DataSourceFilterManager filterManager;

    //! Manager filtrów danych
    DataSourcePerspectiveManager perspectiveManager;

    QTreeWidgetItem * currentPerspectiveItem;

    //Manager zawartoœci
    DataSourceContentManager contentManager;

    CommunicationDataSource * dataSource;
    DownloadStatusWidget * downloadStatusWidget;

    //! Aktualne pobieranie
    CommunicationDataSource::DownloadRequestPtr currentDownloadRequest;
    //! Request do synchronizacji p³ytkiej kopii
    CommunicationDataSource::DownloadRequestPtr shallowCopyRequest;	

    std::map<void*, CommunicationDataSource::DownloadRequestPtr> registeredRequests;

    //! Timer do odœwie¿ania statusu pobierania
    QTimer downloadRefreshTimer;

    //! Pliki œci¹gniête dla danego requesta - bêda potem ³adowane do localStorage
    std::vector<std::string> downloadedFiles;

    //! Czy pobieranie przerwane
    bool downloadCanceled;
    //! Czy pobieranie zakoñczy³o siê niepowodzeniem
    bool downloadCrashed;
    //! Informacja o niepowodzeniu
    QString downloadError;

    core::shared_ptr<LocalDataLoader> localDataLoader;

    //! Pliki zaanga¿owane w operacje u¿ytkownika - œci¹gnij / ³aduj / wy³aduj
    //! Wyznaczmy je przy realizacji menu kontekstowego
    //! Potem musz¹ zostaæ zresetowane!!
    //! Pliki do za³adowania
    std::set<int> filesToLoad;
    //! Pliki do wy³adowania - EXPEREIMENTAL!!
    std::set<int> filesToUnload;
    //! Pliki do œci¹gniêcia
    std::set<int> filesToDownload;

	//! Pliki aktualnie za³adowane do DM
	std::set<int> filesLoadedToDM;

	//! ------------ Logika danych w DM (ObjectWrappers) ----------------

	typedef std::pair<core::ObjectWrapperPtr, std::vector<core::ObjectWrapperPtr>> MappingValue;

	std::map<int, MappingValue> patientsMapping;
	std::map<int, MappingValue> subjectsMapping;
	std::map<int, MappingValue> sessionsMapping;
	std::map<int, MappingValue> motionsMapping;

	std::map<std::string, std::set<int>> projects;
};

#endif	//	HEADER_GUARD___DATASOURCEWIDGET_H__
