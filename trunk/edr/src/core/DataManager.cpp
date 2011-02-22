#include "CorePCH.h"
#include "DataManager.h"

#include <core/Log.h>
#include <core/Filesystem.h>
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

using namespace core;

DataManager::DataManager(const std::string& resourcesPath, const std::string& trialsPath) : resourcesPath(resourcesPath), trialsPath(trialsPath)
{
    clear();
    loadTrialData = false;
}

DataManager::~DataManager()
{
    this->clear();
}

void DataManager::clear()
{
    clearLocalTrials();
    clearParsers();
}

const IDataManager::LocalTrial& DataManager::getLocalTrial(int i) const
{
    return localTrialsList[i];
}

int DataManager::getLocalTrialsCount() const
{
    return localTrialsList.size();
}

const std::string& DataManager::getApplicationSkinsFilePath(int i)
{
    return applicationSkinsPaths[i];
}

int DataManager::getApplicationSkinsFilePathCount()
{
    return applicationSkinsPaths.size();
}

const boost::filesystem::path& DataManager::getResourcesPath() const
{
    return resourcesPath;
}

const boost::filesystem::path& DataManager::getTrialsPath() const
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

const IDataManager::LocalTrial& DataManager::getCurrentLocalTrial() const
{
    return currentTrial;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void DataManager::registerParser(core::IParserPtr parser)
{
    //unikalne ID parsera
    if(registeredParsersIDMap.find(parser->getID()) == registeredParsersIDMap.end())
    {
        //wyci�gamy kazde supportowane rozszerzenie
        boost::char_separator<char> separators(";*");
        std::string ext = parser->getSupportedExtensions();
        boost::tokenizer<boost::char_separator<char>> parameterExtensionToker(ext, separators);
        for(boost::tokenizer<boost::char_separator<char>>::iterator it = parameterExtensionToker.begin(); it != parameterExtensionToker.end(); ++it)
        {
            std::string extension = *it;
            if(extension.front() != '.')
            {
                extension.insert(extension.begin(), '.');
            }
            boost::to_lower(extension);
            //czy jest juz jakis parser obslugujacy to rozszerzenie?
            if(registeredParsersExtMap.find(extension) != registeredParsersExtMap.end())
            {
                LOG_WARNING("There is at least one parser that support extension" << extension);
            }
            //rejestrujemy parser dla kazdego rozszerzenia z osobna
            registeredParsersExtMap.insert(std::make_pair(extension, parser));
            LOG_INFO("Parser for " << extension << " files registered.");
        }
        registeredParsersIDMap.insert(std::make_pair(parser->getID(), parser));
        registeredParsersList.push_back(parser);
    }
    else
    {
        throw std::runtime_error("Parser with this ID already registered.");
    }
}

int DataManager::getNumParsers() const
{
    return static_cast<int>(currentParsersList.size());
}

int DataManager::getNumRawParsers() const
{
    return static_cast<int>(registeredParsersList.size());
}

core::IParserPtr DataManager::getParser(int idx)
{
    if(idx < static_cast<int>(currentParsersList.size()))
    {
        //sprawdzamy, czy wybrany parser przeprowadzil juz parsowanie na pliku, jesli nie, parsujemy
        ParsedFilesMap::iterator parsedItr = parsedFilesMap.find(currentParsersList[idx].first);
        if(parsedItr == parsedFilesMap.end())
        {
            currentParsersList[idx].second->parseFile(currentParsersList[idx].first);
            parsedFilesMap.insert(std::make_pair<IDataManager::Path, bool>(currentParsersList[idx].first, true));
        }
        if(parsedItr != parsedFilesMap.end() && parsedItr->second == false)
        {
            currentParsersList[idx].second->parseFile(currentParsersList[idx].first);
            parsedItr->second = true;
        }
        return currentParsersList[idx].second;
    }
    return core::IParserPtr();
}

core::IParserPtr DataManager::getParser(const std::string& filename)
{
    for(ParsersMap::iterator it = currentParsersFilepathMap.begin(); it != currentParsersFilepathMap.end(); ++it)
    {
        //plik ma taka sama nazwe co szukany
        if(it->first.filename().compare(filename) == 0)
        {
            //sprawdzamy, czy wybrany parser przeprowadzil juz parsowanie na pliku, jesli nie, parsujemy
            ParsedFilesMap::iterator parsedItr = parsedFilesMap.find(it->first);
            if(parsedItr == parsedFilesMap.end())
            {
                it->second->parseFile(it->first);
                parsedFilesMap.insert(std::make_pair<IDataManager::Path, bool>(it->first, true));
            }
            if(parsedItr != parsedFilesMap.end() && parsedItr->second == false)
            {
                it->second->parseFile(it->first);
                parsedItr->second = true;
            }
            return it->second;
        }
    }
    return core::IParserPtr();
}

core::IParserPtr DataManager::createRawParser(int idx)
{
    if(idx < static_cast<int>(registeredParsersList.size()))
    {
        return core::IParserPtr(registeredParsersList[idx]->create());
    }
    else
    {
        return core::IParserPtr();
    }
}

core::IParserPtr DataManager::createRawParser(const std::string& extension)
{
    ParsersMultimap::iterator it = registeredParsersExtMap.find(extension);
    if(it != registeredParsersExtMap.end())
    {
        if(registeredParsersExtMap.count(extension) > 1)
        {
            LOG_WARNING("Multiple parsers found for extension: " << extension << ".");
        }
        //jesli sa parsery - zwracamy tylko pierwszy!
        return core::IParserPtr(it->second->create());
    }
    return core::IParserPtr();
}

void DataManager::findLocalTrials()
{
    localTrialsMap.clear();
    localTrialsList.clear();
    //przeszukujemy liste prob pomiarowych, nie plikow
    std::vector<std::string> tempPaths = Filesystem::listSubdirectories(trialsPath.string());
    BOOST_FOREACH(std::string path, tempPaths)
    {
        try
        {
            LocalTrial trial = findLocalTrialsPaths(path);
            localTrialsMap.insert(std::make_pair<IDataManager::Path, IDataManager::LocalTrial>(IDataManager::Path(path), trial));
            localTrialsList.push_back(trial);
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
    std::vector<std::string> temp = Filesystem::listFiles(this->resourcesPath.string(), true, ext);
    resourcesPaths.insert(resourcesPaths.end(), temp.begin(), temp.end());
    //szukaj mesh
    temp = Filesystem::listFiles(this->resourcesPath.string(), true, ".fmesh");
    resourcesPaths.insert(resourcesPaths.end(), temp.begin(), temp.end());
    //szukaj styli qt
    temp = Filesystem::listFiles(this->resourcesPath.string(), true, ".qss");
    applicationSkinsPaths.insert(applicationSkinsPaths.end(), temp.begin(), temp.end());
}

void DataManager::loadLocalTrial(int i)
{
    //ladowanie parserow dla i-tego triala z listy
    loadTrial(localTrialsList[i]);
}

void DataManager::loadLocalTrial(const IDataManager::Path& path)
{
    for(LocalTrialsMap::iterator it = localTrialsMap.begin(); it != localTrialsMap.end(); ++it)
    {
        if(it->first == path)
        {
            loadTrial(it->second);
            return;
        }
    }
    LOG_WARNING("No trial in resources.\n");
    loadTrial(findLocalTrialsPaths(path));
}

IDataManager::LocalTrial DataManager::findLocalTrialsPaths(const IDataManager::Path& path)
{
    IDataManager::LocalTrial trial;

    //przeszukujemy katalog w poszukiwaniu plikow:
    std::vector<std::string> masks;
    masks.push_back(".c3d");
    masks.push_back(".amc");
    masks.push_back(".asf");
    masks.push_back(".avi");
    std::vector<std::string> filesPath = Filesystem::listFiles(path.string(), true, masks);
    BOOST_FOREACH(std::string path, filesPath)
    {
        trial.push_back(path);
    }
    return trial;
}

void DataManager::loadTrial(const IDataManager::LocalTrial& trial)
{
    clearCurrentTrial();
    currentTrial = trial;
    loadTrialData = true;
    //proba zaladowania parsera plikow c3d jesli takowy plik i parser istnieje
    BOOST_FOREACH(boost::filesystem::path path, trial)
    {
        core::IParserPtr parser = createRawParser(path.extension());
        if(parser)
        {
            //dodajemy do listy parserow aktualnej proby pomiarowej
            currentParsersList.push_back(std::make_pair(path, parser));
            currentTrialParsersList.push_back(parser);
            currentParsersFilepathMap.insert(std::make_pair(path, parser));
        }
        else
        {
            LOG_WARNING("No parser found for " << path.string());
        }
    }
}

void DataManager::loadResources()
{
    BOOST_FOREACH(std::string resourcePath, resourcesPaths)
    {
        boost::filesystem::path path(resourcePath);
        //sciezka istnieje, jest plikiem i zgadza sie nazwa
        if(boost::filesystem::exists(path) && !boost::filesystem::is_directory(path))// && !path.extension().compare(filename))
        {
            IParserPtr parser = createRawParser(path.extension());
            //jesli parser dla tego rozszerzenia istnieje, dodajemy go do listy aktualnie zaladowanych parserow (zasobow)
            if(parser)
            {
                currentParsersList.push_back(std::make_pair(path, parser));

                currentParsersFilepathMap.insert(std::make_pair(path, parser));
            }
            else
            {
                LOG_WARNING("No parser found for " << path.filename());
            }
        }
    }
}

void DataManager::clearLocalTrials()
{
    localTrialsMap.clear();
    localTrialsList.clear();
    clearCurrentTrial();
}

void DataManager::clearCurrentTrial()
{
    //wyszukaj parsery triala w listach i slownikach
    BOOST_FOREACH(core::IParserPtr trial, currentTrialParsersList)
    {
        for(ParsersPathList::iterator it = currentParsersList.begin(); it != currentParsersList.end(); ++it)
        {
            if(trial == (*it).second)
            {
                parsedFilesMap.erase(parsedFilesMap.find(it->first));
                currentParsersList.erase(it);
                break;
            }
        }
        for(ParsersMap::iterator it = currentParsersFilepathMap.begin(); it != currentParsersFilepathMap.end(); ++it)
        {
            if(trial == (*it).second)
            {
                currentParsersFilepathMap.erase(it);
                break;
            }
        }
    }
    currentTrialParsersList.clear();
}

void DataManager::clearParsers()
{
    currentParsersList.clear();
    clearCurrentTrial();
}
