/**
@author Marek Daniluk
@brief Klasa CommunicationManager wykorzystuj¹ca wzorce projektowe fasada oraz singleton do zarz¹dzania po³¹czeniem
z baz¹ danych i przesy³ania/odbierania plików miêdzy serwerem i klientem. Obecnie implementacja opiera siê o protokó³ FTP
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
#include <OpenThreads/Mutex>
#include <queue>
#include <boost/function.hpp>

typedef OpenThreads::ScopedLock<OpenThreads::Mutex> ScopedLock;

namespace communication
{
    class CommunicationManager : public utils::Observable<CommunicationManager>, private OpenThreads::Thread
    {
    private:

        //! Sekwencja lokalnych prób pomiarowych.
        typedef std::vector<core::IDataManager::LocalTrial> LocalTrialsList;

    public:

        enum Request
        {
            DownloadFile,
            DownloadTrial,
            CopyDB,
            PingServer
            //UpdateTrials
        };

        /**
        Stany managera pomocne przy wykonywaniu dzia³añ wspó³bie¿nie.
        */
        enum State
        {
            Ready, /** Gotowy do wszelkich dzia³añ */
            DownloadingFile, /** Trwa pobieranie pojedynczego pliku */
            DownloadingTrial, /** Trwa pobieranie plików próby pomiarowej */
            //UpdateTrials, /** Trwa odnawianie informacji o zasobach bazodanowych */
            CopyingDB, /** Trwa odnawianie informacji o encjach db*/
            PingingServer, /** Pingowanie serwera */
        };

        struct BasicRequest
        {
            Request type;
            unsigned int id;
        };

        typedef boost::function<void(const BasicRequest &)> RequestCallback;
        typedef boost::function<void(const BasicRequest &, const std::string &)> RequestErrorCallback;

        struct RequestCallbacks
        {
            RequestCallback onBeginCallback;
            RequestCallback onEndCallback;
            RequestCallback onCancelCallback;
            RequestErrorCallback onErrorCallback;
        };

        /**
        Typ zlecenia przekazywany do kolejki zleceñ CommunicationManagera
        */
        struct CompleteRequest
        {
            BasicRequest request;
            RequestCallbacks callbacks;
        };

        typedef std::queue<CompleteRequest> RequestsQueue;

    private:

        void popRequest(CompleteRequest & reuest)
        {
            ScopedLock lock(requestsMutex);
            reuest = requestsQueue.front();
            requestsQueue.pop();
        }

    public:

        void pushRequest(const CompleteRequest & request)
        {
            ScopedLock lock(requestsMutex);
            requestsQueue.push(request);

            if(isRunning() == false){
                start();
            }
        }

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
        @return p³ytka kopia DB
        */
        const communication::ShallowCopy::ShallowCopy& getShalloCopy() const
        {
            return shallowCopy;
        }
        /**
        @return metadane z DB
        */
        const communication::MetaData::MetaData& getMetadata() const
        {
            return metaData;
        }
        /**
        £adowanie lokalnych prób pomiarowych. Metoda wykorzystuje DataManagera.
        */
        void loadLocalTrials();
        /**
        Zwraca postêp w procentach aktualnie wykonywanego zadania jako pejedynczego transferu
        @return wartoœæ procentowa (od 0 do 100) pokazuj¹ca postêp wykonywanej operacji
        */
        int getProgress() const;
        /**
        Zwraca postêp w procentach aktualnie wykonywanego zadania jako calosci
        @return wartoœæ procentowa (od 0 do 100) pokazuj¹ca postêp wykonywanej operacji
        */
        int getTotalProgress() const;
        /**
        Informuje ile plików ma byæ œci¹gniêtych przy operacji pobierania plików.
        @return ile plików ma byæ œci¹gnietych
        */
        int getFilesToDownloadCount() const
        {
            return filesToDownload;
        };
        /**
        Informuje który z kolei plik jest aktualnie œci¹gany,
        @return który plik z kolei jest aktualnie œci¹gany
        */
        int getActualDownloadFileNumber() const
        {
            return actualDownloadFileNumer;
        };

        void ping(const RequestCallbacks & callbacks = RequestCallbacks());

        /**
        P³ytka kopia bazy danych.
        */
        void copyDbData(const RequestCallbacks & callbacks = RequestCallbacks());

        std::string getTrialDirectoryName(int trialID);

        /**
        Pobieranie próby pomiarowej.
        @param trialID ID próby pomiarowej która ma byæ pobrana
        */
        void downloadTrial(unsigned int trialID, const RequestCallbacks & callbacks = RequestCallbacks());
        /**
        Pobieranie pojedynczego pliku.
        @param fileID ID pliku który ma byæ pobrany
        */
        void downloadFile(unsigned int fileID, const RequestCallbacks & callbacks = RequestCallbacks());
        ///**
        //Przerwanie operacji pobierania pliku lub próby pomiarowej.
        //*/
        void cancelDownload()
        {
            if(state != CopyingDB){
                transportManager->abort();
                cancelDownloading = true;
            }
        };
        /**
        Listuje próby pomiarowe znajduj¹ce siê na lokalnym dysku.
        @return lista prób pomiarowych z lokalnego dysku
        */
        const LocalTrialsList& getLocalTrials() const
        {
            return localTrials;
        };
        /**
        Ustala Data Managera. Potrzebne przy ³adowaniu prób pomiarowych z lokalnego dysku.
        @param dataManager wskaŸnik na instancjê Data Managera
        */
        void setDataManager(core::IDataManager* dataManager)
        {
            this->dataManager = dataManager;
            loadLocalTrials();
        };
        /**
        Zwraca wskaŸnik na instancjê Data Managera.
        @return wskaŸnik na instancjê Data Managera
        */
        core::IDataManager* getDataManager()
        {
            return dataManager;
        };
        /**
        Metoda ³aduje do edytora listê plików.
        @param files lista plików do za³adowania
        */
        void loadFiles(const std::vector<core::Filesystem::Path> files);
        /**
        Usuwanie listy plików z edytora.
        @param files lista plików do usuniêcia
        */
        void removeFiles(const std::vector<core::Filesystem::Path> files);

    protected:
        /**
        Ustala stan w jakim znajduje siê Communication Service.
        @param state stan jaki ustaliæ jako aktualny dla CS
        */
        void setState(State state)
        {
            ScopedLock lock(trialsMutex);
            this->state = state;    
        };

    public:
        /**
        Sprawdza stan w jakim znajduje siê Communication Service.
        @return aktualny stan CS
        */
        State getState()
        {
            ScopedLock lock(trialsMutex);
            return state;
        };
        
        ///**
        //Pingowanie serwera.
        //*/
        //void ping();
        /**
        Podaje informacjê czy serwer odpowiedzia³ na ostatni ping.
        @return czy serwer odpowiedzia³?
        */
        bool isServerResponse() const
        {
            return serverResponse;
        };

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

        unsigned int filesToDownload;
        unsigned int actualDownloadFileNumer;

        /**
        Jedyna instancja klasy CommunicationManager.
        */
        static CommunicationManager* instance;
        /**
        WskaŸnik na klasê TransportWSDL_FTPS odpowiedzialn¹ za transport danych
        */
        core::shared_ptr<communication::TransportWSDL_FTPS> transportManager;
        /**
        WskaŸnik na klasê QueryWSDL odpowiedzialn¹ za odpytywanie bazy danych
        */
        core::shared_ptr<communication::QueryWSDL> queryManager;
        /**
        Stan Communication Service
        */
        State state;

        /**
        Kolejka zapytañ
        */
        RequestsQueue requestsQueue;

        /**
        P³ytka kopia db
        */
        communication::ShallowCopy::ShallowCopy shallowCopy;
        /**
        Metadane db
        */
        communication::MetaData::MetaData metaData;
        /**
        Drzewo performer-subject-trial
        */
        std::vector<communication::PerformerPtr> performers;
        /**
        Lista lokalnych prób pomiarowych
        */
        LocalTrialsList localTrials;
        ///**
        //Lista prób pomiarowych na serwerze
        //*/
        //std::vector<Trial> serverTrials;
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
        WskaŸnik na instancjê zajmuj¹c¹ siê obs³ug¹ zasobów lokalnych
        */
        core::IDataManager* dataManager;
        /**
        Muteks zabezpieczaj¹cy przed zakleszczeniami.
        */
        OpenThreads::Mutex trialsMutex;
        /**
        Muteks synchronizuj¹cy obs³uge kolejki zleceñ
        */
        mutable OpenThreads::Mutex requestsMutex;
        /**
        Parsowanie plików xml p³ytkiej kopii bazy danych.
        */
        void readDbSchemas(const std::string& shallowCopyDir, const std::string& metaDataDir);
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

        core::IDataManager::LocalTrial findLocalTrialsPaths(const core::Filesystem::Path& path);

        bool finish;

        bool cancelDownloading;

        std::string shallowCopyPath;
        std::string metadataPath;
    };
}
#endif //HEADER_GUARD_COMMUNICATION_COMMUNICATIONMANAGER_H__
