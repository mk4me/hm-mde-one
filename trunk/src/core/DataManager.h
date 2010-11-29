#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include <core/IDataManager.h>

class DataManager: public IDataManager
{
public:
//------------------------------------------------------------------------------------------------------------------------------
	DataManager(const std::string& meshesDir = "data/resources/meshes/", const std::string& shadersDir = "data/resources/shaders/", const std::string& trialsDir = "data/trials/");

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

	virtual const std::string& getAnimation(const std::string& name);
	virtual const Resources& getAnimations();

	virtual const std::string& getSkeleton(const std::string& name);
	virtual const Resources& getSkeletons();

	void setShadersDir(const std::string& dir) {this->shadersDir = dir;};
	virtual const std::string& getShadersDir() const {return this->shadersDir;};

	void setMeshesDir(const std::string& dir) {this->meshesDir = dir;};
	virtual const std::string& getMeshesDir() const {return this->meshesDir;};

	void setTrialsDir(const std::string& dir) {this->trialsDir = dir;};
	virtual const std::string& getTrialsDir() const {return this->trialsDir;};

    virtual ~DataManager();
private:
	Resources shadersPaths;
	Resources meshesPaths;
	Resources videosPaths;
	Resources c3dsPaths;
	Resources animationPaths;
	Resources skeletonPaths;

	std::string shadersDir;
	std::string meshesDir;
	std::string trialsDir;

	std::string getFilename(const std::string& dir);
};

#endif // DATA_MANAGER_H
