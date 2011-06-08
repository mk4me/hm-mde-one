/**
@author Marek Daniluk
@brief Klasa DataManager implementuje interfejs core::IDataManager. S³u¿y do zarz¹dzania zasobami edytora, które dziel¹ siê na dane
konfiguracyjne i próby pomiarowe. Zasoby ³adowane s¹ z odpowiednich œcie¿ek, domyœlnie zasoby sta³e powinny znajdowaæ siê w
resources, próby pomiarowe s¹ wyszukiwane i pobierane do trials.
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
    //! Opis pojedynczego rozszerzenia plików
    struct ExtensionDescription
    {
        //! Opis rozszerzenia
        std::string description;
        //! Mapa mo¿liwych typów i ich nazw, które mo¿an wyci¹gn¹c z tego rozszerzenia, to czy w konretnym pliku siê pojawi¹ czy nie to inna sprawa
        //! Zapytanie takie mozna robiæ bez parsowania w oparciu o rejestrowane prototypy parserów dla danych rozszerzeñ
        //! Info czy dany typ wyst¹pi³ w konretnym pliku czy nie wymaga parsowania i s³u¿y do tego specjalna funkcja
        std::map<core::TypeInfo, std::string> possibleTypes;
    };

    //! Sekwencja lokalnych prób pomiarowych.
    typedef std::vector<std::pair<IDataManager::Path, LocalTrial> > LocalTrialsList;
    //! S³ownik rozszerzeñ i ich opisów.
    typedef std::map<std::string, ExtensionDescription> ExtensionDescriptions;

private:
    //! Deklaracja wewnêtrznej reprezentacji parsera, obudowauj¹cej core::IParser
    class Parser;
    //! Deklaracja predykatu. Zagnie¿d¿ony, aby mieæ dostêp do typu Parser.
    struct FindByFilenamePredicate;
    //! Deklaracja predykatu. Zagnie¿d¿ony, aby mieæ dostêp do typu Parser.
    struct FindByRelativePathPredicate;

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
    typedef std::multimap< core::TypeInfo, ObjectsMapEntry > ObjectsByType;
    //! Mapa fabryk obiektów.
    typedef std::map<core::TypeInfo, core::IObjectWrapperFactoryPtr> ObjectFactories;
    typedef std::multimap< core::TypeInfo, std::pair<DataProcessorPtr, core::ObjectWrapperWeakPtr> > ObjectFromProcessors;

    typedef std::map<core::TypeInfo, core::ObjectWrapperConstPtr> RegisteredTypesPrototypes;

private:
    //! S³ownik fabryk typów.
    ObjectFactories objectFactories;
    //! S³owniki parserów niezainicjalizowanych.
    IParsersByID registeredParsers;
    //! S³ownik parserów przypisanych do rozszerzeñ.
    IParsersByExtensions registeredExtensions;
    //! S³ownik opisu rozszerzeñ zarejestrowanych przez parsery
    ExtensionDescriptions extensionDescriptions;

    //! Obiekty pochodz¹ce z parserów.
    ObjectsByType currentObjects;
        
    ObjectFromProcessors objectsFromDataProcessors;

    //! Lista parserów przypisanych do plików.
    ParsersList currentParsers;
    
    //! Sekwencja lokalnych prób pomiarowych.
    LocalTrialsList localTrialsList;

    //! Lista zasobów.
    std::vector<std::string> resourcesPaths;
    //! Lista skórek dla UI
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
     //! \param path Œcie¿ka.
     //! \param resource Czy to jest zasób niezmienny?

     //ParserPtr createParser(const Path& path, bool resource);
    //! Pomocnicza metoda, tworzy parsery dla zadanego rozszerzenia. Wspiera dodawanie wielu parserów dla jednego pliku.
    //! Uwaga: plik nie zostaje jeszcze przeparsowany!
    //! Zwraca obiekty domenowe ktore moga powstac w wyniku parsowania danego pliku!! jeszcze nie sa zainicjowane
    std::vector<core::ObjectWrapperPtr> createParsers( const Path& path, bool resource );
    
    //! \param œcie¿ka do folderu z plikami próby pomiarowej
    //! \return Pojedyncza próba pomiarowa ze œcie¿kami do wszystkich jej plików.
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
    //! \return true jesli typ Ÿród³owy wspiera typ docelowy lub sa identyczne
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
    //! \return Liczba niezainicjalizowanych parserów.
    int getNumRegisteredParsers() const;
    //! \param idx Indeks parsera.
    //! \return Parser o zadanym indeksie. Parser zawsze bêdzie niezainicjowany.
    core::IParserConstPtr getRegisteredParser(int idx) const;

    //! PARSUJE PLIK I SPRAWDZA KTORE OBIEKTY/ NIE SA PUSTE!!
    //! \param extension Rozszerzenie dla ktorego chcemy pobrac mozliwe typy w nim wystepujace
    //! \return Obiekty domenowe zainicjalizowane przez parser -> ró¿ne od nullptr!!
    std::vector<core::ObjectWrapperPtr> getAvaiableObjectsForFiles(const std::vector<Path> & paths);
  
// core::IDataManager
public:
    //! Tutaj nastêpuje leniwa inicjalizacja.
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
    //! \return Liczba parserów aktualnie za³adowanej próby pomiarowej.
    UTILS_DEPRECATED(virtual int getNumParsers() const);


    //! Szuka na dysku lokalnych prób pomiarowych.
    UTILS_DEPRECATED(virtual void findLocalTrials());
    //! Szuka na dysku zasobów.
    virtual void findResources();
    //! \param trial do za³adowania, czyli inicjalizacja parserów
    UTILS_DEPRECATED(virtual void loadTrial(const LocalTrial& trial));
    //! \param files lista do za³adowania, inicjalizacja parserów
    
    virtual void loadFiles(const std::vector<Path>& files) {
        core::ObjectWrapper::Types t;
        loadFiles(files, t);
    }
    //! \param files lista do za³adowania, inicjalizacja parserów
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
    
    //! ³adowanie zasobów, czyli inicjalizacja parserów
    virtual void loadResources();
    
    virtual void addFiles(const std::vector<Path>& files);
    virtual void removeFiles(const std::vector<Path>& files);
   
    UTILS_DEPRECATED(virtual const LocalTrial& getLocalTrial(int i) const);
    UTILS_DEPRECATED(virtual int getLocalTrialsCount() const);

    //! \param Za³aduj i-t¹ próbê pomiarow¹ z listy triali.
    UTILS_DEPRECATED(virtual void loadLocalTrial(int i));
    //! \param path Za³aduj próbê pomiarow¹ z podanej œcie¿ki.
    UTILS_DEPRECATED(virtual void loadLocalTrial(const Path& path));

    //! Czyœci informacje o lokalnych próbach pomiarowych.
    UTILS_DEPRECATED(void clearLocalTrials());
    //! Czyszczenie po parserach.
    void clearParsers();
    //! Czyœci informacje o aktualnej próbie pomiarowej.
    UTILS_DEPRECATED(void clearCurrentTrial());


public:
    //! Tutaj nastêpuje leniwa inicjalizacja.
    //! \see core::IDataManager::getObjects
    // void getObjects(std::vector<core::ObjectWrapperConstPtr>& objects, const std::type_info& type, bool exact = false) const;

private:
    //! Mapuje obiekty 
    void mapObjectsToTypes(const std::vector<core::ObjectWrapperPtr>& objects, ParserPtr parser );
    //! Usuwa obiekty kieruj¹c siê jakimiœ wyznacznikami.
    template <class Predicate>
    void removeObjects( Predicate pred );

    void dropRemovedWrappers(ObjectFromProcessors& objectsToCheck);
};

#endif // DATA_MANAGER_H
