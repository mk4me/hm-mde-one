/**
@author Marek Daniluk
@brief Klasa DataManager implementuje interfejs core::IDataManager. S�u�y do zarz�dzania zasobami edytora, kt�re dziel� si� na dane
konfiguracyjne i pr�by pomiarowe. Zasoby �adowane s� z odpowiednich �cie�ek, domy�lnie zasoby sta�e powinny znajdowa� si� w
resources, pr�by pomiarowe s� wyszukiwane i pobierane do trials.
*/
#ifndef HEADER_GUARD_CORE_DATAMANAGER_H__
#define HEADER_GUARD_CORE_DATAMANAGER_H__
#include <boost/filesystem.hpp>
#include <boost/type_traits.hpp>
#include <core/IDataManager.h>
#include <core/IParser.h>
#include <core/ObjectWrapperFactory.h>
#include <utils/Utils.h>

class DataManager: public core::IDataManager
{
public:
    DataManager(const std::string& resourcesPath = "resources/", const std::string& trialsPath = "trials/");
    virtual ~DataManager();

public:

    virtual void clear();

    virtual const std::string& getApplicationSkinsFilePath(int i);
    virtual int getApplicationSkinsFilePathCount();

    //! ustawia pelna sciezke do folderu "MyDocuments\EDR"
    void setUserDataPath(const IDataManager::Path& path);
    //! ustawia pelna sciezke do folderu "ApplicationData\EDR"
    void setResourcesPath(const IDataManager::Path& path);
    //! ustawia pelna sciezke do folderu z zasobami aplikacji
    void setApplicationDataPath(const IDataManager::Path& path);
    virtual void setTrialsPath(const IDataManager::Path& trials);

    //! zwraca pelna sciezke do folderu "MyDocuments\EDR"
    virtual const IDataManager::Path& getUserDataPath() const;
    //! zwraca pelna sciezke do folderu "ApplicationData\EDR"
    virtual const IDataManager::Path& getApplicationDataPath() const;
    //! zwraca pelna sciezke do folderu z zasobami aplikacji
    virtual const IDataManager::Path& getResourcesPath() const;
    //! 
    virtual const IDataManager::Path& getTrialsPath() const;
    
    UTILS_DEPRECATED(virtual bool isLoadLocalTrialData() const
    {
        return loadTrialData;
    });
    UTILS_DEPRECATED(virtual void setLoadLocalTrialData(bool load)
    {
        loadTrialData = load;
    });
    
    
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


public:
    //! Opis pojedynczego rozszerzenia plik�w
    struct ExtensionDescription
    {
        //! Opis rozszerzenia
        std::string description;
        //! Mapa mo�liwych typ�w i ich nazw, kt�re mo�an wyci�gn�c z tego rozszerzenia, to czy w konretnym pliku si� pojawi� czy nie to inna sprawa
        //! Zapytanie takie mozna robi� bez parsowania w oparciu o rejestrowane prototypy parser�w dla danych rozszerze�
        //! Info czy dany typ wyst�pi� w konretnym pliku czy nie wymaga parsowania i s�u�y do tego specjalna funkcja
        std::map<core::TypeInfo, std::string> possibleTypes;
    };

    //! Sekwencja lokalnych pr�b pomiarowych.
    typedef std::vector<std::pair<IDataManager::Path, LocalTrial> > LocalTrialsList;
    //! S�ownik rozszerze� i ich opis�w.
    typedef std::map<std::string, ExtensionDescription> ExtensionDescriptions;

private:
    //! Deklaracja wewn�trznej reprezentacji parsera, obudowauj�cej core::IParser
    class Parser;
    //! Deklaracja predykatu. Zagnie�d�ony, aby mie� dost�p do typu Parser.
    struct FindByFilenamePredicate;
    //! Deklaracja predykatu. Zagnie�d�ony, aby mie� dost�p do typu Parser.
    struct FindByRelativePathPredicate;

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
    typedef std::multimap< core::TypeInfo, ObjectsMapEntry > ObjectsByType;
    //! Mapa fabryk obiekt�w.
    typedef std::map<core::TypeInfo, core::IObjectWrapperFactoryPtr> ObjectFactories;
    typedef std::multimap< core::TypeInfo, std::pair<DataProcessorPtr, core::ObjectWrapperWeakPtr> > ObjectFromProcessors;

    typedef std::map<core::TypeInfo, core::ObjectWrapperConstPtr> RegisteredTypesPrototypes;

private:
    //! S�ownik fabryk typ�w.
    ObjectFactories objectFactories;
    //! S�owniki parser�w niezainicjalizowanych.
    IParsersByID registeredParsers;
    //! S�ownik parser�w przypisanych do rozszerze�.
    IParsersByExtensions registeredExtensions;
    //! S�ownik opisu rozszerze� zarejestrowanych przez parsery
    ExtensionDescriptions extensionDescriptions;

    //! Obiekty pochodz�ce z parser�w.
    ObjectsByType currentObjects;
        
    ObjectFromProcessors objectsFromDataProcessors;

    //! Lista parser�w przypisanych do plik�w.
    ParsersList currentParsers;
    
    //! Sekwencja lokalnych pr�b pomiarowych.
    LocalTrialsList localTrialsList;

    //! Lista zasob�w.
    std::vector<std::string> resourcesPaths;
    //! Lista sk�rek dla UI
    std::vector<std::string> applicationSkinsPaths;

    //! sciezka do folderu z zasobami aplikacji
    Path resourcesPath;
    //! sciezka do folderu "ApplicationData\EDR"
    Path applicationDataPath;
    //! sciezka do folderu "MyDocuments\EDR"
    Path userDataPath;
    //!
    Path trialsPath;
    bool loadTrialData;

    RegisteredTypesPrototypes registeredTypesPrototypes;

protected:
     //! Tworzy parser dla danego pliku. Plik jeszcze nie jest przeparsowany!
     //! \param path �cie�ka.
     //! \param resource Czy to jest zas�b niezmienny?

     //ParserPtr createParser(const Path& path, bool resource);
    //! Pomocnicza metoda, tworzy parsery dla zadanego rozszerzenia. Wspiera dodawanie wielu parser�w dla jednego pliku.
    //! Uwaga: plik nie zostaje jeszcze przeparsowany!
    //! Zwraca obiekty domenowe ktore moga powstac w wyniku parsowania danego pliku!! jeszcze nie sa zainicjowane
    std::vector<core::ObjectWrapperPtr> createParsers( const Path& path, bool resource );
    
    //! \param �cie�ka do folderu z plikami pr�by pomiarowej
    //! \return Pojedyncza pr�ba pomiarowa ze �cie�kami do wszystkich jej plik�w.
    LocalTrial findLocalTrialsPaths(const Path& path);


public:
    static DataManager* getInstance()
    {
        return static_cast<DataManager*>(core::getDataManager());
    }

    void registerObjectFactory(core::IObjectWrapperFactoryPtr factory);

    virtual core::ObjectWrapperPtr createWrapper(const core::TypeInfo& type);
    virtual core::ObjectWrapperCollectionPtr createWrapperCollection(const core::TypeInfo& typeInfo);

    const core::ObjectWrapperConstPtr & getTypePrototype(const core::TypeInfo & typeInfo) const;

    //! \param sourceTypeInfo Typ z ktorego chcemy pobrac dane
    //! \param destTypeInfo Typ do ktorego chcemy zapisac dane
    //! \return true jesli typ �r�d�owy wspiera typ docelowy lub sa identyczne
    bool isTypeCompatible(const core::TypeInfo & sourceTypeInfo, const core::TypeInfo & destTypeInfo) const
    {
        if(sourceTypeInfo == destTypeInfo){
            return true;
        }

        bool ret = false;
        core::ObjectWrapper::Types supportedTypes;
        getTypePrototype(sourceTypeInfo)->getSupportedTypes(supportedTypes);
        if(std::find(supportedTypes.begin(), supportedTypes.end(), destTypeInfo) != supportedTypes.end()){
            ret = true;
        }

        return ret;
    }

    const ExtensionDescriptions & getRegisteredExtensionsWithDescriptions() const
    {
        return extensionDescriptions;
    }

    //! Rejestruje zadany parser.
    //! \param newService
    void registerParser(core::IParserPtr parser);
    //! \return Liczba niezainicjalizowanych parser�w.
    int getNumRegisteredParsers() const;
    //! \param idx Indeks parsera.
    //! \return Parser o zadanym indeksie. Parser zawsze b�dzie niezainicjowany.
    core::IParserConstPtr getRegisteredParser(int idx) const;

    //! PARSUJE PLIK I SPRAWDZA KTORE OBIEKTY/ NIE SA PUSTE!!
    //! \param extension Rozszerzenie dla ktorego chcemy pobrac mozliwe typy w nim wystepujace
    //! \return Obiekty domenowe zainicjalizowane przez parser -> r�ne od nullptr!!
    std::vector<core::ObjectWrapperPtr> getAvaiableObjectsForFiles(const std::vector<Path> & paths);
  
// core::IDataManager
public:
    //! Tutaj nast�puje leniwa inicjalizacja.
    //! \see core::IDataManager::getObjects
    virtual void getObjects(std::vector<core::ObjectWrapperPtr>& objects, const core::TypeInfo& type, bool exact = false);
    virtual void getObjectsFromParsers(std::vector<core::ObjectWrapperPtr>& objects, const core::TypeInfo& type, bool exact = false);
    //! \see core::IDataManager::isExtensionSupported
    virtual bool isExtensionSupported(const std::string& extension) const;

    //! Dodawanie obiektow domenowych z innych zrodel niz parsery
    //! \param dataProcessor zrodlo danych
    //! \param objects wektor ze stworzonymi obiektami domenowymi (zrodlo powinno dalej je przechowywac!)
    virtual void addObjects(DataProcessorPtr dataProcessor, const std::vector<core::ObjectWrapperPtr>& objects);

    //! Dodawanie obiektu domenowego z innego zrodla niz parser
    //! \param dataProcessor zrodlo obiektu
    //! \param object dodawany obiekt (zrodlo powinno dalej go przechowywac)
    virtual void addObject(DataProcessorPtr dataProcessor, core::ObjectWrapperPtr object);

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
    UTILS_DEPRECATED(virtual void findLocalTrials());
    //! Szuka na dysku zasob�w.
    virtual void findResources();
    //! \param trial do za�adowania, czyli inicjalizacja parser�w
    UTILS_DEPRECATED(virtual void loadTrial(const LocalTrial& trial));
    //! \param files lista do za�adowania, inicjalizacja parser�w
    
    virtual void loadFiles(const std::vector<Path>& files) {
        core::ObjectWrapper::Types t;
        loadFiles(files, t);
    }
    //! \param files lista do za�adowania, inicjalizacja parser�w
    virtual void loadFiles(const std::vector<Path>& files, const core::ObjectWrapper::Types& types)
    {
        std::vector<core::ObjectWrapperPtr> objects;
        loadFiles(files, types, objects);
    }
    
    void loadFiles(const std::vector<Path>& files, const core::ObjectWrapper::Types& types, std::vector<core::ObjectWrapperPtr> & objects);
    void loadFiles(const std::vector<Path>& files, std::vector<core::ObjectWrapperPtr> & objects)
    {
        core::ObjectWrapper::Types t;
        loadFiles(files, t, objects);
    }
    
    //! �adowanie zasob�w, czyli inicjalizacja parser�w
    virtual void loadResources();
    
    virtual void addFiles(const std::vector<Path>& files);
    virtual void removeFiles(const std::vector<Path>& files);
   
    UTILS_DEPRECATED(virtual const LocalTrial& getLocalTrial(int i) const);
    UTILS_DEPRECATED(virtual int getLocalTrialsCount() const);

    //! \param Za�aduj i-t� pr�b� pomiarow� z listy triali.
    UTILS_DEPRECATED(virtual void loadLocalTrial(int i));
    //! \param path Za�aduj pr�b� pomiarow� z podanej �cie�ki.
    UTILS_DEPRECATED(virtual void loadLocalTrial(const Path& path));

    //! Czy�ci informacje o lokalnych pr�bach pomiarowych.
    UTILS_DEPRECATED(void clearLocalTrials());
    //! Czyszczenie po parserach.
    void clearParsers();
    //! Czy�ci informacje o aktualnej pr�bie pomiarowej.
    UTILS_DEPRECATED(void clearCurrentTrial());


public:
    //! Tutaj nast�puje leniwa inicjalizacja.
    //! \see core::IDataManager::getObjects
    // void getObjects(std::vector<core::ObjectWrapperConstPtr>& objects, const std::type_info& type, bool exact = false) const;

private:
    //! Mapuje obiekty 
    void mapObjectsToTypes(const std::vector<core::ObjectWrapperPtr>& objects, ParserPtr parser );
    //! Usuwa obiekty kieruj�c si� jakimi� wyznacznikami.
    template <class Predicate>
    void removeObjects( Predicate pred );

    void dropRemovedWrappers(ObjectFromProcessors& objectsToCheck);
};

#endif // DATA_MANAGER_H
