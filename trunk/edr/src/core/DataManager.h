/**
@author Marek Daniluk
@brief Klasa DataManager implementuje interfejs core::IDataManager. S³u¿y do zarz¹dzania zasobami edytora, które dziel¹ siê na dane
konfiguracyjne i próby pomiarowe. Zasoby ³adowane s¹ z odpowiednich œcie¿ek, domyœlnie zasoby sta³e powinny znajdowaæ siê w
data/resources, próby pomiarowe s¹ wyszukiwane i pobierane do data/trials.
*/
#ifndef HEADER_GUARD_CORE_DATAMANAGER_H__
#define HEADER_GUARD_CORE_DATAMANAGER_H__
#include <boost/filesystem.hpp>
#include <boost/type_traits.hpp>
#include <core/IDataManager.h>
#include <core/IParser.h>
#include <utils/Utils.h>

class DataManager: public core::IDataManager
{
private:
    DataManager(const std::string& resourcesPath = "data/resources/", const std::string& trialsPath = "data/trials/");
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
    //! Sekwencja lokalnych prób pomiarowych.
    typedef std::vector<std::pair<IDataManager::Path, LocalTrial> > LocalTrialsList;

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

private:
    //! Instancja.
    static DataManager* instance;

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

    //! sciezka do folderu z zasobami aplikacji
    Path resourcesPath;
    //! sciezka do folderu "ApplicationData\EDR"
    Path applicationDataPath;
    //! sciezka do folderu "MyDocuments\EDR"
    Path userDataPath;
    //!
    Path trialsPath;
    bool loadTrialData;

protected:
//     //! Tworzy parser dla danego pliku. Plik jeszcze nie jest przeparsowany!
//     //! \param path Œcie¿ka.
//     //! \param resource Czy to jest zasób niezmienny?
//     ParserPtr createParser(const Path& path, bool resource);
    //! Pomocnicza metoda, tworzy parsery dla zadanego rozszerzenia. Wspiera dodawanie wielu parserów dla jednego pliku.
    //! Uwaga: plik nie zostaje jeszcze przeparsowany!
    void createParsers( const Path& path, bool resource );
    
    //! \param œcie¿ka do folderu z plikami próby pomiarowej
    //! \return Pojedyncza próba pomiarowa ze œcie¿kami do wszystkich jej plików.
    LocalTrial findLocalTrialsPaths(const Path& path);


public:
    //! \return Instancja DataManagera.
    inline static DataManager* getInstance()
    { 
        return instance;
    }
    //! Tworzy instancjê DataManagera.
    static void createInstance();
    //! Niszczy instancjê DataManagera.
    static void destroyInstance();

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
    //! Tutaj nastêpuje leniwa inicjalizacja.
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
    UTILS_DEPRECATED(virtual void findLocalTrials());
    //! Szuka na dysku zasobów.
    virtual void findResources();
    //! \param trial do za³adowania, czyli inicjalizacja parserów
    UTILS_DEPRECATED(virtual void loadTrial(const LocalTrial& trial));
    //! \param files lista do za³adowania, inicjalizacja parserów
    virtual void loadFiles(const std::vector<Path>& files);

    
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

    
};

#endif // DATA_MANAGER_H
