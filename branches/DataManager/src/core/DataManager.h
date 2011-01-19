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

class DataManager: public IDataManager
{
public:
    //! S�ownik us�ug.
    typedef std::map<UniqueID, core::IParserPtr> ParsersMap;
    //! Sekwencja us�ug.
    typedef std::vector<core::IParserPtr> ParsersList;
	//------------------------------------------------------------------------------------------------------------------------------
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

    //! Rejestruje zadan� us�ug�.
    //! \param newService
    virtual void registerParser(core::IParserPtr parser);
    //! \return Liczba us�ug.
    virtual int getNumParsers() const;
    //! \param idx Indeks us�ugi.
    //! \return Us�uga o zadanym indeksie.
    virtual core::IParserPtr getParser(int idx);
    //! \param id ID us�ugi do wyszukania.
    //! \return Odnaleziona us�uga b�d� NULL.
    virtual core::IParserPtr getParser(UniqueID id);

	virtual ~DataManager();
protected:
	LocalTrial loadLocalTrial(const std::string& path);
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

	
    //! S�ownik us�ug.
    ParsersMap parsersMap; 
    //! Sekwencja us�ug.
    ParsersList parsersList;

};

#endif // DATA_MANAGER_H
