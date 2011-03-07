#include "CorePCH.h"
#include "DataManager.h"

#include <core/Log.h>
#include <core/Filesystem.h>
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <list>
#include <boost/regex.hpp>

using namespace core;

////////////////////////////////////////////////////////////////////////////////

//! Wewnêtrzna reprezentacja parsera u¿ywana przez DataManagera.
class DataManager::Parser
{
public:
    typedef IDataManager::Path Path;

private:
    //! Prawdziwy wewnêtrzny parser.
    const IParserPtr parser;
    //! Parsowany plik.
    const Path filePath;
    //! Czy parser zwi¹zany jest z zasobami sta³ymi?
    const bool resource;
    //! Czy przeparsowano plik?
    bool parsed;
    //! Czy u¿yto parsera do przeparsowania?
    bool used;


public:
    //! \param parser Faktyczny parser. To ten obiekt kontroluje jego
    //!     czas ¿ycia.
    //! \param resource Czy parser jest zwi¹zany z zasobami sta³ymi?
    Parser(IParser* parser, const Path& path, bool resource = false) :
    parser(parser), parsed(false), used(false), resource(resource), filePath(path)
    {
        UTILS_ASSERT(parser);
        UTILS_ASSERT(!filePath.empty());
    }
    //! Destruktor drukuj¹cy wiadomoœæ o wy³adowaniu pliku.
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
    //! \return Czy parser zwi¹zany jest z zasobami sta³ymi?
    inline bool isResource() const
    {
        return resource;
    }
    //! \return Czy u¿yto tego parsera?
    inline bool isUsed() const
    {
        return used;
    }
    //! \return Czy uda³o siê przeparsowaæ plik?
    inline bool isParsed() const
    {
        return parsed;
    }
    //! \return Œcie¿ka do pliku.
    inline const Path& getPath() const
    {
        return filePath;
    }
    //! \return
    inline IParserPtr getParser() const
    {
        return parser;
    }

    //! Mo¿e rzucaæ wyj¹tkami!
    void parseFile()
    {
        UTILS_ASSERT(!isUsed());
        UTILS_ASSERT(!filePath.empty());
        LOG_DEBUG("Parsing file: " << getPath() );
        used = true;
        parser->parseFile(filePath);
        parsed = true;
    }
    //! Nie rzuca wyj¹tkami.
    //! \return Czy uda³o siê przeparsowaæ?
    bool tryParse()
    {
        try {
            parseFile();
            return true;
        } catch (const std::exception& ex) {
            LOG_ERROR("Error during parsing file " << getPath() << ": " << ex.what());
            return false;
        }
    }
    //! \param objects Lista wrappowanych obiektów, zainicjowanych (przeparsowany parser)
    //!         b¹dŸ nie.
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
        // TODO: powinno byæ porównanie regexowe
        const std::string filename = ptr->getPath().filename();
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
        // TODO: powinno byæ porównanie regexowe
        const std::string filename = ptr->getPath().relative_path().file_string();
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

const boost::filesystem::path& DataManager::getResourcesPath() const
{
    return resourcesPath;
}

const boost::filesystem::path& DataManager::getTrialsPath() const
{
    return trialsPath;
}

void DataManager::setResourcesPath(const IDataManager::Path& resources)
{
    resourcesPath = resources;
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
        // uzupe³nienie s³ownika rozszerzeñ
        typedef boost::char_separator<char> Separator;
        typedef boost::tokenizer<Separator> Tokenizer;
        std::string extensions = parser->getSupportedExtensions();
        Separator separator(";*");
        Tokenizer tokenizer(extensions, separator);
        BOOST_FOREACH( std::string extension, tokenizer ) {
            // dodajemy wiod¹c¹ kropkê
            if(extension.front() != '.') {
                extension.insert(extension.begin(), '.');
            }
            // lowercase
            boost::to_lower(extension);
            // czy parser dla rozszerzenia ju¿ istnieje?
            if ( isExtensionSupported(extension) ) {
                LOG_WARNING("There is at least one parser that supports extension " << extension);
            }
            // rejestrujemy
            registeredExtensions.insert(std::make_pair(extension, parser));
            LOG_INFO("Parser for " << extension << " files registered.");
        }
        // uzupe³nienie pozosta³ych kolekcji
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
        if ( parser->tryParse() ) {
            return parser->getParser();
        }
    } else if ( parser->isParsed() ) {
        return parser->getParser();
    }
    // musia³ byæ b³¹d
    return IParserPtr();
}

// TODO: powinno byæ zwracanie listy
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


DataManager::ParserPtr DataManager::createParser( const Path& path, bool resource )
{
    // próbujemy pobraæ prototyp dla zadanego rozszerzenia
    std::string extension = path.extension();
    IParsersByExtensions::iterator it = registeredExtensions.find( extension );
    if ( it != registeredExtensions.end() ) {
        if(registeredExtensions.count(extension) > 1) {
            // TODO: wybór parsera!
            LOG_WARNING("Multiple parsers found for extension: " << extension);
        }

        // tworzymy parser
        ParserPtr parser(new Parser(it->second->create(), path, resource));
        std::vector<ObjectWrapperPtr> objects;
        parser->getObjects(objects);

        // mapujemy obiekty
        mapObjectsToTypes(objects, parser);

        return parser;
    }
    return ParserPtr();
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
        ParserPtr parser = createParser(path, false);
        if ( parser ) {
            currentParsers.push_back(parser);
        } else {
            LOG_WARNING("No parser found for " << path);
        }
    }
}

void DataManager::loadResources()
{
    BOOST_FOREACH(std::string resourcePath, resourcesPaths) {
        boost::filesystem::path path(resourcePath);
        //sciezka istnieje, jest plikiem i zgadza sie nazwa
        if (boost::filesystem::exists(path) && !boost::filesystem::is_directory(path)) {
            ParserPtr parser = createParser(path, true);
            if ( parser ) {
                currentParsers.push_back(parser);
            } else {
                LOG_WARNING("No parser found for " << path);
            }
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
    // najpierw usuwamy te obiekty, które pochodz¹ z parserów, które nie s¹ przypisane do zasobów
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
    // je¿eli znaleŸliœmy dany typ...
    ObjectsByType::iterator found = currentObjects.find( type );
    if ( found != currentObjects.end() ) {
        // to iterujemy po wszystkich jego wariantach
        for ( ObjectsByType::iterator last = currentObjects.upper_bound( type ); found != last; ++found ) {
            // to, czy sprawdzamy czy to dok³adnie ten typ kontroluje prametr exact
            ObjectWrapperPtr object = found->second.object;
            ParserPtr parser = found->second.parser;
            UTILS_ASSERT(!object->isNull() || !parser->isUsed(), "Obiekt nie powinien byæ sparsowany...");
            if ( !exact || object->isTypeEqual(type) ) {
                // czy obiekt ju¿ jest?
                if ( object->isNull() ) {
                    // mo¿e musimy przeparsowaæ?
                    if ( !parser->isUsed() ) {
                        LOG_DEBUG("Loading object of type \"" << object->getTypeInfo().name() << "\" when looking for \"" << type.name() << "\"");
                        if ( parser->tryParse() ) {
                            if ( object->isNull() ) {
                                // ci¹gle coœ jest nie tak, komunikat o b³êdzie
                                LOG_ERROR("Error parsing file " << parser->getPath() << ": not all object loaded.");
                            } else {
                                // uda³o siê, hurra
                                // pozosta³e obiekty z tego parsera te¿ powinny byæ za³adowane
                                objects.push_back(object);
                            }
                        } else {
                            // TODO: dodaæ parser do listy do usuniêcia, usun¹æ
                        }
                    }
                } else {
                    // wszystko ok, dodajemy
                    objects.push_back(object);
                }
            }
        }
    }
}

void DataManager::mapObjectsToTypes( const std::vector<ObjectWrapperPtr>& objects, ParserPtr parser )
{
    // pobieramy obiekty i dodajemy je do s³ownika obiektów
    BOOST_FOREACH(ObjectWrapperPtr object, objects) {
        ObjectsMapEntry entry = { object, parser };
        std::list<ObjectWrapper::Type> supportedTypes;
        object->getSupportedTypes(supportedTypes);
        BOOST_FOREACH( ObjectWrapper::Type type, supportedTypes ) {
            currentObjects.insert( std::make_pair(type, entry) );
        }
    }
}