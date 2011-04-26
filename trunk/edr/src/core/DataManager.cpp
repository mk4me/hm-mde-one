#include "CorePCH.h"
#include "DataManager.h"

#include <core/Log.h>
#include <core/Filesystem.h>
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <list>
#include <boost/regex.hpp>
#include <utils/Push.h>

using namespace core;

//! Wewn�trzna reprezentacja parsera u�ywana przez DataManagera.
class DataManager::Parser
{
public:
    typedef IDataManager::Path Path;

private:
    //! Prawdziwy wewn�trzny parser.
    const IParserPtr parser;
    //! Parsowany plik.
    const Path filePath;
    //! Czy parser zwi�zany jest z zasobami sta�ymi?
    const bool resource;
    //! Czy przeparsowano plik?
    bool parsed;
    //! Czy u�yto parsera do przeparsowania?
    bool used;
    //! Czy w�a�nie parsujemy?
    bool parsing;


public:
    //! \param parser Faktyczny parser. To ten obiekt kontroluje jego
    //!     czas �ycia.
    //! \param resource Czy parser jest zwi�zany z zasobami sta�ymi?
    Parser(IParser* parser, const Path& path, bool resource = false) :
    parser(parser), parsed(false), used(false), resource(resource), filePath(path), parsing(false)
    {
        UTILS_ASSERT(parser);
        UTILS_ASSERT(!filePath.empty());
    }
    //! Destruktor drukuj�cy wiadomo�� o wy�adowaniu pliku.
    ~Parser()
    {
        if ( isParsed() ) {
            LOG_DEBUG("Unloading parser for file: " << getPath() );
        } else if ( isUsed() ) {
            LOG_DEBUG("Unloading invalid parser for file: " << getPath() );
        } else {
            LOG_DEBUG("Unloading unused parser for file: " << getPath() );
        }
    }

public:
    //! \return Czy parser zwi�zany jest z zasobami sta�ymi?
    inline bool isResource() const
    {
        return resource;
    }
    //! \return Czy u�yto tego parsera?
    inline bool isUsed() const
    {
        return used;
    }
    //! \return Czy uda�o si� przeparsowa� plik?
    inline bool isParsed() const
    {
        return parsed;
    }
    //!
    inline bool isParsing() const
    {
        return parsing;
    }
    //! \return �cie�ka do pliku.
    inline const Path& getPath() const
    {
        return filePath;
    }
    //! \return
    inline IParserPtr getParser() const
    {
        return parser;
    }

    //! Mo�e rzuca� wyj�tkami!
    void parseFile(DataManager* dataManager)
    {
        UTILS_ASSERT(!isUsed());
        UTILS_ASSERT(!filePath.empty());
        LOG_DEBUG("Parsing file: " << getPath() );
        used = true;
        utils::Push<bool> parsingPushed(parsing, true);
        parser->parseFile(dataManager, filePath.string());
        parsed = true;
    }
    //! Nie rzuca wyj�tkami.
    //! \return Czy uda�o si� przeparsowa�?
    bool tryParse(DataManager* dataManager)
    {
        try {
            parseFile(dataManager);
            return true;
        } catch (const std::exception& ex) {
            LOG_ERROR("Error during parsing file " << getPath() << ": " << ex.what());
            return false;
        }
    }
    //! \param objects Lista wrappowanych obiekt�w, zainicjowanych (przeparsowany parser)
    //!         b�d� nie.
    inline void getObjects(std::vector<ObjectWrapperPtr>& objects)
    {
        parser->getObjects(objects);
    }
};

////////////////////////////////////////////////////////////////////////////////

struct DataManager::FindByFilenamePredicate {
    std::string filter;

    FindByFilenamePredicate(const std::string& filetr) :
    filter(filter)
    {}

    bool operator()( const DataManager::ParserPtr& ptr ) const
    {
        // TODO: powinno by� por�wnanie regexowe
        const std::string filename = ptr->getPath().filename().string();
        return filter == filename;
    }
};

////////////////////////////////////////////////////////////////////////////////

struct DataManager::FindByRelativePathPredicate {
    std::string filter;

    FindByRelativePathPredicate(const std::string& filetr) :
    filter(filter)
    {}

    bool operator()( const DataManager::ParserPtr& ptr ) const
    {
        // TODO: powinno by� por�wnanie regexowe
        const std::string filename = ptr->getPath().relative_path().string();
        return filter == filename;
    }
};

////////////////////////////////////////////////////////////////////////////////


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

const core::IDataManager::LocalTrial& DataManager::getLocalTrial(int i) const
{
    return localTrialsList[i].second;
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

const boost::filesystem::path& DataManager::getApplicationDataPath() const
{
    UTILS_ASSERT(!applicationDataPath.empty(), "Path should be initialized first");
    return applicationDataPath;
}

const boost::filesystem::path& DataManager::getUserDataPath() const
{
    UTILS_ASSERT(!userDataPath.empty(), "Path should be initialized first");
    return userDataPath;
}

const boost::filesystem::path& DataManager::getResourcesPath() const
{
    UTILS_ASSERT(!resourcesPath.empty(), "Path should be initialized first");
    return resourcesPath;
}

const boost::filesystem::path& DataManager::getTrialsPath() const
{
    return trialsPath;
}

void DataManager::setUserDataPath( const Path& path )
{
    UTILS_ASSERT(!path.empty(), "Path should not be empty");
    //UTILS_ASSERT(!path.is_relative(), "Path should not be relative");
    //UTILS_ASSERT(!path.has_filename(), "Path should not point to file");
    userDataPath = path;
}

void DataManager::setApplicationDataPath( const Path& path )
{
    UTILS_ASSERT(!path.empty(), "Path should not be empty");
    //UTILS_ASSERT(!path.is_relative(), "Path should not be relative");
    //UTILS_ASSERT(!path.has_filename(), "Path should not point to file");
    applicationDataPath = path;
}

void DataManager::setResourcesPath(const Path& path)
{
    UTILS_ASSERT(!path.empty(), "Path should not be empty");
    //UTILS_ASSERT(!path.is_relative(), "Path should not be relative");
    //UTILS_ASSERT(!path.has_filename(), "Path should not point to file");
    resourcesPath = path;
}

void DataManager::setTrialsPath(const IDataManager::Path& trials)
{
    trialsPath = trials;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void DataManager::registerParser(core::IParserPtr parser)
{
    //unikalne ID parsera
    if(registeredParsers.find(parser->getID()) == registeredParsers.end()) {
        // uzupe�nienie s�ownika rozszerze�
        typedef boost::char_separator<char> Separator;
        typedef boost::tokenizer<Separator> Tokenizer;
        std::string extensions = parser->getSupportedExtensions();
        Separator separator(";*");
        Tokenizer tokenizer(extensions, separator);
        BOOST_FOREACH( std::string extension, tokenizer ) {
            // dodajemy wiod�c� kropk�
            if(extension.front() != '.') {
                extension.insert(extension.begin(), '.');
            }
            // lowercase
            boost::to_lower(extension);
            // czy parser dla rozszerzenia ju� istnieje?
            if ( isExtensionSupported(extension) ) {
                LOG_WARNING("There is at least one parser that supports extension " << extension);
            }
            // rejestrujemy
            registeredExtensions.insert(std::make_pair(extension, parser));
            LOG_INFO("Parser for " << extension << " files registered.");
        }
        // uzupe�nienie pozosta�ych kolekcji
        registeredParsers.insert(std::make_pair(parser->getID(), parser));
    } else {
        throw std::runtime_error("Parser with this ID already registered.");
    }
}

int DataManager::getNumRegisteredParsers() const
{
    return static_cast<int>(registeredParsers.size());
}

core::IParserConstPtr DataManager::getRegisteredParser( int idx ) const
{
    UTILS_ASSERT(idx >= 0 && idx < getNumRegisteredParsers());
    auto it = registeredParsers.begin();
    std::advance( it, idx );
    return const_pointer_cast<const IParser>(it->second);
}


int DataManager::getNumParsers() const
{
    return static_cast<int>(currentParsers.size());
}


IParserPtr DataManager::getParser(int idx)
{
    UTILS_ASSERT(idx < getNumParsers());
    ParserPtr& parser = currentParsers[idx];
    return parser->getParser();
}

IParserPtr DataManager::getInitializedParser( int idx )
{
    UTILS_ASSERT(idx < getNumParsers());
    ParserPtr& parser = currentParsers[idx];
    if ( !parser->isUsed() ) {
        LOG_DEBUG("Loading parser for file: " << parser->getPath());
        if ( parser->tryParse(this) ) {
            return parser->getParser();
        }
    } else if ( parser->isParsed() ) {
        return parser->getParser();
    }
    // musia� by� b��d
    return IParserPtr();
}

// TODO: powinno by� zwracanie listy
IParserPtr DataManager::getParser(const std::string& filter)
{
    throw std::runtime_error("Not supported.");

    FindByFilenamePredicate predicate(filter);
    ParsersList::iterator it = std::find_if( currentParsers.begin(), currentParsers.end(), predicate );
    ParsersList::iterator last = currentParsers.end();
    while ( it != currentParsers.end() ) {
        return (*it)->getParser();
        //it = std::find_if( it+1, currentParsers.end(), predicate );
    }
    return IParserPtr();
}

IParserPtr DataManager::getInitializedParser( const std::string& filter )
{
    throw std::runtime_error("Not supported.");
}

void DataManager::findLocalTrials()
{
    localTrialsList.clear();
    //przeszukujemy liste prob pomiarowych, nie plikow
    std::vector<std::string> tempPaths = Filesystem::listSubdirectories(trialsPath.string());
    BOOST_FOREACH(std::string path, tempPaths)
    {
        try
        {
            LocalTrial trial = findLocalTrialsPaths(path);
            localTrialsList.push_back(std::make_pair<IDataManager::Path, IDataManager::LocalTrial>(IDataManager::Path(path), trial));
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
    //ext.push_back(".avi");
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
    loadTrial(localTrialsList[i].second);
}

void DataManager::loadLocalTrial(const core::IDataManager::Path& path)
{
    for(LocalTrialsList::iterator it = localTrialsList.begin(); it != localTrialsList.end(); ++it)
    {
        if(it->first == path)
        {
            loadTrial(it->second);
            return;
        }
    }
    LOG_WARNING("No such trial in local resources.\n");
    loadTrial(findLocalTrialsPaths(path));
}

IDataManager::LocalTrial DataManager::findLocalTrialsPaths(const core::IDataManager::Path& path)
{
    core::IDataManager::LocalTrial trial;

    //przeszukujemy katalog w poszukiwaniu plikow:
    std::vector<std::string> masks;
    masks.push_back(".c3d");
    masks.push_back(".amc");
    masks.push_back(".asf");
    masks.push_back(".avi");
    masks.push_back(".imgsequence");
    std::vector<std::string> filesPath = Filesystem::listFiles(path.string(), true, masks);
    BOOST_FOREACH(std::string path, filesPath)
    {
        trial.push_back(path);
    }
    return trial;
}

void DataManager::loadTrial(const core::IDataManager::LocalTrial& trial)
{
    clearCurrentTrial();
    loadTrialData = true;
    //proba zaladowania parsera plikow c3d jesli takowy plik i parser istnieje
    BOOST_FOREACH(boost::filesystem::path path, trial)
    {
        createParsers(path, false);
    }
}

void DataManager::loadFiles(const std::vector<core::IDataManager::Path>& files, const core::ObjectWrapper::Types& types)
{
    loadTrialData = true;

    // TODO: zamiast wektora powinien by� set!
    std::set<Path> allFiles(files.begin(), files.end());

    // usuwamy zb�dne obiekty i parsery
    {
        // szukamy parser�w do usuni�cia
        ParsersList::iterator last = std::remove_if( currentParsers.begin(), currentParsers.end(), 
            [&](const ParserPtr& ptr) { return allFiles.find(ptr->getPath()) == allFiles.end(); } 
        );
        std::set<ParserPtr> parsersToRemove(last, currentParsers.end());
        // usuwamy obiekty z ze zb�dnych parser�w
        removeObjects( [&](const ObjectsMapEntry& entry) { return parsersToRemove.find(entry.parser) != parsersToRemove.end(); } );
        // fizycznie usuwamy zb�dne wektory
        currentParsers.erase( last, currentParsers.end() );
    }

    // dodajemy nowe
    {
        // usuwamy z allFile te parsery, kt�re ju� s�
        std::for_each( currentParsers.begin(), currentParsers.end(), [&](const ParserPtr& ptr) {
            auto found = allFiles.find(ptr->getPath());
            if ( found != allFiles.end() ) {
                allFiles.erase(ptr->getPath());
            }
        });
        BOOST_FOREACH(const Path& path, allFiles) {
            createParsers(path, false);
        }
    }


//     clearCurrentTrial();
//     
//     BOOST_FOREACH(boost::filesystem::path path, files)
//     {
//         createParsers(path, false);
//     }
}


void DataManager::addFiles( const std::vector<Path>& files )
{
    loadTrialData = true;
    BOOST_FOREACH(boost::filesystem::path path, files) {
        createParsers(path, false);
    }
}

void DataManager::removeFiles( const std::vector<Path>& files )
{
    loadTrialData = true;

    // TODO: zamiast wektora powinien by� set!
    std::set<Path> filesToRemove(files.begin(), files.end());

    // szukamy parser�w dla zadanych plik�w
    ParsersList::iterator last = std::remove_if( currentParsers.begin(), currentParsers.end(), 
        [&](const ParserPtr& ptr) { return filesToRemove.find(ptr->getPath()) != filesToRemove.end(); } 
    );
    std::set<ParserPtr> parsersToRemove(last, currentParsers.end());

    // usuwamy obiekty dla zadanych parser�w
    for ( ObjectsByType::iterator it = currentObjects.begin(); it != currentObjects.end(); ) {
        ObjectsMapEntry& entry = it->second;
        if ( parsersToRemove.find(entry.parser) != parsersToRemove.end() ) {
            // usuwamy wpis
            ObjectsByType::iterator toErase = it++;
            currentObjects.erase(toErase);
        } else {
            ++it;
        }
    }

    // fizycznie usuwamy zb�dne wektory
    currentParsers.erase( last, currentParsers.end() );
}

void DataManager::loadResources()
{
    BOOST_FOREACH(std::string resourcePath, resourcesPaths) {
        boost::filesystem::path path(resourcePath);
        //sciezka istnieje, jest plikiem i zgadza sie nazwa
        if (boost::filesystem::exists(path) && !boost::filesystem::is_directory(path)) {
            createParsers(path, true);
        }
    }
}

void DataManager::clearLocalTrials()
{
    localTrialsList.clear();
    clearCurrentTrial();
}

void DataManager::clearParsers()
{
    currentParsers.clear();
    currentObjects.clear();
}

void DataManager::clearCurrentTrial()
{
    // najpierw usuwamy te obiekty, kt�re pochodz� z parser�w, kt�re nie s� przypisane do zasob�w
    for ( ObjectsByType::iterator it = currentObjects.begin(); it != currentObjects.end(); ) {
        ObjectsMapEntry& entry = it->second;
        if ( !entry.parser->isResource() ) {
            // usuwamy wpis
            ObjectsByType::iterator toErase = it++;
            currentObjects.erase(toErase);
        } else {
            ++it;
        }
    }

    // teraz usuwamy same parsery
    ParsersList::iterator last = std::remove_if( currentParsers.begin(), currentParsers.end(), 
        [](const ParserPtr& ptr) { return !ptr->isResource(); } 
    );
    currentParsers.erase( last, currentParsers.end() );
}

bool DataManager::isExtensionSupported( const std::string& extension ) const
{
    return registeredExtensions.find(extension) != registeredExtensions.end();
}

void DataManager::getObjects( std::vector<core::ObjectWrapperPtr>& objects, const std::type_info& type, bool exact /*= false*/ )
{
    // obiekty kt�re b�dziemy usuwa�
    std::set<ObjectWrapperPtr> invalid;

    // je�eli znale�li�my dany typ...
    ObjectsByType::iterator found = currentObjects.find( type );
    if ( found != currentObjects.end() ) {
        // to iterujemy po wszystkich jego wariantach
        for ( ObjectsByType::iterator last = currentObjects.upper_bound( type ); found != last; ++found ) {
            // to, czy sprawdzamy czy to dok�adnie ten typ kontroluje prametr exact
            ObjectWrapperPtr object = found->second.object;
            ParserPtr parser = found->second.parser;
            //UTILS_ASSERT(!object->isNull() || !parser->isUsed(), "Obiekt nie powinien by� sparsowany...");
            if ( !exact || object->isTypeEqual(type) ) {
                // czy obiekt ju� jest?
                if ( object->isNull() ) {
                    if ( parser->isParsing() ) {
                        throw std::runtime_error("Parsers recursion detected!");
                    }
                    // mo�e musimy przeparsowa�?
                    if ( !parser->isUsed() ) {
                        LOG_DEBUG("Loading object of type \"" << object->getTypeInfo().name() << "\" when looking for \"" << type.name() << "\"");
                        if ( parser->tryParse(this) ) {
                            if ( object->isNull() ) {
                                // ci�gle co� jest nie tak, komunikat o b��dzie
                                LOG_DEBUG("Object is null and parser for " << parser->getPath() << " is already used.");
                                invalid.insert(object);
                            } else {
                                // uda�o si�, hurra
                                // pozosta�e obiekty z tego parsera te� powinny by� za�adowane
                                objects.push_back(object);
                            }
                        } else {
                            // TODO: doda� parser do listy do usuni�cia, usun��
                            invalid.insert(object);
                        }
                    } else {
                        // 
                        LOG_DEBUG("Object is null and parser for " << parser->getPath() << " is already used.");
                        invalid.insert(object);
                    }
                } else {
                    // wszystko ok, dodajemy
                    objects.push_back(object);
                }
            }
        }
    }

    // usuni�cie niew�a�ciwych wpis�w
    if ( !invalid.empty() ) {
        LOG_DEBUG("Removing " << invalid.size() << " null objects after looking for " << type.name());
        for ( ObjectsByType::iterator it = currentObjects.begin(), last = currentObjects.end(); it != last && !invalid.empty(); ) {
            if ( invalid.erase(it->second.object) ) {
                ObjectsByType::iterator toErase = it++;
                currentObjects.erase(toErase);
            } else {
                ++it;
            }
        }
    }
}

void DataManager::mapObjectsToTypes( const std::vector<ObjectWrapperPtr>& objects, ParserPtr parser )
{
    // pobieramy obiekty i dodajemy je do s�ownika obiekt�w
    BOOST_FOREACH(ObjectWrapperPtr object, objects) {
        if ( !object ) {
            LOG_ERROR("Unitialized object from " << parser->getPath() << " (id: " << parser->getParser()->getID() << ")");

        } else {
            ObjectsMapEntry entry = { object, parser };
            std::list<TypeInfo> supportedTypes;
            object->getSupportedTypes(supportedTypes);
            BOOST_FOREACH( TypeInfo type, supportedTypes ) {
                currentObjects.insert( std::make_pair(type, entry) );
            }
        }
    }
}

// DataManager::ParserPtr DataManager::createParser( const Path& path, bool resource )
// {
//     // pr�bujemy pobra� prototyp dla zadanego rozszerzenia
//     std::string extension = path.extension();
//     IParsersByExtensions::iterator it = registeredExtensions.find( extension );
//     if ( it != registeredExtensions.end() ) {
//         if(registeredExtensions.count(extension) > 1) {
//             // TODO: wyb�r parsera!
//             LOG_WARNING("Multiple parsers found for extension: " << extension);
//         }
// 
//         // tworzymy parser
//         ParserPtr parser(new Parser(it->second->create(), path, resource));
//         std::vector<ObjectWrapperPtr> objects;
//         parser->getObjects(objects);
// 
//         // mapujemy obiekty
//         mapObjectsToTypes(objects, parser);
// 
//         return parser;
//     }
//     return ParserPtr();
// }


void DataManager::createParsers( const Path& path, bool resource )
{
    // pr�bujemy pobra� prototyp dla zadanego rozszerzenia
    std::string extension = path.extension().string();
    IParsersByExtensions::iterator found = registeredExtensions.find( extension );
    if ( found != registeredExtensions.end() ) {
        IParsersByExtensions::iterator last = registeredExtensions.upper_bound(extension);
        if(std::distance(found, last) > 1) {
            // TODO: wyb�r parsera!
            LOG_WARNING("Multiple parsers found for extension: " << extension);
        }
        // to iterujemy po wszystkich jego wariantach
        for ( ; found != last; ++found ) {
            // tworzymy parser
            ParserPtr parser(new Parser(found->second->create(), path, resource));
            std::vector<ObjectWrapperPtr> objects;
            parser->getObjects(objects);
            // mapujemy obiekty
            mapObjectsToTypes(objects, parser);
            // dodajemy do bie��cej listy
            currentParsers.push_back(parser);
        }
    } else {
        LOG_WARNING("No parser found for " << path);
    }
}

template <class Predicate>
void DataManager::removeObjects( Predicate predicate )
{
    // usuwamy obiekty dla zadanych parser�w
    for ( ObjectsByType::iterator it = currentObjects.begin(); it != currentObjects.end(); ) {
        ObjectsMapEntry& entry = it->second;
        if ( predicate(entry) ) {
            // usuwamy wpis
            ObjectsByType::iterator toErase = it++;
            currentObjects.erase(toErase);
        } else {
            ++it;
        }
    }
}




