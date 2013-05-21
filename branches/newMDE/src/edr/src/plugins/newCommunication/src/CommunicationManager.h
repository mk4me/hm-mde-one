/**
@author Marek Daniluk
@brief Klasa CommunicationManager wykorzystująca wzorce projektowe fasada oraz singleton do zarządzania połączeniem
z bazą danych i przesyłania/odbierania plików między serwerem i klientem. Obecnie implementacja opiera się o protokół FTP
i web serwisy wsdl.
*/

#ifndef HEADER_GUARD_COMMUNICATION_COMMUNICATIONMANAGER_H__
#define HEADER_GUARD_COMMUNICATION_COMMUNICATIONMANAGER_H__

#include "WSConnection.h"
#include <corelib/IDataManagerReader.h>
#include <corelib/IServiceManager.h>
#include <webserviceslib/Entity.h>
#include <webserviceslib/IFtpsConnection.h>
#include <webserviceslib/IFileStoremanWS.h>
#include <webserviceslib/DownloadHelper.h>
#include <OpenThreads/ScopedLock>
#include <OpenThreads/ReentrantMutex>
#include <queue>
#include <boost/function.hpp>
#include <curl/curl.h>

//! Klasa odpowiedzialna za dostarczanie plików z bazy danych
class CommunicationManager
{
private:
    //! Typ lokalnego lokowania obiektów synchronizujących
    typedef OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> ScopedLock;

	//! Wątek przetwarzający zlecenia z CommunicationManagera
	class ProcessingThread : public OpenThreads::Thread
	{
	public:
		//! Konstruktor
		//! \patam manager Manager którego obsługujemy
		ProcessingThread(CommunicationManager * manager);
		virtual ~ProcessingThread();

		//! Metoda obsługująca managera
		virtual void run();
		//! \param ms Ilośc milisekund jakie mam spać kiedy nie ma nic do przetwarzania
		void setIdleSleep(unsigned int ms);
		//! \return Ilośc milisekund jakie mam spać kiedy nie ma nic do przetwarzania
		unsigned int idleSleep() const;
		//! Iformuję wątek że ma się zakończyć jak najszybciej - przy kolejnym przejsciu pętli
		//! Po tym wywołaniu będę na niego najprawdopodobniej czekał!!
		void finish();

	private:
		//! Obsługiwnay manager
		CommunicationManager * manager;
		//! Ilośc milisekund jakie mam spać kiedy nie ma nic do przetwarzania
		unsigned int idleSleep_;
		//! Czy kończyć pracę?
		bool finish_;
	};

	friend class ProcessingThread;

public:
    //! Typ zlecenie
    enum Request
    {
        DownloadFile,           //! Prośba o plik
        Complex,                //! Złożona prośba (można zagnieżdżać i dodawać wszystkie pozostałe
        DownloadPhoto,          //! Prośba o plik
        CopyMotionShallowCopy,  //! Prośba o plik z płytką kopią bazy danych ruchu
        CopyMotionMetadata,     //! Prośba o plik z metadanymi ruchu
        CopyMedicalShallowCopy, //! Prośba o plik z płyką kopia danych medycznych
        CopyMedicalMetadata,    //! Prośba o plik z metadanymi medycznymi
        PingServer              //! Prośba o sprawdzenie dostępności serwera poprzez ping
    };

    /**
    Stany managera pomocne przy wykonywaniu działań współbieżnie.
    */
    enum State
    {
        Ready, /** Gotowy do wszelkich działań */
        ProcessingComplex, /** Przetwarzanie kompleksowego zapytania */
        DownloadingFile, /** Trwa pobieranie pojedynczego pliku */
        DownloadingPhoto, /** Trwa pobieranie pojedynczego zdjęcia */
        CopyingMotionShallowCopy, /** Trwa pobieranie pliku z płytką kopią bazy danych ruchu*/
        CopyingMotionMetadata, /** Trwa pobieranie pliku z metadanymi ruchu*/
        CopyingMedicalShallowCopy, /** Trwa pobieranie pliku z płyką kopia danych medycznych*/
        CopyingMedicalMetadata, /** Trwa pobieranie pliku z metadanymi medycznymi*/
        PingingServer /** Pingowanie serwera */
    };

	//! Forward declaration
    class MetadataRequest;
    class FileRequest;
    class PhotoRequest;
    class ComplexRequest;

    //! Klasa reprezentująca podstawę wszystkich zleceń
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
		//! \return Postęp zadania
        virtual double getProgress() const = 0;
		//! \return Czy zadanie zakończone
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
		//! \param urlToPing Adres serwera który pingujemy
		PingRequest(const std::string & urlToPing);
		//! \param ustawiamy czy serwer nam odpowiedział
		void setServerResponse(bool response);

	public:
		//! \return Adres serwera który będziemy/już pingowaliśmy
		const std::string & urlToPing() const;
		//! \return Postęp zadania
		virtual double getProgress() const;

	private:
		//! Adres serwera do pingowania
		std::string urlToPing_;

		//! Czy serwer odpowiedział
		bool response;
	};

	//! Klasa odpowiedzialna za request plików z metadanymi (bez identyfikatorów) - implementuje progress pobierania
    class MetadataRequest : public BasicRequest, public webservices::IFtpsConnection::IProgress
    {
        friend class CommunicationManager;
        friend class FileRequest;
        friend class PhotoRequest;

    private:
		//! Kosntruktor
		//! \param type Typ requesta
		//! \param filePath Ścieżka zapisu pliku
        MetadataRequest(Request type, const std::string & filePath);

    public:
		//! \return Ścieżka pliku
        const std::string & getFilePath() const;
		//! Implementacja interfejsu webservices::IFtpsConnection::IProgress
        virtual void setProgress(double p);
		//! Impementacja interfejsu webservices::IFtpsConnection::IProgress
        virtual double getProgress() const;

    private:
		//! Progres ściągania pliku
        double progress;
		//! Ścieżka docelowa pliku
        std::string filePath;
    };

	//! Klasa odpowiedzialna za request plików (z identyfikatorami)
    class FileRequest : public MetadataRequest
    {
        friend class CommunicationManager;

    private:
		//! Kosntruktor
		//! \param filePath Ścieżka zapisu pliku
		//! \param fileID ID pliku do ściągnięcia
        FileRequest(const std::string & filePath, unsigned int fileID);

    public:
		//! \return Identyfikator pliku do ściągnięcia
        unsigned int getFileID() const;

    private:
		//! Identyfikator pliku do ściągnięcia
        unsigned int fileID;
    };

	//! Klasa odpowiedzialna za request zdjęć
    class PhotoRequest : public MetadataRequest
    {
        friend class CommunicationManager;

    private:
		//! Kosntruktor
		//! \param filePath Ścieżka zapisu pliku
		//! \param photoID ID zdjęcia do ściągnięcia
        PhotoRequest(const std::string & filePath, unsigned int photoID);

    public:
		//! Identyfikator zdjęcia
        unsigned int getPhotoID() const;

    private:
		//! Identyfikator zdjęcia
        unsigned int photoID;
    };

    typedef utils::shared_ptr<BasicRequest> BasicRequestPtr;
	typedef utils::shared_ptr<PingRequest> PingRequestPtr;
    typedef utils::shared_ptr<MetadataRequest> MetadataRequestPtr;
    typedef utils::shared_ptr<FileRequest> FileRequestPtr;
    typedef utils::shared_ptr<PhotoRequest> PhotoRequestPtr;

    typedef boost::function<void(const BasicRequestPtr &)> RequestCallback;
    typedef boost::function<void(const BasicRequestPtr &, const std::string &)> RequestErrorCallback;

    //! Struktura przechowująca metody wywoływane przy odpowiednich etapach wykonania zlecenia
    struct RequestCallbacks
    {
        //! Wywołanie na rozpoczęcie przetwarzania zlecenia
        RequestCallback onBeginCallback;
        //! Wywołanie na zakończenie przetwarzania zlecenia (powodzenie)
        RequestCallback onEndCallback;
        //! Wywołanie na anulowanie przetwarzania zlecenia
        RequestCallback onCancelCallback;
        //! Wywołanie na zakończenie przetwarzanie zlecenia z błędem
        RequestErrorCallback onErrorCallback;
    };

    /**
    Typ zlecenia przekazywany do kolejki zleceń CommunicationManagera
    */
    struct CompleteRequest
    {
		//! Request
        BasicRequestPtr request;
		//! Callbacki requesta
        RequestCallbacks callbacks;
    };

	//! Typ kolejki requestów do obsługi
    typedef std::queue<CompleteRequest> RequestsQueue;

    //! Złożone zlecenie
    class ComplexRequest : public BasicRequest
    {
        friend class CommunicationManager;

    private:
        ComplexRequest(const std::vector<CompleteRequest> & requests);

    public:
		//! \return Ilość zagnieżdżonych requestów
        unsigned int size() const;
		//! \return Czy request jest pusty
        bool empty() const;
		//! \param i Indeks zagnieżdżonego requesta
        const CompleteRequest & getRequest(unsigned int i) const;
		//! \return Całkowity postęp requesta
        virtual double getProgress() const;

    private:
		//! Zagnieżdżone requesty
        std::vector<CompleteRequest> requests;
    };

    typedef utils::shared_ptr<ComplexRequest> ComplexRequestPtr;

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
    @param buffer wskaźnik do bloku pamięci o rozmiarze size*nmemb
    @param size rozmiar w bajtach elementu do odczytania
    @param nmemb liczba elementów do odczytania
    @param stream wskaźnik na strumień danych
    @return ilość bajtów przetworzonych przez funkcję
    */
    static size_t pingDataCallback(void *buffer, size_t size, size_t nmemb, void *stream);

	//! \param request [out] Kolejny request do obsługi
    void popRequest(CompleteRequest & reuest);

	//! \param downloaderHelper Obiekt pomagający prz ściąganiu plików zależny od tego czy ściągamy plik danych, zdjęcie czy metadane
    void setCurrentDownloadHelper(webservices::IDownloadHelper * downloadHelper);

	//! Funkcje przetwarzające różne requesty i zapamiętujące błędy powstałe podczas przetwarzania
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

	//! \param motionFileStoremanService Serwis do ściągania plików ruchu
	void setMotionFileStoremanService(const webservices::MotionFileStoremanWSPtr & motionFileStoremanService);
	//! \param medicalFileStoremanService Serwis do ściągania plików medycznych
	void setMedicalFileStoremanService(const webservices::MedicalFileStoremanWSPtr & medicalFileStoremanService);

	//! \param medicalFtps Ftps do ściągania danych medycznych
	void setMedicalFtps(const webservices::FtpsConnectionPtr & medicalFtps);
	//! \param motionFtps Ftps do ściągania danych ruchu
	void setMotionFtps(const webservices::FtpsConnectionPtr & motionFtps);

	//! \return Połącznie medyczne
	const WSConnectionPtr & medicalConnection();
	//! \return Połącznie ruchu
	const WSConnectionPtr & motionConnection();

	//! \return Ftps do ściągania danych medycznych
	const webservices::FtpsConnectionPtr & medicalFtps();
	//! \return Ftps do ściągania danych ruchu
	const webservices::FtpsConnectionPtr & motionFtps();

	//! \param request Request wrzucany do kolejki managera - będzie przetworzony kiedy wszystkie przed nim zostaną obsłużone
    void pushRequest(const CompleteRequest & request);

	//! Metody tworzące obsługiwane typy zleceń
    static ComplexRequestPtr createRequestComplex(const std::vector<CompleteRequest> & requests);
    static FileRequestPtr createRequestFile(unsigned int fileID, const std::string & filePath);
    static PhotoRequestPtr createRequestPhoto(unsigned int fileID, const std::string & filePath);
    static MetadataRequestPtr createRequestMotionShallowCopy(const std::string & filePath);
    static MetadataRequestPtr createRequestMotionMetadata(const std::string & filePath);
    static MetadataRequestPtr createRequestMedicalShallowCopy(const std::string & filePath);
    static MetadataRequestPtr createRequestMedicalMetadata(const std::string & filePath);
    static PingRequestPtr createRequestPing(const std::string & urlToPing);

	//! \param request Zlecenie które chcemy anulować jeśli jeszcze nie było przetwarzane - do przetworzenia lub przetwarzane
    void cancelRequest(const BasicRequestPtr & request);

	//! \return Prawda jeśli w kolejce nie ma już nic do przetwarznia - manager jest w stanie IDLE
    bool requestsQueueEmpty() const;

	//! Anyluje wszystkie zlecenia w kolejce managera
    void cancelAllPendingRequests();

    /**
    Zwraca postęp w procentach aktualnie wykonywanego zadania jako pejedynczego transferu
    @return wartość procentowa (od 0 do 100) pokazująca postęp wykonywanej operacji
    */
    int getProgress() const;

private:
    /**
    Ustala stan w jakim znajduje się Communication Service.
    @param state stan jaki ustalić jako aktualny dla CS
    */
    void setState(State state);

public:
    /**
    Sprawdza stan w jakim znajduje się Communication Service.
    @return aktualny stan CS
    */
    State getState();

public:

    /**
    Statyczna metoda pobierająca jedyna instancję klasy CommuniationManagera.
    @return jedyna instancja CommunicationManagera
    */
    static CommunicationManager* getInstance();
    /**
    Statyczna metoda usuwająca jedyną instancję CommunicationManagera.
    */
    static void destoryInstance();


private:

	// ---------------------------- dostarczane z zewnątrz ------------------------
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
    Wskaźnik na klasę IDownloadHelper zajmującą się aktualmnym pobieraniem
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
    Kolejka zapytań
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
    Muteks zabezpieczający przed zakleszczeniami.
    */
    OpenThreads::ReentrantMutex trialsMutex;
    /**
    Muteks synchronizujący obsługę kolejki zleceń
    */
    mutable OpenThreads::ReentrantMutex requestsMutex;

    mutable OpenThreads::ReentrantMutex downloadHelperMutex;
	//! Czy kończymy przetwarzanie
    bool finish;
	//! Czy przerwać ściąganie
    bool cancelDownloading;
	//! Wątek przetwarzający zlecenia communication managera
	utils::shared_ptr<ProcessingThread> processingThread;
};

#endif //HEADER_GUARD_COMMUNICATION_COMMUNICATIONMANAGER_H__
