#include "DataManager.h"

#include <core/IModel.h>
#include <boost/regex.hpp>
#include <core/Filesystem.h>

DataManager::DataManager(const std::string& resourcesPath, const std::string& trialsPath) : resourcesPath(resourcesPath), trialsPath(trialsPath)
{
	clear();
	loadResources();
	loadTrials();
}

DataManager::~DataManager()
{
    this->clear();
}

void DataManager::loadResources()
{
	shadersPaths.clear();
	meshesPaths.clear();
	applicationSkinsPaths.clear();
	//szukaj shaderow
	shadersPaths = Filesystem::listFiles(this->resourcesPath, true, "*.frag");
	//szukaj tbs file
	meshesPaths = Filesystem::listFiles(this->resourcesPath, true, "*.tbs");
	//szukaj styli qt
	applicationSkinsPaths = Filesystem::listFiles(this->resourcesPath, true, "*.qss");
}

void DataManager::loadTrials()
{
	trials.clear();
	//teraz przeszukujemy liste prob pomiarowych, nie plikow
	std::vector<std::string> tempPaths = Filesystem::listSubdirectories(trialsPath);
	boost::cmatch matches;
	boost::regex e("(.+)(\\d{4}-\\d{2}-\\d{2}.+)");
	for(std::vector<std::string>::iterator it = tempPaths.begin(); it != tempPaths.end(); ++it)
	{
		//sprawdzamy, czy zgadza sie nazwa folderu
		if(boost::regex_match(it->c_str(), matches, e))
		{
			//mamy triala, zapisujemy jego nazwe i sciezke do katalogu
			LocalTrial t;
			//t.trialPath = it->c_str();
			//t.name = matches[2];
			t.setTrialPath(it->c_str());
			t.setName(matches[2]);
			//przeszukujemy katalog w poszukiwaniu plikow:
			//proba pomiarowa moze miec maksymalnie 1 plik c3d, amc, asf i 4 avi
			std::vector<std::string> filesPath = Filesystem::listFiles(t.getTrialPath(), false, "*.c3d");
			if(filesPath.size() > 0)
			{
				t.setC3dPath(filesPath[0]);
				//t.c3dPath = filesPath[0];
			}
			filesPath = Filesystem::listFiles(t.getTrialPath(), false, "*.amc");
			if(filesPath.size() > 0)
			{
				t.setAnimationsPaths(filesPath);
				//t.animationPath = filesPath[0];
			}
			filesPath = Filesystem::listFiles(t.getTrialPath(), false, "*.asf");
			if(filesPath.size() > 0)
			{
				t.setSkeletonPath(filesPath[0]);
				//t.skeletonPath = filesPath[0];
			}
			filesPath = Filesystem::listFiles(t.getTrialPath(), false, "*.avi");
			if(filesPath.size() == 4)
			{
				t.setVideosPaths(filesPath);
				//t.videosPaths.clear();
				//t.videosPaths.push_back(filesPath[0]);
				//t.videosPaths.push_back(filesPath[1]);
				//t.videosPaths.push_back(filesPath[2]);
				//t.videosPaths.push_back(filesPath[3]);
			}
			trials.push_back(t);
		}
	}

	//videosPaths.clear();
	//c3dsPaths.clear();
	//animationPaths.clear();
	//skeletonPaths.clear();
	////nie przeszukujemy podkatalogow, sprawdzamy konkretny katalog aby wyluskac sciezki dla jednej proby pomiarowej
	////szukaj plikow video
	//videosPaths = Filesystem::listFiles(this->trialsPath, false, "*.avi");
	////szukaj plikow c3d
	//c3dsPaths = Filesystem::listFiles(this->trialsPath, false, "*.c3d");
	////szukaj plikow animacji
	//animationPaths = Filesystem::listFiles(this->trialsPath, false, "*.amc");
	////szukaj plikow animacji szkieletowej
	//skeletonPaths = Filesystem::listFiles(this->trialsPath, false, "*.asf");
}

void DataManager::clear() {
	this->shadersPaths.clear();
	this->meshesPaths.clear();
	this->applicationSkinsPaths.clear();
	//this->videosPaths.clear();
	//this->c3dsPaths.clear();
	//this->animationPaths.clear();
	//this->skeletonPaths.clear();
	this->trials.clear();
}

const LocalTrial& DataManager::getLocalTrial(int i) const
{
	return trials[i];
}

int DataManager::getLocalTrialsCount() const
{
	return trials.size();
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

const std::string& DataManager::getApplicationSkinsFilePath(int i)
{
	return applicationSkinsPaths[i];
}

int DataManager::getApplicationSkinsFilePathCount()
{
	return applicationSkinsPaths.size();
}
//
//const std::string& DataManager::getVideoFilePath(int i)
//{
//	return videosPaths[i];
//}
//
//int DataManager::getVideoFilePathCount()
//{
//	return videosPaths.size();
//}
//
//const std::string& DataManager::getC3dFilePath(int i)
//{
//	return c3dsPaths[i];
//}
//
//int DataManager::getC3dFilePathCount()
//{
//	return c3dsPaths.size();
//}
//
//const std::string& DataManager::getAnimationFilePath(int i)
//{
//	return animationPaths[i];
//}
//
//int DataManager::getAnimationFilePathCount()
//{
//	return animationPaths.size();
//}
//
//const std::string& DataManager::getSkeletonFilePath(int i)
//{
//	return skeletonPaths[i];
//}
//
//int DataManager::getSkeletonFilePathCount()
//{
//	return skeletonPaths.size();
//}

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
//
//std::vector<std::string>* DataManager::getAnimationPathList()
//{
//    return &animationPaths;
//}

const LocalTrial& DataManager::getActualTrial() const
{
	return trials[actualTrialIndex];
}

void DataManager::setActualTrial(int i)
{
	actualTrialIndex = i;

}
void DataManager::setActualTrial(const std::string& path)
{
	for(size_t i = 0; i < trials.size(); i++)
	{
		if(trials.at(i).getTrialPath().compare(path))
		{
			actualTrialIndex = i;
			return;
		}
	}
}
