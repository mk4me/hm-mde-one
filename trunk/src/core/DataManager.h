#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include <core/IDataManager.h>

class DataManager: public IDataManager
{
public:
//------------------------------------------------------------------------------------------------------------------------------
	DataManager(const std::string& resourcesPath = "data/resources/", const std::string& trialsPath = "data/trials/");

 	virtual void loadResources();
	virtual void loadTrials();

	virtual void clear();

    virtual const std::string& getShaderFilePath(int i);
    virtual int getShaderFilePathCount();

    virtual const std::string& getMeshFilePath(int i);
    virtual int getMeshFilePathCount();

    virtual const std::string& getVideoFilePath(int i);
    virtual int getVideoFilePathCount();

    virtual const std::string& getC3dFilePath(int i);
    virtual int getC3dFilePathCount();

    virtual const std::string& getSkeletonFilePath(int i);
    virtual int getSkeletonFilePathCount();

    virtual const std::string& getAnimationFilePath(int i);
    virtual int getAnimationFilePathCount();
    virtual std::vector<std::string>* getAnimationPathList();

	virtual const std::string& getResourcesPath() const;
	virtual const std::string& getTrialsPath() const;

	virtual void setResourcesPath(const std::string& resources);
	virtual void setTrialsPath(const std::string& trials);

    virtual ~DataManager();
private:
	std::vector<std::string> shadersPaths;
	std::vector<std::string> meshesPaths;
	std::vector<std::string> videosPaths;
	std::vector<std::string> c3dsPaths;
	std::vector<std::string> animationPaths;
	std::vector<std::string> skeletonPaths;

	std::string resourcesPath;
	std::string trialsPath;

};

#endif // DATA_MANAGER_H
