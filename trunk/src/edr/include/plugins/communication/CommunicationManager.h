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
            Complex,
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
            ProcessingComplex, /** Przetwarzanie kompleksowego zapytania */
            DownloadingFile, /** Trwa pobieranie pojedynczego pliku */
            DownloadingPhoto, /** Trwa pobieranie pojedynczego pliku */
            CopyingMotionShallowCopy, /** Trwa odnawianie informacji o encjach db*/
            CopyingMotionMetadata, /** Trwa odnawianie informacji o encjach db*/
            CopyingMedicalShallowCopy, /** Trwa odnawianie informacji o encjach db*/
            CopyingMedicalMetadata, /** Trwa odnawianie informacji o encjach db*/
            PingingServer /** Pingowanie serwera */
        };


        class MetadataRequest;
        class FileRequest;
        class PhotoRequest;
        class ComplexRequest;

        class BasicRequest
        {
            friend class CommunicationManager;
            friend class MetadataRequest;
            friend class FileRequest;
            friend class PhotoRequest;
            friend class ComplexRequest;

        public:

            virtual ~BasicRequest() {}

            void cancel()
            {
                OpenThreads::ScopedLock<OpenThreads::Mutex> lock(cancelLock);
                cancelled = true;
            }

            bool isCancelled() const
            {
                OpenThreads::ScopedLock<OpenThreads::Mutex> lock(cancelLock);
                return cancelled;
            }

            Request getType() const
            {
                return type;
            }

            virtual double getProgress() const
            {
                return 0;
            }

            bool isComplete() const
            {
                return getProgress() == 100.0;
            }

        private:
            BasicRequest(Request type) : type(type), cancelled(false) {}

        private:
            mutable OpenThreads::Mutex cancelLock;
            bool cancelled;
            Request type;
        };

        class MetadataRequest : public BasicRequest, public FtpsConnection::IProgress
        {
            friend class CommunicationManager;
            friend class FileRequest;
            friend class PhotoRequest;

        private:

            MetadataRequest(Request type, const std::string & filePath) : BasicRequest(type), filePath(filePath), progress(0) {}

        public:
            const std::string & getFilePath() const
            {
                return filePath;
            }

            virtual void setProgress(double p)
            {
                progress = p;
            }

            virtual double getProgress() const
            {
                return progress;
            }

        private:
            double progress;
            std::string filePath;
        };

        class FileRequest : public MetadataRequest
        {
            friend class CommunicationManager;

        private:

            FileRequest(const std::string & filePath, unsigned int fileID) : MetadataRequest(DownloadFile, filePath), fileID(fileID) {}

        public:

            unsigned int getFileID() const
            {
                return fileID;
            }

        private:

            unsigned int fileID;
        };

        class PhotoRequest : public MetadataRequest
        {
            friend class CommunicationManager;

        private:

            PhotoRequest(const std::string & filePath, unsigned int photoID) : MetadataRequest(DownloadPhoto, filePath), photoID(photoID) {}
            
        public:

            unsigned int getPhotoID() const
            {
                return photoID;
            }

        private:

            unsigned int photoID;
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

        /**
        Typ zlecenia przekazywany do kolejki zlece� CommunicationManagera
        */
        struct CompleteRequest
        {
            RequestPtr request;
            RequestCallbacks callbacks;
        };

        typedef std::queue<CompleteRequest> RequestsQueue;

        class ComplexRequest : public BasicRequest
        {
            friend class CommunicationManager;

        private:
            ComplexRequest(const std::vector<CompleteRequest> & requests) : BasicRequest(Complex), requests(requests) {}

        public:

            unsigned int size() const
            {
                return requests.size();
            }

            bool empty() const
            {
                return requests.empty();
            }

            const CompleteRequest & getRequest(unsigned int i) const
            {
                return requests.at(i);
            }

            virtual double getProgress() const
            {
                double ret = 0;

                for(auto it = requests.begin(); it != requests.end(); it++){
                    double loc = (*it).request->getProgress();
                    if(loc == 0){
                        break;
                    }

                    ret += loc;
                }

                return 100.0 * (ret / (100 * requests.size()));
            }

        private:
            std::vector<CompleteRequest> requests;
        };

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

        void setCurrentTransportManager(const core::shared_ptr<communication::TransportWSDL_FTPSBase> & transportManager)
        {
            ScopedLock lock(transportManagerMutex);
            currentTransportManager = transportManager;
        }

        FtpsConnection::OperationStatus processComplex(const CompleteRequest & request, std::string & message = std::string());
        FtpsConnection::OperationStatus processPhoto(const CompleteRequest & request, std::string & message = std::string());
        FtpsConnection::OperationStatus processFile(const CompleteRequest & request, std::string & message = std::string());
        FtpsConnection::OperationStatus processMotionShallowCopy(const CompleteRequest & request, std::string & message = std::string());
        FtpsConnection::OperationStatus processMotionMetadata(const CompleteRequest & request, std::string & message = std::string());
        FtpsConnection::OperationStatus processMedicalShallowCopy(const CompleteRequest & request, std::string & message = std::string());
        FtpsConnection::OperationStatus processMedicalMetadata(const CompleteRequest & request, std::string & message = std::string());
        FtpsConnection::OperationStatus processPing(const CompleteRequest & request, std::string & message = std::string());

    public:

        void pushRequest(const CompleteRequest & request)
        {
            ScopedLock lock(requestsMutex);
            requestsQueue.push(request);

            if(isRunning() == false){
                start();
            }
        }

        static RequestPtr createRequestComplex(const std::vector<CompleteRequest> & requests);
        static RequestPtr createRequestFile(unsigned int fileID, const std::string & filePath);
        static RequestPtr createRequestPhoto(unsigned int fileID, const std::string & filePath);
        static RequestPtr createRequestMotionShallowCopy(const std::string & filePath);
        static RequestPtr createRequestMotionMetadata(const std::string & filePath);
        static RequestPtr createRequestMedicalShallowCopy(const std::string & filePath);
        static RequestPtr createRequestMedicalMetadata(const std::string & filePath);
        static RequestPtr createRequestPing();

        void cancelRequest(const RequestPtr & request);

        bool requestsQueueEmpty() const
        {
            ScopedLock lock(requestsMutex);
            return requestsQueue.empty();
        }

        void cancelAllPendingRequests()
        {
            ScopedLock lock(requestsMutex);
            while(requestsQueue.empty() == false)
            {
                if(requestsQueue.front().callbacks.onCancelCallback.empty() == false){
                    requestsQueue.front().callbacks.onCancelCallback(requestsQueue.front().request);
                }

                requestsQueue.pop();
            }
        }

        /**
        Zwraca post�p w procentach aktualnie wykonywanego zadania jako pejedynczego transferu
        @return warto�� procentowa (od 0 do 100) pokazuj�ca post�p wykonywanej operacji
        */
        int getProgress() const;

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

    public:
        /**
        Sprawdza stan w jakim znajduje si� Communication Service.
        @return aktualny stan CS
        */
        State getState()
        {
            ScopedLock lock(trialsMutex);
            return state;
        };
        
        /**
        Podaje informacj� czy serwer odpowiedzia� na ostatni ping.
        @return czy serwer odpowiedzia�?
        */
        bool isServerResponse() const
        {
            return serverResponse;
        };

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

        /**
        Jedyna instancja klasy CommunicationManager.
        */
        static CommunicationManager* instance;
        /**
        Wska�nik na klas� MotionTransportWSDL_FTPS odpowiedzialn� za transport danych ruchu
        */
        core::shared_ptr<communication::MotionTransportWSDL_FTPS> motionTransportManager;

        /**
        Wska�nik na klas� MotionTransportWSDL_FTPS odpowiedzialn� za transport danych medycznych
        */
        core::shared_ptr<communication::MedicalTransportWSDL_FTPS> medicalTransportManager;

        /**
        Wska�nik na klas� TransportWSDL_FTPSBase odpowiedzialn� za aktualny transport danych
        */
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

        mutable OpenThreads::ReentrantMutex transportManagerMutex;

        bool finish;

        bool cancelDownloading;
    };
}
#endif //HEADER_GUARD_COMMUNICATION_COMMUNICATIONMANAGER_H__
