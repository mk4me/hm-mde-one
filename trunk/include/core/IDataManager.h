#ifndef I_DATA_MANAGER_H
#define I_DATA_MANAGER_H

class IDataManager
{
public:
	virtual ~IDataManager() {};

 	virtual void LoadResources() = 0;

	virtual void Clear() = 0;

    virtual const std::string& GetShaderFilePath(int i) = 0;
    virtual int GetShaderFilePathCount() = 0;

    virtual const std::string& GetMeshFilePath(int i) = 0;
    virtual int GetMeshFilePathCount() = 0;

    virtual const std::string& GetVideoFilePath(int i) = 0;
    virtual int GetVideoFilePathCount() = 0;

    virtual const std::string& GetC3dFilePath(int i) = 0;
    virtual int GetC3dFilePathCount() = 0;

    virtual const std::string& GetSkeletonFilePath(int i) = 0;
    virtual int GetSkeletonFilePathCount() = 0;

    virtual const std::string& GetAnimationFilePath(int i) = 0;
    virtual int GetAnimationFilePathCount() = 0;

	virtual const std::string& GetResourcesPath() const = 0;
	virtual const std::string& GetTrialsPath() const = 0;
};

#endif // I_DATA_MANAGER_H