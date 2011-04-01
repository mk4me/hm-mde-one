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

typedef OpenThreads::ScopedLock<OpenThreads::Mutex> ScopedLock;

namespace communication
{
    class CommunicationManager : public utils::Observable<CommunicationManager>, public OpenThreads::Thread
    {
    private:
        //! Sekwencja lokalnych prób pomiarowych.
        typedef std::vector<core::IDataManager::LocalTrial> LocalTrialsList;
    public:
        /**
        Stany managera pomocne przy wykonywaniu dzia³añ wspó³bie¿nie.
        */
        enum State
        {
            Ready, /** Gotowy do wszelkich dzia³añ */
            DownloadingFile, /** Trwa pobieranie pojedynczego pliku */
            DownloadingTrial, /** Trwa pobieranie plików próby pomiarowej */
            UpdatingServerTrials, /** Trwa odnawianie informacji o zasobach bazodanowych */
            UpdateTrials, /** Trwa odnawianie informacji o zasobach bazodanowych */
            CopyDB, /** Trwa odnawianie informacji o encjach db*/
            PingServer, /** Pingowanie serwera */
            Error /** Wyst¹pi³ b³¹d */
        };
        /**
        @return p³ytka kopia DB
        */
        const ShallowCopy::ShallowCopy& getShalloCopy() const
        {
            return shallowCopy;
        }
        /**
        @return metadane z DB
        */
        const MetaData::MetaData& getMetadata() const
        {
            return metaData;
        }
        /**
        £adowanie lokalnych prób pomiarowych. Metoda wykorzystuje DataManagera.
        */
        void loadLocalTrials();
        /**
        Ustala wartoœæ trialsDir wskazuj¹c¹ gdzie powinny znajdowaæ siê lokalne próby pomiarowe.
        @param dir miejsce gdzie sk³adowane s¹ lokalne próby pomiarowe
        */
        void setTrialsDir(const std::string& dir);
        /**
        Zwraca wartoœæ trialsDir wskazuj¹c¹ gdzie powinny znajdowaæ siê lokalne próby pomiarowe.
        @return wartoœæ trialsDir
        */
        const boost::filesystem::path& getTrialsDir() const;
        /**
        Zwraca postêp w procentach aktualnie wykonywanego zadania.
        @return wartoœæ procentowa (od 0 do 100) pokazuj¹ca postêp wykonywanej operacji
        */
        int getProgress() const;
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
            return actualFile;
        };
        /**
        Aktualizacja informacji o próbach pomiarowych z serwera.
        */
        void listSessionContents();
        /**
        P³ytka kopia bazy danych.
        */
        void copyDbData();
        /**
        Pobieranie próby pomiarowej.
        @param trialID ID próby pomiarowej która ma byæ pobrana
        */
        void downloadTrial(unsigned int trialID);
        /**
        Pobieranie pojedynczego pliku.
        @param fileID ID pliku który ma byæ pobrany
        */
        void downloadFile(unsigned int fileID);
        /**
        Przerwanie operacji pobierania pliku lub próby pomiarowej.
        */
        void cancelDownloading()
        {
            transportManager->abort();
        };
        ///**
        //Listuje próby pomiarowe znajduj¹ce siê na serwerze.
        //@return lista prób pomiarowych z serwera
        //*/
        //const std::vector<Trial>& getServerTrials() const
        //{
        //    return serverTrials;
        //};
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
        Metoda ustala próbê pomiarow¹ jako aktualn¹ i ³aduje j¹ do edytora.
        @param localTrial lokalna próba pomiarowa do za³adowania
        */
        void loadTrial(const core::IDataManager::LocalTrial& localTrial);
        /**
        Metoda ³aduje do edytora listê plików.
        @param files lista plików do za³adowania
        */
        void loadFiles(const std::vector<core::IDataManager::Path> files);
        /**
        Ustala stan w jakim znajduje siê Communication Service.
        @param state stan jaki ustaliæ jako aktualny dla CS
        */
        void setState(const State& state)
        {
            ScopedLock lock(trialsMutex);
            this->state = state;
        };
        /**
        Sprawdza stan w jakim znajduje siê Communication Service.
        @return aktualny stan CS
        */
        const State& getState()
        {
            ScopedLock lock(trialsMutex);
            return state;
        };
        /**
        Pingowanie serwera.
        */
        void ping();
        /**
        Podaje informacjê czy serwer odpowiedzia³ na ostatni ping.
        @return czy serwer odpowiedzia³?
        */
        bool isServerResponse() const
        {
            return serverResponse;
        };
        /**
        Metoda run pochodzi z interfejsu OpenThreads::Thread i zosta³a przes³oniêta do przeniesienia operacji do osobnego w¹tku.
        */
        virtual void run();
        /**
        Zwraca komunikat b³êdu gdy CS znajduje siê w stanie Error.
        @return komunikat o b³êdzie
        */
        const std::string& getErrorMessage() const
        {
            return errorMessage;
        };
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
        /**
        Jedyna instancja klasy CommunicationManager.
        */
        static CommunicationManager* instance;
        /**
        Œcie¿ka do miejsca na lokalnym dysku gdzie powinny znajdowaæ siê próby pomiarowe.
        */
        boost::filesystem::path trialsDir;
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
        Komuniat ostaniego b³êdu
        */
        std::string errorMessage;
        /**
        P³ytka kopia db
        */
        ShallowCopy::ShallowCopy shallowCopy;
        /**
        Metadane db
        */
        MetaData::MetaData metaData;
        /**
        Lista lokalnych prób pomiarowych
        */
        LocalTrialsList localTrials;
        ///**
        //Lista prób pomiarowych na serwerze
        //*/
        //std::vector<Trial> serverTrials;
        /**
        id encji wykorzystywanej przy operacjach pobiarnia i aktualizacji informacji
        */
        int entityID;
        /**
        Procentowa wartoœæ postêpu wykonywanej operacji
        */
        int progress;
        /**
        Iloœæ plików do pobrania przy operacji pobierania
        */
        int filesToDownload;
        /**
        Który z kolei plik jest aktualnie pobierany
        */
        int actualFile;
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
    };
}
#endif //HEADER_GUARD_COMMUNICATION_COMMUNICATIONMANAGER_H__
