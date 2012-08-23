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

//! èrÛd≥o danych EDR typu communication - dostarcza dancyh z Bazy Danych Ruchu
class CommunicationDataSource : public communication::ICommunicationDataSource, public core::ISource
{
    UNIQUE_ID("{441D9894-1019-4382-97EE-F18A511A49CB}","Communication Data Source");

	//! Zaprzyjaüniona klasa realizujπca widok danych
	friend class DataSourceWidget;

private:
	//! Typ smart pointera do requesta úciπgania danych
	typedef core::shared_ptr<DownloadRequest> DownloadRequestPtr;

	//! Struktura opisujπca stan danego po≥πczenia z WebService
	struct ConnectionStatus
	{
		//! Czy po≥πczenie by≥o juø inicjalizowane
		bool initialized;
		//! Adres us≥ugi danego po≥πczenia
		std::string webServiceURL;
	};

public:
	//! Domyúlny konstruktor
    CommunicationDataSource();
	//! Destruktor
    virtual ~CommunicationDataSource();

    //! Inicjalizacja ürÛd≥a. NastÍpuje juø po wczytaniu pluginÛw i skonstruowaniu
    //! (nie zainicjalizowaniu!) wszystkich ürÛde≥.
    virtual void init(core::IMemoryDataManager * memoryDM, core::IFileDataManager * fileDM, core::IServiceManager * serviceManager);

    //! èrÛd≥o nie musi mieÊ wizualnej reprezentacji.
    //! \return Widget tworzony przez ürÛd≥o bπdü NULL.
    virtual QWidget* getWidget(core::IActionsGroupManager * actionsGroupManager);

    //! \return Nazwa ürÛd≥a.
    virtual std::string getName() const;

	//! \param offline Czy ürÛd≥o danych ma dzia≥aÊ w trybie offline?
	void setOfflineMode(bool offline = true);
	//! \return Czy ürÛd≥o dzia≥a w trybie offline
	bool offlineMode() const;

    //! \param user Nazwa uøytkownika
    //! \param password Has≥o uøytkownika
    virtual void login(const std::string & user, const std::string & password);
    //! Wylogowuje uøytkownika
    virtual void logout();
    //! \return prawda jeúli uøytkownik zalogowany
    virtual bool isLogged() const;

    //! \return Dane aktualnego uøytkownika ( w szczegÛlnoúci pusty obiekt jeúli niezalogowano)
    virtual const User * currentUser() const;

	//! Implementacja interfejsu communication::ICommunicationDataSource
	virtual void showPatientCard(const PluginSubject::SubjectConstPtr & subject, const communication::PatientConstPtr & patient = communication::PatientConstPtr());
	virtual void showUserDataCard();
	virtual void showConfigurationCard();

private:

	//! \param connection Po≥πczenie z webService ktÛre inicjalizujÍ - ustawiam adres jeøeli jest inny niø aktualnie ustawiony w po≥πczeniu
	//! Ta metoda w przypadku zmiany adresu us≥ugi powinna úciπgnπc jej definicjÍ, sparsowaÊ jπ i przygotowaÊ po≥aczenie do dalszej pracy
	//! Jest to leniwa inicjalizacja, kiedy nie chcemy od razu nawiazaywaÊ po≥πczenia poniewaø user moøe sobie zarzyczyÊ trybu offline
	void ensureConnection(const webservices::WSConnectionPtr & connection);

	//! \param login Login ktÛry prÛbujemy aktywowac po stronie bazy danych
	//! \param activationCode Kod aktywacyjny otrzymany w mailu
	//! \return Czy uda≥o siÍ aktywowaÊ konto
	bool tryActivateAccount(const std::string & login, const std::string & activationCode);

	//! Metoda zak≥ada konto uøytkownika w obu bazach danych - medycznej i ruchu
	//! \param login Login uøytkownika
	//! \param email Email uøytkownika
	//! \param password Has≥o uøytkownika
	//! \param firstName ImiÍ uøytkownika
	//! \param lastName Nazwisko uøytkownika
	//! \return true jeúli uda≥o siÍ za≥oøyÊ konto
	bool registerUser(const std::string & login, const std::string & email, const std::string & password,
		const std::string & firstName, const std::string & lastName);

	//! Odúwieøa fileManager w kontekúcie nowej p≥ytkiej kopii bazy danych - moøe dosz≥y jakieú nowe pliki?
	void refreshFileManager();

	//! \param user Aktualny uøytkownik zalogowany do bazy danych
	void setCurrentUser(const User & user);

	//! \param user Uøytkownik wg ktÛrego ustawiany po≥πczenia
	static void setConnectionsCredentials(const User & user);

	//! \param certPath åcieøka certyfikatu serwera dla po≥πczeÒ https
	static void setConnectionsSerwerCertificatePath(const core::Filesystem::Path & certPath);

	//! \param shallowCopy Nowa p≥ytka kopia bazy danych do ustawienia
	void setShallowCopy(const communication::ShallowCopy & shallowCopy);
	//! Odúwieøa dane po ustawieniu nowej p≥ytkiej kopii bazy danych - stosuje filtr jeúli wymagane i odúwieøa widok
	void refreshFilteredData();

	//! Czyúci lokalne dane uøytkownika po wylogowaniu
	void clearUserLocalSpace();
	//! Tworzy lokalne dane uøytkownika przy logowaniu
	void createUserLocalSpace();

	//! \return Czy p≥ytka kopia bazy danych jest kompletna
	bool isShallowCopyComplete() const;
	//! \return Czy p≥ytka kopia bazy danych jest aktualna
	bool isShallowCopyCurrent();

	//! Odúwieøa p≥ytkπ kopiÍ bazy danych : pobiera nieaktualne/brakujπce, tworzy, waliduje,  ≥aduje do localStorage, odúwieøa filtry i perspektywy
	//! \return Obiekt pomocniczy przy pobieraniu danych - pozwala odczytywaÊ informacje o statusie pobierania
	DownloadRequestPtr generateDownloadShallowCopyRequestToLocalUserSpace();

	//! Wyciπgamy dane z lokalnego storage (SQLite/SQLCipher - szyfrowany SQLite) - tylko przy logowaniu
	void extractDataFromLocalStorageToUserSpace();
	//! \param file Plik ktÛry chcemy wypakowaÊ z lokalnego storage
	//! \param sessionName Nazwa sesji z ktÛrej pochodzi plik
	void extractFileFromLocalStorageToUserSpace(const webservices::MotionShallowCopy::File * file, const std::string & sessionName);

	//! \param oldShallowCopy Poprzednia p≥ytka kopia dla ktÛrej wypakowywano dane
	//! \param newShallowCopy Nowa p≥ytka kopia dla ktÛrej wypakowywujemy dane
	//! Algorytm opiera siÍ na rÛønicy zbioru plikÛw do ktÛrych uøytkownik ma dostep zdefiniowany p≥ytkπ kopiπ bazy danych
	//! Usuwamy to do czego nei mamy juø dostepu i wyciπgamy to co jest i do czego mamy juø dostep
	//! Ewentualnie odúwieøamy to co siÍ zgadza
	void extractDataFromLocalStorageToUserSpace(const communication::ShallowCopy & prevShallowCopy, const communication::ShallowCopy & newShallowCopy);

	//! Wyciπgamy p≥ytkπ kopiÍ bazy danych z lokalnego storage (SQLite)
	void extractShallowCopyFromLocalStorageToUserSpace();
	//! \param shallowCopy P≥ytka kopia bazy danych wyciπgniÍta z local user space
	//! \return Czy uda≥o siÍ stworzyÊ poprawnπ p≥ytkπ kopiÍ (parsowanie ok, spÛjnoúÊ ok)
	bool buildShallowCopyFromLocalUserSpace(communication::ShallowCopy & shallowCopy);
	//! Zapisujemy p≥ytkπ kopiÍ bazy danych z local user space do local storage dla aktualnego uøytkownika
	void saveShallowCopyInLocalStorage();
	//! Usuwamy p≥ytkπ kopie bazy danych z local user space
	void removeShallowCopyFromUserSpace();
	//! Usuwamy p≥ytkπ kopie bazy danych z local storage dla aktualnego uøytkownika
	void removeShallowCopyFromLocalStorage();

	void extractFileFromLocalStorage(const std::string & fileName, const core::Filesystem::Path & destPath, std::vector<core::Filesystem::Path> & extractedFilesPaths);

	//! \param fileID Idnetyfikator pliku ktÛry ≥aduje do DM
	DownloadRequestPtr generateDownloadFileRequest(int fileID);
	//! \param fileIDs ZbiÛr idnetyfikatorÛw plikÛw ktÛre ≥aduje do DM
	DownloadRequestPtr generateDownloadFilesRequest(const std::set<int> & fileIDs);

	//! Na potrzeby obs≥ugi rozpakowywania plikÛw VSK z archiwum zip
	//! \param inFile Dane wejsciowe - spakowany plik zip, albo plik z archiwum zip
	//! \param outFile Gdzie zapisujemy dane z rozpakowanego pliku zip
	static bool copyData(QIODevice &inFile, QIODevice &outFile);

private:
    // ------------------------ LOGIKA -------------------------------
	//! Aktualny uøytkownik
	User currentUser_;
    //! Manager logowania - zawiera uøytkownika
    DataSourceLoginManager * loginManager;

    //! Pe≥na p≥ytka kopia bazy danych
    communication::ShallowCopy fullShallowCopy;

    //! Manager statusu plikÛw
    core::shared_ptr<FileStatusManager> fileStatusManager;

	//! Manager danych lokalnych
	DataSourcePathsManager * pathsManager;

    //! Manager danych lokalnych
    DataSourceLocalStorage * localStorage;

    //! Manager statusÛw pe≥nej p≥ytkiej kopi
    core::shared_ptr<DataSourceStatusManager> fullShallowCopyStatus;

	//! ------------------------------- Obs≥uga DM ------------------------------

	//! Manager surowych danych w pamiÍci
	core::IMemoryDataManager * memoryDM;

	//! Manager plikÛw
	core::IFileDataManager * fileDM;

	//! Manager serwisÛw - tylko i wy≥πcznie na potrzeby PluginSubject!!
	core::IServiceManager * serviceManager;

	//! ---------------------------Obs≥uga plugin subject ------------------------

	std::map<int, core::ObjectWrapperPtr> sSubjects;
	std::map<int, core::ObjectWrapperPtr> sSessions;
	std::map<int, core::ObjectWrapperPtr> sMotion;

	//! ------------------------------- Web Services -----------------------------

	//! Czy user zaøπda≥ trybu offline?
	bool offlineMode_;
	//! Mapa po≥πczeÒ i ich statusÛw
	std::map<webservices::WSConnectionPtr, ConnectionStatus> connectionsStatus;
	//! Adres serwera ktÛry pingujemy
	std::string serwerPingUrl;
	//! Po≥πczenie z web services dla danych ruchu
	WSConnectionPtr motionFileStoremanWSConnection;
	//! Po≥πczenie z web services dla danych medycznych
	WSConnectionPtr medicalFileStoremanConnection;

	//! ------------------------------- FTPS -----------------------------

	//! Po≥πczenie ftps z bazπ danych ruchu
	webservices::FtpsConnectionPtr motionFtps_;

	//! Po≥πczenie ftps z bazπ danych medycznych
	webservices::FtpsConnectionPtr medicalFtps_;

    // -------------------------- UI ------------------------------------
	//! Widget ürÛd≥a danych
	DataSourceWidget * dataSourceWidget;

};

#endif HEADER_GUARD___COMMUNICATIONDATASOURCE_H__
