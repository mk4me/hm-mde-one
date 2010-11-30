#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include <core/IDataManager.h>

class DataManager: public IDataManager
{
public:
//------------------------------------------------------------------------------------------------------------------------------
	DataManager(const std::string& resourcesPath = "data/resources/", const std::string& trialsPath = "data/trials/");

 	virtual void LoadResources();

	virtual void Clear();

    virtual const std::string& GetShaderFilePath(int i);
    virtual int GetShaderFilePathCount();

    virtual const std::string& GetMeshFilePath(int i);
    virtual int GetMeshFilePathCount();

    virtual const std::string& GetVideoFilePath(int i);
    virtual int GetVideoFilePathCount();

    virtual const std::string& GetC3dFilePath(int i);
    virtual int GetC3dFilePathCount();

    virtual const std::string& GetSkeletonFilePath(int i);
    virtual int GetSkeletonFilePathCount();

    virtual const std::string& GetAnimationFilePath(int i);
    virtual int GetAnimationFilePathCount();

	virtual const std::string& GetResourcesPath() const;
	virtual const std::string& GetTrialsPath() const;

	virtual void SetResourcesPath(const std::string& resources);
	virtual void SetTrialsPath(const std::string& trials);

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
