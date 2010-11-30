#include "DataManager.h"

#include <core/IModel.h>
#include <core/Filesystem.h>
#include <tinyxml.h>

DataManager::DataManager(const std::string& resourcesPath, const std::string& trialsPath) : resourcesPath(resourcesPath), trialsPath(trialsPath)
{
	Clear();
	LoadResources();
}

DataManager::~DataManager()
{
    this->Clear();
}

void DataManager::LoadResources()
{
	//szukaj shaderow
	shadersPaths = Filesystem::listFiles(this->resourcesPath, true, "*.frag");
	//szukaj meshy
	meshesPaths = Filesystem::listFiles(this->resourcesPath, true, "*.tbs");
	//szukaj plikow video
	videosPaths = Filesystem::listFiles(this->trialsPath, true, "*.avi");
	//szukaj plikow c3d
	c3dsPaths = Filesystem::listFiles(this->trialsPath, true, "*.c3d");
	//szukaj plikow animacji
	animationPaths = Filesystem::listFiles(this->trialsPath, true, "*.amc");
	//szukaj plikow animacji szkieletowej
	skeletonPaths = Filesystem::listFiles(this->trialsPath, true, "*.asf");
}

void DataManager::Clear() {
	this->shadersPaths.clear();
	this->meshesPaths.clear();
	this->videosPaths.clear();
	this->c3dsPaths.clear();
	this->animationPaths.clear();
	this->skeletonPaths.clear();
}

const std::string& DataManager::GetShaderFilePath(int i)
{
	return shadersPaths[i];
}

int DataManager::GetShaderFilePathCount()
{
	return shadersPaths.size();
}

const std::string& DataManager::GetMeshFilePath(int i)
{
	return meshesPaths[i];
}

int DataManager::GetMeshFilePathCount()
{
	return meshesPaths.size();
}

const std::string& DataManager::GetVideoFilePath(int i)
{
	return videosPaths[i];
}

int DataManager::GetVideoFilePathCount()
{
	return videosPaths.size();
}

const std::string& DataManager::GetC3dFilePath(int i)
{
	return c3dsPaths[i];
}

int DataManager::GetC3dFilePathCount()
{
	return c3dsPaths.size();
}

const std::string& DataManager::GetAnimationFilePath(int i)
{
	return animationPaths[i];
}

int DataManager::GetAnimationFilePathCount()
{
	return animationPaths.size();
}

const std::string& DataManager::GetSkeletonFilePath(int i)
{
	return skeletonPaths[i];
}

int DataManager::GetSkeletonFilePathCount()
{
	return skeletonPaths.size();
}

const std::string& DataManager::GetResourcesPath() const
{
	return resourcesPath;
}

const std::string& DataManager::GetTrialsPath() const
{
	return trialsPath;
}

void DataManager::SetResourcesPath(const std::string& resources)
{
	resourcesPath = resources;
}

void DataManager::SetTrialsPath(const std::string& trials)
{
	trialsPath = trials;
}
