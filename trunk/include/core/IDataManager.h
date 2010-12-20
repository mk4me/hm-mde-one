#ifndef I_DATA_MANAGER_H
#define I_DATA_MANAGER_H

#include <core/LocalTrial.h>
#include <vector>

class IDataManager
{
public:
	virtual ~IDataManager() {};

 	virtual void loadResources() = 0;
	virtual void loadTrials() = 0;

	virtual const LocalTrial& getLocalTrial(int i) const = 0;
	virtual int getLocalTrialsCount() const = 0;

	virtual const LocalTrial& getActualTrial() const = 0;
	virtual void setActualTrial(int i) = 0;
	virtual void setActualTrial(const std::string& name) = 0;

	virtual void clear() = 0;

    virtual const std::string& getShaderFilePath(int i) = 0;
    virtual int getShaderFilePathCount() = 0;

    virtual const std::string& getMeshFilePath(int i) = 0;
    virtual int getMeshFilePathCount() = 0;

    virtual const std::string& getApplicationSkinsFilePath(int i) = 0;
    virtual int getApplicationSkinsFilePathCount() = 0;

	virtual bool isLoadTrialData() const = 0;
	virtual void setLoadTrialData(bool load) = 0;

	virtual const std::string& getResourcesPath() const = 0;
	virtual const std::string& getTrialsPath() const = 0;
};

#endif // I_DATA_MANAGER_H