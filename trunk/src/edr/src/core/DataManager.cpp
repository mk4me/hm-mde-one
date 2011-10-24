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
	const Filesystem::Path filePath;
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
	Parser(IParser* parser, const Filesystem::Path& path) :
	  parser(parser), parsed(false), used(false), filePath(path), parsing(false)
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

    inline void reset()
    {
        used = false;
        parsed = false;
    }

	//!
	inline bool isParsing() const
	{
		return parsing;
	}

	//! \return Œcie¿ka do pliku.
	inline const Filesystem::Path& getPath() const
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
		utils::Push<bool> parsingPushed(parsing, true);
		parser->parseFile(filePath.string());
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
	inline void getObjects(core::Objects& objects)
	{
		parser->getObjects(objects);
	}
};

DataManager::ParserInitializer::ParserInitializer(const ParserPtr & parser, DataManager * dm) : parser(parser), dataManager(dm)
{

}

DataManager::ParserInitializer::~ParserInitializer()
{

}


void DataManager::ParserInitializer::initialize(core::ObjectWrapperPtr & object)
{
    if(parser->isUsed() == false){
        if(parser->tryParse() == true){
            LOG_DEBUG("Parser ID " << parser->getParser()->getID() << " successfully initialized object of type " << object->getTypeInfo().name() << " parsing file " << parser->getPath());
        }else{
            LOG_DEBUG("Parser ID " << parser->getParser()->getID() << " failed to initialized object of type " << object->getTypeInfo().name() << " parsing file " << parser->getPath());
        }
    }
}

void DataManager::ParserInitializer::doDeinitialize(core::ObjectWrapperPtr & object)
{
    auto it = dataManager->objectsByParsers.find(parser);
    if(it != dataManager->objectsByParsers.end() ){
        for(auto objectIT = it->second.begin(); objectIT != it->second.end(); objectIT++){
            if(*objectIT != nullptr){
                (*objectIT)->reset();
            }
        }

        parser->reset();
    }
}

DataManager * ManagerHelper<DataManager>::manager = nullptr;

DataManager::DataManager()
{

}

DataManager::~DataManager()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void DataManager::registerParser(const core::IParserPtr & parser)
{
    //unikalne ID parsera
    if(registeredParsers.find(parser->getID()) == registeredParsers.end()) {
        // uzupe³nienie s³ownika rozszerzeñ
        core::IParser::Extensions extensions;
        // pobranie s³ownika rozszerzeñ z parsera
        parser->getSupportedExtensions(extensions);

        if(extensions.empty() == true){
            std::stringstream str;
            str << "Parser ID: "<< parser->getID() << " Trying to register parser with 0 supported extensions";
            throw std::runtime_error(str.str());
        }

        //weryfikacja rozszerzeñ
        for(auto it = extensions.begin(); it != extensions.end(); it++){

            if(it->first.empty() == true){
                std::stringstream str;
                str << "Parser ID: "<< parser->getID() << " Trying to register extension of length 0";
                throw std::runtime_error(str.str());
            }

            if(it->second.types.empty() == true){
                std::stringstream str;
                str << "Trying to register extension " << it->first << " without supported types for parser " << parser->getID();
                throw std::runtime_error(str.str());                
            }
        }

        for(auto it = extensions.begin(); it != extensions.end(); it++){
            //w³aœciwe rozszerzenie
            std::string extension(it->first);

            //uzupe³niamy kropke
            if(extension.front() != '.'){
                extension.insert(extension.begin(), '.');
            }

            // lowercase rozszerzenia
            boost::to_lower(extension);

            auto extIT = registeredExtensions.find(extension);

            if(extIT != registeredExtensions.end()) {
                LOG_WARNING("There is at least one parser that supports extension " << extension);                
            }else{
                //tworzymy nowy wpis dla nowego rozszerzenia
                extIT = registeredExtensions.insert(SupportedExtensionsPersistenceData::value_type(extension, ExtendedExtensionDescription())).first;
            }

            // aktualizujemy parsery dla danego rozszerzenia
            extIT->second.parsers.insert(parser);
            //prubujemy aktualizaowac opisy tego rozszerzenia
            if(it->second.description.empty() == false){
                extIT->second.descriptions.push_back(it->second.description);
            }
            //aktualizujemy typy tego rozszerzenia
            extIT->second.types.insert(it->second.types.begin(), it->second.types.end());

            LOG_INFO("Parser for extension " << extension << " registered. Parser ID: " << parser->getID());
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

core::ObjectWrapperPtr DataManager::getWrapper(void * rawPtr) const
{
    ScopedLock lock(stateMutex);

    auto it = rawPointerToObjectWrapper.find(rawPtr);
    if(it != rawPointerToObjectWrapper.end()){
        return it->second;
    }

    return ObjectWrapperPtr();
}

void DataManager::getManagedData(core::Objects & objects) const
{
    ScopedLock lock(stateMutex);
    objects.insert(this->objects.begin(), this->objects.end());
}


void DataManager::initializeData(core::ObjectWrapperPtr & data)
{
    ScopedLock lock(stateMutex);

    UTILS_ASSERT((data != nullptr), "Niezainicjowany ObjectWrapper");
    if(data->isNull() == true){
        //szukamy inicjalizatora - mamy go w spisie obiektow
        auto initializerIT = objectsWithInitializers.find(data);

        if(initializerIT->second == nullptr){
            throw std::runtime_error("Trying to initialize object without initializer");
        }

        initializerIT->second->initialize(data);

        if(data->isNull() == false){
            rawPointerToObjectWrapper[data->getRawPtr()] = data;
        }else{
            LOG_DEBUG("Data type initialization failed: " << data->getTypeInfo().name());
        }
    }
}


bool DataManager::isInitializable(const core::ObjectWrapperPtr & data) const
{
    ScopedLock lock(stateMutex);

    //szukamy inicjalizatora - mamy go w spisie obiektow
    auto initializerIT = objectsWithInitializers.find(data);

    if(initializerIT == objectsWithInitializers.end() || initializerIT->second == nullptr){
        return false;
    }
    
    return true;
}


void DataManager::deinitializeData(core::ObjectWrapperPtr & data)
{
    ScopedLock lock(stateMutex);

    if(data->isNull() == false){
        //szukamy inicjalizatora - mamy go w spisie obiektow
        auto initializerIT = objectsWithInitializers.find(data);

        if(initializerIT == objectsWithInitializers.end() || initializerIT->second == nullptr){
            throw std::runtime_error("Trying deinitialize data without initializer");
        }

        void* rawPtr = data->getRawPtr();

        initializerIT->second->deinitialize(data);

        if(data == nullptr){
            rawPointerToObjectWrapper.erase(rawPtr);
        }
    }
}


void DataManager::addData(const core::ObjectWrapperPtr & data, const core::DataInitializerPtr & initializer)
{
    ScopedLock lock(stateMutex);

    auto it = objects.find(data);

    if(it != objects.end()){
        throw std::runtime_error("Trying to add data already managed by DataManager");
    }

    if(data->isNull() == false || initializer != nullptr){

        objects.insert(data);

        //dodaj do grupowania po typach
        core::TypeInfoList types;
        data->getSupportedTypes(types);

        for(auto it = types.begin(); it != types.end(); it++){
            objectsByTypes[*it].insert(data);
        }

        if(initializer != nullptr){
            objectsWithInitializers[data] = initializer;
        }
    }else{
        throw std::runtime_error("Attempting to add empty data without initializer");
    }

    this->core::IMemoryDataManager::notify();
}


void DataManager::removeData(const core::ObjectWrapperPtr & data)
{
    ScopedLock lock(stateMutex);

    auto it = objects.find(data);

    if(it == objects.end()){
        throw std::runtime_error("Trying to remove data not managed by DataManager");
    }

    objects.erase(data);

    //usun z grupowania po typach
    core::TypeInfoList types;
    data->getSupportedTypes(types);

    for(auto it = types.begin(); it != types.end(); it++){
        auto IT = objectsByTypes.find(*it);
        IT->second.erase(data);
        if(IT->second.empty() == true){
            objectsByTypes.erase(IT);
        }
    }

    objectsWithInitializers.erase(data);

    if(data->isNull() == false){
        rawPointerToObjectWrapper.erase(data->getRawPtr());
    }

    this->core::IMemoryDataManager::notify();
}

void DataManager::getManagedData(core::Files & files) const
{
    ScopedLock lock(stateMutex);

    for(auto it = parsersByFiles.begin(); it != parsersByFiles.end(); it++){
        files.insert(it->first);
    }
}

void DataManager::addData(const core::Filesystem::Path & file)
{
    ScopedLock lock(stateMutex);

    auto extIT = registeredExtensions.find(file.extension().string());

    if(extIT == registeredExtensions.end()){
        throw std::runtime_error("Trying to add unsupported file. Extension not registered by any parser");
    }

    auto fileIT = parsersByFiles.find(file);    
    if(fileIT != parsersByFiles.end()){
        throw std::runtime_error("Trying to add file that already is managed by DataManager");
    }

    Parsers parsers;
    Objects totalObjects;

    //jeœli pliku nie ma dodaj go, stwórz parsery i rozszerz dostêpne dane wraz z ich opisem
    for(auto parserIT = extIT->second.parsers.begin(); parserIT != extIT->second.parsers.end(); parserIT++){
        //twworzymy w³asne opakowanie parsera klienckiego
        ParserPtr parser(new Parser((*parserIT)->create(), file));
        Objects objects;
        //pobieramy udostepniane obiekty
        parser->getObjects(objects);

        Objects verifiedObjects;

        //zarejestrowanie obiektów i ich zwi¹zku z parserem i typami danych
        for(auto objectIT = objects.begin(); objectIT != objects.end(); objectIT++){

            if(*objectIT == nullptr){

                LOG_DEBUG("Unitialized object " << parser->getPath() << " for parser ID " << parser->getParser()->getID());

            }else{

                if((*objectIT)->getSource().empty() == true){
                    LOG_DEBUG("Parser ID " << parser->getParser()->getID() << " not initialized properly source for type " << (*objectIT)->getTypeInfo().name() << " while parsing file " << parser->getPath() << " Setting source to file path");
                    (*objectIT)->setSource(file.string());
                }

                //ObjectByParsers
                verifiedObjects.insert(*objectIT);
                //ParsersByObjects
                parsersByObjects[*objectIT] = parser;
                
                addData(*objectIT, core::DataInitializerPtr(new ParserInitializer(parser, this)));

                //UWAGA!!
                //mapowanie surowego wskaŸnika do ObjectWrappera jest robione podczas parsowania!!
                //teraz mamy leniw¹ inicjalizacjê
            }
        }

        if(verifiedObjects.empty() == false){
            //pomocnicza zmienna - ¿ebyt nie czytaæ mapy za ka¿dym razem dla tego samego argumentu
            objectsByParsers[parser].insert(verifiedObjects.begin(), verifiedObjects.end());
            //kojarzymy nowy parser z plikiem
            parsers.insert(parser);

            totalObjects.insert(verifiedObjects.begin(), verifiedObjects.end());
        }
    }

    if(parsers.empty() == true){
        LOG_DEBUG("Any of known parsers did not provide proper object wrappers for file: " << file);
    }else{
        parsersByFiles.insert(ParsersByFiles::value_type(file, parsers));
        LOG_INFO("File: " << file << " sussesfully loaded to DataManager");
        
        this->core::IFileDataManager::notify();
    }
}

void DataManager::removeData(const core::Filesystem::Path & file)
{
    ScopedLock lock(stateMutex);

    auto fileIT = parsersByFiles.find(file);    
    if(fileIT != parsersByFiles.end()){
        throw std::runtime_error("Trying to remove file that is not managed by DataManager");
    }

    Objects totalObjects;

    //zwalniamy zasoby parserów ¿eby potem zwolniæ same parsery
    for(auto parserIT = fileIT->second.begin(); parserIT != fileIT->second.end(); parserIT++){
        auto objectsByParserIT = objectsByParsers.find(*parserIT);
        
        totalObjects.insert(objectsByParserIT->second.begin(), objectsByParserIT->second.end());

        for(auto objectIT = objectsByParserIT->second.begin(); objectIT != objectsByParserIT->second.end(); objectIT++){

            removeData(*objectIT);

            parsersByObjects.erase(*objectIT);
        }

        //usuwamy ten wpis
        objectsByParsers.erase(objectsByParserIT);
    }

    //usuwamy ten wpis
    parsersByFiles.erase(fileIT);

   this->core::IFileDataManager::notify();
}

void DataManager::initializeData(const core::Filesystem::Path & file)
{
    ScopedLock lock(stateMutex);

    Objects toInitialize;
    
    getObjectsForData(file, toInitialize);

    Objects invalid;

    for(auto objectIT = toInitialize.begin(); objectIT != toInitialize.end(); objectIT++){
        initializeData(core::const_pointer_cast<ObjectWrapper>(*objectIT));

        if((*objectIT)->isNull()){
            //nie powiod³a siê inicjalizacja - albo parser rzucil bledem podczas parsowania, albo w danych nie bylo tego czego szukamy i co deklarowal parser
            invalid.insert(*objectIT);
            LOG_DEBUG("Failed to initialize data type " << (*objectIT)->getTypeInfo().name() << " for file " << file);
        }
    }

    // usuniêcie niew³aœciwych wpisów
    if ( invalid.empty() == false ) {
        LOG_DEBUG("Removing " << invalid.size() << " null or untrustfull objects after parsing " << file);
        for(auto it = invalid.begin(); it != invalid.end(); it++){
            
            auto parserIT = parsersByObjects.find(*it);
            objectsByParsers[parserIT->second].erase(*it);
            parsersByObjects.erase(parserIT);        

            removeData(*it);
        }
    }
}

void DataManager::deinitializeData(const core::Filesystem::Path & file)
{
    ScopedLock lock(stateMutex);

    Objects toDeinitialize;
    getObjectsForData(file, toDeinitialize);

    Parsers parsersQueue;

    Objects invalid;

    for(auto objectIT = toDeinitialize.begin(); objectIT != toDeinitialize.end(); objectIT++){
        deinitializeData(core::const_pointer_cast<ObjectWrapper>(*objectIT));
    }
}

void DataManager::getObjectsForData(const core::Filesystem::Path & file, Objects & objects) const
{
    ScopedLock lock(stateMutex);

    auto fileIT = parsersByFiles.find(file);    
    if(fileIT == parsersByFiles.end()){
        throw std::runtime_error("Trying to get object for file that is not managed by DataManager");
    }

    for(auto parserIT = fileIT->second.begin(); parserIT != fileIT->second.end(); parserIT++){
        auto it = objectsByParsers.find(*parserIT);
        objects.insert(it->second.begin(), it->second.end());
    }
}

const DataManager::Extensions & DataManager::getSupportedFilesExtensions() const
{
    return extensions;
}

const IFileDataManager::ExtensionDescription & DataManager::getExtensionDescription(const std::string & extension) const
{
    auto it = registeredExtensions.find(extension);
    if(it == registeredExtensions.end()){
        throw std::runtime_error("Request for description of unsupported/unregistered extension");
    }

    return it->second;
}

const core::Types & DataManager::getSupportedTypes() const
{
    return registeredTypes;
}

const core::Types & DataManager::getTypeBaseTypes(const core::TypeInfo & type) const
{
    auto it = typesHierarchy.find(type);
    if(it == typesHierarchy.end()){
        throw std::runtime_error("Request for description of unsupported type");
    }

    return it->second.first;
}

const core::Types & DataManager::getTypeDerrivedTypes(const core::TypeInfo & type) const
{
    auto it = typesHierarchy.find(type);
    if(it == typesHierarchy.end()){
        throw std::runtime_error("Request for description of unsupported type");
    }

    return it->second.second;
}

void DataManager::getObjects(core::ObjectWrapperCollection& objects)
{
    ScopedLock lock(stateMutex);

    core::Objects ob;
    getObjects(ob, objects.getTypeInfo(), objects.exactTypes());
    objects.loadCollectionWithData(ob.begin(), ob.end());
}

void DataManager::getObjects( core::Objects& objects, const core::TypeInfo& type, bool exact /*= false*/ )
{
    ScopedLock lock(stateMutex);

    if(registeredTypes.find(type) == registeredTypes.end()){
        std::stringstream str;
        str << "Request for unsupported data type " << type.name();
        throw std::runtime_error(str.str());
    }

    core::Objects invalid;
    core::Types types;
    types.insert(type);

    if(exact == false){
        auto & derrived = getTypeDerrivedTypes(type);
        types.insert(derrived.begin(), derrived.end());
    }

    for(auto typeIT = types.begin(); typeIT != types.end(); typeIT++){

        auto typeObjectsIT = objectsByTypes.find(*typeIT);

        if(typeObjectsIT == objectsByTypes.end()){
            //nie mamy jeszcze zadnego obiektu tego typu wiec idziemyu dalej
            continue;
        }

        for(auto objectIT = typeObjectsIT->second.begin(); objectIT != typeObjectsIT->second.end(); objectIT++){
            
            initializeData(core::const_pointer_cast<ObjectWrapper>(*objectIT));

            if((*objectIT)->isNull() == true){
                //jesli nadal nie udalo sie zainicjalizowac danych to trzeba je usunac
                invalid.insert(*objectIT);
            }else{
                objects.insert(*objectIT);
            }
        }
    }

    if ( invalid.empty() == false ) {
        LOG_DEBUG("Removing " << invalid.size() << " null or untrustfull objects after data request of type " << type.name());
        for(auto it = invalid.begin(); it != invalid.end(); it++){
            
            removeData(*it);

            //sprawdz czy dane pochodza z pliku, jesli tak to aktualizuj info o parserach

            /*auto parserIT = parsersByObjects.find(*it);

            if(parserIT != parsersByObjects.end()){
                objectsByParsers[parserIT->second].erase(*it);
                parsersByObjects.erase(parserIT);
            }*/
        }
    }
}

void DataManager::registerObjectFactory( const core::IObjectWrapperFactoryPtr & factory )
{
	core::TypeInfo type = factory->getType();
    auto it = registeredTypes.find(type);

	if ( it != registeredTypes.end() ) {
		LOG_ERROR("Factory for " << type.name() << " already exists.");
	}else{
		//tworzymy prototyp by miec dostep do informacji o wspieranych typach
		core::ObjectWrapperConstPtr proto(factory->createWrapper());

        //rejestrujemy typ i jego prototyp
        registeredTypesPrototypes.insert(std::make_pair(type, proto));

        //aktualizujemy hierarchiê typów
        registeredTypes.insert(type);

        //hierarchia typow
        core::TypeInfoList types;
        proto->getSupportedTypes(types);

        types.remove(type);

        typesHierarchy[type].first.insert(types.begin(), types.end());

        for(auto typeIT = types.begin(); typeIT != types.end(); typeIT++){
            typesHierarchy[*typeIT].second.insert(type);
        }
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
