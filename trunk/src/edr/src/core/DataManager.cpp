#include "CorePCH.h"
#include "DataManager.h"

#include "Log.h"
#include <core/Filesystem.h>
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <algorithm>
#include <list>
#include <boost/regex.hpp>
#include <utils/Push.h>
#include <core/ServiceManager.h>

using namespace core;
using namespace std;

//! Wewnêtrzna reprezentacja parsera u¿ywana przez DataManagera.
class DataManager::Parser
{
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
	//! Czy w³aœnie parsujemy?
	bool parsing;


public:
	//! \param parser Faktyczny parser. To ten obiekt kontroluje jego
	//!     czas ¿ycia.
	//! \param resource Czy parser jest zwi¹zany z zasobami sta³ymi?
	Parser(IParser* parser, const Path& path, bool resource = false) :
	  parser(parser), parsed(false), used(false), resource(resource), filePath(path), parsing(false)
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
	//!
	inline bool isParsing() const
	{
		return parsing;
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
	//! Nie rzuca wyj¹tkami.
	//! \return Czy uda³o siê przeparsowaæ?
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
		// TODO: powinno byæ porównanie regexowe
		const std::string filename = ptr->getPath().relative_path().string();
		return filter == filename;
	}
};

////////////////////////////////////////////////////////////////////////////////


DataManager::DataManager()
{
	clear();
}

DataManager::~DataManager()
{
	this->clear();
}

void DataManager::clear()
{
	clearParsers();
}

const std::string& DataManager::getApplicationSkinsFilePath(int i)
{
	return applicationSkinsPaths[i];
}

int DataManager::getApplicationSkinsFilePathCount()
{
	return applicationSkinsPaths.size();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void DataManager::registerParser(const core::IParserPtr & parser)
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
			//rejestrujemy opis rozszerzenia
			std::string extensionDescription(parser->getExtensionDescription(extension));
			auto it = extensionDescriptions.find(extension);

			if(extensionDescription.empty() == false){    
				if(it != extensionDescriptions.end()){
					if(it->second.description.empty() == false){
						it->second.description.append(" | " + extensionDescription);
					}else{
						it->second.description = extensionDescription;
					}
				}else{
					ExtensionDescription extDescription;
					extDescription.description = extensionDescription;                    
					it = extensionDescriptions.insert(std::make_pair(extension, extDescription)).first;
				}
			}else{
				if(it == extensionDescriptions.end()){
					it = extensionDescriptions.insert(std::make_pair(extension, ExtensionDescription())).first;
				}
			}

			//rejestruj typy dla danego rozszerzenia ktorych moze dostarczyc parser!!
			std::vector<core::ObjectWrapperPtr> objects;
			parser->getExtensionObjects(extension, objects);

			UTILS_ASSERT((objects.empty() == false), "Dla danego rozszerzenia parser nie generuje zadnych obiektow!!");

			for(auto iT = objects.begin(); iT != objects.end(); iT++){
				it->second.possibleTypes.insert(std::make_pair((*iT)->getTypeInfo(), (*iT)->getClassName()));
			}

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

void DataManager::findResources(const std::string& resourcesPath)
{
	resourcesPaths.clear();
	//szukaj shaderow
	std::vector<std::string> ext;
	ext.push_back(".frag");
	ext.push_back(".vert");
	//ext.push_back(".avi");
	try {
		std::vector<std::string> temp = Filesystem::listFiles(resourcesPath, true, ext);
		resourcesPaths.insert(resourcesPaths.end(), temp.begin(), temp.end());
		//szukaj mesh
		temp = Filesystem::listFiles(resourcesPath, true, ".fmesh");
		resourcesPaths.insert(resourcesPaths.end(), temp.begin(), temp.end());
		//szukaj styli qt
		temp = Filesystem::listFiles(resourcesPath, true, ".qss");
		applicationSkinsPaths.insert(applicationSkinsPaths.end(), temp.begin(), temp.end());
	} catch(std::exception& e) {
		LOG_INFO("Finding resources exception: " << e.what());
	}
}

IDataManager::LocalTrial DataManager::findLocalTrialsPaths(const core::Filesystem::Path& path)
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

core::ObjectWrapperPtr DataManager::getWrapper(void * rawPtr) const
{
    auto it = rawPointerToObjectWrapper.find(rawPtr);
    if(it != rawPointerToObjectWrapper.end()){
        return it->second;
    }

    return ObjectWrapperPtr();
}

void DataManager::addExternalData(const core::ObjectWrapperPtr & object)
{
    if(object == nullptr){
        LOG_ERROR("Trying to store not initialized ObjectWrapper pointer");
        return;
    }

    auto insData = externalData.insert(object);
    if(insData.second == false){
        LOG_WARNING("Given object already added to external data: " << object->getClassName());
    }else{

        core::ObjectWrapper::Types supportedTypes;
        object->getSupportedTypes(supportedTypes);

        UTILS_ASSERT((supportedTypes.empty() == false), "Obiekt nie wspiera ¿adnych typów - b³¹d!");

        //uzupe³niamy dane wg typów
        for(auto it = supportedTypes.begin(); it != supportedTypes.end(); it++){
            groupedExternalData[*it].insert(object);
        }

        //odœwie¿amy dane w serwisach
        ServiceManager::getInstance()->setDataPassRequired(true);
    }
}

void DataManager::removeExternalData(const core::ObjectWrapperPtr & object)
{
    if(object == nullptr){
        LOG_ERROR("Trying to remove not initialized ObjectWrapper pointer");
        return;
    }

    auto eData = externalData.erase(object);
    if(eData == 0){
        LOG_WARNING("Given object not exist in external data: " << object->getClassName());
    }else{

        core::ObjectWrapper::Types supportedTypes;
        object->getSupportedTypes(supportedTypes);

        UTILS_ASSERT((supportedTypes.empty() == false), "Obiekt nie wspiera ¿adnych typów - b³¹d!");

        //uzupe³niamy dane wg typów
        for(auto it = supportedTypes.begin(); it != supportedTypes.end(); it++){
            groupedExternalData[*it].erase(object);
        }

        ServiceManager::getInstance()->setDataPassRequired(true);
    }
}

void DataManager::loadFiles(const std::vector<core::Filesystem::Path>& files, const core::ObjectWrapper::Types& types, std::vector<core::ObjectWrapperPtr> & objects)
{
	std::set<Path> allFiles(files.begin(), files.end());

	// usuwamy zbêdne obiekty i parsery
	{
		// szukamy parserów do usuniêcia
		ParsersList::iterator last = std::remove_if( currentParsers.begin(), currentParsers.end(), 
			[&](const ParserPtr& ptr) { return allFiles.find(ptr->getPath()) == allFiles.end(); } 
		);
		std::set<ParserPtr> parsersToRemove(last, currentParsers.end());
		// usuwamy obiekty z ze zbêdnych parserów
		removeObjects( [&](const ObjectsMapEntry& entry) { return parsersToRemove.find(entry.parser) != parsersToRemove.end(); } );
		// fizycznie usuwamy zbêdne wektory
		currentParsers.erase( last, currentParsers.end() );
	}

	// dodajemy nowe
	{
		// usuwamy z allFile te parsery, które ju¿ s¹
		std::for_each( currentParsers.begin(), currentParsers.end(), [&](const ParserPtr& ptr) {
			auto found = allFiles.find(ptr->getPath());
			if ( found != allFiles.end() ) {
				allFiles.erase(ptr->getPath());
			}
		});
		BOOST_FOREACH(const Path& path, allFiles) {
			std::vector<core::ObjectWrapperPtr> loc(createParsers(path, false));
			objects.insert(objects.end(), loc.begin(), loc.end());
			fileLoadedSignal(path, true);
			wrappersAddedSignal(loc, true);
		}
	}

    ServiceManager::getInstance()->setDataPassRequired(true);
}


void DataManager::addFiles( const std::vector<Path>& files )
{
	BOOST_FOREACH(core::Filesystem::Path path, files) {
		createParsers(path, false);
	}

    ServiceManager::getInstance()->setDataPassRequired(true);
}

void DataManager::removeFiles( const std::vector<Path>& files )
{
	std::set<Path> filesToRemove(files.begin(), files.end());

	// szukamy parserów dla zadanych plików
	ParsersList::iterator last = std::remove_if( currentParsers.begin(), currentParsers.end(), 
		[&](const ParserPtr& ptr) { return filesToRemove.find(ptr->getPath()) != filesToRemove.end(); } 
	);
	std::set<ParserPtr> parsersToRemove(last, currentParsers.end());

	// usuwamy obiekty dla zadanych parserów
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

	// fizycznie usuwamy zbêdne wektory
	currentParsers.erase( last, currentParsers.end() );

    ServiceManager::getInstance()->setDataPassRequired(true);
}

void DataManager::loadResources()
{
	BOOST_FOREACH(std::string resourcePath, resourcesPaths) {
		core::Filesystem::Path path(resourcePath);
		//sciezka istnieje, jest plikiem i zgadza sie nazwa
		if (core::Filesystem::pathExists(path) && core::Filesystem::isDirectory(path) == false) {
			createParsers(path, true);
		}
	}
}

void DataManager::clearParsers()
{
	currentParsers.clear();
	currentObjects.clear();
}

bool DataManager::isExtensionSupported( const std::string& extension ) const
{
	return registeredExtensions.find(extension) != registeredExtensions.end();
}

void DataManager::getObjects( std::vector<core::ObjectWrapperPtr>& objects, const core::TypeInfo& type, bool exact /*= false*/ )
{
    //pobranie danych dostarczonych do DataManagera z zewn¹trz
    getExternalData(objects, type, exact);

	// pobranie obiektow z parserow
	getObjectsFromParsers(objects, type, exact);

	// pobranie obiektow z procesorow danych
	dropRemovedWrappers(objectsFromDataProcessors);
	ObjectFromProcessors::iterator found = objectsFromDataProcessors.find( type );
	if ( found != objectsFromDataProcessors.end() ) {
		for ( ObjectFromProcessors::iterator last = objectsFromDataProcessors.upper_bound( type ); found != last; ++found ) {
			ObjectWrapperPtr object = found->second.second.lock();
			if ( !exact || object->isTypeEqual(type) ) {
				UTILS_ASSERT(!object->isNull());
				// wszystko ok, dodajemy
				objects.push_back(object);
			}
		}
	}
}

void DataManager::getExternalData(std::vector<core::ObjectWrapperPtr>& objects, const core::TypeInfo& type, bool exact)
{
    auto it = groupedExternalData.find(type);
    if(it != groupedExternalData.end()){
        for(auto iT = it->second.begin(); iT != it->second.end(); iT++){
            core::ObjectWrapperPtr object(*iT);
            if ( !exact || object->isTypeEqual(type) ) {
                objects.push_back(object);
            }
        }
    }
}

void DataManager::getObjectsFromParsers( std::vector<core::ObjectWrapperPtr>& objects, const core::TypeInfo & type, bool exact /*= false*/ )
{
	// obiekty które bêdziemy usuwaæ
	std::set<ObjectWrapperPtr> invalid;

	// je¿eli znaleŸliœmy dany typ...
	ObjectsByType::iterator found = currentObjects.find( type );
	if ( found != currentObjects.end() ) {
		// to iterujemy po wszystkich jego wariantach
		for ( ObjectsByType::iterator last = currentObjects.upper_bound( type ); found != last; ++found ) {

			// to, czy sprawdzamy czy to dok³adnie ten typ kontroluje prametr exact
			ObjectWrapperPtr object = found->second.object;
			ParserPtr parser = found->second.parser;

			if ( !exact || object->isTypeEqual(type) ) {
				// czy obiekt ju¿ jest?
				if ( object->isNull() ) {
					if ( parser->isParsing() ) {
						throw std::runtime_error("Parsers recursion detected!");
					}
					// mo¿e musimy przeparsowaæ?
					if ( !parser->isUsed() ) {
						LOG_DEBUG("Loading object of type \"" << object->getTypeInfo().name() << "\" when looking for \"" << type.name() << "\"");
						if ( parser->tryParse(this) ) {
							if ( object->isNull() ) {
								// ci¹gle coœ jest nie tak, komunikat o b³êdzie
								LOG_DEBUG("Object is null and parser for " << parser->getPath() << " is already used.");
								invalid.insert(object);
							} else {
								// uda³o siê, hurra
								// pozosta³e obiekty z tego parsera te¿ powinny byæ za³adowane
								objects.push_back(object);
							}
						} else {
							// dodaæ parser do listy do usuniêcia
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

	// usuniêcie niew³aœciwych wpisów
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

void DataManager::mapObjectsToTypes( const std::vector<ObjectWrapperPtr>& objects, const ParserPtr & parser )
{
	// pobieramy obiekty i dodajemy je do s³ownika obiektów
	BOOST_FOREACH(ObjectWrapperPtr object, objects) {
		if ( !object ) {
			LOG_ERROR("Unitialized object from " << parser->getPath() << " (id: " << parser->getParser()->getID() << ")");

		} else {
			ObjectsMapEntry entry = { object, parser };
			core::TypeInfoList supportedTypes;
			object->getSupportedTypes(supportedTypes);
			BOOST_FOREACH( TypeInfo type, supportedTypes ) {
				currentObjects.insert( std::make_pair(type, entry) );
			}
		}
	}
}

std::vector<ObjectWrapperPtr> DataManager::createParsers( const Path& path, bool resource )
{
	std::vector<core::ObjectWrapperPtr> ret;
	// próbujemy pobraæ prototyp dla zadanego rozszerzenia
	std::string extension = path.extension().string();
	IParsersByExtensions::iterator found = registeredExtensions.find( extension );
	if ( found != registeredExtensions.end() ) {
		IParsersByExtensions::iterator last = registeredExtensions.upper_bound(extension);
		if(std::distance(found, last) > 1) {
			// TODO: wybór parsera!
			LOG_WARNING("Multiple parsers found for extension: " << extension);
		}
		// to iterujemy po wszystkich jego wariantach
		for ( ; found != last; ++found ) {
			// tworzymy parser
			ParserPtr parser(new Parser(found->second->create(), path, resource));
			std::vector<ObjectWrapperPtr> objects;
			parser->getObjects(objects);
			ret.insert(ret.end(), objects.begin(), objects.end());
			// mapujemy obiekty
			mapObjectsToTypes(objects, parser);
			// dodajemy do bie¿¹cej listy
			currentParsers.push_back(parser);
		}
	} else {
		LOG_WARNING("No parser found for " << path);
	}

	return ret;
}

template <class Predicate>
void DataManager::removeObjects( Predicate predicate )
{
	// usuwamy obiekty dla zadanych parserów
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

void DataManager::registerObjectFactory( const core::IObjectWrapperFactoryPtr & factory )
{
	core::TypeInfo type = factory->getType();
	if ( !objectFactories.insert(std::make_pair(type, factory)).second ) {
		LOG_ERROR("Factory for " << type.name() << " already exists.");
	}else{
		//tworzymy prototyp by miec dostep do informacji o wspieranych typach
		core::ObjectWrapperConstPtr proto(factory->createWrapper());

		//rejestrujemy typ i jego prototyp
		registeredTypesPrototypes.insert(std::make_pair(type, proto));
	}
}

core::ObjectWrapperPtr DataManager::createWrapper( const core::TypeInfo& type )
{
	auto found = objectFactories.find(type);
	if ( found != objectFactories.end() ) {
		return ObjectWrapperPtr(found->second->createWrapper());
	} else {
		// TODO: elaborate
		throw std::runtime_error("Type not supported.");
	}
}

core::ObjectWrapperCollectionPtr DataManager::createWrapperCollection(const core::TypeInfo& typeInfo)
{
	auto found = objectFactories.find(typeInfo);
	if ( found != objectFactories.end() ) {
		return ObjectWrapperCollectionPtr(found->second->createWrapperCollection());
	} else {
		// TODO: elaborate
		throw std::runtime_error("Type not supported.");
	}
}

const core::ObjectWrapperConstPtr & DataManager::getTypePrototype(const core::TypeInfo & typeInfo) const
{
	auto it = registeredTypesPrototypes.find(typeInfo);
	if(it == registeredTypesPrototypes.end()){
		throw std::runtime_error("Given type is not supported! It was not registered in application.");
	}

	return it->second;
}

void DataManager::addObjects(const DataManager::DataProcessorPtr & dataProcessor, const std::vector<core::ObjectWrapperPtr>& objects) 
{
	for ( int i = objects.size() - 1; i >= 0; --i) { 
		ObjectWrapper::Types types;
		objects[i]->getSupportedTypes(types);
		for (ObjectWrapper::Types::iterator it = types.begin(); it != types.end(); it++) {
			std::pair<DataProcessorPtr, ObjectWrapperWeakPtr> p = std::make_pair(dataProcessor, ObjectWrapperWeakPtr(objects[i]));
			objectsFromDataProcessors.insert(std::make_pair(*it, p));
		}
	}
}

void DataManager::dropRemovedWrappers(ObjectFromProcessors& objectsToCheck)
{
	for (ObjectFromProcessors::iterator it = objectsToCheck.begin(); it != objectsToCheck.end(); ) {
		if (!it->second.second.lock()) {
			ObjectFromProcessors::iterator toDelete = it++;
			objectsToCheck.erase(toDelete);
		} else {
			it++;
		}
	}
}

void DataManager::addObject( const DataProcessorPtr & dataProcessor,  const core::ObjectWrapperPtr & object )
{
	std::vector<core::ObjectWrapperPtr> objects;
	objects.push_back(object);
	addObjects(dataProcessor, objects);
}

void DataManager::addFileCallback( boost::function<void (const core::Filesystem::Path&, bool)> function )
{
	this->fileLoadedSignal.connect(function);
}

void DataManager::removeFileCallback( boost::function<void (const core::Filesystem::Path&, bool)> function )
{						
	//this->fileLoadedSignal.disconnect(function);					
}		

void DataManager::addWrappersCallback( boost::function<void (const std::vector<core::ObjectWrapperPtr>&, bool)> function )
{
	this->wrappersAddedSignal.connect(function);
}

void DataManager::removeWrappersCallback( boost::function<void (const std::vector<core::ObjectWrapperPtr>&, bool)> function )
{
	//this->wrappersAddedSignal.disconnect(function);
}

bool DataManager::tryParseWrapper( core::ObjectWrapperPtr wrapper )
{
	// je¿eli znaleŸliœmy dany typ...
	TypeInfo type = wrapper->getTypeInfo();
	ObjectsByType::iterator found = currentObjects.find( type );
	if ( found != currentObjects.end() ) {
		// to iterujemy po wszystkich jego wariantach
		for ( ObjectsByType::iterator last = currentObjects.upper_bound( type ); found != last; ++found ) {

			// to, czy sprawdzamy czy to dok³adnie ten typ kontroluje prametr exact
			ObjectWrapperPtr object = found->second.object;

			if (object == wrapper) {
				ParserPtr parser = found->second.parser;
				if ( object->isNull() ) {
					if ( parser->isParsing() ) {
						return false;
					}
					// mo¿e musimy przeparsowaæ?
					if ( !parser->isUsed() ) {
						LOG_DEBUG("Loading object of type \"" << object->getTypeInfo().name() << "\" when looking for \"" << type.name() << "\"");
						parser->tryParse(this);
						if (object->isNull()) {
							return false;
						}
						return true;
					} 
				} else {
					return true;
				}
			}
		}
	}

	return false;
}


