#include "DataManager.h"

#include <core/Log.h>
#include <boost/regex.hpp>
#include <core/Filesystem.h>
#include <boost/foreach.hpp>

DataManager::DataManager(const std::string& resourcesPath, const std::string& trialsPath) : resourcesPath(resourcesPath), trialsPath(trialsPath)
{
	clear();
	loadResources();
	loadTrials();
	loadTrialData = false;
	actualTrialIndex = 0;
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
	std::vector<std::string> ext;
	ext.push_back(".frag");
	ext.push_back(".vert");
	shadersPaths = Filesystem::listFiles(this->resourcesPath, true, ext);
	//szukaj tbs file
	meshesPaths = Filesystem::listFiles(this->resourcesPath, true, ".fmesh");
	//szukaj styli qt
	applicationSkinsPaths = Filesystem::listFiles(this->resourcesPath, true, ".qss");
}

void DataManager::loadTrials()
{
	trials.clear();
	//teraz przeszukujemy liste prob pomiarowych, nie plikow
	std::vector<std::string> tempPaths = Filesystem::listSubdirectories(trialsPath);
	boost::cmatch matches;
	boost::regex e("(.+)(\\d{4}-\\d{2}-\\d{2}.+)");
	BOOST_FOREACH(std::string path, tempPaths)
	{
		//sprawdzamy, czy zgadza sie nazwa folderu
		if(boost::regex_match(path.c_str(), matches, e))
		{
			//mamy triala, zapisujemy jego nazwe i sciezke do katalogu
			LocalTrial t;
			t.setTrialPath(path.c_str());
			t.setName(matches[2]);
			//przeszukujemy katalog w poszukiwaniu plikow:
			//proba pomiarowa moze miec maksymalnie 1 plik c3d, amc, asf i 4 avi
			std::vector<std::string> filesPath = Filesystem::listFiles(t.getTrialPath(), false, ".c3d");
			if(filesPath.size() > 0)
			{
				t.setC3dPath(filesPath[0]);
			}
			filesPath = Filesystem::listFiles(t.getTrialPath(), false, ".amc");
			if(filesPath.size() > 0)
			{
				t.setAnimationsPaths(filesPath);
			}
			filesPath = Filesystem::listFiles(t.getTrialPath(), false, ".asf");
			if(filesPath.size() > 0)
			{
				t.setSkeletonPath(filesPath[0]);
			}
			filesPath = Filesystem::listFiles(t.getTrialPath(), false, ".avi");
			if(filesPath.size() == 4)
			{
				t.setVideosPaths(filesPath);
			}
			trials.push_back(t);
		}
	}
}

void DataManager::clear() {
	this->shadersPaths.clear();
	this->meshesPaths.clear();
	this->applicationSkinsPaths.clear();
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

const LocalTrial& DataManager::getActualTrial() const
{
	return trials[actualTrialIndex];
}

void DataManager::setActualTrial(int i)
{
	actualTrialIndex = i;
	loadTrialData = true;

}
void DataManager::setActualTrial(const std::string& name)
{
	for(size_t i = 0; i < trials.size(); i++)
	{
		if(trials.at(i).getName().compare(name) == 0)
		{
			actualTrialIndex = i;
			loadTrialData = true;
			return;
		}
	}
	LOG_ERROR << ": !Blad ladowania proby pomiarowej, brak proby pomiarowej w zasobach\n";
}
