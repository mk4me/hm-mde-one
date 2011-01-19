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

class DataManager: public IDataManager
{
public:
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

};

#endif // DATA_MANAGER_H
