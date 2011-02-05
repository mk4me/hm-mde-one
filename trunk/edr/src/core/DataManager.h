/**
@author Marek Daniluk
@brief Klasa DataManager implementuje interfejs IDataManager. S�u�y do zarz�dzania zasobami edytora, kt�re dziel� si� na dane
konfiguracyjne i pr�by pomiarowe. Zasoby �adowane s� z odpowiednich �cie�ek, domy�lnie zasoby sta�e powinny znajdowa� si� w
data/resources, pr�by pomiarowe s� wyszukiwane i pobierane do data/trials.
*/
#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include <core/IDataManager.h>
#include <core/LocalTrial.h>
#include <core/IParser.h>

class DataManager: public IDataManager
{
public:
    DataManager(const std::string& resourcesPath = "data/resources/", const std::string& trialsPath = "data/trials/");

    virtual void loadResources();
    virtual void loadTrials();

    virtual const LocalTrial& getLocalTrial(int i) const;
    virtual int getLocalTrialsCount() const;

    virtual const LocalTrial& getActualLocalTrial() const;
    virtual void setActualLocalTrial(int i);
    virtual void setActualLocalTrial(const std::string& name);

    virtual void clear();

    virtual const std::string& getShaderFilePath(int i);
    virtual int getShaderFilePathCount();

    virtual const std::string& getMeshFilePath(int i);
    virtual int getMeshFilePathCount();

    virtual const std::string& getApplicationSkinsFilePath(int i);
    virtual int getApplicationSkinsFilePathCount();

    virtual const std::string& getResourcesPath() const;
    virtual const std::string& getTrialsPath() const;

    virtual void setResourcesPath(const std::string& resources);
    virtual void setTrialsPath(const std::string& trials);

    virtual bool isLoadLocalTrialData() const {return loadTrialData;};
    virtual void setLoadLocalTrialData(bool load) {loadTrialData = load;};

    virtual ~DataManager();
protected:
    //	LocalTrial loadLocalTrial(const std::string& path);
private:
    std::vector<std::string> shadersPaths;
    std::vector<std::string> meshesPaths;
    std::vector<std::string> applicationSkinsPaths;
    std::vector<LocalTrial> trials;
    size_t actualTrialIndex;

    LocalTrial unknownTrial;

    std::string resourcesPath;
    std::string trialsPath;

    bool loadTrialData;
    bool loadUnknownTrialData;
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    //! S�ownik parser�w wed�ug ID.
    typedef std::map<UniqueID, core::IParserPtr> ParsersIDMap;
    //! S�ownik parser�w wed�ug rozszerze�.
    typedef std::map<std::string, core::IParserPtr> ParsersExtMap;
    //! Sekwencja parser�w.
    typedef std::vector<core::IParserPtr> ParsersList;
    //------------------------------------------------------------------------------------------------------------------------------
    //! S�owniki parser�w niezainicjalizowanych.
    ParsersIDMap parsersIDMap;
    ParsersExtMap parsersExtMap;
    //! Sekwencja parser�w niezainicjalizowanych.
    ParsersList parsersList;

    //! S�owniki parser�w aktualnej pr�by pomiarowej.
    ParsersIDMap actualTrialParsersIDMap;
    ParsersExtMap actualTrialParsersExtMap;
    //! Sekwencja parser�w aktualnej pr�by pomiarowej.
    ParsersList actualTrialParsersList;

    //! Lista lokalnych pr�b pomiarowych.
    std::vector<LocalTrial> localTrialsList;
protected:
    //! \param idx Indeks parsera.
    //! \return Parser o zadanym indeksie.
    virtual core::IParserPtr getParser(int idx);
    //! \param id ID parsera do wyszukania.
    //! \return Odnaleziony parser b�d� NULL.
    virtual core::IParserPtr getParser(UniqueID id);
    //! \param extension rozszerzenie parsera.
    //! \return Odnaleziony parser b�d� NULL.
    core::IParserPtr getParser(const std::string& extension);
    
    //! \param idx Indeks niezainicjalizowanego parsera.
    //! \return Parser o zadanym indeksie z listy parser�w niezainicjalizowanych.
    core::IParserPtr getRawParser(int idx);
    //! \param id ID niezainicjalizowanego parsera do wyszukania.
    //! \return Odnaleziony parser b�d� NULL.
    core::IParserPtr getRawParser(UniqueID id);
    //! \param extension rozszerzenie niezainicjalizowanego parsera.
    //! \return Odnaleziony parser b�d� NULL.
    core::IParserPtr getRawParser(const std::string& extension);
    //! \return Liczba niezainicjalizowanych parser�w.
    virtual int getNumRawParsers() const;

    //! Szuka na dysku lokalnych prob pomiarowych.
    void findLocalTrials();
    //! \param trial do za�adowania, czyli inicjalizacja parser�w
    void loadActualTrialParsers(const LocalTrial& trial);
    //! \param �cie�ka do folderu z plikami pr�by pomiarowej
    //! \return Pojedyncza pr�ba pomiarowa ze �cie�kami do wszystkich jej plik�w.
    LocalTrial findLocalTrialsPaths(const std::string& path);
public:
    //! Rejestruje zadany parser.
    //! \param newService
    virtual void registerParser(core::IParserPtr parser);
    //! \return Liczba parser�w aktualnie za�adowanej pr�by pomiarowej.
    virtual int getNumParsers() const;
    //! \param Za�aduj i-t� pr�b� pomiarow� z listy triali.
    virtual void loadLocalTrial(int i);
    //! \param Za�aduj pr�b� pomiarow� o podanej nazwie.
    virtual void loadLocalTrial(const std::string& name);
    //! Czy�ci informacje o lokalnych pr�bach pomiarowych.
    void clearLocalTrials();
    //! Czy�ci informacje o aktualnej pr�bie pomiarowej.
    void clearActualLocalTrial();
};

#endif // DATA_MANAGER_H
