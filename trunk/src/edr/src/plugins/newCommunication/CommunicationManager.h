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

//! Klasa odpowiedzialna za dostarczanie plików z bazy danych
class CommunicationManager : public utils::Observable<CommunicationManager>, private OpenThreads::Thread
{
private:
    //! Typ lokalnego lokowania obiektów synchronizuj¹cych
    typedef OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> ScopedLock;

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
        virtual double getProgress() const;
		//! \return Czy zadanie zakoñczone
        bool isComplete() const;

    private:
        BasicRequest(Request type);

    private:
        mutable OpenThreads::Mutex cancelLock;
        bool canceled;
        Request type;
    };

    class MetadataRequest : public BasicRequest, public webservices::IFtpsConnection::IProgress
    {
        friend class CommunicationManager;
        friend class FileRequest;
        friend class PhotoRequest;

    private:

        MetadataRequest(Request type, const std::string & filePath);

    public:
        const std::string & getFilePath() const;

        virtual void setProgress(double p);

        virtual double getProgress() const;

    private:
        double progress;
        std::string filePath;
    };

    class FileRequest : public MetadataRequest
    {
        friend class CommunicationManager;

    private:

        FileRequest(const std::string & filePath, unsigned int fileID);

    public:

        unsigned int getFileID() const;

    private:

        unsigned int fileID;
    };

    class PhotoRequest : public MetadataRequest
    {
        friend class CommunicationManager;

    private:

        PhotoRequest(const std::string & filePath, unsigned int photoID);
            
    public:

        unsigned int getPhotoID() const;

    private:

        unsigned int photoID;
    };

    typedef core::shared_ptr<BasicRequest> BasicRequestPtr;
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
        BasicRequestPtr request;
        RequestCallbacks callbacks;
    };

    typedef std::queue<CompleteRequest> RequestsQueue;

    //! Z³o¿one zlecenie
    class ComplexRequest : public BasicRequest
    {
        friend class CommunicationManager;

    private:
        ComplexRequest(const std::vector<CompleteRequest> & requests);

    public:

        unsigned int size() const;

        bool empty() const;

        const CompleteRequest & getRequest(unsigned int i) const;

        virtual double getProgress() const;

    private:
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

    void popRequest(CompleteRequest & reuest);

    void setCurrentDownloadHelper(webservices::IDownloadHelper * downloadHelper);

    webservices::IFtpsConnection::OperationStatus processComplex(const CompleteRequest & request, std::string & message = std::string());
    webservices::IFtpsConnection::OperationStatus processPhoto(const CompleteRequest & request, std::string & message = std::string());
    webservices::IFtpsConnection::OperationStatus processFile(const CompleteRequest & request, std::string & message = std::string());
    webservices::IFtpsConnection::OperationStatus processMotionShallowCopy(const CompleteRequest & request, std::string & message = std::string());
    webservices::IFtpsConnection::OperationStatus processMotionMetadata(const CompleteRequest & request, std::string & message = std::string());
    webservices::IFtpsConnection::OperationStatus processMedicalShallowCopy(const CompleteRequest & request, std::string & message = std::string());
    webservices::IFtpsConnection::OperationStatus processMedicalMetadata(const CompleteRequest & request, std::string & message = std::string());
    webservices::IFtpsConnection::OperationStatus processPing(const CompleteRequest & request, std::string & message = std::string());

public:

	void setUrlToPingServer(const std::string & urlToPing);

	void setMotionFileStoremanService(const webservices::MotionFileStoremanWSPtr & motionFileStoremanService);
	void setMedicalFileStoremanService(const webservices::MedicalFileStoremanWSPtr & medicalFileStoremanService);

	void setMedicalFtps(const webservices::FtpsConnectionPtr & medicalFtps);
	void setMotionFtps(const webservices::FtpsConnectionPtr & motionFtps);

	const WSConnectionPtr & medicalConnection();
	const WSConnectionPtr & motionConnection();

	const webservices::FtpsConnectionPtr & medicalFtps();
	const webservices::FtpsConnectionPtr & motionFtps();

    void pushRequest(const CompleteRequest & request);

    static ComplexRequestPtr createRequestComplex(const std::vector<CompleteRequest> & requests);
    static FileRequestPtr createRequestFile(unsigned int fileID, const std::string & filePath);
    static PhotoRequestPtr createRequestPhoto(unsigned int fileID, const std::string & filePath);
    static MetadataRequestPtr createRequestMotionShallowCopy(const std::string & filePath);
    static MetadataRequestPtr createRequestMotionMetadata(const std::string & filePath);
    static MetadataRequestPtr createRequestMedicalShallowCopy(const std::string & filePath);
    static MetadataRequestPtr createRequestMedicalMetadata(const std::string & filePath);
    static BasicRequestPtr createRequestPing();

    void cancelRequest(const BasicRequestPtr & request);

    bool requestsQueueEmpty() const;

    void cancelAllPendingRequests();

    /**
    Zwraca postêp w procentach aktualnie wykonywanego zadania jako pejedynczego transferu
    @return wartoœæ procentowa (od 0 do 100) pokazuj¹ca postêp wykonywanej operacji
    */
    int getProgress() const;

protected:
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
        
    /**
    Podaje informacjê czy serwer odpowiedzia³ na ostatni ping.
    @return czy serwer odpowiedzia³?
    */
    bool isServerResponse() const;

private:

    /**
    Metoda run pochodzi z interfejsu OpenThreads::Thread i zosta³a przes³oniêta do przeniesienia operacji do osobnego w¹tku.
    */

    virtual void run();

    void init();

    void deinit();

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
    WskaŸnik na klasê MotionTransportWSDL_FTPS odpowiedzialn¹ za transport danych ruchu
    */
    //core::shared_ptr<communication::MotionTransportWSDL_FTPS> motionTransportManager;

    /**
    WskaŸnik na klasê MotionTransportWSDL_FTPS odpowiedzialn¹ za transport danych medycznych
    */
    //core::shared_ptr<communication::MedicalTransportWSDL_FTPS> medicalTransportManager;

    /**
    WskaŸnik na klasê IDownloadHelper zajmuj¹c¹ siê aktualmnym pobieraniem
    */
    webservices::IDownloadHelper * currentDownloadHelper;

    /**
    WskaŸnik na klasê MotionQueryWSDL odpowiedzialn¹ za odpytywanie bazy danych
    */
    //core::shared_ptr<communication::MotionQueryWSDL> motionQueryManager;

    /**
    WskaŸnik na klasê MotionQueryWSDL odpowiedzialn¹ za odpytywanie bazy danych
    */
    //core::shared_ptr<communication::MedicalQueryWSDL> medicalQueryManager;

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
    Informacja o stanie instancji curla.
    */
    CURLcode pingCurlResult;
    /**
    Muteks zabezpieczaj¹cy przed zakleszczeniami.
    */
    OpenThreads::ReentrantMutex trialsMutex;
    /**
    Muteks synchronizuj¹cy obs³uge kolejki zleceñ
    */
    mutable OpenThreads::ReentrantMutex requestsMutex;

	mutable OpenThreads::ReentrantMutex curlMutex;

    mutable OpenThreads::ReentrantMutex downloadHelperMutex;

    bool finish;

    bool cancelDownloading;
};

#endif //HEADER_GUARD_COMMUNICATION_COMMUNICATIONMANAGER_H__
