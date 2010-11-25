#ifndef I_DATA_MANAGER_H
#define I_DATA_MANAGER_H

//#include <string>

class IModel;
class IC3DModel;

class IDataManager
{
public:
	typedef std::map<std::string, std::string> Resources;

	virtual ~IDataManager() {};

//------------------------------------------------------------------------------------------------------------------------------
	virtual void loadResources() = 0;

	virtual void clear() = 0;

	virtual const std::string& getShader(const std::string& name) = 0;
	virtual const Resources& getShaders() = 0;

	virtual const std::string& getMesh(const std::string& name) = 0;
	virtual const Resources& getMeshes() = 0;

	virtual const std::string& getVideo(const std::string& name) = 0;
	virtual const Resources& getVideos() = 0;

	virtual const std::string& getC3D(const std::string& name) = 0;
	virtual const Resources& getC3Ds() = 0;
//------------------------------------------------------------------------------------------------------------------------------

    virtual void Clear() = 0;

    //virtual void GetVideoFile() = 0;

    virtual std::string GetFileName() = 0;

    //virtual std::string GetVideoFilePath(int i) = 0;
    //virtual int GetVideoFilePathCount() = 0;

    virtual std::string GetSkeletonFilePath(int i) = 0;
    virtual int GetSkeletonFilePathCount() = 0;

    virtual std::string GetAnimationFilePath(int i) = 0;
    virtual int GetAnimationFilePathCount() = 0;

    virtual std::string GetMeshFilePathPath(int i) = 0;
    virtual int GetMeshFilePathCount() = 0;

    virtual std::string GetC3dFilePath(int i) = 0;
    virtual int GetC3dFilePathCount() = 0;

    virtual IModel* GetModel() = 0;

    virtual int GetC3DModelCount() = 0;
    virtual IC3DModel* GetC3DModel(int i = 0) = 0;
    virtual void AddC3DModel(IC3DModel *c3dmodel) = 0;
};

#endif // I_DATA_MANAGER_H