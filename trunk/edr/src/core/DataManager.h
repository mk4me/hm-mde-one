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
    //! S³ownik parserów wed³ug ID.
    typedef std::map<UniqueID, core::IParserPtr> ParsersIDMap;
    //! S³ownik parserów wed³ug rozszerzeñ.
    typedef std::map<std::string, core::IParserPtr> ParsersExtMap;
    //! Sekwencja parserów.
    typedef std::vector<core::IParserPtr> ParsersList;
	//------------------------------------------------------------------------------------------------------------------------------
    //! S³owniki parserów niezainicjalizowanych.
    ParsersIDMap parsersIDMap;
    ParsersExtMap parsersExtMap;
    //! Sekwencja parserów niezainicjalizowanych.
    ParsersList parsersList;
    //! Sekwencja parserów aktualnej próby pomiarowej.
    ParsersList actualTrialParsersList;
    //! Lista lokalnych prób pomiarowych.
	std::vector<LocalTrial> localTrialsList;
public:
    //! Rejestruje zadany parser.
    //! \param newService
    virtual void registerParser(core::IParserPtr parser);
    //! \return Liczba parserów.
    virtual int getNumParsers() const;
    //! \param idx Indeks parsera.
    //! \return Parser o zadanym indeksie.
    virtual core::IParserPtr getParser(int idx);
    //! \param id ID parsera do wyszukania.
    //! \return Odnaleziony parser b¹dŸ NULL.
    virtual core::IParserPtr getParser(UniqueID id);

    //! \param extension rozszerzenie parsera.
    //! \return Odnaleziony parser b¹dŸ NULL.
    core::IParserPtr getParser(const std::string& extension);
    //! \return rozszerzenia plików obs³ugiwanych przez parsery.
    const std::vector<std::string> getSupportedExtensions();
    void findLocalTrials();
	LocalTrial findLocalTrialsPaths(const std::string& path);
    void loadLocalTrial(int i);
    void loadLocalTrial(const std::string& name);
    void loadActualTrialParsers(const LocalTrial& trial);
    ParsersList& getActualTrialParsers();

};

#endif // DATA_MANAGER_H
