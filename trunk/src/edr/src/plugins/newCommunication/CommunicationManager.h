/**
@author Marek Daniluk
@brief Klasa CommunicationManager wykorzystuj�ca wzorce projektowe fasada oraz singleton do zarz�dzania po��czeniem
z baz� danych i przesy�ania/odbierania plik�w mi�dzy serwerem i klientem. Obecnie implementacja opiera si� o protok� FTP
i web serwisy wsdl.
*/

#ifndef HEADER_GUARD_COMMUNICATION_COMMUNICATIONMANAGER_H__
#define HEADER_GUARD_COMMUNICATION_COMMUNICATIONMANAGER_H__

#include "WSConnection.h"
#include <core/IDataManager.h>
#include <core/IServiceManager.h>
#include <webserviceslib/Entity.h>
#include <webserviceslib/IFtpsConnection.h>
#include <webserviceslib/IFileStoremanWS.h>
#include <webserviceslib/DownloadHelper.h>
#include <OpenThreads/ScopedLock>
#include <OpenThreads/ReentrantMutex>
#include <queue>
#include <boost/function.hpp>
#include <curl/curl.h>

//! Klasa odpowiedzialna za dostarczanie plik�w z bazy danych
class CommunicationManager
{
private:
    //! Typ lokalnego lokowania obiekt�w synchronizuj�cych
    typedef OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> ScopedLock;

	//! W�tek przetwarzaj�cy zlecenia z CommunicationManagera
	class ProcessingThread : public OpenThreads::Thread
	{
	public:
		//! Konstruktor
		//! \patam manager Manager kt�rego obs�ugujemy
		ProcessingThread(CommunicationManager * manager);
		virtual ~ProcessingThread();

		//! Metoda obs�uguj�ca managera
		virtual void run();
		//! \param ms Ilo�c milisekund jakie mam spa� kiedy nie ma nic do przetwarzania
		void setIdleSleep(unsigned int ms);
		//! \return Ilo�c milisekund jakie mam spa� kiedy nie ma nic do przetwarzania
		unsigned int idleSleep() const;
		//! Iformuj� w�tek �e ma si� zako�czy� jak najszybciej - przy kolejnym przejsciu p�tli
		//! Po tym wywo�aniu b�d� na niego najprawdopodobniej czeka�!!
		void finish();

	private:
		//! Obs�ugiwnay manager
		CommunicationManager * manager;
		//! Ilo�c milisekund jakie mam spa� kiedy nie ma nic do przetwarzania
		unsigned int idleSleep_;
		//! Czy ko�czy� prac�?
		bool finish_;
	};

	friend class ProcessingThread;

public:
    //! Typ zlecenie
    enum Request
    {
        DownloadFile,           //! Pro�ba o plik
        Complex,                //! Z�o�ona pro�ba (mo�na zagnie�d�a� i dodawa� wszystkie pozosta�e
        DownloadPhoto,          //! Pro�ba o plik
        CopyMotionShallowCopy,  //! Pro�ba o plik z p�ytk� kopi� bazy danych ruchu
        CopyMotionMetadata,     //! Pro�ba o plik z metadanymi ruchu
        CopyMedicalShallowCopy, //! Pro�ba o plik z p�yk� kopia danych medycznych
        CopyMedicalMetadata,    //! Pro�ba o plik z metadanymi medycznymi
        PingServer              //! Pro�ba o sprawdzenie dost�pno�ci serwera poprzez ping
    };

    /**
    Stany managera pomocne przy wykonywaniu dzia�a� wsp�bie�nie.
    */
    enum State
    {
        Ready, /** Gotowy do wszelkich dzia�a� */
        ProcessingComplex, /** Przetwarzanie kompleksowego zapytania */
        DownloadingFile, /** Trwa pobieranie pojedynczego pliku */
        DownloadingPhoto, /** Trwa pobieranie pojedynczego zdj�cia */
        CopyingMotionShallowCopy, /** Trwa pobieranie pliku z p�ytk� kopi� bazy danych ruchu*/
        CopyingMotionMetadata, /** Trwa pobieranie pliku z metadanymi ruchu*/
        CopyingMedicalShallowCopy, /** Trwa pobieranie pliku z p�yk� kopia danych medycznych*/
        CopyingMedicalMetadata, /** Trwa pobieranie pliku z metadanymi medycznymi*/
        PingingServer /** Pingowanie serwera */
    };

	//! Forward declaration
    class MetadataRequest;
    class FileRequest;
    class PhotoRequest;
    class ComplexRequest;

    //! Klasa reprezentuj�ca podstaw� wszystkich zlece�
    class BasicRequest
    {
        friend class CommunicationManager;
        friend class MetadataRequest;
        friend class FileRequest;
        friend class PhotoRequest;
        friend class ComplexRequest;

    public:
		//! Wirtualny destruktor
        virtual ~BasicRequest();
		//! Anuluj zadanie
        void cancel();
		//! \return Czy zadanie anulowane
        bool isCancelled() const;
		//! \return Typ zadania
        Request getType() const;
		//! \return Post�p zadania
        virtual double getProgress() const = 0;
		//! \return Czy zadanie zako�czone
        bool isComplete() const;

    private:
		//! Prywatny konstruktor
		//! \param type Typ requesta
        BasicRequest(Request type);

    private:
		//! Mutex do anulowania requesta
        mutable OpenThreads::Mutex cancelLock;
		//! Czy request anulowany
        bool canceled;
		//! Typ requesta
        Request type;
    };

	//! Zlecenie pingu zadanego adresu
	class PingRequest : public BasicRequest
	{
		friend class CommunicationManager;

	public:
		//! Konstruktor
		//! \param urlToPing Adres serwera kt�ry pingujemy
		PingRequest(const std::string & urlToPing);
		//! \param ustawiamy czy serwer nam odpowiedzia�
		void setServerResponse(bool response);

	public:
		//! \return Adres serwera kt�ry b�dziemy/ju� pingowali�my
		const std::string & urlToPing() const;
		//! \return Post�p zadania
		virtual double getProgress() const;

	private:
		//! Adres serwera do pingowania
		std::string urlToPing_;

		//! Czy serwer odpowiedzia�
		bool response;
	};

	//! Klasa odpowiedzialna za request plik�w z metadanymi (bez identyfikator�w) - implementuje progress pobierania
    class MetadataRequest : public BasicRequest, public webservices::IFtpsConnection::IProgress
    {
        friend class CommunicationManager;
        friend class FileRequest;
        friend class PhotoRequest;

    private:
		//! Kosntruktor
		//! \param type Typ requesta
		//! \param filePath �cie�ka zapisu pliku
        MetadataRequest(Request type, const std::string & filePath);

    public:
		//! \return �cie�ka pliku
        const std::string & getFilePath() const;
		//! Implementacja interfejsu webservices::IFtpsConnection::IProgress
        virtual void setProgress(double p);
		//! Impementacja interfejsu webservices::IFtpsConnection::IProgress
        virtual double getProgress() const;

    private:
		//! Progres �ci�gania pliku
        double progress;
		//! �cie�ka docelowa pliku
        std::string filePath;
    };

	//! Klasa odpowiedzialna za request plik�w (z identyfikatorami)
    class FileRequest : public MetadataRequest
    {
        friend class CommunicationManager;

    private:
		//! Kosntruktor
		//! \param filePath �cie�ka zapisu pliku
		//! \param fileID ID pliku do �ci�gni�cia
        FileRequest(const std::string & filePath, unsigned int fileID);

    public:
		//! \return Identyfikator pliku do �ci�gni�cia
        unsigned int getFileID() const;

    private:
		//! Identyfikator pliku do �ci�gni�cia
        unsigned int fileID;
    };

	//! Klasa odpowiedzialna za request zdj��
    class PhotoRequest : public MetadataRequest
    {
        friend class CommunicationManager;

    private:
		//! Kosntruktor
		//! \param filePath �cie�ka zapisu pliku
		//! \param photoID ID zdj�cia do �ci�gni�cia
        PhotoRequest(const std::string & filePath, unsigned int photoID);

    public:
		//! Identyfikator zdj�cia
        unsigned int getPhotoID() const;

    private:
		//! Identyfikator zdj�cia
        unsigned int photoID;
    };

    typedef core::shared_ptr<BasicRequest> BasicRequestPtr;
	typedef core::shared_ptr<PingRequest> PingRequestPtr;
    typedef core::shared_ptr<MetadataRequest> MetadataRequestPtr;
    typedef core::shared_ptr<FileRequest> FileRequestPtr;
    typedef core::shared_ptr<PhotoRequest> PhotoRequestPtr;

    typedef boost::function<void(const BasicRequestPtr &)> RequestCallback;
    typedef boost::function<void(const BasicRequestPtr &, const std::string &)> RequestErrorCallback;

    //! Struktura przechowuj�ca metody wywo�ywane przy odpowiednich etapach wykonania zlecenia
    struct RequestCallbacks
    {
        //! Wywo�anie na rozpocz�cie przetwarzania zlecenia
        RequestCallback onBeginCallback;
        //! Wywo�anie na zako�czenie przetwarzania zlecenia (powodzenie)
        RequestCallback onEndCallback;
        //! Wywo�anie na anulowanie przetwarzania zlecenia
        RequestCallback onCancelCallback;
        //! Wywo�anie na zako�czenie przetwarzanie zlecenia z b��dem
        RequestErrorCallback onErrorCallback;
    };

    /**
    Typ zlecenia przekazywany do kolejki zlece� CommunicationManagera
    */
    struct CompleteRequest
    {
		//! Request
        BasicRequestPtr request;
		//! Callbacki requesta
        RequestCallbacks callbacks;
    };

	//! Typ kolejki request�w do obs�ugi
    typedef std::queue<CompleteRequest> RequestsQueue;

    //! Z�o�one zlecenie
    class ComplexRequest : public BasicRequest
    {
        friend class CommunicationManager;

    private:
        ComplexRequest(const std::vector<CompleteRequest> & requests);

    public:
		//! \return Ilo�� zagnie�d�onych request�w
        unsigned int size() const;
		//! \return Czy request jest pusty
        bool empty() const;
		//! \param i Indeks zagnie�d�onego requesta
        const CompleteRequest & getRequest(unsigned int i) const;
		//! \return Ca�kowity post�p requesta
        virtual double getProgress() const;

    private:
		//! Zagnie�d�one requesty
        std::vector<CompleteRequest> requests;
    };

    typedef core::shared_ptr<ComplexRequest> ComplexRequestPtr;

private:

    /**
    Ukryty konstruktor
    */
    CommunicationManager();
    /**
    Ukryty destruktor
    */
    virtual ~CommunicationManager();
    /**
    Metoda statyczna (wymagana przez curla) typu callback do odbierania danych podczas pingowania.
    @param buffer wska�nik do bloku pami�ci o rozmiarze size*nmemb
    @param size rozmiar w bajtach elementu do odczytania
    @param nmemb liczba element�w do odczytania
    @param stream wska�nik na strumie� danych
    @return ilo�� bajt�w przetworzonych przez funkcj�
    */
    static size_t pingDataCallback(void *buffer, size_t size, size_t nmemb, void *stream);

	//! \param request [out] Kolejny request do obs�ugi
    void popRequest(CompleteRequest & reuest);

	//! \param downloaderHelper Obiekt pomagaj�cy prz �ci�ganiu plik�w zale�ny od tego czy �ci�gamy plik danych, zdj�cie czy metadane
    void setCurrentDownloadHelper(webservices::IDownloadHelper * downloadHelper);

	//! Funkcje przetwarzaj�ce r�ne requesty i zapami�tuj�ce b��dy powsta�e podczas przetwarzania
	webservices::IFtpsConnection::OperationStatus processComplex(const CompleteRequest & request, std::string & message);
    webservices::IFtpsConnection::OperationStatus processPhoto(const CompleteRequest & request, std::string & message);
    webservices::IFtpsConnection::OperationStatus processFile(const CompleteRequest & request, std::string & message);
    webservices::IFtpsConnection::OperationStatus processMotionShallowCopy(const CompleteRequest & request, std::string & message);
    webservices::IFtpsConnection::OperationStatus processMotionMetadata(const CompleteRequest & request, std::string & message);
    webservices::IFtpsConnection::OperationStatus processMedicalShallowCopy(const CompleteRequest & request, std::string & message);
    webservices::IFtpsConnection::OperationStatus processMedicalMetadata(const CompleteRequest & request, std::string & message);
    webservices::IFtpsConnection::OperationStatus processPing(const CompleteRequest & request, std::string & message);

	webservices::IFtpsConnection::OperationStatus processComplex(const CompleteRequest & request) { std::string temp; return processComplex(request, temp); }
    webservices::IFtpsConnection::OperationStatus processPhoto(const CompleteRequest & request) { std::string temp; return processPhoto(request, temp); }
    webservices::IFtpsConnection::OperationStatus processFile(const CompleteRequest & request) { std::string temp; return processFile(request, temp); }
    webservices::IFtpsConnection::OperationStatus processMotionShallowCopy(const CompleteRequest & request) { std::string temp; return processMotionShallowCopy(request, temp); }
    webservices::IFtpsConnection::OperationStatus processMotionMetadata(const CompleteRequest & request) { std::string temp; return processMotionMetadata(request, temp); }
    webservices::IFtpsConnection::OperationStatus processMedicalShallowCopy(const CompleteRequest & request) { std::string temp; return processMedicalShallowCopy(request, temp); }
    webservices::IFtpsConnection::OperationStatus processMedicalMetadata(const CompleteRequest & request) { std::string temp; return processMedicalMetadata(request, temp); }
    webservices::IFtpsConnection::OperationStatus processPing(const CompleteRequest & request) { std::string temp; return processPing(request, temp); }

public:

	//! \param motionFileStoremanService Serwis do �ci�gania plik�w ruchu
	void setMotionFileStoremanService(const webservices::MotionFileStoremanWSPtr & motionFileStoremanService);
	//! \param medicalFileStoremanService Serwis do �ci�gania plik�w medycznych
	void setMedicalFileStoremanService(const webservices::MedicalFileStoremanWSPtr & medicalFileStoremanService);

	//! \param medicalFtps Ftps do �ci�gania danych medycznych
	void setMedicalFtps(const webservices::FtpsConnectionPtr & medicalFtps);
	//! \param motionFtps Ftps do �ci�gania danych ruchu
	void setMotionFtps(const webservices::FtpsConnectionPtr & motionFtps);

	//! \return Po��cznie medyczne
	const WSConnectionPtr & medicalConnection();
	//! \return Po��cznie ruchu
	const WSConnectionPtr & motionConnection();

	//! \return Ftps do �ci�gania danych medycznych
	const webservices::FtpsConnectionPtr & medicalFtps();
	//! \return Ftps do �ci�gania danych ruchu
	const webservices::FtpsConnectionPtr & motionFtps();

	//! \param request Request wrzucany do kolejki managera - b�dzie przetworzony kiedy wszystkie przed nim zostan� obs�u�one
    void pushRequest(const CompleteRequest & request);

	//! Metody tworz�ce obs�ugiwane typy zlece�
    static ComplexRequestPtr createRequestComplex(const std::vector<CompleteRequest> & requests);
    static FileRequestPtr createRequestFile(unsigned int fileID, const std::string & filePath);
    static PhotoRequestPtr createRequestPhoto(unsigned int fileID, const std::string & filePath);
    static MetadataRequestPtr createRequestMotionShallowCopy(const std::string & filePath);
    static MetadataRequestPtr createRequestMotionMetadata(const std::string & filePath);
    static MetadataRequestPtr createRequestMedicalShallowCopy(const std::string & filePath);
    static MetadataRequestPtr createRequestMedicalMetadata(const std::string & filePath);
    static PingRequestPtr createRequestPing(const std::string & urlToPing);

	//! \param request Zlecenie kt�re chcemy anulowa� je�li jeszcze nie by�o przetwarzane - do przetworzenia lub przetwarzane
    void cancelRequest(const BasicRequestPtr & request);

	//! \return Prawda je�li w kolejce nie ma ju� nic do przetwarznia - manager jest w stanie IDLE
    bool requestsQueueEmpty() const;

	//! Anyluje wszystkie zlecenia w kolejce managera
    void cancelAllPendingRequests();

    /**
    Zwraca post�p w procentach aktualnie wykonywanego zadania jako pejedynczego transferu
    @return warto�� procentowa (od 0 do 100) pokazuj�ca post�p wykonywanej operacji
    */
    int getProgress() const;

private:
    /**
    Ustala stan w jakim znajduje si� Communication Service.
    @param state stan jaki ustali� jako aktualny dla CS
    */
    void setState(State state);

public:
    /**
    Sprawdza stan w jakim znajduje si� Communication Service.
    @return aktualny stan CS
    */
    State getState();

public:

    /**
    Statyczna metoda pobieraj�ca jedyna instancj� klasy CommuniationManagera.
    @return jedyna instancja CommunicationManagera
    */
    static CommunicationManager* getInstance();
    /**
    Statyczna metoda usuwaj�ca jedyn� instancj� CommunicationManagera.
    */
    static void destoryInstance();


private:

	// ---------------------------- dostarczane z zewnatrz ------------------------
	webservices::FtpsConnectionPtr motionFtps_;
	webservices::FtpsConnectionPtr medicalFtps_;

	webservices::MotionFileStoremanWSPtr motionFileStoremanService_;
	webservices::MedicalFileStoremanWSPtr medicalFileStoremanService_;

	// ----------------------- inner ------------------------------
	webservices::BasicDownloadHelper fileDownloadHelper;
	webservices::BasicDownloadHelper photoDownloadHelper;

	webservices::ShallowDownloadHelper motionShallowDownloadHelper;
	webservices::ShallowDownloadHelper medicalShallowDownloadHelper;

    /**
    Jedyna instancja klasy CommunicationManager.
    */
    static CommunicationManager* instance;

    /**
    Wska�nik na klas� IDownloadHelper zajmuj�c� si� aktualmnym pobieraniem
    */
    webservices::IDownloadHelper * currentDownloadHelper;

    /**
    Stan Communication Service
    */
    State state;

    /**
    Aktualnie realizowane zapytanie
    */
    CompleteRequest currentRequest;

    /**
    Kolejka zapyta�
    */
    RequestsQueue requestsQueue;

    /**
    Czy serwer odpowiada na ping
    */
    bool serverResponse;
    /**
    Instancja curla do pingowania serwera.
    */
    CURL* pingCurl;
    /**
    Muteks zabezpieczaj�cy przed zakleszczeniami.
    */
    OpenThreads::ReentrantMutex trialsMutex;
    /**
    Muteks synchronizuj�cy obs�uge kolejki zlece�
    */
    mutable OpenThreads::ReentrantMutex requestsMutex;

    mutable OpenThreads::ReentrantMutex downloadHelperMutex;
	//! Czy ko�czymy przetwarzanie
    bool finish;
	//! Czy przerwa� �ci�ganie
    bool cancelDownloading;
	//! W�tek przetwarzaj�cy zlecenia communication managera
	core::shared_ptr<ProcessingThread> processingThread;
};

#endif //HEADER_GUARD_COMMUNICATION_COMMUNICATIONMANAGER_H__
