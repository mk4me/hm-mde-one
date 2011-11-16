/**
@author Marek Daniluk
@brief Klasa CommunicationManager wykorzystuj�ca wzorce projektowe fasada oraz singleton do zarz�dzania po��czeniem
z baz� danych i przesy�ania/odbierania plik�w mi�dzy serwerem i klientem. Obecnie implementacja opiera si� o protok� FTP
i web serwisy wsdl.
*/

#ifndef HEADER_GUARD_COMMUNICATION_COMMUNICATIONMANAGER_H__
#define HEADER_GUARD_COMMUNICATION_COMMUNICATIONMANAGER_H__

#include <core/IDataManager.h>
#include <core/IServiceManager.h>
#include <plugins/communication/Entity.h>
#include <plugins/communication/QueryWSDL.h>
#include <plugins/communication/TransportWSDL_FTPS.h>
#include <OpenThreads/ScopedLock>
#include <OpenThreads/ReentrantMutex>
#include <queue>
#include <boost/function.hpp>

namespace communication
{
    class CommunicationManager : public utils::Observable<CommunicationManager>, private OpenThreads::Thread
    {
    private:

        typedef OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> ScopedLock;

    public:

        enum Request
        {
            DownloadFile,
            DownloadPhoto,
            CopyMotionShallowCopy,
            CopyMotionMetadata,
            CopyMedicalShallowCopy,
            CopyMedicalMetadata,
            PingServer
        };

        /**
        Stany managera pomocne przy wykonywaniu dzia�a� wsp�bie�nie.
        */
        enum State
        {
            Ready, /** Gotowy do wszelkich dzia�a� */
            DownloadingFile, /** Trwa pobieranie pojedynczego pliku */
            DownloadingPhoto, /** Trwa pobieranie pojedynczego pliku */
            CopyingMotionShallowCopy, /** Trwa odnawianie informacji o encjach db*/
            CopyingMotionMetadata, /** Trwa odnawianie informacji o encjach db*/
            CopyingMedicalShallowCopy, /** Trwa odnawianie informacji o encjach db*/
            CopyingMedicalMetadata, /** Trwa odnawianie informacji o encjach db*/
            PingingServer /** Pingowanie serwera */
        };

        struct BasicRequest
        {
            public:

            BasicRequest(bool cancelled, Request type) : cancelled(cancelled), type(type) {}

            public:

            bool cancelled;
            const Request type;
            virtual ~BasicRequest() {}
        };

        struct MetadataRequest : public BasicRequest
        {
            public:

            MetadataRequest(bool cancelled, Request type, const std::string & filePath) : BasicRequest(cancelled, type), filePath(filePath) {}

            public:

            const std::string filePath;
        };

        struct FileRequest : public MetadataRequest
        {
            public:

            FileRequest(bool cancelled, Request type, const std::string & filePath, unsigned int fileID) : MetadataRequest(cancelled, type, filePath), fileID(fileID) {}

            public:

            const unsigned int fileID;
        };

        struct PhotoRequest : public MetadataRequest
        {
            public:

            PhotoRequest(bool cancelled, Request type, const std::string & filePath, unsigned int photoID) : MetadataRequest(cancelled, type, filePath), photoID(photoID) {}

            public:

            const unsigned int photoID;
        };

        typedef core::shared_ptr<BasicRequest> RequestPtr;

        typedef boost::function<void(const RequestPtr &)> RequestCallback;
        typedef boost::function<void(const RequestPtr &, const std::string &)> RequestErrorCallback;

        struct RequestCallbacks
        {
            RequestCallback onBeginCallback;
            RequestCallback onEndCallback;
            RequestCallback onCancelCallback;
            RequestErrorCallback onErrorCallback;
        };

    private:

         /**
        Typ zlecenia przekazywany do kolejki zlece� CommunicationManagera
        */
        struct CompleteRequest
        {
            RequestPtr request;
            RequestCallbacks callbacks;
        };

        typedef std::queue<CompleteRequest> RequestsQueue;

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

        void popRequest(CompleteRequest & reuest)
        {
            ScopedLock lock(requestsMutex);
            reuest = requestsQueue.front();
            requestsQueue.pop();
        }

        void pushRequest(const CompleteRequest & request)
        {
            ScopedLock lock(requestsMutex);
            requestsQueue.push(request);

            if(isRunning() == false){
                start();
            }
        }

        void setCurrentTransportManager(const core::shared_ptr<communication::TransportWSDL_FTPSBase> & transportManager)
        {
            ScopedLock lock(transportManagerMutex);
            currentTransportManager = transportManager;
        }

    public:

        RequestPtr requestFile(unsigned int fileID, const std::string filePath, const RequestCallbacks & callbacks = RequestCallbacks());
        RequestPtr requestPhoto(unsigned int fileID, const std::string filePath, const RequestCallbacks & callbacks = RequestCallbacks());
        RequestPtr requestMotionShallowCopy(const std::string filePath, const RequestCallbacks & callbacks = RequestCallbacks());
        RequestPtr requestMotionMetadata(const std::string filePath, const RequestCallbacks & callbacks = RequestCallbacks());
        RequestPtr requestMedicalShallowCopy(const std::string filePath, const RequestCallbacks & callbacks = RequestCallbacks());
        RequestPtr requestMedicalMetadata(const std::string filePath, const RequestCallbacks & callbacks = RequestCallbacks());
        RequestPtr requestPing(const RequestCallbacks & callbacks = RequestCallbacks());

        void cancelRequest(const RequestPtr & request);

    //    /*core::Filesystem::Path getFileTheoreticalPath(const communication::MotionShallowCopy::File * file) const
    //    {
    //        return  getSessionTheoreticalPath( file->isSessionFile() == true ? file->session : file->trial->session) / file->fileName;
    //    }

    //    core::Filesystem::Path getSessionTheoreticalPath(const communication::MotionShallowCopy::Session * session) const
    //    {
    //        return getLocalDataPath() / session->sessionName;
    //    }

    //    core::Filesystem::Path getLocalDataPath() const
    //    {
    //        return localDataPath;
    //    }*/

        bool requestsQueueEmpty() const
        {
            ScopedLock lock(requestsMutex);
            return requestsQueue.empty();
        }

    //    /*void cancelAllPendingRequests()
    //    {
    //        ScopedLock lock(requestsMutex);
    //        while(requestsQueue.empty() == false)
    //        {
    //            if(requestsQueue.front().callbacks.onCancelCallback.empty() == false){
    //                requestsQueue.front().callbacks.onCancelCallback(requestsQueue.front().request);
    //            }

    //            requestsQueue.pop();
    //        }
    //    }*/

    //    ///**
    //    //@return p�ytka kopia DB ruchu
    //    //*/
    //    //const MotionShallowCopyConstPtr& getMotionShallowCopy() const
    //    //{
    //    //    return constMotionShallowCopy;
    //    //}
    //    ///**
    //    //@return metadane z DB ruchu
    //    //*/
    //    //const communication::MotionMetaData::MetaData& getMotionMetadata() const
    //    //{
    //    //    return motionMetaData;
    //    //}

    //    ///**
    //    //@return p�ytka kopia DB medycznych
    //    //*/
    //    //const MedicalShallowCopyConstPtr& getMedicalShallowCopy() const
    //    //{
    //    //    return constMedicalShallowCopy;
    //    //}

    //    ///**
    //    //@return metadane z DB medycznych
    //    //*/
    //    //const communication::MedicalMetaData::MetaData& getMedicalMetadata() const
    //    //{
    //    //    return medicalMetaData;
    //    //}

        /**
        Zwraca post�p w procentach aktualnie wykonywanego zadania jako pejedynczego transferu
        @return warto�� procentowa (od 0 do 100) pokazuj�ca post�p wykonywanej operacji
        */
        int getProgress() const;
    //    ///**
    //    //Zwraca post�p w procentach aktualnie wykonywanego zadania jako calosci
    //    //@return warto�� procentowa (od 0 do 100) pokazuj�ca post�p wykonywanej operacji
    //    //*/
    //    //int getTotalProgress() const;
    //    ///**
    //    //Informuje ile plik�w ma by� �ci�gni�tych przy operacji pobierania plik�w.
    //    //@return ile plik�w ma by� �ci�gnietych
    //    //*/
    //    //int getFilesToDownloadCount() const
    //    //{
    //    //    return filesToDownload;
    //    //};
    //    ///**
    //    //Informuje kt�ry z kolei plik jest aktualnie �ci�gany,
    //    //@return kt�ry plik z kolei jest aktualnie �ci�gany
    //    //*/
    //    //int getActualDownloadFileNumber() const
    //    //{
    //    //    return actualDownloadFileNumer;
    //    //};

    //    //! Ping serwera z baz� danych
    //    //void ping(const RequestCallbacks & callbacks = RequestCallbacks());

    //    /**
    //    P�ytka kopia bazy danych ruchu.
    //    */
    //    //void copyMotionDbData(const RequestCallbacks & callbacks = RequestCallbacks());

    //    /**
    //    P�ytka kopia bazy danych medycznych.
    //    */
    //    //void copyMedicalDbData(const RequestCallbacks & callbacks = RequestCallbacks());

    //    //const std::string & getTrialName(int trialID);

    //    //const std::string & getSessionName(int sessionID);

    //    //const std::string & getFileName(int fileID);

    //    //const communication::MotionShallowCopy::Trial * getTrial(int trialID);

    //    //const communication::MotionShallowCopy::Session * getSession(int sessionID);

    //    //const communication::MotionShallowCopy::Performer * getPerformer(int performerID);

    //    //const communication::MotionShallowCopy::File * getFile(int fileID);

    //    //void downloadPerformer(unsigned int perrformerID, const RequestCallbacks & callbacks = RequestCallbacks());

    //    //void downloadSession(unsigned int sessionID, const RequestCallbacks & callbacks = RequestCallbacks());

    //    ///**
    //    //Pobieranie pr�by pomiarowej.
    //    //@param trialID ID pr�by pomiarowej kt�ra ma by� pobrana
    //    //*/
    //    //void downloadTrial(unsigned int trialID, const RequestCallbacks & callbacks = RequestCallbacks());
    //    ///**
    //    //Pobieranie pojedynczego pliku.
    //    //@param fileID ID pliku kt�ry ma by� pobrany
    //    //*/
    //    //void downloadFile(unsigned int fileID, const RequestCallbacks & callbacks = RequestCallbacks());

    //    ///**
    //    //Pobieranie pojedynczego zdj�cia pacjenta.
    //    //@param photoID ID pliku ze zdj�ciem pacjenta kt�ry ma by� pobrany
    //    //*/
    //    //void downloadPhoto(unsigned int photoID, const RequestCallbacks & callbacks = RequestCallbacks());

        /**
        Przerwanie operacji pobierania pliku lub pr�by pomiarowej.
        */
        //void cancelDownload()
        //{
        //    //if(state != CopyingMotionDB){
        //    if(currentTransportManager != nullptr){
        //        currentTransportManager->abort();
        //        cancelDownloading = true;
        //    //}
        //};

    protected:
        /**
        Ustala stan w jakim znajduje si� Communication Service.
        @param state stan jaki ustali� jako aktualny dla CS
        */
        void setState(State state)
        {
            ScopedLock lock(trialsMutex);
            this->state = state;    
        };

    //public:
    //    /**
    //    Sprawdza stan w jakim znajduje si� Communication Service.
    //    @return aktualny stan CS
    //    */
    //    State getState()
    //    {
    //        ScopedLock lock(trialsMutex);
    //        return state;
    //    };
        
        /**
        Podaje informacj� czy serwer odpowiedzia� na ostatni ping.
        @return czy serwer odpowiedzia�?
        */
        /*bool isServerResponse() const
        {
            return serverResponse;
        };*/

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

        //unsigned int filesToDownload;
        //unsigned int actualDownloadFileNumer;

        /**
        Jedyna instancja klasy CommunicationManager.
        */
        static CommunicationManager* instance;
        /**
        Wska�nik na klas� MotionTransportWSDL_FTPS odpowiedzialn� za transport danych
        */
        core::shared_ptr<communication::MotionTransportWSDL_FTPS> motionTransportManager;

        /**
        Wska�nik na klas� MotionTransportWSDL_FTPS odpowiedzialn� za transport danych
        */
        core::shared_ptr<communication::MedicalTransportWSDL_FTPS> medicalTransportManager;

        core::shared_ptr<communication::TransportWSDL_FTPSBase> currentTransportManager;

        /**
        Wska�nik na klas� MotionQueryWSDL odpowiedzialn� za odpytywanie bazy danych
        */
        core::shared_ptr<communication::MotionQueryWSDL> motionQueryManager;

        /**
        Wska�nik na klas� MotionQueryWSDL odpowiedzialn� za odpytywanie bazy danych
        */
        core::shared_ptr<communication::MedicalQueryWSDL> medicalQueryManager;

        /**
        Stan Communication Service
        */
        State state;

        CompleteRequest currentRequest;

        /**
        Kolejka zapyta�
        */
        RequestsQueue requestsQueue;

        ///**
        //P�ytka kopia db ruchu
        //*/
        //MotionShallowCopyConstPtr constMotionShallowCopy;
        ///**
        //Metadane db ruchu
        //*/
        //communication::MotionMetaData::MetaData motionMetaData;

        ///**
        //P�ytka kopia db medycznej
        //*/
        //MedicalShallowCopyConstPtr constMedicalShallowCopy;
        ///**
        //Metadane db medycznej
        //*/
        //communication::MedicalMetaData::MetaData medicalMetaData;

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

        mutable OpenThreads::ReentrantMutex transportManagerMutex;
        /**
        Parsowanie plik�w xml p�ytkiej kopii bazy danych ruchu.
        */
        //void readMotionDbSchemas(const std::string& shallowCopyPath, const std::string& metaDataPath);

        /**
        Parsowanie plik�w xml p�ytkiej kopii bazy danych ruchu.
        */
        //void readMedicalDbSchemas(const std::string& shallowCopyPath, const std::string& metaDataPath);

        bool finish;

        bool cancelDownloading;

        /*std::string motionShallowCopyPath;
        std::string motionMetadataPath;

        std::string medicalShallowCopyPath;
        std::string medicalMetadataPath;

        std::string localDataPath;

        core::Filesystem::Path localDBImagesPath;*/
    };
}
#endif //HEADER_GUARD_COMMUNICATION_COMMUNICATIONMANAGER_H__
