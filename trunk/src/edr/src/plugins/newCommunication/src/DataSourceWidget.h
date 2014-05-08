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
#include "StatusWidget.h"
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

	//! Obiekt łapiący klawisz ENTER w oknie logowania i wyzwalający logowanie/wylogowywanie
	class LoginEventFilter : public QObject
	{
	public:
		//! Konstruktor
		//! \param sourceWidget Widget obsługujący źródło, przez któy loguje/wylogowuje
		//! \param parent Rodzic tego obiektu
		LoginEventFilter(DataSourceWidget * sourceWidget, QObject *parent = nullptr);

		virtual bool eventFilter(QObject * watched, QEvent * event);

	private:
		//! Widget obsługujący źródło danych
		DataSourceWidget * sourceWidget;
	};

	friend class LoginEventFilter;

public:

    DataSourceWidget(DataSourceFilterManager* fm,
		CommunicationDataSource * dataSource, QWidget * parent = nullptr);

    virtual ~DataSourceWidget();

	static void showMessage(const QString & title, const QString & message, int delay);

public slots:
    // medusa code
	//void showUserData();
	void showConfiguration();
	void showPatientCard();

	void setPatientCard(webservices::MedicalShallowCopy::Patient * patient, webservices::MotionShallowCopy::Performer * subject);
	
private slots:

	//! Zmieniamy metode działania - online lub offline
	void connectionModeChanged();

    //! odświeża status danych
    void refreshStatus();
    //! Odświeża status po ściąganiu danych
    void refreshStatusAfterDownload();

    //! Zimana aktualnego filtra
    void onFilterChange(int idx);
    //! Edycja filtra
    void onFilterEdit();
    //! Dodanie filtra
    void onFilterAdd();
    //! Usunuęcie filtra
    void onFilterRemove();

    void onLogin();
    void onLoginRecovery();
    void onRegistration();
	void onActivate();
	void onPasswordReset();

    //! Zmiana perspektywy
    void onPerspectiveChange(int idx);
    //! Zmiana profilu zawartości
    void onContentChange(int idx);

    void perspectiveContextMenu(const QPoint & pos);

	void onSaveProject();
	void onDeleteProject();
	void onLoadProject();

    void updateShallowCopy();

    void onDownload();
    void onForced();
    void onLoad();
	void onUnload();

    //! Wywoływany po stronie wątku UI update
    void onUpdateDownloadRequest();
    //! Próba schowania widgeta
    void tryHideStatusWidget();
    //! Odświeża progress pobierania
    void refreshDownloadProgress();

    void onPerspectiveSelectionChanged();

private:

	void filterChange(const int idx);

	void onLogin(const QString & user, const QString & password);

	bool synchronizationRequiredDialog();

	static QString formatFileSize(unsigned long long size);

	void generateItemSpecyficContextMenu(QMenu & menu, QTreeWidget * perspective);
	void generateGeneralContextMenu(QMenu & menu, QTreeWidget * perspective);
	void generateCommonContextMenu(QMenu & menu, QTreeWidget * perspective);


	static utils::shared_ptr<communication::AntropometricData> createAntropometricData(const webservices::MotionShallowCopy::Attrs & attrs);
	static float getAntropometricValue(const std::string & attribute, const webservices::MotionShallowCopy::Attrs & attrs, float defValue = 0.0);

	void loadFiles(const std::set<int> & files);
	void unloadFiles(const std::set<int> & files, bool showMessage = true);

	void saveProject(const std::string & projectName, const std::set<int> & projectFiles);
	void loadProject(const std::string & projectName);
	void deleteProject(const std::string & projectName);

	void filteredFiles(std::set<int> & files) const;

	static bool isItemLoadable(const QTreeWidgetItem * item);

	void loadSubjectHierarchy(const std::map<int, std::vector<core::VariantConstPtr>> & loadedFilesObjects);

    void updateOrAddRoot( core::IHierarchyItemConstPtr root, std::set<core::IHierarchyItemConstPtr>& roots, core::IMemoryDataManager::HierarchyTransactionPtr hierarchyTransaction );

    void unloadSubjectHierarchy(const std::set<int> & filesIDs);
	void unloadSubjectHierarchy();

    void refreshStatus(const std::set<int> & filesIDs);
    //! Odświeża zawartość aktualnej perspektywy dla aktualnego contentu - głównie dla statusów
    void refreshCurrentPerspectiveContent();

    static void getItemsFiles(QTreeWidgetItem * item, std::set<int> & filesIDs, const communication::ShallowCopy & shallowCopy);
	static void getAdditionalItemsFiles(QTreeWidgetItem * item, std::set<int> & filesIDs);
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

    void saveCredentials();
    void loadCredentials();

    QString getCredentialsIniPath() const;

    void clearCredentials();
    QString crypt(const QString& input, bool encyprt) const;
    void setCompactMode( bool compact );

private:
	//! Event filter do obsługi ENTER przy logowaniu/wylogownaiu
	LoginEventFilter * loginEventFilter;

    //! Aktualnie ustawiony content perspektyw
    std::map<QTreeWidget*, int> perspectivesContent;

    //! Przefiltrowana płytka kopia bazy danych
    communication::ShallowCopy filteredShallowCopy;

    //! Manager statusów przefiltrowanej płytkiej kopi
    utils::shared_ptr<DataSourceStatusManager> filteredShallowCopyStatus;

    //! Manager karty pacjenta
    PatientCardManager patientCardManager;
	//! Manager filtrow
	DataSourceFilterManager* fm;

    //! Manager filtrów danych
    DataSourcePerspectiveManager perspectiveManager;

    QTreeWidgetItem * currentPerspectiveItem;

    //Manager zawartości
    DataSourceContentManager contentManager;

    CommunicationDataSource * dataSource;
    DownloadStatusWidget * downloadStatusWidget;
    communication::StatusWidget * statusWidget;

    //! Aktualne pobieranie
    CommunicationDataSource::DownloadRequestPtr currentDownloadRequest;
    //! Request do synchronizacji płytkiej kopii
    CommunicationDataSource::DownloadRequestPtr shallowCopyRequest;	

    std::map<void*, CommunicationDataSource::DownloadRequestPtr> registeredRequests;

    //! Timer do odświeżania statusu pobierania
    QTimer downloadRefreshTimer;

    //! Pliki ściągnięte dla danego requesta - będa potem ładowane do localStorage
    std::vector<std::string> downloadedFiles;

    //! Czy pobieranie przerwane
    bool downloadCanceled;
    //! Czy pobieranie zakończyło się niepowodzeniem
    bool downloadCrashed;
    //! Informacja o niepowodzeniu
    QString downloadError;

    utils::shared_ptr<LocalDataLoader> localDataLoader;

    //! Pliki zaangażowane w operacje użytkownika - ściągnij / ładuj / wyładuj
    //! Wyznaczmy je przy realizacji menu kontekstowego
    //! Potem muszą zostać zresetowane!!
    //! Pliki do załadowania
    std::set<int> filesToLoad;
    //! Pliki do wyładowania - EXPEREIMENTAL!!
    std::set<int> filesToUnload;
    //! Pliki do ściągnięcia
    std::set<int> filesToDownload;
    //! Pliki do ponownego ściągnięcia
    std::set<int> filesToForcedDownload;

	//! Pliki aktualnie załadowane do DM
	std::set<int> filesLoadedToDM;

	//! ------------ Logika danych w DM (ObjectWrappers) ----------------

	typedef std::pair<core::VariantPtr, std::vector<core::VariantPtr>> MappingValue;

	std::map<int, MappingValue> patientsMapping;
	std::map<int, MappingValue> subjectsMapping;
	std::map<int, MappingValue> sessionsMapping;
	std::map<int, MappingValue> motionsMapping;

	std::map<std::string, std::set<int>> projects;

    std::map<std::set<int>, std::set<core::IHierarchyItemConstPtr>> files2roots;
    std::map<QString, core::IHierarchyItemConstPtr> name2root;
};

#endif	//	HEADER_GUARD___DATASOURCEWIDGET_H__
