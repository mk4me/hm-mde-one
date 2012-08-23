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

//! �r�d�o danych EDR typu communication - dostarcza dancyh z Bazy Danych Ruchu
class CommunicationDataSource : public communication::ICommunicationDataSource, public core::ISource
{
    UNIQUE_ID("{441D9894-1019-4382-97EE-F18A511A49CB}","Communication Data Source");

	//! Zaprzyja�niona klasa realizuj�ca widok danych
	friend class DataSourceWidget;

private:
	//! Typ smart pointera do requesta �ci�gania danych
	typedef core::shared_ptr<DownloadRequest> DownloadRequestPtr;

	//! Struktura opisuj�ca stan danego po��czenia z WebService
	struct ConnectionStatus
	{
		//! Czy po��czenie by�o ju� inicjalizowane
		bool initialized;
		//! Adres us�ugi danego po��czenia
		std::string webServiceURL;
	};

public:
	//! Domy�lny konstruktor
    CommunicationDataSource();
	//! Destruktor
    virtual ~CommunicationDataSource();

    //! Inicjalizacja �r�d�a. Nast�puje ju� po wczytaniu plugin�w i skonstruowaniu
    //! (nie zainicjalizowaniu!) wszystkich �r�de�.
    virtual void init(core::IMemoryDataManager * memoryDM, core::IFileDataManager * fileDM, core::IServiceManager * serviceManager);

    //! �r�d�o nie musi mie� wizualnej reprezentacji.
    //! \return Widget tworzony przez �r�d�o b�d� NULL.
    virtual QWidget* getWidget(core::IActionsGroupManager * actionsGroupManager);

    //! \return Nazwa �r�d�a.
    virtual std::string getName() const;

	//! \param offline Czy �r�d�o danych ma dzia�a� w trybie offline?
	void setOfflineMode(bool offline = true);
	//! \return Czy �r�d�o dzia�a w trybie offline
	bool offlineMode() const;

    //! \param user Nazwa u�ytkownika
    //! \param password Has�o u�ytkownika
    virtual void login(const std::string & user, const std::string & password);
    //! Wylogowuje u�ytkownika
    virtual void logout();
    //! \return prawda je�li u�ytkownik zalogowany
    virtual bool isLogged() const;

    //! \return Dane aktualnego u�ytkownika ( w szczeg�lno�ci pusty obiekt je�li niezalogowano)
    virtual const User * currentUser() const;

	//! Implementacja interfejsu communication::ICommunicationDataSource
	virtual void showPatientCard(const PluginSubject::SubjectConstPtr & subject, const communication::PatientConstPtr & patient = communication::PatientConstPtr());
	virtual void showUserDataCard();
	virtual void showConfigurationCard();

private:

	//! \param connection Po��czenie z webService kt�re inicjalizuj� - ustawiam adres je�eli jest inny ni� aktualnie ustawiony w po��czeniu
	//! Ta metoda w przypadku zmiany adresu us�ugi powinna �ci�gn�c jej definicj�, sparsowa� j� i przygotowa� po�aczenie do dalszej pracy
	//! Jest to leniwa inicjalizacja, kiedy nie chcemy od razu nawiazaywa� po��czenia poniewa� user mo�e sobie zarzyczy� trybu offline
	void ensureConnection(const webservices::WSConnectionPtr & connection);

	//! \param login Login kt�ry pr�bujemy aktywowac po stronie bazy danych
	//! \param activationCode Kod aktywacyjny otrzymany w mailu
	//! \return Czy uda�o si� aktywowa� konto
	bool tryActivateAccount(const std::string & login, const std::string & activationCode);

	//! Metoda zak�ada konto u�ytkownika w obu bazach danych - medycznej i ruchu
	//! \param login Login u�ytkownika
	//! \param email Email u�ytkownika
	//! \param password Has�o u�ytkownika
	//! \param firstName Imi� u�ytkownika
	//! \param lastName Nazwisko u�ytkownika
	//! \return true je�li uda�o si� za�o�y� konto
	bool registerUser(const std::string & login, const std::string & email, const std::string & password,
		const std::string & firstName, const std::string & lastName);

	//! Od�wie�a fileManager w kontek�cie nowej p�ytkiej kopii bazy danych - mo�e dosz�y jakie� nowe pliki?
	void refreshFileManager();

	//! \param user Aktualny u�ytkownik zalogowany do bazy danych
	void setCurrentUser(const User & user);

	//! \param user U�ytkownik wg kt�rego ustawiany po��czenia
	static void setConnectionsCredentials(const User & user);

	//! \param certPath �cie�ka certyfikatu serwera dla po��cze� https
	static void setConnectionsSerwerCertificatePath(const core::Filesystem::Path & certPath);

	//! \param shallowCopy Nowa p�ytka kopia bazy danych do ustawienia
	void setShallowCopy(const communication::ShallowCopy & shallowCopy);
	//! Od�wie�a dane po ustawieniu nowej p�ytkiej kopii bazy danych - stosuje filtr je�li wymagane i od�wie�a widok
	void refreshFilteredData();

	//! Czy�ci lokalne dane u�ytkownika po wylogowaniu
	void clearUserLocalSpace();
	//! Tworzy lokalne dane u�ytkownika przy logowaniu
	void createUserLocalSpace();

	//! \return Czy p�ytka kopia bazy danych jest kompletna
	bool isShallowCopyComplete() const;
	//! \return Czy p�ytka kopia bazy danych jest aktualna
	bool isShallowCopyCurrent();

	//! Od�wie�a p�ytk� kopi� bazy danych : pobiera nieaktualne/brakuj�ce, tworzy, waliduje,  �aduje do localStorage, od�wie�a filtry i perspektywy
	//! \return Obiekt pomocniczy przy pobieraniu danych - pozwala odczytywa� informacje o statusie pobierania
	DownloadRequestPtr generateDownloadShallowCopyRequestToLocalUserSpace();

	//! Wyci�gamy dane z lokalnego storage (SQLite/SQLCipher - szyfrowany SQLite) - tylko przy logowaniu
	void extractDataFromLocalStorageToUserSpace();
	//! \param file Plik kt�ry chcemy wypakowa� z lokalnego storage
	//! \param sessionName Nazwa sesji z kt�rej pochodzi plik
	void extractFileFromLocalStorageToUserSpace(const webservices::MotionShallowCopy::File * file, const std::string & sessionName);

	//! \param oldShallowCopy Poprzednia p�ytka kopia dla kt�rej wypakowywano dane
	//! \param newShallowCopy Nowa p�ytka kopia dla kt�rej wypakowywujemy dane
	//! Algorytm opiera si� na r�nicy zbioru plik�w do kt�rych u�ytkownik ma dostep zdefiniowany p�ytk� kopi� bazy danych
	//! Usuwamy to do czego nei mamy ju� dostepu i wyci�gamy to co jest i do czego mamy ju� dostep
	//! Ewentualnie od�wie�amy to co si� zgadza
	void extractDataFromLocalStorageToUserSpace(const communication::ShallowCopy & prevShallowCopy, const communication::ShallowCopy & newShallowCopy);

	//! Wyci�gamy p�ytk� kopi� bazy danych z lokalnego storage (SQLite)
	void extractShallowCopyFromLocalStorageToUserSpace();
	//! \param shallowCopy P�ytka kopia bazy danych wyci�gni�ta z local user space
	//! \return Czy uda�o si� stworzy� poprawn� p�ytk� kopi� (parsowanie ok, sp�jno�� ok)
	bool buildShallowCopyFromLocalUserSpace(communication::ShallowCopy & shallowCopy);
	//! Zapisujemy p�ytk� kopi� bazy danych z local user space do local storage dla aktualnego u�ytkownika
	void saveShallowCopyInLocalStorage();
	//! Usuwamy p�ytk� kopie bazy danych z local user space
	void removeShallowCopyFromUserSpace();
	//! Usuwamy p�ytk� kopie bazy danych z local storage dla aktualnego u�ytkownika
	void removeShallowCopyFromLocalStorage();

	void extractFileFromLocalStorage(const std::string & fileName, const core::Filesystem::Path & destPath, std::vector<core::Filesystem::Path> & extractedFilesPaths);

	//! \param fileID Idnetyfikator pliku kt�ry �aduje do DM
	DownloadRequestPtr generateDownloadFileRequest(int fileID);
	//! \param fileIDs Zbi�r idnetyfikator�w plik�w kt�re �aduje do DM
	DownloadRequestPtr generateDownloadFilesRequest(const std::set<int> & fileIDs);

	//! Na potrzeby obs�ugi rozpakowywania plik�w VSK z archiwum zip
	//! \param inFile Dane wejsciowe - spakowany plik zip, albo plik z archiwum zip
	//! \param outFile Gdzie zapisujemy dane z rozpakowanego pliku zip
	static bool copyData(QIODevice &inFile, QIODevice &outFile);

private:
    // ------------------------ LOGIKA -------------------------------
	//! Aktualny u�ytkownik
	User currentUser_;
    //! Manager logowania - zawiera u�ytkownika
    DataSourceLoginManager * loginManager;

    //! Pe�na p�ytka kopia bazy danych
    communication::ShallowCopy fullShallowCopy;

    //! Manager statusu plik�w
    core::shared_ptr<FileStatusManager> fileStatusManager;

	//! Manager danych lokalnych
	DataSourcePathsManager * pathsManager;

    //! Manager danych lokalnych
    DataSourceLocalStorage * localStorage;

    //! Manager status�w pe�nej p�ytkiej kopi
    core::shared_ptr<DataSourceStatusManager> fullShallowCopyStatus;

	//! ------------------------------- Obs�uga DM ------------------------------

	//! Manager surowych danych w pami�ci
	core::IMemoryDataManager * memoryDM;

	//! Manager plik�w
	core::IFileDataManager * fileDM;

	//! Manager serwis�w - tylko i wy��cznie na potrzeby PluginSubject!!
	core::IServiceManager * serviceManager;

	//! ---------------------------Obs�uga plugin subject ------------------------

	std::map<int, core::ObjectWrapperPtr> sSubjects;
	std::map<int, core::ObjectWrapperPtr> sSessions;
	std::map<int, core::ObjectWrapperPtr> sMotion;

	//! ------------------------------- Web Services -----------------------------

	//! Czy user za��da� trybu offline?
	bool offlineMode_;
	//! Mapa po��cze� i ich status�w
	std::map<webservices::WSConnectionPtr, ConnectionStatus> connectionsStatus;
	//! Adres serwera kt�ry pingujemy
	std::string serwerPingUrl;
	//! Po��czenie z web services dla danych ruchu
	WSConnectionPtr motionFileStoremanWSConnection;
	//! Po��czenie z web services dla danych medycznych
	WSConnectionPtr medicalFileStoremanConnection;

	//! ------------------------------- FTPS -----------------------------

	//! Po��czenie ftps z baz� danych ruchu
	webservices::FtpsConnectionPtr motionFtps_;

	//! Po��czenie ftps z baz� danych medycznych
	webservices::FtpsConnectionPtr medicalFtps_;

    // -------------------------- UI ------------------------------------
	//! Widget �r�d�a danych
	DataSourceWidget * dataSourceWidget;

};

#endif HEADER_GUARD___COMMUNICATIONDATASOURCE_H__
