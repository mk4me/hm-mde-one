#include "DataManager.h"

#include <core/IModel.h>
#include <core/Filesystem.h>
#include <tinyxml.h>

DataManager::DataManager(const std::string& resourcesPath, const std::string& trialsPath) : resourcesPath(resourcesPath), trialsPath(trialsPath)
{
	clear();
	loadResources();
}

DataManager::~DataManager()
{
    this->clear();
}

void DataManager::loadResources()
{
	//szukaj shaderow
	shadersPaths = Filesystem::listFiles(this->resourcesPath, true, "*.frag");
	//szukaj meshy
	meshesPaths = Filesystem::listFiles(this->resourcesPath, true, "*.tbs");
}

void DataManager::loadTrials()
{
	//szukaj plikow video
	videosPaths = Filesystem::listFiles(this->trialsPath, false, "*.avi");
	//szukaj plikow c3d
	c3dsPaths = Filesystem::listFiles(this->trialsPath, false, "*.c3d");
	//szukaj plikow animacji
	animationPaths = Filesystem::listFiles(this->trialsPath, false, "*.amc");
	//szukaj plikow animacji szkieletowej
	skeletonPaths = Filesystem::listFiles(this->trialsPath, false, "*.asf");
}

void DataManager::clear() {
	this->shadersPaths.clear();
	this->meshesPaths.clear();
	this->videosPaths.clear();
	this->c3dsPaths.clear();
	this->animationPaths.clear();
	this->skeletonPaths.clear();
}

const std::string& DataManager::getShaderFilePath(int i)
{
	return shadersPaths[i];
}

int DataManager::getShaderFilePathCount()
{
	return shadersPaths.size();
}

const std::string& DataManager::getMeshFilePath(int i)
{
	return meshesPaths[i];
}

int DataManager::getMeshFilePathCount()
{
	return meshesPaths.size();
}

const std::string& DataManager::getVideoFilePath(int i)
{
	return videosPaths[i];
}

int DataManager::getVideoFilePathCount()
{
	return videosPaths.size();
}

const std::string& DataManager::getC3dFilePath(int i)
{
	return c3dsPaths[i];
}

int DataManager::getC3dFilePathCount()
{
	return c3dsPaths.size();
}

const std::string& DataManager::getAnimationFilePath(int i)
{
	return animationPaths[i];
}

int DataManager::getAnimationFilePathCount()
{
	return animationPaths.size();
}

const std::string& DataManager::getSkeletonFilePath(int i)
{
	return skeletonPaths[i];
}

int DataManager::getSkeletonFilePathCount()
{
	return skeletonPaths.size();
}

const std::string& DataManager::getResourcesPath() const
{
	return resourcesPath;
}

const std::string& DataManager::getTrialsPath() const
{
	return trialsPath;
}

void DataManager::setResourcesPath(const std::string& resources)
{
	resourcesPath = resources;
}

void DataManager::setTrialsPath(const std::string& trials)
{
	trialsPath = trials;
}
