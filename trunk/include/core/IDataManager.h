#ifndef I_DATA_MANAGER_H
#define I_DATA_MANAGER_H

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

	virtual const std::string& getAnimation(const std::string& name) = 0;
	virtual const Resources& getAnimations() = 0;

	virtual const std::string& getSkeleton(const std::string& name) = 0;
	virtual const Resources& getSkeletons() = 0;
	
	virtual const std::string& getShadersDir() const = 0;
	virtual const std::string& getMeshesDir() const = 0;
	virtual const std::string& getTrialsDir() const = 0;
//------------------------------------------------------------------------------------------------------------------------------

};

#endif // I_DATA_MANAGER_H