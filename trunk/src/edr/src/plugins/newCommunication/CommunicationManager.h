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

//! Klasa odpowiedzialna za dostarczanie plik�w z bazy danych
class CommunicationManager : public utils::Observable<CommunicationManager>, private OpenThreads::Thread
{
private:
    //! Typ lokalnego lokowania obiekt�w synchronizuj�cych
    typedef OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> ScopedLock;

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
        virtual double getProgress() const;
		//! \return Czy zadanie zako�czone
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
        BasicRequestPtr request;
        RequestCallbacks callbacks;
    };

    typedef std::queue<CompleteRequest> RequestsQueue;

    //! Z�o�one zlecenie
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
    @param buffer wska�nik do bloku pami�ci o rozmiarze size*nmemb
    @param size rozmiar w bajtach elementu do odczytania
    @param nmemb liczba element�w do odczytania
    @param stream wska�nik na strumie� danych
    @return ilo�� bajt�w przetworzonych przez funkcj�
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
    Zwraca post�p w procentach aktualnie wykonywanego zadania jako pejedynczego transferu
    @return warto�� procentowa (od 0 do 100) pokazuj�ca post�p wykonywanej operacji
    */
    int getProgress() const;

protected:
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
        
    /**
    Podaje informacj� czy serwer odpowiedzia� na ostatni ping.
    @return czy serwer odpowiedzia�?
    */
    bool isServerResponse() const;

private:

    /**
    Metoda run pochodzi z interfejsu OpenThreads::Thread i zosta�a przes�oni�ta do przeniesienia operacji do osobnego w�tku.
    */

    virtual void run();

    void init();

    void deinit();

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
    Wska�nik na klas� MotionTransportWSDL_FTPS odpowiedzialn� za transport danych ruchu
    */
    //core::shared_ptr<communication::MotionTransportWSDL_FTPS> motionTransportManager;

    /**
    Wska�nik na klas� MotionTransportWSDL_FTPS odpowiedzialn� za transport danych medycznych
    */
    //core::shared_ptr<communication::MedicalTransportWSDL_FTPS> medicalTransportManager;

    /**
    Wska�nik na klas� IDownloadHelper zajmuj�c� si� aktualmnym pobieraniem
    */
    webservices::IDownloadHelper * currentDownloadHelper;

    /**
    Wska�nik na klas� MotionQueryWSDL odpowiedzialn� za odpytywanie bazy danych
    */
    //core::shared_ptr<communication::MotionQueryWSDL> motionQueryManager;

    /**
    Wska�nik na klas� MotionQueryWSDL odpowiedzialn� za odpytywanie bazy danych
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
    Informacja o stanie instancji curla.
    */
    CURLcode pingCurlResult;
    /**
    Muteks zabezpieczaj�cy przed zakleszczeniami.
    */
    OpenThreads::ReentrantMutex trialsMutex;
    /**
    Muteks synchronizuj�cy obs�uge kolejki zlece�
    */
    mutable OpenThreads::ReentrantMutex requestsMutex;

	mutable OpenThreads::ReentrantMutex curlMutex;

    mutable OpenThreads::ReentrantMutex downloadHelperMutex;

    bool finish;

    bool cancelDownloading;
};

#endif //HEADER_GUARD_COMMUNICATION_COMMUNICATIONMANAGER_H__
