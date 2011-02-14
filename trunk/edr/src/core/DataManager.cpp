#include "CorePCH.h"
#include "DataManager.h"

#include <core/Log.h>
#include <boost/regex.hpp>
#include <core/Filesystem.h>
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

using namespace core;

DataManager::DataManager(const std::string& resourcesPath, const std::string& trialsPath) : resourcesPath(resourcesPath), trialsPath(trialsPath)
{
    clear();
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
    findResources();
    loadResourcesEx();
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
    //nowy DM
    findLocalTrials();
}

void DataManager::clear()
{
    this->shadersPaths.clear();
    this->meshesPaths.clear();
    this->applicationSkinsPaths.clear();
    this->trials.clear();
    //nowy DM
    clearParsers();
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
    if(registeredParsersIDMap.find(parser->getID()) == registeredParsersIDMap.end())
    {
        registeredParsersIDMap.insert(std::make_pair(parser->getID(), parser));
        registeredParsersExtMap.insert(std::make_pair(boost::to_lower_copy(parser->getSupportedExtensions()), parser));
        registeredParsersList.push_back(parser);
    }
    else
    {
        throw std::runtime_error("Parser with this ID already registered.");
    }
}

int DataManager::getNumParsers() const
{
    return static_cast<int>(actualParsersList.size());
}

int DataManager::getNumRawParsers() const
{
    return static_cast<int>(registeredParsersList.size());
}

core::IParserPtr DataManager::getParser(int idx)
{
    if ( idx < static_cast<int>(actualParsersList.size()) ) {
        return actualParsersList[idx];
    } else {
        return core::IParserPtr();
    }
}

core::IParserPtr DataManager::getParser(const std::string& filename)
{
    for(ParsersMap::iterator it = actualParsersFilenameMap.begin(); it != actualParsersFilenameMap.end(); ++it)
    {
        std::string parserFilename = it->first;
        if(parserFilename.compare(filename) == 0)
        {
            return it->second;
        }
    }
    return core::IParserPtr();
}

core::IParserPtr DataManager::getRawParser(int idx)
{
    if ( idx < static_cast<int>(registeredParsersList.size()) ) {
        return registeredParsersList[idx];
    } else {
        return core::IParserPtr();
    }
}

core::IParserPtr DataManager::getRawParser(const std::string& extension)
{
    boost::char_separator<char> separators(";*.");
    boost::tokenizer<boost::char_separator<char>> parameterExtensionToker(extension, separators);
    std::string readyToCompareExtension = *parameterExtensionToker.begin();
    boost::to_lower(readyToCompareExtension);
    for(ParsersMap::iterator it = registeredParsersExtMap.begin(); it != registeredParsersExtMap.end(); ++it)
    {
        std::string extensions = it->first;
        boost::tokenizer<boost::char_separator<char>> tokens(extensions, separators);
        BOOST_FOREACH(std::string ext, tokens)
        {
            if(readyToCompareExtension.compare(ext) == 0)
            {
                return it->second;
            }
        }
    }
    return core::IParserPtr();
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

void DataManager::findResources()
{
    resourcesPaths.clear();
    //szukaj shaderow
    std::vector<std::string> ext;
    ext.push_back(".frag");
    ext.push_back(".vert");
    std::vector<std::string> temp = Filesystem::listFiles(this->resourcesPath, true, ext);
    resourcesPaths.insert(resourcesPaths.end(), temp.begin(), temp.end());
    //TODO: szukaj fmesh file czy tbs? na razie daje fmesh...
    temp = Filesystem::listFiles(this->resourcesPath, true, ".fmesh");
    resourcesPaths.insert(resourcesPaths.end(), temp.begin(), temp.end());
    //szukaj styli qt
    temp = Filesystem::listFiles(this->resourcesPath, true, ".qss");
    resourcesPaths.insert(resourcesPaths.end(), temp.begin(), temp.end());
}

void DataManager::loadLocalTrial(int i)
{
    //czyscimy dotychczasowa liste zaladowanych parserow
    clearActualLocalTrial();
    //ladowanie parserow dla i-tego triala z listy
    loadActualTrial(localTrialsList[i]);
}

void DataManager::loadLocalTrial(const std::string& name)
{
    //sprawdzamy nazwe
    boost::cmatch matches;
    boost::regex e("(.*)(\\d{4}-\\d{2}-\\d{2}.+)");
    //sprawdzamy, czy zgadza sie nazwa folderu
    if(boost::regex_match(name.c_str(), matches, e))
    {
        for(size_t i = 0; i < localTrialsList.size(); i++)
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
    t.setTrialPath(path.c_str());
    //TODO: przerzucic filtracje nazw folderow prob pomiarowych do communication service
    boost::cmatch matches;
    boost::regex e("(.+)(\\d{4}-\\d{2}-\\d{2}.+)");
    //sprawdzamy, czy zgadza sie nazwa folderu
    if(boost::regex_match(path.c_str(), matches, e))
    {
        t.setName(matches[2]);
    }
    else
    {
        //nazwa sie nie zgadza, ale pozwalamy na zaladowanie pliku mimo wszystko...
        t.setName("unnamed");
    }
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
    return t;
}

void DataManager::loadActualTrial(const LocalTrial& trial)
{
    //TODO: LocalTrial nie bedzie juz raczej potrzebne
    //proba zaladowania parsera plikow c3d jesli takowy plik i parser istnieje
    if(trial.isC3d())
    {
        //szukamy czy jest parser obslugujacy c3d
        core::IParserPtr c3dParser = getRawParser(".c3d");
        if(c3dParser)
        {
            //parsujemy
            c3dParser->parseFile(trial.getC3dPath());
            //dodajemy do listy parserow aktualnej proby pomiarowej
            actualParsersList.push_back(c3dParser);
            actualTrialParsersList.push_back(c3dParser);

            boost::filesystem::path path(trial.getC3dPath());
            actualParsersFilenameMap.insert(std::make_pair(path.filename(), c3dParser));
        }
    }
    if(trial.isSkeleton())
    {
        core::IParserPtr asfParser = getRawParser(".asf");
        if(asfParser)
        {
            asfParser->parseFile(trial.getSkeletonPath());
            actualParsersList.push_back(asfParser);
            actualTrialParsersList.push_back(asfParser);

            boost::filesystem::path path(trial.getSkeletonPath());
            actualParsersFilenameMap.insert(std::make_pair(path.filename(), asfParser));
        }
    }
    if(trial.isAnimations())
    {
        std::vector<std::string> anims = trial.getAnimationsPaths();
        BOOST_FOREACH(std::string anim, anims)
        {
            core::IParserPtr animParser = getRawParser(".amc");
            if(animParser)
            {
                animParser->parseFile(anim);
                actualParsersList.push_back(animParser);
                actualTrialParsersList.push_back(animParser);

                boost::filesystem::path path(anim);
                actualParsersFilenameMap.insert(std::make_pair(path.filename(), animParser));
            }
        }
    }
    if(trial.isVideos())
    {
        std::vector<std::string> vids = trial.getVideosPaths();
        BOOST_FOREACH(std::string vid, vids)
        {
            core::IParserPtr videoParser = getRawParser(".avi");
            if(videoParser)
            {
                videoParser->parseFile(vid);
                actualParsersList.push_back(videoParser);
                actualTrialParsersList.push_back(videoParser);

                boost::filesystem::path path(vid);
                actualParsersFilenameMap.insert(std::make_pair(path.filename(), videoParser));
            }
        }
    }
}

void DataManager::loadResourcesEx()
{
    BOOST_FOREACH(std::string resourcePath, resourcesPaths)
    {
        boost::filesystem::path path(resourcePath);
        //sciezka istnieje, jest plikiem i zgadza sie nazwa
        if(boost::filesystem::exists(path) && !boost::filesystem::is_directory(path))// && !path.extension().compare(filename))
        {
            IParserPtr parser = getRawParser(path.extension());
            //jesli parser dla tego rozszerzenia istnieje, dodajemy go do listy aktualnie zaladowanych parserow (zasobow)
            if(parser)
            {
                actualParsersExtMap.insert(std::make_pair(path.extension(), parser));
                actualParsersList.push_back(parser);

                actualParsersFilenameMap.insert(std::make_pair(path.filename(), parser));
            }
        }
    }
}

void DataManager::clearLocalTrials()
{
    localTrialsList.clear();
    clearActualLocalTrial();
}

void DataManager::clearActualLocalTrial()
{
    actualTrialParsersList.clear();
}

void DataManager::clearParsers()
{
    actualParsersExtMap.clear();
    actualParsersList.clear();
    clearActualLocalTrial();
}
