/********************************************************************
    created:  2011/10/28
    created:  28:10:2011   13:11
    filename: CommunicationDataSource.h
    author:   Mateusz Janiak

    purpose:
*********************************************************************/
#ifndef HEADER_GUARD___COMMUNICATIONDATASOURCE_H__
#define HEADER_GUARD___COMMUNICATIONDATASOURCE_H__

#include <core/Filesystem.h>
#include <plugins/newCommunication/ICommunicationDataSource.h>
#include <plugins/newCommunication/DataSourceShallowCopy.h>
#include <webserviceslib/IFtpsConnection.h>
#include "DataSourceUser.h"
#include <core/ISource.h>
#include "WSConnection.h"

//! Forward declarations
class DownloadRequest;
class DataSourcePathsManager;
class FileStatusManager;
class DataSourceLocalStorage;
class DataSourceStatusManager;
class DataSourceLoginManager;
class DataSourceWidget;

//! Źródło danych EDR typu communication - dostarcza dancyh z Bazy Danych Ruchu
class CommunicationDataSource : public communication::ICommunicationDataSource, public core::ISource
{
    UNIQUE_ID("{441D9894-1019-4382-97EE-F18A511A49CB}","Communication Data Source");

	//! Zaprzyjaźniona klasa realizująca widok danych
	friend class DataSourceWidget;

private:
	//! Typ smart pointera do requesta ściągania danych
	typedef core::shared_ptr<DownloadRequest> DownloadRequestPtr;

	//! Struktura opisująca stan danego połączenia z WebService
	struct ConnectionStatus
	{
		//! Czy połączenie było już inicjalizowane
		bool initialized;
		//! Adres usługi danego połączenia
		std::string webServiceURL;
	};

public:
	//! Domyślny konstruktor
    CommunicationDataSource();
	//! Destruktor
    virtual ~CommunicationDataSource();

    //! Inicjalizacja źródła. Następuje już po wczytaniu pluginów i skonstruowaniu
    //! (nie zainicjalizowaniu!) wszystkich źródeł.
    virtual void init(core::IMemoryDataManager * memoryDM, core::IFileDataManager * fileDM, core::IServiceManager * serviceManager);

    //! Źródło nie musi mieć wizualnej reprezentacji.
    //! \return Widget tworzony przez źródło bądź NULL.
    virtual QWidget* getWidget(core::IActionsGroupManager * actionsGroupManager);

    //! \return Nazwa źródła.
    virtual std::string getName() const;

	//! \param offline Czy źródło danych ma działać w trybie offline?
	void setOfflineMode(bool offline = true);
	//! \return Czy źródło działa w trybie offline
	bool offlineMode() const;

    //! \param user Nazwa użytkownika
    //! \param password Hasło użytkownika
    virtual void login(const std::string & user, const std::string & password);
    //! Wylogowuje użytkownika
    virtual void logout();
    //! \return prawda jeśli użytkownik zalogowany
    virtual bool isLogged() const;

    //! \return Dane aktualnego użytkownika ( w szczególności pusty obiekt jeśli niezalogowano)
    virtual const User * currentUser() const;

	//! Implementacja interfejsu communication::ICommunicationDataSource
	virtual void showPatientCard(const PluginSubject::SubjectConstPtr & subject, const communication::PatientConstPtr & patient = communication::PatientConstPtr());
	virtual void showUserDataCard();
	virtual void showConfigurationCard();

private:

	//! \param connection Połączenie z webService które inicjalizuję - ustawiam adres jeżeli jest inny niż aktualnie ustawiony w połączeniu
	//! Ta metoda w przypadku zmiany adresu usługi powinna ściągnąc jej definicję, sparsować ją i przygotować połaczenie do dalszej pracy
	//! Jest to leniwa inicjalizacja, kiedy nie chcemy od razu nawiazaywać połączenia ponieważ user może sobie zarzyczyć trybu offline
	void ensureConnection(const webservices::WSConnectionPtr & connection);

	//! \param login Login który próbujemy aktywować po stronie bazy danych
	//! \param activationCode Kod aktywacyjny otrzymany w mailu
	//! \return Czy udało się aktywować konto
	bool tryActivateAccount(const std::string & login, const std::string & activationCode);

	//! Metoda zakłada konto użytkownika w obu bazach danych - medycznej i ruchu
	//! \param login Login użytkownika
	//! \param email Email użytkownika
	//! \param password Hasło użytkownika
	//! \param firstName Imię użytkownika
	//! \param lastName Nazwisko użytkownika
	//! \return true jeśli udało się założyć konto
	bool registerUser(const std::string & login, const std::string & email, const std::string & password,
		const std::string & firstName, const std::string & lastName);

	//! Odświeża fileManager w kontekście nowej płytkiej kopii bazy danych - może doszły jakieś nowe pliki?
	void refreshFileManager();

	//! \param user Aktualny użytkownik zalogowany do bazy danych
	void setCurrentUser(const User & user);

	//! \param user Użytkownik wg którego ustawiany połączenia
	static void setConnectionsCredentials(const User & user);

	//! \param certPath Ścieżka certyfikatu serwera dla połączeń https
	static void setConnectionsSerwerCertificatePath(const core::Filesystem::Path & certPath);

	//! \param shallowCopy Nowa płytka kopia bazy danych do ustawienia
	void setShallowCopy(const communication::ShallowCopy & shallowCopy);
	//! Odświeża dane po ustawieniu nowej płytkiej kopii bazy danych - stosuje filtr jeśli wymagane i odświeża widok
	void refreshFilteredData();

	//! Czyści lokalne dane użytkownika po wylogowaniu
	void clearUserLocalSpace();
	//! Tworzy lokalne dane użytkownika przy logowaniu
	void createUserLocalSpace();

	//! \return Czy płytka kopia bazy danych jest kompletna
	bool isShallowCopyComplete() const;
	//! \return Czy płytka kopia bazy danych jest aktualna
	bool isShallowCopyCurrent();

	//! Odświeża płytką kopię bazy danych : pobiera nieaktualne/brakujące, tworzy, waliduje,  ładuje do localStorage, odświeża filtry i perspektywy
	//! \return Obiekt pomocniczy przy pobieraniu danych - pozwala odczytywać informacje o statusie pobierania
	DownloadRequestPtr generateDownloadShallowCopyRequestToLocalUserSpace();

	//! Wyciągamy dane z lokalnego storage (SQLite/SQLCipher - szyfrowany SQLite) - tylko przy logowaniu
	void extractDataFromLocalStorageToUserSpace();
	//! \param file Plik który chcemy wypakować z lokalnego storage
	//! \param sessionName Nazwa sesji z której pochodzi plik
	void extractFileFromLocalStorageToUserSpace(const webservices::MotionShallowCopy::File * file, const std::string & sessionName);

	//! \param oldShallowCopy Poprzednia płytka kopia dla której wypakowywano dane
	//! \param newShallowCopy Nowa płytka kopia dla której wypakowywujemy dane
	//! Algorytm opiera się na różnicy zbioru plików do których użytkownik ma dostęp zdefiniowany płytką kopią bazy danych
	//! Usuwamy to do czego nei mamy już dostępu i wyciągamy to co jest i do czego mamy już dostęp
	//! Ewentualnie odświeżamy to co się zgadza
	void extractDataFromLocalStorageToUserSpace(const communication::ShallowCopy & prevShallowCopy, const communication::ShallowCopy & newShallowCopy);

	//! Wyciągamy płytką kopię bazy danych z lokalnego storage (SQLite)
	void extractShallowCopyFromLocalStorageToUserSpace();
	//! \param shallowCopy Płytka kopia bazy danych wyciągnięta z local user space
	//! \return Czy udało się stworzyć poprawną płytką kopię (parsowanie ok, spójność ok)
	bool buildShallowCopyFromLocalUserSpace(communication::ShallowCopy & shallowCopy);
	//! Zapisujemy płytką kopię bazy danych z local user space do local storage dla aktualnego użytkownika
	void saveShallowCopyInLocalStorage();
	//! Usuwamy płytką kopie bazy danych z local user space
	void removeShallowCopyFromUserSpace();
	//! Usuwamy płytką kopie bazy danych z local storage dla aktualnego użytkownika
	void removeShallowCopyFromLocalStorage();

	void extractFileFromLocalStorage(const std::string & fileName, const core::Filesystem::Path & destPath, std::vector<core::Filesystem::Path> & extractedFilesPaths);

	//! \param fileID Idnetyfikator pliku który ładuje do DM
	DownloadRequestPtr generateDownloadFileRequest(int fileID);
	//! \param fileIDs Zbiór idnetyfikatorów plików które ładuje do DM
	DownloadRequestPtr generateDownloadFilesRequest(const std::set<int> & fileIDs);

	//! Na potrzeby obsługi rozpakowywania plików VSK z archiwum zip
	//! \param inFile Dane wejsciowe - spakowany plik zip, albo plik z archiwum zip
	//! \param outFile Gdzie zapisujemy dane z rozpakowanego pliku zip
	static bool copyData(QIODevice &inFile, QIODevice &outFile);

private:
    // ------------------------ LOGIKA -------------------------------
	//! Aktualny użytkownik
	User currentUser_;
    //! Manager logowania - zawiera użytkownika
    DataSourceLoginManager * loginManager;

    //! Pełna płytka kopia bazy danych
    communication::ShallowCopy fullShallowCopy;

    //! Manager statusu plików
    core::shared_ptr<FileStatusManager> fileStatusManager;

	//! Manager danych lokalnych
	DataSourcePathsManager * pathsManager;

    //! Manager danych lokalnych
    DataSourceLocalStorage * localStorage;

    //! Manager statusów pełnej płytkiej kopi
    core::shared_ptr<DataSourceStatusManager> fullShallowCopyStatus;

	//! ------------------------------- Obsługa DM ------------------------------

	//! Manager surowych danych w pamięci
	core::IMemoryDataManager * memoryDM;

	//! Manager plików
	core::IFileDataManager * fileDM;

	//! Manager serwisów - tylko i wyłącznie na potrzeby PluginSubject!!
	core::IServiceManager * serviceManager;

	//! ---------------------------Obsługa plugin subject ------------------------

	std::map<int, core::ObjectWrapperPtr> sSubjects;
	std::map<int, core::ObjectWrapperPtr> sSessions;
	std::map<int, core::ObjectWrapperPtr> sMotion;

	//! ------------------------------- Web Services -----------------------------

	//! Czy user zażądał trybu offline?
	bool offlineMode_;
	//! Mapa połączeń i ich statusów
	std::map<webservices::WSConnectionPtr, ConnectionStatus> connectionsStatus;
	//! Adres serwera który pingujemy
	std::string serwerPingUrl;
	//! Połączenie z web services dla danych ruchu
	WSConnectionPtr motionFileStoremanWSConnection;
	//! Połączenie z web services dla danych medycznych
	WSConnectionPtr medicalFileStoremanConnection;

	//! ------------------------------- FTPS -----------------------------

	//! Połączenie ftps z bazą danych ruchu
	webservices::FtpsConnectionPtr motionFtps_;

	//! Połączenie ftps z bazą danych medycznych
	webservices::FtpsConnectionPtr medicalFtps_;

    // -------------------------- UI ------------------------------------
	//! Widget źródła danych
	DataSourceWidget * dataSourceWidget;

};

#endif HEADER_GUARD___COMMUNICATIONDATASOURCE_H__
