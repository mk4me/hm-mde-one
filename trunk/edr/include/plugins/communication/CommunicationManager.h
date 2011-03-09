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

typedef OpenThreads::ScopedLock<OpenThreads::Mutex> ScopedLock;

namespace communication
{
    class CommunicationManager : public utils::Observable<CommunicationManager>, public OpenThreads::Thread
    {
    private:
        //! Sekwencja lokalnych pr�b pomiarowych.
        typedef std::vector<core::IDataManager::LocalTrial> LocalTrialsList;
    public:
        /**
        Stany managera pomocne przy wykonywaniu dzia�a� wsp�bie�nie.
        */
        enum State
        {
            Ready, /** Gotowy do wszelkich dzia�a� */
            DownloadingFile, /** Trwa pobieranie pojedynczego pliku */
            DownloadingTrial, /** Trwa pobieranie plik�w pr�by pomiarowej */
            UpdatingServerTrials, /** Trwa odnawianie informacji o zasobach bazodanowych */
            UpdateTrials, /** Trwa odnawianie informacji o zasobach bazodanowych */
            ShallowCopyDB, /** Trwa odnawianie informacji o encjach db*/
            MetadataDB, /** Trwa odnawianie informacji o metadanych db*/
            PingServer, /** Pingowanie serwera */
            Error /** Wyst�pi� b��d */
        };
        /**
        Ustala spos�b w jaki ma odbywa� si� transport danych.
        @param transportManager wska�nik na instancj� odpowiedzialn� za transport danych
        */
        void setTransportManager(ITransportable* transportManager);
        /**
        Zwraca wska�nik na instancj� odpowiedzialn� za transport danych.
        @return wska�nik na instancj� odpowiedzialn� za transport danych
        */
        ITransportable* getTransportManager();
        /**
        Ustala spos�b w jaki ma odbywa� si� odpytywanie bazy danych.
        @param queryManager wska�nik na instancj� odpowiedzialn� za odpytywanie bazy danych
        */
        void setQueryManager(IQueryable* queryManager);
        /**
        Zwraca wska�nik na instancj� odpowiedzialn� za odpytywanie bazy danych.
        @return wska�nik na instancj� odpowiedzialn� za odpytywanie bazy danych
        */
        IQueryable* getQueryManager();
        /**
        Zapis danych pr�b pomiarowych do pliku w formacie XML.
        @param filename nazwa pliku do kt�rego zapisa� dane pr�b pomiarowych
        */
        void saveToXml(const std::string& filename);
        /**
        Odczyt danych pr�b pomiarowych z pliku w formacie XML.
        @param filename nazwa pliku z kt�rego odczyta� dane pr�b pomiarowych
        */
        void loadFromXml(const std::string& filename);
        /**
        �adowanie lokalnych pr�b pomiarowych. Metoda wykorzystuje DataManagera.
        */
        void loadLocalTrials();
        /**
        Ustala warto�� trialsDir wskazuj�c� gdzie powinny znajdowa� si� lokalne pr�by pomiarowe.
        @param dir miejsce gdzie sk�adowane s� lokalne pr�by pomiarowe
        */
        void setTrialsDir(const std::string& dir);
        /**
        Zwraca warto�� trialsDir wskazuj�c� gdzie powinny znajdowa� si� lokalne pr�by pomiarowe.
        @return warto�� trialsDir
        */
        const std::string& getTrialsDir() const;
        /**
        Zwraca post�p w procentach aktualnie wykonywanego zadania.
        @return warto�� procentowa (od 0 do 100) pokazuj�ca post�p wykonywanej operacji
        */
        int getProgress() const;
        /**
        Informuje ile plik�w ma by� �ci�gni�tych przy operacji pobierania plik�w.
        @return ile plik�w ma by� �ci�gnietych
        */
        int getFilesToDownloadCount() const
        {
            return filesToDownload;
        };
        /**
        Informuje kt�ry z kolei plik jest aktualnie �ci�gany,
        @return kt�ry plik z kolei jest aktualnie �ci�gany
        */
        int getActualDownloadFileNumber() const
        {
            return actualFile;
        };
        /**
        Aktualizacja informacji o pr�bach pomiarowych z serwera.
        */
        void listSessionContents();
        /**
        P�ytka kopia bazy danych.
        */
        void updateShallowCopy();
        /**
        Metadane z bazy danych.
        */
        void updateMetadata();
        /**
        Pobieranie pr�by pomiarowej.
        @param trialID ID pr�by pomiarowej kt�ra ma by� pobrana
        */
        void downloadTrial(unsigned int trialID);
        /**
        Pobieranie pojedynczego pliku.
        @param fileID ID pliku kt�ry ma by� pobrany
        */
        void downloadFile(unsigned int fileID);
        /**
        Przerwanie operacji pobierania pliku lub pr�by pomiarowej.
        */
        void cancelDownloading()
        {
            transportManager->abort();
        };
        /**
        Listuje pr�by pomiarowe znajduj�ce si� na serwerze.
        @return lista pr�b pomiarowych z serwera
        */
        const std::vector<Trial>& getServerTrials() const
        {
            return serverTrials;
        };
        /**
        Listuje pr�by pomiarowe znajduj�ce si� na lokalnym dysku.
        @return lista pr�b pomiarowych z lokalnego dysku
        */
        const LocalTrialsList& getLocalTrials() const
        {
            return localTrials;
        };
        /**
        Czy robiono aktualizacj� informacji o pr�bach pomiarowych serwera?
        @return true gdy robiono aktualizacj�, w przeciwnym przypadku zwraca false
        */
        bool isUpdated() const
        {
            return this->isLastUpdate;
        };
        /**
        Zwraca dat� ostatniej aktualizacji informacji o pr�bach. Nale�y u�ywa� z metod� isUpdated().
        @return data ostatniej aktualizacji informacji
        */
        const DateTime& getLastUpdateTime() const
        {
            return lastUpdate;
        };
        /**
        Ustala Data Managera. Potrzebne przy �adowaniu pr�b pomiarowych z lokalnego dysku.
        @param dataManager wska�nik na instancj� Data Managera
        */
        void setDataManager(core::IDataManager* dataManager)
        {
            this->dataManager = dataManager;loadLocalTrials();
        };
        /**
        Zwraca wska�nik na instancj� Data Managera.
        @return wska�nik na instancj� Data Managera
        */
        core::IDataManager* getDataManager()
        {
            return dataManager;
        };
        //void setServiceManager(IServiceManager* serviceManager) {this->serviceManager = serviceManager;};
        //IServiceManager* getServiceManager() {return serviceManager;};
        /**
        Metoda ustala pr�b� pomiarow� jako aktualn� i �aduje j� do edytora.
        @param localTrial lokalna pr�ba pomiarowa do za�adowania
        */
        void loadTrial(const core::IDataManager::LocalTrial& localTrial);
        /**
        Ustala stan w jakim znajduje si� Communication Service.
        @param state stan jaki ustali� jako aktualny dla CS
        */
        void setState(const State& state)
        {
            ScopedLock lock(trialsMutex);
            this->state = state;
        };
        /**
        Sprawdza stan w jakim znajduje si� Communication Service.
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
        Podaje informacj� czy serwer odpowiedzia� na ostatni ping.
        @return czy serwer odpowiedzia�?
        */
        bool isServerResponse() const
        {
            return serverResponse;
        };
        /**
        Metoda run pochodzi z interfejsu OpenThreads::Thread i zosta�a przes�oni�ta do przeniesienia operacji do osobnego w�tku.
        */
        virtual void run();
        /**
        Zwraca komunikat b��du gdy CS znajduje si� w stanie Error.
        @return komunikat o b��dzie
        */
        const std::string& getErrorMessage() const
        {
            return errorMessage;
        };
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
        �cie�ka do miejsca na lokalnym dysku gdzie powinny znajdowa� si� pr�by pomiarowe.
        */
        std::string trialsDir;
        /**
        Stan Communication Service
        */
        State state;
        /**
        Komuniat ostaniego b��du
        */
        std::string errorMessage;
        /**
        P�ytka kopia db
        */
        ShallowCopy::ShallowCopy shallowCopy;
        /**
        Metadane db
        */
        MetaData::MetaData metaData;
        /**
        Lista lokalnych pr�b pomiarowych
        */
        LocalTrialsList localTrials;
        /**
        Lista pr�b pomiarowych na serwerze
        */
        std::vector<Trial> serverTrials;
        /**
        Data ostatniej aktualizacji informacji o pr�bach pomiarowych na serwerze
        */
        DateTime lastUpdate;
        /**
        Czy aktualizaowano informacje o pr�bach pomiarowych na serwerze?
        */
        bool isLastUpdate;
        /**
        id encji wykorzystywanej przy operacjach pobiarnia i aktualizacji informacji
        */
        int entityID;
        /**
        Procentowa warto�� post�pu wykonywanej operacji
        */
        int progress;
        /**
        Ilo�� plik�w do pobrania przy operacji pobierania
        */
        int filesToDownload;
        /**
        Kt�ry z kolei plik jest aktualnie pobierany
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
        Wska�nik na instancj� zajmuj�c� si� obs�ug� zasob�w lokalnych
        */
        core::IDataManager* dataManager;
        /**
        Wska�nik na instancj� zajmuj�c� si� transportem danych
        */
        ITransportable* transportManager;
        /**
        Wska�nik na instancj� zajmuj�c� si� odpytywaniem bazy danych
        */
        IQueryable* queryManager;
        /**
        Muteks zabezpieczaj�cy przed zakleszczeniami.
        */
        OpenThreads::Mutex trialsMutex;
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
    };
}
#endif
