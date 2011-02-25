/**
@author Marek Daniluk
@brief Klasa DataManager implementuje interfejs IDataManager. S³u¿y do zarz¹dzania zasobami edytora, które dziel¹ siê na dane
konfiguracyjne i próby pomiarowe. Zasoby ³adowane s¹ z odpowiednich œcie¿ek, domyœlnie zasoby sta³e powinny znajdowaæ siê w
data/resources, próby pomiarowe s¹ wyszukiwane i pobierane do data/trials.
*/
#ifndef HEADER_GUARD_CORE_DATAMANAGER_H__
#define HEADER_GUARD_CORE_DATAMANAGER_H__

#include <core/IDataManager.h>
#include <core/IParser.h>

class DataManager: public IDataManager
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
    //! S³ownik parserów wed³ug ID.
    typedef std::map<UniqueID, core::IParserPtr> ParsersIDMap;
    //! Multis³ownik parserów.
    typedef std::multimap<std::string, core::IParserPtr> ParsersMultimap;
    //! Sekwencja parserów.
    typedef std::vector<core::IParserPtr> ParsersList;
    //! Sekwencja parserów z informacj¹ czy zosta³y przeparsowane.
    typedef std::vector<std::pair<IDataManager::Path, std::pair<bool, core::IParserPtr> > > ParsersPathList;
    //! Sekwencja lokalnych prób pomiarowych.
    typedef std::vector<std::pair<IDataManager::Path, LocalTrial> > LocalTrialsList;
    //------------------------------------------------------------------------------------------------------------------------------
private:
    //! S³owniki parserów niezainicjalizowanych.
    ParsersIDMap registeredParsersIDMap;
    ParsersMultimap registeredParsersExtMap;
    //! Sekwencja parserów niezainicjalizowanych.
    ParsersList registeredParsersList;
    
    //! Sekwencja parserów aktualnie zainicjalizowanych.
    ParsersPathList currentParsersList;

    //! Sekwencja parserów aktualnej próby pomiarowej.
    ParsersList currentTrialParsersList;
    
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
    //! \param idx Indeks niezainicjalizowanego parsera.
    //! \return Parser o zadanym indeksie z listy parserów niezainicjalizowanych.
    core::IParserPtr createRawParser(int idx);
    //! \param extension rozszerzenie niezainicjalizowanego parsera.
    //! \return Odnaleziony parser b¹dŸ NULL.
    core::IParserPtr createRawParser(const std::string& extension);
    //! \return Liczba niezainicjalizowanych parserów.
    virtual int getNumRawParsers() const;
    
    //! \param œcie¿ka do folderu z plikami próby pomiarowej
    //! \return Pojedyncza próba pomiarowa ze œcie¿kami do wszystkich jej plików.
    LocalTrial findLocalTrialsPaths(const Path& path);
public:
    //! \param idx Indeks parsera.
    //! \return Parser o zadanym indeksie.
    virtual core::IParserPtr getParser(int idx);
    //! \param filename nazwa pliku parsera.
    //! \return Odnaleziony parser b¹dŸ NULL.
    virtual core::IParserPtr getParser(const std::string& filename);

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

    //! Rejestruje zadany parser.
    //! \param newService
    virtual void registerParser(core::IParserPtr parser);
    //! \return Liczba parserów aktualnie za³adowanej próby pomiarowej.
    virtual int getNumParsers() const;
    //! \param Za³aduj i-t¹ próbê pomiarow¹ z listy triali.
    virtual void loadLocalTrial(int i);
    //! \param path Za³aduj próbê pomiarow¹ z podanej œcie¿ki.
    virtual void loadLocalTrial(const Path& path);
    //! Czyœci informacje o lokalnych próbach pomiarowych.
    void clearLocalTrials();
    //! Czyœci informacje o aktualnej próbie pomiarowej.
    void clearCurrentTrial();
    //! Czyszczenie po parserach.
    void clearParsers();
};

#endif // DATA_MANAGER_H
