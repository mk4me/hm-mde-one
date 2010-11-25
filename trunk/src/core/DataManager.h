#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include <core/IDataManager.h>

//class IModel;
//class IC3DModel;

class DataManager: public IDataManager
{
public:
//------------------------------------------------------------------------------------------------------------------------------
	DataManager(const std::string& meshes_dir = "resources/meshes", const std::string& shaders_dir = "resources/shaders", const std::string& trials_dir = "resources/trials");

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

	void setShadersDir(const std::string& dir) {this->m_shaders_dir = dir; setDirSlashes(this->m_shaders_dir);};
	const std::string& getShadersDir() const {return this->m_shaders_dir;};

	void setMeshesDir(const std::string& dir) {this->m_meshes_dir = dir; setDirSlashes(this->m_meshes_dir);};
	const std::string& getMeshesDir() const {return this->m_meshes_dir;};

	void setTrialsDir(const std::string& dir) {this->m_trials_dir = dir; setDirSlashes(this->m_trials_dir);};
	const std::string& getTrialsDir() const {return this->m_trials_dir;};
//------------------------------------------------------------------------------------------------------------------------------

    DataManager(std::string address, IModel* model, IC3DModel* c3dModel);
    virtual ~DataManager();

    //virtual void GetVideoFile();
    virtual void Clear();
    virtual std::string GetFileName();
    //virtual std::string GetVideoFilePath(int i);
    //virtual int GetVideoFilePathCount();

    virtual std::string GetSkeletonFilePath(int i);
    virtual int GetSkeletonFilePathCount();

    virtual std::string GetAnimationFilePath(int i);
    virtual int GetAnimationFilePathCount();

    virtual std::string GetMeshFilePathPath(int i);
    virtual int GetMeshFilePathCount();

    virtual std::string GetC3dFilePath(int i);
    virtual int GetC3dFilePathCount();
    

    virtual IModel* GetModel();

    virtual int GetC3DModelCount();
    virtual IC3DModel* GetC3DModel(int i = 0);
    virtual void AddC3DModel(IC3DModel *c3dmodel);

private:
//------------------------------------------------------------------------------------------------------------------------------
	Resources m_shaders_paths;
	Resources m_meshes_paths;
	Resources m_videos_paths;
	Resources m_c3ds_paths;

	std::string m_shaders_dir;
	std::string m_meshes_dir;
	std::string m_trials_dir;

	void setDirSlashes(std::string& dir);
//------------------------------------------------------------------------------------------------------------------------------

    std::vector<std::string> m_SkeletonFilePathList;
    std::vector<std::string> m_AnimationFilePathList;
    std::vector<std::string> m_MeshFilePathList;

    std::vector<std::string> m_c3dFilePathList;

    std::string m_pFileName;

    IModel* m_pModel;
    std::vector<IC3DModel* > m_pC3dModelList;
};



#endif // DATA_MANAGER_H
