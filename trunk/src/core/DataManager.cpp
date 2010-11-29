#include "DataManager.h"

#include <core/IModel.h>
#include <core/Filesystem.h>
#include <tinyxml.h>

DataManager::~DataManager()
{
    this->clear();
}

DataManager::DataManager(const std::string& meshesDir, const std::string& shadersDir, const std::string& trialsDir) : shadersDir(shadersDir), meshesDir(meshesDir), trialsDir(trialsDir)
{
	clear();
	loadResources();
}

void DataManager::loadResources()
{
	//szukaj shaderow
	std::vector<std::string> shaders = Filesystem::listFiles(this->shadersDir, false, "*.frag");
	for(std::vector<std::string>::iterator it = shaders.begin(); it != shaders.end(); ++it)
	{
		this->shadersPaths[getFilename((*it))] = (*it);
	}
	//szukaj meshy
	std::vector<std::string> meshes = Filesystem::listFiles(this->meshesDir, false, "*.tbs");
	for(std::vector<std::string>::iterator it = meshes.begin(); it != meshes.end(); ++it)
	{
		this->meshesPaths[getFilename((*it))] = (*it);
	}
	//szukaj plikow video
	std::vector<std::string> videos = Filesystem::listFiles(this->trialsDir, true, "*.avi");
	for(std::vector<std::string>::iterator it = videos.begin(); it != videos.end(); ++it)
	{
		this->videosPaths[getFilename((*it))] = (*it);
	}
	//szukaj plikow c3d
	std::vector<std::string> c3ds = Filesystem::listFiles(this->trialsDir, true, "*.c3d");
	for(std::vector<std::string>::iterator it = c3ds.begin(); it != c3ds.end(); ++it)
	{
		this->c3dsPaths[getFilename((*it))] = (*it);
	}
	//szukaj plikow animacji
	std::vector<std::string> aniamtions = Filesystem::listFiles(this->trialsDir, true, "*.amc");
	for(std::vector<std::string>::iterator it = aniamtions.begin(); it != aniamtions.end(); ++it)
	{
		this->animationPaths[getFilename((*it))] = (*it);
	}
	//szukaj plikow animacji szkieletowej
	std::vector<std::string> skeletons = Filesystem::listFiles(this->trialsDir, true, "*.asf");
	for(std::vector<std::string>::iterator it = skeletons.begin(); it != skeletons.end(); ++it)
	{
		this->skeletonPaths[getFilename((*it))] = (*it);
	}
}

std::string DataManager::getFilename(const std::string& dir)
{
	std::string filename = dir;

	if(filename.rfind("/") == std::string.npos)
	{
		return filename;
	}
	else
	{
		filename = filename.substr(filename.rfind("/")+1);
	}
	return filename;
}

void DataManager::clear() {
	this->shadersPaths.clear();
	this->meshesPaths.clear();
	this->videosPaths.clear();
	this->c3dsPaths.clear();
	this->animationPaths.clear();
	this->skeletonPaths.clear();
}

const std::string& DataManager::getShader(const std::string& name)
{
	std::map<std::string, std::string>::iterator it = this->shadersPaths.find(name);
	if(it != this->shadersPaths.end())
	{
		return (*it).second;
	}
	return std::string();
}

const std::map<std::string, std::string>& DataManager::getShaders()
{
	return this->shadersPaths;
}

const std::string& DataManager::getMesh(const std::string& name)
{
	std::map<std::string, std::string>::iterator it = this->meshesPaths.find(name);
	if(it != this->meshesPaths.end())
	{
		return (*it).second;
	}
	return std::string();
}

const std::map<std::string, std::string>& DataManager::getMeshes()
{
	return this->meshesPaths;
}

const std::string& DataManager::getVideo(const std::string& name)
{
	std::map<std::string, std::string>::iterator it = this->videosPaths.find(name);
	if(it != this->videosPaths.end())
	{
		return (*it).second;
	}
	return std::string();
}

const std::map<std::string, std::string>& DataManager::getVideos()
{
	return this->videosPaths;
}

const std::string& DataManager::getC3D(const std::string& name)
{
	std::map<std::string, std::string>::iterator it = this->c3dsPaths.find(name);
	if(it != this->c3dsPaths.end())
	{
		return (*it).second;
	}
	return std::string();
}

const std::map<std::string, std::string>& DataManager::getC3Ds()
{
	return this->c3dsPaths;
}

const std::string& DataManager::getAnimation(const std::string& name)
{
	std::map<std::string, std::string>::iterator it = this->animationPaths.find(name);
	if(it != this->animationPaths.end())
	{
		return (*it).second;
	}
	return std::string();
}

const std::map<std::string, std::string>& DataManager::getAnimations()
{
	return this->animationPaths;
}

const std::string& DataManager::getSkeleton(const std::string& name)
{
	std::map<std::string, std::string>::iterator it = this->skeletonPaths.find(name);
	if(it != this->skeletonPaths.end())
	{
		return (*it).second;
	}
	return std::string();
}

const std::map<std::string, std::string>& DataManager::getSkeletons()
{
	return this->skeletonPaths;
}
