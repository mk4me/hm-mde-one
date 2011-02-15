/**
@author Marek Daniluk
@brief Klasa DataManager implementuje interfejs IDataManager. S³u¿y do zarz¹dzania zasobami edytora, które dziel¹ siê na dane
konfiguracyjne i próby pomiarowe. Zasoby ³adowane s¹ z odpowiednich œcie¿ek, domyœlnie zasoby sta³e powinny znajdowaæ siê w
data/resources, próby pomiarowe s¹ wyszukiwane i pobierane do data/trials.
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

    virtual bool isLoadLocalTrialData() const
    {
        return loadTrialData;
    };
    virtual void setLoadLocalTrialData(bool load)
    {
        loadTrialData = load;
    };
    
    virtual void setActualLocalTrial(int i);
    virtual void setActualLocalTrial(const std::string& name);

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
    //! S³ownik parserów wed³ug ID.
    typedef std::map<UniqueID, core::IParserPtr> ParsersIDMap;
    //! S³ownik parserów.
    typedef std::map<std::string, core::IParserPtr> ParsersMap;
    //! Multis³ownik parserów.
    typedef std::multimap<std::string, core::IParserPtr> ParsersMultimap;
    //! Sekwencja parserów.
    typedef std::vector<core::IParserPtr> ParsersList;
    //------------------------------------------------------------------------------------------------------------------------------
private:
    //! S³owniki parserów niezainicjalizowanych.
    ParsersIDMap registeredParsersIDMap;
    ParsersMultimap registeredParsersExtMap;
    //! Sekwencja parserów niezainicjalizowanych.
    ParsersList registeredParsersList;
    
    //! S³ownik parserów aktualnie zainicjalizowanych.
    //ParsersMultimap actualParsersExtMap;
    ParsersMap actualParsersFilenameMap;
    //! Sekwencja parserów aktualnie zainicjalizowanych.
    ParsersList actualParsersList;

    //! Sekwencja parserów aktualnej próby pomiarowej.
    ParsersList actualTrialParsersList;

    //! Lista lokalnych prób pomiarowych.
    std::vector<LocalTrial> localTrialsList;

    //! Lista zasobów.
    std::vector<std::string> resourcesPaths;
protected:
    //! \param idx Indeks parsera.
    //! \return Parser o zadanym indeksie.
    virtual core::IParserPtr getParser(int idx);
    //! \param filename nazwa pliku parsera.
    //! \return Odnaleziony parser b¹dŸ NULL.
    core::IParserPtr getParser(const std::string& filename);

    //! \param idx Indeks niezainicjalizowanego parsera.
    //! \return Parser o zadanym indeksie z listy parserów niezainicjalizowanych.
    core::IParserPtr getRawParser(int idx);
    //! \param extension rozszerzenie niezainicjalizowanego parsera.
    //! \return Odnaleziony parser b¹dŸ NULL.
    core::IParserPtr getRawParser(const std::string& extension);
    //! \return Liczba niezainicjalizowanych parserów.
    virtual int getNumRawParsers() const;
    
    //! Szuka na dysku lokalnych prób pomiarowych.
    void findLocalTrials();
    //! Szuka na dysku zasobów.
    void findResources();
    //! \param œcie¿ka do folderu z plikami próby pomiarowej
    //! \return Pojedyncza próba pomiarowa ze œcie¿kami do wszystkich jej plików.
    LocalTrial findLocalTrialsPaths(const std::string& path);
public:
    //! \param trial do za³adowania, czyli inicjalizacja parserów
    void loadActualTrial(const LocalTrial& trial);
    //! ³adowanie zasobów, czyli inicjalizacja parserów
    void loadResourcesEx();
    //! Rejestruje zadany parser.
    //! \param newService
    virtual void registerParser(core::IParserPtr parser);
    //! \return Liczba parserów aktualnie za³adowanej próby pomiarowej.
    virtual int getNumParsers() const;
    //! \param Za³aduj i-t¹ próbê pomiarow¹ z listy triali.
    virtual void loadLocalTrial(int i);
    //! \param Za³aduj próbê pomiarow¹ o podanej nazwie.
    virtual void loadLocalTrial(const std::string& name);
    //! Czyœci informacje o lokalnych próbach pomiarowych.
    void clearLocalTrials();
    //! Czyœci informacje o aktualnej próbie pomiarowej.
    void clearActualTrial();
    //! Czyszczenie po parserach.
    void clearParsers();
};

#endif // DATA_MANAGER_H
