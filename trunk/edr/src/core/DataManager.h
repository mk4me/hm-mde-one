/**
@author Marek Daniluk
@brief Klasa DataManager implementuje interfejs IDataManager. S�u�y do zarz�dzania zasobami edytora, kt�re dziel� si� na dane
konfiguracyjne i pr�by pomiarowe. Zasoby �adowane s� z odpowiednich �cie�ek, domy�lnie zasoby sta�e powinny znajdowa� si� w
data/resources, pr�by pomiarowe s� wyszukiwane i pobierane do data/trials.
*/
#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include <core/IDataManager.h>
#include <core/IParser.h>

class DataManager: public IDataManager
{
public:
    DataManager(const std::string& resourcesPath = "data/resources/", const std::string& trialsPath = "data/trials/");

    virtual const LocalTrial& getCurrentLocalTrial() const;

    virtual void clear();

    virtual const std::string& getApplicationSkinsFilePath(int i);
    virtual int getApplicationSkinsFilePathCount();

    virtual const Path& getResourcesPath() const;
    virtual const Path& getTrialsPath() const;

    virtual void setResourcesPath(const std::string& resources);
    virtual void setTrialsPath(const std::string& trials);

    virtual bool isLoadLocalTrialData() const
    {
        return loadTrialData;
    };
    virtual void setLoadLocalTrialData(bool load)
    {
        loadTrialData = load;
    };
    
    virtual ~DataManager();
private:
    std::vector<std::string> shadersPaths;
    std::vector<std::string> meshesPaths;
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    //! S�ownik parser�w wed�ug ID.
    typedef std::map<UniqueID, core::IParserPtr> ParsersIDMap;
    //! S�ownik parser�w.
    typedef std::map<std::string, core::IParserPtr> ParsersMap;
    //! Multis�ownik parser�w.
    typedef std::multimap<std::string, core::IParserPtr> ParsersMultimap;
    //! Sekwencja parser�w.
    typedef std::vector<core::IParserPtr> ParsersList;
    //! S�ownik lokalnych pr�b pomiarowych wed�ug �cie�ek.
    typedef std::map<IDataManager::Path, LocalTrial> LocalTrialsMap;
    //! Sekwencja lokalnych pr�b pomiarowych.
    typedef std::vector<LocalTrial> LocalTrialsList;
    //------------------------------------------------------------------------------------------------------------------------------
private:
    //! S�owniki parser�w niezainicjalizowanych.
    ParsersIDMap registeredParsersIDMap;
    ParsersMultimap registeredParsersExtMap;
    //! Sekwencja parser�w niezainicjalizowanych.
    ParsersList registeredParsersList;
    
    //! S�ownik parser�w aktualnie zainicjalizowanych.
    ParsersMap currentParsersFilenameMap;
    //! Sekwencja parser�w aktualnie zainicjalizowanych.
    ParsersList currentParsersList;

    //! Sekwencja parser�w aktualnej pr�by pomiarowej.
    ParsersList currentTrialParsersList;
    
    //! S�ownik lokalnych pr�b pomiarowych.
    LocalTrialsMap localTrialsMap;
    //! Sekwencja lokalnych pr�b pomiarowych.
    LocalTrialsList localTrialsList;
    //! Aktualnie za�adowany trial
    LocalTrial currentTrial;

    //! Lista zasob�w.
    std::vector<std::string> resourcesPaths;
    
    //! Lista sk�rek dla UI
    std::vector<std::string> applicationSkinsPaths;

    Path resourcesPath;
    Path trialsPath;

    bool loadTrialData;
protected:
    //! \param idx Indeks parsera.
    //! \return Parser o zadanym indeksie.
    virtual core::IParserPtr getParser(int idx);
    //! \param filename nazwa pliku parsera.
    //! \return Odnaleziony parser b�d� NULL.
    core::IParserPtr getParser(const std::string& filename);

    //! \param idx Indeks niezainicjalizowanego parsera.
    //! \return Parser o zadanym indeksie z listy parser�w niezainicjalizowanych.
    core::IParserPtr getRawParser(int idx);
    //! \param extension rozszerzenie niezainicjalizowanego parsera.
    //! \return Odnaleziony parser b�d� NULL.
    core::IParserPtr getRawParser(const std::string& extension);
    //! \return Liczba niezainicjalizowanych parser�w.
    virtual int getNumRawParsers() const;
    
    //! \param �cie�ka do folderu z plikami pr�by pomiarowej
    //! \return Pojedyncza pr�ba pomiarowa ze �cie�kami do wszystkich jej plik�w.
    LocalTrial findLocalTrialsPaths(const Path& path);
public:
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

    //! Rejestruje zadany parser.
    //! \param newService
    virtual void registerParser(core::IParserPtr parser);
    //! \return Liczba parser�w aktualnie za�adowanej pr�by pomiarowej.
    virtual int getNumParsers() const;
    //! \param Za�aduj i-t� pr�b� pomiarow� z listy triali.
    virtual void loadLocalTrial(int i);
    //! \param path Za�aduj pr�b� pomiarow� z podanej �cie�ki.
    virtual void loadLocalTrial(const Path& path);
    //! Czy�ci informacje o lokalnych pr�bach pomiarowych.
    void clearLocalTrials();
    //! Czy�ci informacje o aktualnej pr�bie pomiarowej.
    void clearCurrentTrial();
    //! Czyszczenie po parserach.
    void clearParsers();
};

#endif // DATA_MANAGER_H
