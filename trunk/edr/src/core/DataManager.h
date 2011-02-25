/**
@author Marek Daniluk
@brief Klasa DataManager implementuje interfejs core::IDataManager. S³u¿y do zarz¹dzania zasobami edytora, które dziel¹ siê na dane
konfiguracyjne i próby pomiarowe. Zasoby ³adowane s¹ z odpowiednich œcie¿ek, domyœlnie zasoby sta³e powinny znajdowaæ siê w
data/resources, próby pomiarowe s¹ wyszukiwane i pobierane do data/trials.
*/
#ifndef HEADER_GUARD_CORE_DATAMANAGER_H__
#define HEADER_GUARD_CORE_DATAMANAGER_H__

#include <core/IDataManager.h>
#include <core/IParser.h>
#include <utils/Utils.h>

class DataManager: public core::IDataManager
{
public:
    DataManager(const std::string& resourcesPath = "data/resources/", const std::string& trialsPath = "data/trials/");

    virtual void clear();

    virtual const std::string& getApplicationSkinsFilePath(int i);
    virtual int getApplicationSkinsFilePathCount();

    virtual const IDataManager::Path& getResourcesPath() const;
    virtual const IDataManager::Path& getTrialsPath() const;

    virtual void setResourcesPath(const IDataManager::Path& resources);
    virtual void setTrialsPath(const IDataManager::Path& trials);

    virtual bool isLoadLocalTrialData() const
    {
        return loadTrialData;
    };
    virtual void setLoadLocalTrialData(bool load)
    {
        loadTrialData = load;
    };
    
    virtual ~DataManager();
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


public:
    //! Sekwencja lokalnych prób pomiarowych.
    typedef std::vector<std::pair<IDataManager::Path, LocalTrial> > LocalTrialsList;

// private:
    //! Wewnêtrzna reprezentacja parsera u¿ywana przez DataManagera.
    class Parser
    {
    private:
        //! Prawdziwy wewnêtrzny parser.
        const core::IParserPtr parser;
        //! Parsowany plik.
        const Path filePath;
        //! Czy parser zwi¹zany jest z zasobami sta³ymi?
        const bool resource;
        //! Czy przeparsowano plik?
        bool parsed;
        //! Czy u¿yto parsera do przeparsowania?
        bool used;


    public:
        //! \param parser Faktyczny parser. To ten obiekt kontroluje jego
        //!     czas ¿ycia.
        //! \param resource Czy parser jest zwi¹zany z zasobami sta³ymi?
        Parser(core::IParser* parser, const Path& path, bool resource = false);
        //! Destruktor drukuj¹cy wiadomoœæ o wy³adowaniu pliku.
        ~Parser();

    public:
        //! \return Czy parser zwi¹zany jest z zasobami sta³ymi?
        bool isResource() const;
        //! \return Czy u¿yto tego parsera?
        bool isUsed() const;
        //! \return Czy uda³o siê przeparsowaæ plik?
        bool isParsed() const;
        //! \return Œcie¿ka do pliku.
        const Path& getPath() const;
        //! \return
        core::IParserPtr getParser() const;

        //! Mo¿e rzucaæ wyj¹tkami!
        void parseFile();
        //! Nie rzuca wyj¹tkami.
        //! \return Czy uda³o siê przeparsowaæ?
        bool tryParse();
        //! \return Lista wrappowanych obiektów, zainicjowanych (przeparsowany parser)
        //!         b¹dŸ nie.
        std::vector<core::ObjectWrapperPtr> getObjects();
    };
    //! WskaŸnik na parser.
    typedef core::shared_ptr<Parser> ParserPtr;
    //! S³aby wskaŸnik na parser
    typedef core::weak_ptr<Parser> ParserWeakPtr;
    // TODO: zastanwoiæ siê nad u¿yciem s³abych wskaŸników
    
private:
    //! Wpis s³ownika obiektów.
    struct ObjectsMapEntry {
        //! Obiekt.
        core::ObjectWrapperPtr object;
        //! Parser z którego dany obiekt pochodzi.
        ParserPtr parser;
    };

    //! S³ownik parserów wed³ug ID.
    typedef std::map<UniqueID, core::IParserPtr> IParsersByID;
    //! Multis³ownik parserów.
    typedef std::multimap<std::string, core::IParserPtr> IParsersByExtensions;
    //! Sekwencja parserów.
    typedef std::vector<ParserPtr> ParsersList;
    //! Typ mapy obiektów.
    typedef std::multimap< core::ObjectWrapper::Type, ObjectsMapEntry > ObjectsByType;

private:
    //! S³owniki parserów niezainicjalizowanych.
    IParsersByID registeredParsers;
    //! S³ownik parserów przypisanych do rozszerzeñ.
    IParsersByExtensions registeredExtensions;

    //! Obiekty pochodz¹ce z parserów.
    ObjectsByType currentObjects;
    //! Lista parserów przypisanych do plików.
    ParsersList currentParsers;
    
    //! Sekwencja lokalnych prób pomiarowych.
    LocalTrialsList localTrialsList;

    //! Lista zasobów.
    std::vector<std::string> resourcesPaths;
    //! Lista skórek dla UI
    std::vector<std::string> applicationSkinsPaths;
    Path resourcesPath;
    Path trialsPath;
    bool loadTrialData;

protected:
    //! Tworzy parser dla danego pliku. Plik jeszcze nie jest przeparsowany!
    //! \param path Œcie¿ka.
    //! \param resource Czy to jest zasób niezmienny?
    ParserPtr createParser(const Path& path, bool resource);

    
    //! \param œcie¿ka do folderu z plikami próby pomiarowej
    //! \return Pojedyncza próba pomiarowa ze œcie¿kami do wszystkich jej plików.
    LocalTrial findLocalTrialsPaths(const Path& path);


public:
    //! Rejestruje zadany parser.
    //! \param newService
    void registerParser(core::IParserPtr parser);
    //! \return Liczba niezainicjalizowanych parserów.
    int getNumRegisteredParsers() const;
    //! \param idx Indeks parsera.
    //! \return Parser o zadanym indeksie. Parser zawsze bêdzie niezainicjowany.
    core::IParserConstPtr getRegisteredParser(int idx) const;

// core::IDataManager
public:
    //! \see core::IDataManager::getObjects
    virtual void getObjects(std::vector<core::ObjectWrapperPtr>& objects, const std::type_info& type, bool exact = false);
    //! \see core::IDataManager::isExtensionSupported
    virtual bool isExtensionSupported(const std::string& extension) const;

    //! \see core::IDataManager::getParser
    UTILS_DEPRECATED(virtual core::IParserPtr getParser(int idx));
    //! \see core::IDataManager::getParser
    UTILS_DEPRECATED(virtual core::IParserPtr getParser(const std::string& filter));
    //! \see core::IDataManager::getInitializedParser
    UTILS_DEPRECATED(virtual core::IParserPtr getInitializedParser(int idx));
    //! \see core::IDataManager::getInitializedParser
    UTILS_DEPRECATED(virtual core::IParserPtr getInitializedParser(const std::string& filter));
    //! \return Liczba parserów aktualnie za³adowanej próby pomiarowej.
    UTILS_DEPRECATED(virtual int getNumParsers() const);


    //! Szuka na dysku lokalnych prób pomiarowych.
    virtual void findLocalTrials();
    //! Szuka na dysku zasobów.
    virtual void findResources();
    //! \param trial do za³adowania, czyli inicjalizacja parserów
    virtual void loadTrial(const LocalTrial& trial);
    //! ³adowanie zasobów, czyli inicjalizacja parserów
    virtual void loadResources();

    virtual const LocalTrial& getLocalTrial(int i) const;
    virtual int getLocalTrialsCount() const;





    //! \param Za³aduj i-t¹ próbê pomiarow¹ z listy triali.
    virtual void loadLocalTrial(int i);
    //! \param path Za³aduj próbê pomiarow¹ z podanej œcie¿ki.
    virtual void loadLocalTrial(const Path& path);

    //! Czyœci informacje o lokalnych próbach pomiarowych.
    void clearLocalTrials();
    //! Czyszczenie po parserach.
    void clearParsers();
    //! Czyœci informacje o aktualnej próbie pomiarowej.
    void clearCurrentTrial();



    
};

#endif // DATA_MANAGER_H
