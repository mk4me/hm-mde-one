/**
@author Marek Daniluk
@brief Klasa DataManager implementuje interfejs core::IDataManager. S�u�y do zarz�dzania zasobami edytora, kt�re dziel� si� na dane
konfiguracyjne i pr�by pomiarowe. Zasoby �adowane s� z odpowiednich �cie�ek, domy�lnie zasoby sta�e powinny znajdowa� si� w
data/resources, pr�by pomiarowe s� wyszukiwane i pobierane do data/trials.
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
    //! Sekwencja lokalnych pr�b pomiarowych.
    typedef std::vector<std::pair<IDataManager::Path, LocalTrial> > LocalTrialsList;

// private:
    //! Wewn�trzna reprezentacja parsera u�ywana przez DataManagera.
    class Parser
    {
    private:
        //! Prawdziwy wewn�trzny parser.
        const core::IParserPtr parser;
        //! Parsowany plik.
        const Path filePath;
        //! Czy parser zwi�zany jest z zasobami sta�ymi?
        const bool resource;
        //! Czy przeparsowano plik?
        bool parsed;
        //! Czy u�yto parsera do przeparsowania?
        bool used;


    public:
        //! \param parser Faktyczny parser. To ten obiekt kontroluje jego
        //!     czas �ycia.
        //! \param resource Czy parser jest zwi�zany z zasobami sta�ymi?
        Parser(core::IParser* parser, const Path& path, bool resource = false);
        //! Destruktor drukuj�cy wiadomo�� o wy�adowaniu pliku.
        ~Parser();

    public:
        //! \return Czy parser zwi�zany jest z zasobami sta�ymi?
        bool isResource() const;
        //! \return Czy u�yto tego parsera?
        bool isUsed() const;
        //! \return Czy uda�o si� przeparsowa� plik?
        bool isParsed() const;
        //! \return �cie�ka do pliku.
        const Path& getPath() const;
        //! \return
        core::IParserPtr getParser() const;

        //! Mo�e rzuca� wyj�tkami!
        void parseFile();
        //! Nie rzuca wyj�tkami.
        //! \return Czy uda�o si� przeparsowa�?
        bool tryParse();
        //! \return Lista wrappowanych obiekt�w, zainicjowanych (przeparsowany parser)
        //!         b�d� nie.
        std::vector<core::ObjectWrapperPtr> getObjects();
    };
    //! Wska�nik na parser.
    typedef core::shared_ptr<Parser> ParserPtr;
    //! S�aby wska�nik na parser
    typedef core::weak_ptr<Parser> ParserWeakPtr;
    // TODO: zastanwoi� si� nad u�yciem s�abych wska�nik�w
    
private:
    //! Wpis s�ownika obiekt�w.
    struct ObjectsMapEntry {
        //! Obiekt.
        core::ObjectWrapperPtr object;
        //! Parser z kt�rego dany obiekt pochodzi.
        ParserPtr parser;
    };

    //! S�ownik parser�w wed�ug ID.
    typedef std::map<UniqueID, core::IParserPtr> IParsersByID;
    //! Multis�ownik parser�w.
    typedef std::multimap<std::string, core::IParserPtr> IParsersByExtensions;
    //! Sekwencja parser�w.
    typedef std::vector<ParserPtr> ParsersList;
    //! Typ mapy obiekt�w.
    typedef std::multimap< core::ObjectWrapper::Type, ObjectsMapEntry > ObjectsByType;

private:
    //! S�owniki parser�w niezainicjalizowanych.
    IParsersByID registeredParsers;
    //! S�ownik parser�w przypisanych do rozszerze�.
    IParsersByExtensions registeredExtensions;

    //! Obiekty pochodz�ce z parser�w.
    ObjectsByType currentObjects;
    //! Lista parser�w przypisanych do plik�w.
    ParsersList currentParsers;
    
    //! Sekwencja lokalnych pr�b pomiarowych.
    LocalTrialsList localTrialsList;

    //! Lista zasob�w.
    std::vector<std::string> resourcesPaths;
    //! Lista sk�rek dla UI
    std::vector<std::string> applicationSkinsPaths;
    Path resourcesPath;
    Path trialsPath;
    bool loadTrialData;

protected:
    //! Tworzy parser dla danego pliku. Plik jeszcze nie jest przeparsowany!
    //! \param path �cie�ka.
    //! \param resource Czy to jest zas�b niezmienny?
    ParserPtr createParser(const Path& path, bool resource);

    
    //! \param �cie�ka do folderu z plikami pr�by pomiarowej
    //! \return Pojedyncza pr�ba pomiarowa ze �cie�kami do wszystkich jej plik�w.
    LocalTrial findLocalTrialsPaths(const Path& path);


public:
    //! Rejestruje zadany parser.
    //! \param newService
    void registerParser(core::IParserPtr parser);
    //! \return Liczba niezainicjalizowanych parser�w.
    int getNumRegisteredParsers() const;
    //! \param idx Indeks parsera.
    //! \return Parser o zadanym indeksie. Parser zawsze b�dzie niezainicjowany.
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
    //! \return Liczba parser�w aktualnie za�adowanej pr�by pomiarowej.
    UTILS_DEPRECATED(virtual int getNumParsers() const);


    //! Szuka na dysku lokalnych pr�b pomiarowych.
    virtual void findLocalTrials();
    //! Szuka na dysku zasob�w.
    virtual void findResources();
    //! \param trial do za�adowania, czyli inicjalizacja parser�w
    virtual void loadTrial(const LocalTrial& trial);
    //! �adowanie zasob�w, czyli inicjalizacja parser�w
    virtual void loadResources();

    virtual const LocalTrial& getLocalTrial(int i) const;
    virtual int getLocalTrialsCount() const;





    //! \param Za�aduj i-t� pr�b� pomiarow� z listy triali.
    virtual void loadLocalTrial(int i);
    //! \param path Za�aduj pr�b� pomiarow� z podanej �cie�ki.
    virtual void loadLocalTrial(const Path& path);

    //! Czy�ci informacje o lokalnych pr�bach pomiarowych.
    void clearLocalTrials();
    //! Czyszczenie po parserach.
    void clearParsers();
    //! Czy�ci informacje o aktualnej pr�bie pomiarowej.
    void clearCurrentTrial();



    
};

#endif // DATA_MANAGER_H
