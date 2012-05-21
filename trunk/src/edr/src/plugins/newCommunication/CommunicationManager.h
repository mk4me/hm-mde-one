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

	//! Klasa odpowiedzialna za request plików z metadanymi (bez identyfikatorów) - implementuje progress pobierania
    class MetadataRequest : public BasicRequest, public webservices::IFtpsConnection::IProgress
    {
        friend class CommunicationManager;
        friend class FileRequest;
        friend class PhotoRequest;

    private:

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

        PhotoRequest(const std::string & filePath, unsigned int photoID);
            
    public:
		//! Identyfikator zdjêcia
        unsigned int getPhotoID() const;

    private:
		//! Identyfikator zdjêcia
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

    webservices::IFtpsConnection::OperationStatus processComplex(const CompleteRequest & request, std::string & message = std::string());
    webservices::IFtpsConnection::OperationStatus processPhoto(const CompleteRequest & request, std::string & message = std::string());
    webservices::IFtpsConnection::OperationStatus processFile(const CompleteRequest & request, std::string & message = std::string());
    webservices::IFtpsConnection::OperationStatus processMotionShallowCopy(const CompleteRequest & request, std::string & message = std::string());
    webservices::IFtpsConnection::OperationStatus processMotionMetadata(const CompleteRequest & request, std::string & message = std::string());
    webservices::IFtpsConnection::OperationStatus processMedicalShallowCopy(const CompleteRequest & request, std::string & message = std::string());
    webservices::IFtpsConnection::OperationStatus processMedicalMetadata(const CompleteRequest & request, std::string & message = std::string());
    webservices::IFtpsConnection::OperationStatus processPing(const CompleteRequest & request, std::string & message = std::string());

public:
	//! \param urlToPing Adres serwera który pingujemy
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

	//! Inicjalizuje w¹tek przetwarzaj¹cy requesty
    void init();

	//! Deinicjalizuje w¹tek przetwarzaj¹cy requesty
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
