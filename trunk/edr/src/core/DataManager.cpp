#include "DataManager.h"

#include <core/Log.h>
#include <boost/regex.hpp>
#include <core/Filesystem.h>
#include <boost/foreach.hpp>

using namespace core;

DataManager::DataManager(const std::string& resourcesPath, const std::string& trialsPath) : resourcesPath(resourcesPath), trialsPath(trialsPath)
{
    clear();
    loadResources();
    loadTrials();
    loadTrialData = false;
    loadUnknownTrialData = false;
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
    //TODO: szukaj fmesh file czy tbs? na razie daje fmesh...
    meshesPaths = Filesystem::listFiles(this->resourcesPath, true, ".fmesh");
    //szukaj styli qt
    applicationSkinsPaths = Filesystem::listFiles(this->resourcesPath, true, ".qss");
}

void DataManager::loadTrials()
{
    trials.clear();
    //przeszukujemy liste prob pomiarowych, nie plikow
    std::vector<std::string> tempPaths = Filesystem::listSubdirectories(trialsPath);
    BOOST_FOREACH(std::string path, tempPaths)
    {
        try
        {
            LocalTrial t = findLocalTrialsPaths(path);
            trials.push_back(t);
        }
        catch(std::runtime_error& e) { }
    }
}

void DataManager::clear()
{
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

const LocalTrial& DataManager::getActualLocalTrial() const
{
    if(loadUnknownTrialData)
    {
        return unknownTrial;
    }
    return trials[actualTrialIndex];
}

void DataManager::setActualLocalTrial(int i)
{
    actualTrialIndex = i;
    loadTrialData = true;
    loadUnknownTrialData = false;

}
void DataManager::setActualLocalTrial(const std::string& name)
{
    //sprawdzamy nazwe
    boost::cmatch matches;
    boost::regex e("(.*)(\\d{4}-\\d{2}-\\d{2}.+)");
    //sprawdzamy, czy zgadza sie nazwa folderu
    if(boost::regex_match(name.c_str(), matches, e))
    {
        for(size_t i = 0; i < trials.size(); i++)
        {
            if(trials.at(i).getName().compare(matches[2]) == 0)
            {
                actualTrialIndex = i;
                loadTrialData = true;
                loadUnknownTrialData = false;
                return;
            }
        }
    }
    unknownTrial = findLocalTrialsPaths(name);
    loadTrialData = true;
    loadUnknownTrialData = true;
    LOG_WARNING(": !Brak wskazanej pomiarowej w zasobach. Ladowanie nieznanej proby pomiarowej.\n");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void DataManager::registerParser(core::IParserPtr parser)
{
    //unikalne ID parsera
    if(parsersIDMap.find(parser->getID()) == parsersIDMap.end())
    {
        //unikalne rozszerzenie parsera
        if(parsersExtMap.find(parser->getSupportedExtension()) == parsersExtMap.end())
        {
            parsersIDMap.insert( std::make_pair(parser->getID(), parser));
            parsersExtMap.insert(std::make_pair(parser->getSupportedExtension(), parser));
            parsersList.push_back(parser);
        }
        else
        {
            throw std::runtime_error("Parser with this extension already registered.");
        }
    }
    else
    {
        throw std::runtime_error("Parser with this ID already registered.");
    }
}

int DataManager::getNumParsers() const
{
    return static_cast<int>(parsersList.size());
}

core::IParserPtr DataManager::getParser(int idx)
{
    if ( idx < static_cast<int>(parsersList.size()) ) {
        return parsersList[idx];
    } else {
        return core::IParserPtr();
    }
}

core::IParserPtr DataManager::getParser(UniqueID id)
{
    ParsersIDMap::iterator it = parsersIDMap.find(id);
    if (it != parsersIDMap.end()) {
        return it->second; 
    } else {
        return core::IParserPtr();
    }
}

core::IParserPtr DataManager::getParser(const std::string& extension)
{
    ParsersExtMap::iterator it = parsersExtMap.find(extension);
    if (it != parsersExtMap.end()) {
        return it->second; 
    } else {
        return core::IParserPtr();
    }
}

const std::vector<std::string> DataManager::getSupportedExtensions()
{
    std::vector<std::string> v;
    for(ParsersExtMap::iterator it = parsersExtMap.begin(); it != parsersExtMap.end(); ++it)
    {
        v.push_back(it->first);
    }
    return v;
}

void DataManager::findLocalTrials()
{
    localTrialsList.clear();
    //przeszukujemy liste prob pomiarowych, nie plikow
    std::vector<std::string> tempPaths = Filesystem::listSubdirectories(trialsPath);
    BOOST_FOREACH(std::string path, tempPaths)
    {
        try
        {
            LocalTrial t = findLocalTrialsPaths(path);
            localTrialsList.push_back(t);
        }
        catch(std::runtime_error& e) { }
    }
}

void DataManager::loadLocalTrial(int i)
{
    //czyscimy dotychczasowa liste zaladowanych parserow
    actualTrialParsersList.clear();
    //ladowanie parserow dla i-tego triala z listy
    loadActualTrialParsers(localTrialsList[i]);
}

void DataManager::loadLocalTrial(const std::string& name)
{
    //sprawdzamy nazwe
    boost::cmatch matches;
    boost::regex e("(.*)(\\d{4}-\\d{2}-\\d{2}.+)");
    //sprawdzamy, czy zgadza sie nazwa folderu
    if(boost::regex_match(name.c_str(), matches, e))
    {
        for(size_t i = 0; i < trials.size(); i++)
        {
            if(localTrialsList.at(i).getName().compare(matches[2]) == 0)
            {
                loadLocalTrial(i);
                return;
            }
        }
    }
    LOG_WARNING(": !Brak wskazanej pomiarowej w zasobach.\n");
}

LocalTrial DataManager::findLocalTrialsPaths(const std::string& path)
{
    LocalTrial t;
    boost::cmatch matches;
    boost::regex e("(.+)(\\d{4}-\\d{2}-\\d{2}.+)");
    //sprawdzamy, czy zgadza sie nazwa folderu
    if(boost::regex_match(path.c_str(), matches, e))
    {
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
    }
    else
    {
        throw std::runtime_error("Invalid trial name.");
    }
    return t;
}

void DataManager::loadActualTrialParsers(const LocalTrial& trial)
{
    //proba zaladowania parsera plikow c3d jesli takowy plik i parser istnieje
    if(trial.isC3d())
    {
        core::IParserPtr c3dParser = getParser(".c3d");
        c3dParser->parseFile(trial.getC3dPath());
        actualTrialParsersList.push_back(c3dParser);
    }
    if(trial.isSkeleton())
    {
        core::IParserPtr asfParser = getParser(".asf");
        asfParser->parseFile(trial.getSkeletonPath());
        actualTrialParsersList.push_back(asfParser);
    }
    if(trial.isAnimations())
    {
        std::vector<std::string> anims = trial.getAnimationsPaths();
        BOOST_FOREACH(std::string anim, anims)
        {
            core::IParserPtr animParser = getParser(".amc");
            animParser->parseFile(anim);
            actualTrialParsersList.push_back(animParser);
        }
    }
    if(trial.isVideos())
    {
        std::vector<std::string> vids = trial.getVideosPaths();
        BOOST_FOREACH(std::string vid, vids)
        {
            core::IParserPtr videoParser = getParser(".avi");
            videoParser->parseFile(vid);
            actualTrialParsersList.push_back(videoParser);
        }
    }
}

std::vector<core::IParserPtr>& DataManager::getActualTrialParsers()
{
    return actualTrialParsersList;
}