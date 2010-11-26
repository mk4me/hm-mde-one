#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include <core/IDataManager.h>

class IModel;
class IC3DModel;

class DataManager: public IDataManager
{
public:
//------------------------------------------------------------------------------------------------------------------------------
	DataManager(const std::string& meshesDir = "data/resources/meshes/", const std::string& shadersDir = "data/resources/shaders/", const std::string& trialsDir = "data/resources/trials/");
	DataManager(int i) {};

	virtual void loadResources();

	virtual void clear();

	virtual const std::string& getShader(const std::string& name);
	virtual const Resources& getShaders();

	virtual const std::string& getMesh(const std::string& name);
	virtual const Resources& getMeshes();

	virtual const std::string& getVideo(const std::string& name);
	virtual const Resources& getVideos();

	virtual const std::string& getC3D(const std::string& name);
	virtual const Resources& getC3Ds();

	void setShadersDir(const std::string& dir) {this->shadersDir = dir; setDirSlashes(this->shadersDir);};
	const std::string& getShadersDir() const {return this->shadersDir;};

	void setMeshesDir(const std::string& dir) {this->meshesDir = dir; setDirSlashes(this->meshesDir);};
	const std::string& getMeshesDir() const {return this->meshesDir;};

	void setTrialsDir(const std::string& dir) {this->trialsDir = dir; setDirSlashes(this->trialsDir);};
	const std::string& getTrialsDir() const {return this->trialsDir;};
//------------------------------------------------------------------------------------------------------------------------------

    DataManager(std::string address, IModel* model, IC3DModel* c3dModel);
    virtual ~DataManager();

    virtual void GetVideoFile();
    virtual void Clear();
    virtual std::string GetFileName();
    virtual std::string GetVideoFilePath(int i);
    virtual int GetVideoFilePathCount();

    virtual std::string GetSkeletonFilePath(int i);
    virtual int GetSkeletonFilePathCount();

    virtual std::string GetAnimationFilePath(int i);
    virtual int GetAnimationFilePathCount();
    virtual std::vector<std::string>* GetAnimationList();

    virtual std::string GetMeshFilePathPath(int i);
    virtual int GetMeshFilePathCount();

    virtual std::string GetC3dFilePath(int i);
    virtual int GetC3dFilePathCount();
    

    virtual IModel* GetModel();

    virtual int GetC3DModelCount();
    virtual IC3DModel* GetC3DModel(int i = 0);
    virtual void AddC3DModel(IC3DModel *c3dmodel);

private:
    void ParseDataToNewManager();

private:
//------------------------------------------------------------------------------------------------------------------------------
	Resources shadersPaths;
	Resources meshesPaths;
	Resources videosPaths;
	Resources c3dsPaths;

	std::string shadersDir;
	std::string meshesDir;
	std::string trialsDir;

	void setDirSlashes(std::string& dir);
//------------------------------------------------------------------------------------------------------------------------------

	std::vector<std::string> m_VideoFilePathList;
    std::vector<std::string> m_SkeletonFilePathList;
    std::vector<std::string> m_AnimationFilePathList;
    std::vector<std::string> m_MeshFilePathList;

    std::vector<std::string> m_c3dFilePathList;

    std::string m_pFileName;

    IModel* m_pModel;
    std::vector<IC3DModel* > m_pC3dModelList;
};



#endif // DATA_MANAGER_H
