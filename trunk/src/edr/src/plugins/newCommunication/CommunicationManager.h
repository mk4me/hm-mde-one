/**
@author Marek Daniluk
@brief Klasa CommunicationManager wykorzystuj¹ca wzorce projektowe fasada oraz singleton do zarz¹dzania po³¹czeniem
z baz¹ danych i przesy³ania/odbierania plików miêdzy serwerem i klientem. Obecnie implementacja opiera siê o protokó³ FTP
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

//! Klasa odpowiedzialna za dostarczanie plików z bazy danych
class CommunicationManager
{
private:
    //! Typ lokalnego lokowania obiektów synchronizuj¹cych
    typedef OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> ScopedLock;

	//! W¹tek przetwarzaj¹cy zlecenia z CommunicationManagera
	class ProcessingThread : public OpenThreads::Thread
	{
	public:
		//! Konstruktor
		//! \patam manager Manager którego obs³ugujemy
		ProcessingThread(CommunicationManager * manager);
		virtual ~ProcessingThread();

		//! Metoda obs³uguj¹ca managera
		virtual void run();
		//! \param ms Iloœc milisekund jakie mam spaæ kiedy nie ma nic do przetwarzania
		void setIdleSleep(unsigned int ms);
		//! \return Iloœc milisekund jakie mam spaæ kiedy nie ma nic do przetwarzania
		unsigned int idleSleep() const;
		//! Iformujê w¹tek ¿e ma siê zakoñczyæ jak najszybciej - przy kolejnym przejsciu pêtli
		//! Po tym wywo³aniu bêdê na niego najprawdopodobniej czeka³!!
		void finish();

	private:
		//! Obs³ugiwnay manager
		CommunicationManager * manager;
		//! Iloœc milisekund jakie mam spaæ kiedy nie ma nic do przetwarzania
		unsigned int idleSleep_;
		//! Czy koñczyæ pracê?
		bool finish_;
	};

	friend class ProcessingThread;

public:
    //! Typ zlecenie
    enum Request
    {
        DownloadFile,           //! Proœba o plik
        Complex,                //! Z³o¿ona proœba (mo¿na zagnie¿d¿aæ i dodawaæ wszystkie pozosta³e
        DownloadPhoto,          //! Proœba o plik
        CopyMotionShallowCopy,  //! Proœba o plik z p³ytk¹ kopi¹ bazy danych ruchu
        CopyMotionMetadata,     //! Proœba o plik z metadanymi ruchu
        CopyMedicalShallowCopy, //! Proœba o plik z p³yk¹ kopia danych medycznych
        CopyMedicalMetadata,    //! Proœba o plik z metadanymi medycznymi
        PingServer              //! Proœba o sprawdzenie dostêpnoœci serwera poprzez ping
    };

    /**
    Stany managera pomocne przy wykonywaniu dzia³añ wspó³bie¿nie.
    */
    enum State
    {
        Ready, /** Gotowy do wszelkich dzia³añ */
        ProcessingComplex, /** Przetwarzanie kompleksowego zapytania */
        DownloadingFile, /** Trwa pobieranie pojedynczego pliku */
        DownloadingPhoto, /** Trwa pobieranie pojedynczego zdjêcia */
        CopyingMotionShallowCopy, /** Trwa pobieranie pliku z p³ytk¹ kopi¹ bazy danych ruchu*/
        CopyingMotionMetadata, /** Trwa pobieranie pliku z metadanymi ruchu*/
        CopyingMedicalShallowCopy, /** Trwa pobieranie pliku z p³yk¹ kopia danych medycznych*/
        CopyingMedicalMetadata, /** Trwa pobieranie pliku z metadanymi medycznymi*/
        PingingServer /** Pingowanie serwera */
    };

	//! Forward declaration
    class MetadataRequest;
    class FileRequest;
    class PhotoRequest;
    class ComplexRequest;

    //! Klasa reprezentuj¹ca podstawê wszystkich zleceñ
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
		//! \return Postêp zadania
        virtual double getProgress() const = 0;
		//! \return Czy zadanie zakoñczone
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
		//! \param ustawiamy czy serwer nam odpowiedzia³
		void setServerResponse(bool response);

	public:
		//! \return Adres serwera który bêdziemy/ju¿ pingowaliœmy
		const std::string & urlToPing() const;
		//! \return Postêp zadania
		virtual double getProgress() const;

	private:
		//! Adres serwera do pingowania
		std::string urlToPing_;

		//! Czy serwer odpowiedzia³
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
		//! \param filePath Œcie¿ka zapisu pliku
        MetadataRequest(Request type, const std::string & filePath);

    public:
		//! \return Œcie¿ka pliku
        const std::string & getFilePath() const;
		//! Implementacja interfejsu webservices::IFtpsConnection::IProgress
        virtual void setProgress(double p);
		//! Impementacja interfejsu webservices::IFtpsConnection::IProgress
        virtual double getProgress() const;

    private:
		//! Progres œci¹gania pliku
        double progress;
		//! Œcie¿ka docelowa pliku
        std::string filePath;
    };

	//! Klasa odpowiedzialna za request plików (z identyfikatorami)
    class FileRequest : public MetadataRequest
    {
        friend class CommunicationManager;

    private:
		//! Kosntruktor
		//! \param filePath Œcie¿ka zapisu pliku
		//! \param fileID ID pliku do œci¹gniêcia
        FileRequest(const std::string & filePath, unsigned int fileID);

    public:
		//! \return Identyfikator pliku do œci¹gniêcia
        unsigned int getFileID() const;

    private:
		//! Identyfikator pliku do œci¹gniêcia
        unsigned int fileID;
    };

	//! Klasa odpowiedzialna za request zdjêæ
    class PhotoRequest : public MetadataRequest
    {
        friend class CommunicationManager;

    private:
		//! Kosntruktor
		//! \param filePath Œcie¿ka zapisu pliku
		//! \param photoID ID zdjêcia do œci¹gniêcia
        PhotoRequest(const std::string & filePath, unsigned int photoID);

    public:
		//! Identyfikator zdjêcia
        unsigned int getPhotoID() const;

    private:
		//! Identyfikator zdjêcia
        unsigned int photoID;
    };

    typedef core::shared_ptr<BasicRequest> BasicRequestPtr;
	typedef core::shared_ptr<PingRequest> PingRequestPtr;
    typedef core::shared_ptr<MetadataRequest> MetadataRequestPtr;
    typedef core::shared_ptr<FileRequest> FileRequestPtr;
    typedef core::shared_ptr<PhotoRequest> PhotoRequestPtr;

    typedef boost::function<void(const BasicRequestPtr &)> RequestCallback;
    typedef boost::function<void(const BasicRequestPtr &, const std::string &)> RequestErrorCallback;

    //! Struktura przechowuj¹ca metody wywo³ywane przy odpowiednich etapach wykonania zlecenia
    struct RequestCallbacks
    {
        //! Wywo³anie na rozpoczêcie przetwarzania zlecenia
        RequestCallback onBeginCallback;
        //! Wywo³anie na zakoñczenie przetwarzania zlecenia (powodzenie)
        RequestCallback onEndCallback;
        //! Wywo³anie na anulowanie przetwarzania zlecenia
        RequestCallback onCancelCallback;
        //! Wywo³anie na zakoñczenie przetwarzanie zlecenia z b³êdem
        RequestErrorCallback onErrorCallback;
    };

    /**
    Typ zlecenia przekazywany do kolejki zleceñ CommunicationManagera
    */
    struct CompleteRequest
    {
		//! Request
        BasicRequestPtr request;
		//! Callbacki requesta
        RequestCallbacks callbacks;
    };

	//! Typ kolejki requestów do obs³ugi
    typedef std::queue<CompleteRequest> RequestsQueue;

    //! Z³o¿one zlecenie
    class ComplexRequest : public BasicRequest
    {
        friend class CommunicationManager;

    private:
        ComplexRequest(const std::vector<CompleteRequest> & requests);

    public:
		//! \return Iloœæ zagnie¿d¿onych requestów
        unsigned int size() const;
		//! \return Czy request jest pusty
        bool empty() const;
		//! \param i Indeks zagnie¿d¿onego requesta
        const CompleteRequest & getRequest(unsigned int i) const;
		//! \return Ca³kowity postêp requesta
        virtual double getProgress() const;

    private:
		//! Zagnie¿d¿one requesty
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
    @param buffer wskaŸnik do bloku pamiêci o rozmiarze size*nmemb
    @param size rozmiar w bajtach elementu do odczytania
    @param nmemb liczba elementów do odczytania
    @param stream wskaŸnik na strumieñ danych
    @return iloœæ bajtów przetworzonych przez funkcjê
    */
    static size_t pingDataCallback(void *buffer, size_t size, size_t nmemb, void *stream);

	//! \param request [out] Kolejny request do obs³ugi
    void popRequest(CompleteRequest & reuest);

	//! \param downloaderHelper Obiekt pomagaj¹cy prz œci¹ganiu plików zale¿ny od tego czy œci¹gamy plik danych, zdjêcie czy metadane
    void setCurrentDownloadHelper(webservices::IDownloadHelper * downloadHelper);

	//! Funkcje przetwarzaj¹ce ró¿ne requesty i zapamiêtuj¹ce b³êdy powsta³e podczas przetwarzania
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

	//! \param motionFileStoremanService Serwis do œci¹gania plików ruchu
	void setMotionFileStoremanService(const webservices::MotionFileStoremanWSPtr & motionFileStoremanService);
	//! \param medicalFileStoremanService Serwis do œci¹gania plików medycznych
	void setMedicalFileStoremanService(const webservices::MedicalFileStoremanWSPtr & medicalFileStoremanService);

	//! \param medicalFtps Ftps do œci¹gania danych medycznych
	void setMedicalFtps(const webservices::FtpsConnectionPtr & medicalFtps);
	//! \param motionFtps Ftps do œci¹gania danych ruchu
	void setMotionFtps(const webservices::FtpsConnectionPtr & motionFtps);

	//! \return Po³¹cznie medyczne
	const WSConnectionPtr & medicalConnection();
	//! \return Po³¹cznie ruchu
	const WSConnectionPtr & motionConnection();

	//! \return Ftps do œci¹gania danych medycznych
	const webservices::FtpsConnectionPtr & medicalFtps();
	//! \return Ftps do œci¹gania danych ruchu
	const webservices::FtpsConnectionPtr & motionFtps();

	//! \param request Request wrzucany do kolejki managera - bêdzie przetworzony kiedy wszystkie przed nim zostan¹ obs³u¿one
    void pushRequest(const CompleteRequest & request);

	//! Metody tworz¹ce obs³ugiwane typy zleceñ
    static ComplexRequestPtr createRequestComplex(const std::vector<CompleteRequest> & requests);
    static FileRequestPtr createRequestFile(unsigned int fileID, const std::string & filePath);
    static PhotoRequestPtr createRequestPhoto(unsigned int fileID, const std::string & filePath);
    static MetadataRequestPtr createRequestMotionShallowCopy(const std::string & filePath);
    static MetadataRequestPtr createRequestMotionMetadata(const std::string & filePath);
    static MetadataRequestPtr createRequestMedicalShallowCopy(const std::string & filePath);
    static MetadataRequestPtr createRequestMedicalMetadata(const std::string & filePath);
    static PingRequestPtr createRequestPing(const std::string & urlToPing);

	//! \param request Zlecenie które chcemy anulowaæ jeœli jeszcze nie by³o przetwarzane - do przetworzenia lub przetwarzane
    void cancelRequest(const BasicRequestPtr & request);

	//! \return Prawda jeœli w kolejce nie ma ju¿ nic do przetwarznia - manager jest w stanie IDLE
    bool requestsQueueEmpty() const;

	//! Anyluje wszystkie zlecenia w kolejce managera
    void cancelAllPendingRequests();

    /**
    Zwraca postêp w procentach aktualnie wykonywanego zadania jako pejedynczego transferu
    @return wartoœæ procentowa (od 0 do 100) pokazuj¹ca postêp wykonywanej operacji
    */
    int getProgress() const;

private:
    /**
    Ustala stan w jakim znajduje siê Communication Service.
    @param state stan jaki ustaliæ jako aktualny dla CS
    */
    void setState(State state);

public:
    /**
    Sprawdza stan w jakim znajduje siê Communication Service.
    @return aktualny stan CS
    */
    State getState();

public:

    /**
    Statyczna metoda pobieraj¹ca jedyna instancjê klasy CommuniationManagera.
    @return jedyna instancja CommunicationManagera
    */
    static CommunicationManager* getInstance();
    /**
    Statyczna metoda usuwaj¹ca jedyn¹ instancjê CommunicationManagera.
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
    WskaŸnik na klasê IDownloadHelper zajmuj¹c¹ siê aktualmnym pobieraniem
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
    Kolejka zapytañ
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
    Muteks zabezpieczaj¹cy przed zakleszczeniami.
    */
    OpenThreads::ReentrantMutex trialsMutex;
    /**
    Muteks synchronizuj¹cy obs³uge kolejki zleceñ
    */
    mutable OpenThreads::ReentrantMutex requestsMutex;

    mutable OpenThreads::ReentrantMutex downloadHelperMutex;
	//! Czy koñczymy przetwarzanie
    bool finish;
	//! Czy przerwaæ œci¹ganie
    bool cancelDownloading;
	//! W¹tek przetwarzaj¹cy zlecenia communication managera
	core::shared_ptr<ProcessingThread> processingThread;
};

#endif //HEADER_GUARD_COMMUNICATION_COMMUNICATIONMANAGER_H__
