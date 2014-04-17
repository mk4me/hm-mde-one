/********************************************************************
    created:  2014/04/09
    created:  9:4:2014   9:25
    filename: CommunicationManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/

#ifndef HEADER_GUARD_COMMUNICATION_COMMUNICATIONMANAGER_H__
#define HEADER_GUARD_COMMUNICATION_COMMUNICATIONMANAGER_H__

#include <webserviceslib/WSConnection.h>
#include <corelib/IDataManagerReader.h>
#include <corelib/IServiceManager.h>
#include <webserviceslib/Entity.h>
#include <webserviceslib/IFtpsConnection.h>
#include <webserviceslib/IFileStoremanWS.h>
#include <webserviceslib/DownloadHelper.h>
#include <webserviceslib/UploadHelper.h>
#include <queue>
#include <boost/function.hpp>
#include <curl/curl.h>
#include <threading/SynchronizationPolicies.h>
#include <corelib/IThread.h>
#include <QtCore/QWaitCondition>
#include <QtCore/QMutex>


//! Klasa odpowiedzialna za dostarczanie plików z bazy danych
class CommunicationManager
{
public:
    //! Typ zlecenie
    enum Request
    {
        DownloadFile,             //! Prośba o plik
        UploadFile,               //! Prośba o upload pliku
        ReplaceFile,              //! Prośba o update pliku
        Complex,                  //! Złożona prośba (można zagnieżdżać i dodawać wszystkie pozostałe
        DownloadPhoto,            //! Prośba o plik
        CopyMotionShallowCopy,    //! Prośba o plik z płytką kopią bazy danych ruchu
        CopyMotionMetadata,       //! Prośba o plik z metadanymi ruchu
        CopyMedicalShallowCopy,   //! Prośba o plik z płyką kopia danych medycznych
        CopyMedicalMetadata,      //! Prośba o plik z metadanymi medycznymi
        PingServer,               //! Prośba o sprawdzenie dostępności serwera poprzez ping
        CopyMotionBranchIncrement //! Prośba o różnicową płytką kopię
    };

    /**
    Stany managera pomocne przy wykonywaniu działań współbieżnie.
    */
    enum State
    {
        Ready, /** Gotowy do wszelkich działań */
        ProcessingComplex, /** Przetwarzanie kompleksowego zapytania */
        DownloadingFile, /** Trwa pobieranie pojedynczego pliku */
        UploadingFile, /** Trwa wysyłanie pojedynczego pliku */
        ReplacingFile, /** Trwa aktualizacja pojedynczego pliku */
        DownloadingPhoto, /** Trwa pobieranie pojedynczego zdjęcia */
        CopyingMotionShallowCopy, /** Trwa pobieranie pliku z płytką kopią bazy danych ruchu*/
        CopyingMotionBranchIncrement, /** Trwa pobieranie pliku z przyrostową kopią bazy danych ruchu*/
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
        mutable utils::StrictSyncPolicy cancelLock;
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
        MetadataRequest(Request type, const std::string & filePath, const webservices::DateTime& since = webservices::DateTime());

    public:
		//! \return Ścieżka pliku
        const std::string & getFilePath() const;
		//! Implementacja interfejsu webservices::IFtpsConnection::IProgress
        virtual void setProgress(double p);
		//! Impementacja interfejsu webservices::IFtpsConnection::IProgress
        virtual double getProgress() const;
        //! \return czas potrzebny przy żądaniu kopii różnicowych
        webservices::DateTime getDateTime() const;

    private:
		//! Progres ściągania pliku
        double progress;
		//! Ścieżka docelowa pliku
        std::string filePath;
        //! Dla różnicowych kopii potrzebny jest czas
        webservices::DateTime dateTime;
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

    //! Klasa odpowiedzialna za update plików (z identyfikatorami)
    class ReplaceRequest : public MetadataRequest
    {
        friend class CommunicationManager;

    private:
        //! Kosntruktor
        //! \param filePath Ścieżka do lokalnego pliku
        //! \param filePath Ścieżka zapisu pliku
        //! \param fileID ID pliku do ściągnięcia
        ReplaceRequest( unsigned int fileID, const std::string& sourcePath, const std::string & filePath);

    public:
        //! \return nazwa pliku
        std::string getFileName() const;
        //! \return identyfikator pliku
        unsigned int getFileID() const;
        //! \return ścieżka do lokalnego pliku
        std::string getSourcePath() const;

    private:
        //! Identyfikator update'owanego pliku
        unsigned int fileID;
        //! Ścieżka do lokalnego pliku
        std::string sourcePath;
    };

    //! Klasa odpowiedzialna za request plików (z identyfikatorami)
    class UploadRequest : public MetadataRequest
    {
        friend class CommunicationManager;

    public:
        typedef std::map<std::string, int> Filename2ID;
        DEFINE_SMART_POINTERS(Filename2ID);

    private:
        //! Kosntruktor
        //! \param filePath Ścieżka do lokalnego pliku
        //! \param filePath Ścieżka zapisu pliku
        //! \param fileID ID pliku do ściągnięcia
        UploadRequest(const std::string& sourcePath, const std::string & filePath, unsigned int trialID, Filename2IDPtr uploadedFiles);

    public:
        //! \return Identyfikator triala, pod który ma trafić plik
        unsigned int getTrialID() const;
        //! \return nazwa uploadowanego pliku
        std::string getFileName() const;

        //! \return identyfikator pliku nadany po udanym uploadzie
        int getFileID() const;
        //! ustawia identyfikator pliku nadany po udanym uploadzie
        void setFileID(int val);
        //! kolekcja, do ktorej trafia nazwa i identyfikator po udanym uploadzie
        CommunicationManager::UploadRequest::Filename2IDPtr getUploadedFiles() const;

    private:
        //! Identyfikator triala, pod który ma trafić plik
        unsigned int trialID;
        //! Ścieżka do lokalnego pliku
        std::string sourcePath;
        //! identyfikator pliku nadany po udanym uploadzie
        int fileID;
        //! kolekcja, do ktorej trafia nazwa i identyfikator po udanym uploadzie
        Filename2IDPtr uploadedFiles;
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
    typedef utils::shared_ptr<UploadRequest> UploadRequestPtr;
    typedef utils::shared_ptr<ReplaceRequest> ReplaceRequestPtr;
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


	class RequestsExecutor : public utils::IRunnable
	{
	public:
		//! Typ określający czas w milisekundach
		typedef unsigned int time_type;

	public:
		RequestsExecutor(CommunicationManager * manager,
			const time_type sleepTime = 250000);

		~RequestsExecutor();

		virtual void run();

		void finish();

		const time_type sleepTime() const;

		void setSleepTime(const time_type sleepTime);

	private:
		//! Czas przerwy w mikrosekundach
		time_type sleepTime_;
		//! Czy kończymy obsługiwanie zleceń
		bool finish_;
		//! CommunicationManager
		CommunicationManager * manager;
	};

private:
	//! Mwtoda inicjalizuje wątek obsługujący zlecenia i warstwę sieci
	void init();
	//! Metoda probuje inicjalizować wątek obsługujący zlecenia i warstwę sieci
	void tryInit();

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
    webservices::IFtpsConnection::OperationStatus processUpload(const CompleteRequest & request, std::string & message);
    webservices::IFtpsConnection::OperationStatus processReplace(const CompleteRequest & request, std::string & message);
    webservices::IFtpsConnection::OperationStatus processMotionShallowCopy(const CompleteRequest & request, std::string & message);
    webservices::IFtpsConnection::OperationStatus processMotionMetadata(const CompleteRequest & request, std::string & message);
    webservices::IFtpsConnection::OperationStatus processMedicalShallowCopy(const CompleteRequest & request, std::string & message);
    webservices::IFtpsConnection::OperationStatus processMedicalMetadata(const CompleteRequest & request, std::string & message);
    webservices::IFtpsConnection::OperationStatus processPing(const CompleteRequest & request, std::string & message);
    webservices::IFtpsConnection::OperationStatus processMotionBranchIncrementShallowCopy(const CompleteRequest & request, std::string & message);

	webservices::IFtpsConnection::OperationStatus processComplex(const CompleteRequest & request) { std::string temp; return processComplex(request, temp); }
    webservices::IFtpsConnection::OperationStatus processPhoto(const CompleteRequest & request) { std::string temp; return processPhoto(request, temp); }
    webservices::IFtpsConnection::OperationStatus processFile(const CompleteRequest & request) { std::string temp; return processFile(request, temp); }
    webservices::IFtpsConnection::OperationStatus processUpload(const CompleteRequest & request) { std::string temp; return processUpload(request, temp); }
    webservices::IFtpsConnection::OperationStatus processReplace(const CompleteRequest & request) { std::string temp; return processReplace(request, temp); }
    webservices::IFtpsConnection::OperationStatus processMotionShallowCopy(const CompleteRequest & request) { std::string temp; return processMotionShallowCopy(request, temp); }
    webservices::IFtpsConnection::OperationStatus processMotionMetadata(const CompleteRequest & request) { std::string temp; return processMotionMetadata(request, temp); }
    webservices::IFtpsConnection::OperationStatus processMedicalShallowCopy(const CompleteRequest & request) { std::string temp; return processMedicalShallowCopy(request, temp); }
    webservices::IFtpsConnection::OperationStatus processMedicalMetadata(const CompleteRequest & request) { std::string temp; return processMedicalMetadata(request, temp); }
    webservices::IFtpsConnection::OperationStatus processPing(const CompleteRequest & request) { std::string temp; return processPing(request, temp); }
    webservices::IFtpsConnection::OperationStatus processMotionBranchIncrementShallowCopy(const CompleteRequest & request) { std::string temp; return processMotionBranchIncrementShallowCopy(request, temp); }

    /**
    Ustala stan w jakim znajduje się Communication Service.
    @param state stan jaki ustalić jako aktualny dla CS
    */
    void setState(State state);

public:

	//! Konstruktor
    CommunicationManager(core::IThreadPtr executorThread);
    
	//! Destruktor
    ~CommunicationManager();

	void setMotionFileStoremanService(const webservices::MotionFileStoremanWSPtr & motionFileStoremanService);

	void setMedicalFileStoremanService(const webservices::MedicalFileStoremanWSPtr & medicalFileStoremanService);

	//! \param medicalFtps Ftps do ściągania danych medycznych
	void setMedicalFtps(const webservices::FtpsConnectionPtr & medicalFtps);
	//! \param motionFtps Ftps do ściągania danych ruchu
	void setMotionFtps(const webservices::FtpsConnectionPtr & motionFtps);

	//! \return Połącznie medyczne
	const webservices::WSConnectionPtr & medicalConnection();
	//! \return Połącznie ruchu
	const webservices::WSConnectionPtr & motionConnection();

	//! \return Ftps do ściągania danych medycznych
	const webservices::FtpsConnectionPtr & medicalFtps();
	//! \return Ftps do ściągania danych ruchu
	const webservices::FtpsConnectionPtr & motionFtps();

	//! \param request Request wrzucany do kolejki managera - będzie przetworzony kiedy wszystkie przed nim zostaną obsłużone
    void pushRequest(const CompleteRequest & request);

	//! Metody tworzące obsługiwane typy zleceń
    static ComplexRequestPtr createRequestComplex(const std::vector<CompleteRequest> & requests);
    static FileRequestPtr createRequestFile(unsigned int fileID, const std::string & filePath);
    static UploadRequestPtr createRequestUpload(const std::string & sourcePath, const std::string & filePath, unsigned int trialID, UploadRequest::Filename2IDPtr uploadedFiles);
    static ReplaceRequestPtr createRequestReplace(const std::string & sourcePath, const std::string & filePath, unsigned int fileID);
    static PhotoRequestPtr createRequestPhoto(unsigned int fileID, const std::string & filePath);
    static MetadataRequestPtr createRequestMotionShallowCopy(const std::string & filePath);
    static MetadataRequestPtr createRequestMotionMetadata(const std::string & filePath);
    static MetadataRequestPtr createRequestMedicalShallowCopy(const std::string & filePath);
    static MetadataRequestPtr createRequestMedicalMetadata(const std::string & filePath);
    static PingRequestPtr createRequestPing(const std::string & urlToPing);
    static MetadataRequestPtr createRequestMotionShallowBranchIncrement( const std::string& path, const webservices::DateTime& since );

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

    /**
    Sprawdza stan w jakim znajduje się Communication Service.
    @return aktualny stan CS
    */
    State getState();

private:

	// ---------------------------- dostarczane z zewnątrz ------------------------
	webservices::FtpsConnectionPtr motionFtps_;
	webservices::FtpsConnectionPtr medicalFtps_;

	webservices::MotionFileStoremanWSPtr motionFileStoremanService_;
    webservices::MedicalFileStoremanWSPtr medicalFileStoremanService_;

	// ----------------------- inner ------------------------------
	webservices::BasicDownloadHelper fileDownloadHelper;
	webservices::BasicDownloadHelper photoDownloadHelper;

    webservices::BasicUploadHelper fileUploadHelper;

	webservices::ShallowDownloadHelper motionShallowDownloadHelper;
	webservices::ShallowDownloadHelper medicalShallowDownloadHelper;

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
    utils::RecursiveSyncPolicy trialsMutex;
    /**
    Muteks synchronizujący obsługę kolejki zleceń
    */
    mutable utils::RecursiveSyncPolicy requestsMutex;

    mutable utils::RecursiveSyncPolicy downloadHelperMutex;


    mutable utils::RecursiveSyncPolicy uploadHelperMutex;
	//! Czy kończymy przetwarzanie
    bool finish;
	//! Czy przerwać ściąganie
    bool cancelDownloading;
	//! Wątek obsługujący komunikację
	core::IThreadPtr executorThread;
	//! Wykonawca zleceń ściągania danych
	core::shared_ptr<RequestsExecutor> requestsExecutor;
	//! Warunek oczekiwania na zlecenia w kolejce
	QWaitCondition requestsWait_;
	//! Obiekt synchronizujący kolejkę zleceń
	QMutex requestsSynch_;
};

#endif //HEADER_GUARD_COMMUNICATION_COMMUNICATIONMANAGER_H__
