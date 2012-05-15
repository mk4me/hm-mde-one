#include "CorePCH.h"
#include "DataManager.h"

#include "Log.h"
#include <core/Filesystem.h>
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <algorithm>
#include <list>
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

DataManager::DMObjectWrapper::DMObjectWrapper(const core::ObjectWrapperPtr & wrapper) : wrapper(wrapper)
{
    UTILS_ASSERT(wrapper != nullptr);
}

DataManager::DMObjectWrapper::~DMObjectWrapper()
{

}

const void* DataManager::DMObjectWrapper::getRawPtr() const
{
    return wrapper->getRawPtr();
}

void* DataManager::DMObjectWrapper::getRawPtr()
{
    return wrapper->getRawPtr();
}

void DataManager::DMObjectWrapper::reset()
{
    wrapper->reset();
}

std::size_t DataManager::DMObjectWrapper::getClassID() const
{
    return wrapper->getClassID();
}

const std::string& DataManager::DMObjectWrapper::getClassName() const
{
    return wrapper->getClassName();
}

const std::string& DataManager::DMObjectWrapper::getName() const
{
    return wrapper->getName();
}

void DataManager::DMObjectWrapper::setName(const std::string& name)
{
    return wrapper->setName(name);
}

const std::string& DataManager::DMObjectWrapper::getSource() const
{
    return wrapper->getSource();
}

void DataManager::DMObjectWrapper::setSource(const std::string& source)
{
    wrapper->setSource(source);
}

bool DataManager::DMObjectWrapper::isChanged() const
{
    return wrapper->isChanged();
}

void DataManager::DMObjectWrapper::setChanged(bool changed)
{
    wrapper->setChanged(changed);
}

bool DataManager::DMObjectWrapper::isSupported(const core::TypeInfo& type) const
{
    return wrapper->isSupported(type);
}

bool DataManager::DMObjectWrapper::isPtrSupported(const core::TypeInfo& type) const
{
    return wrapper->isPtrSupported(type);
}

core::TypeInfo DataManager::DMObjectWrapper::getTypeInfo() const
{
    return wrapper->getTypeInfo();
}

std::pair<core::TypeInfo, core::TypeInfo> DataManager::DMObjectWrapper::getPtrTypeInfo() const
{
    return wrapper->getPtrTypeInfo();
}

void DataManager::DMObjectWrapper::getSupportedTypes(Types& supported) const
{
    wrapper->getSupportedTypes(supported);
}

bool DataManager::DMObjectWrapper::isNull() const
{
    return wrapper->isNull();
}

core::ObjectWrapperPtr DataManager::DMObjectWrapper::clone() const
{
    return core::ObjectWrapperPtr(new DMObjectWrapper(wrapper->clone())); 
}

core::ObjectWrapperPtr DataManager::DMObjectWrapper::create() const {
    return ObjectWrapperPtr(new DMObjectWrapper(wrapper->create()));
}

bool DataManager::DMObjectWrapper::setSmartPtr(const void* ptr, const core::TypeInfo& type)
{
    return core::IObjectWrapper::IObjectWrapperHelper::setSmartPtr(wrapper, ptr,type);
    //return wrapper->setSmartPtr(ptr,type);
}

bool DataManager::DMObjectWrapper::getSmartPtr(void* ptr, const core::TypeInfo& type) const
{
    if(wrapper->getRawPtr() == nullptr){
        try{
            DataManager::getInstance()->initializeData(core::ObjectWrapperPtr(core::const_pointer_cast<DMObjectWrapper>(shared_from_this())));
        }
        catch(std::exception & ){

        }
    }

    return core::IObjectWrapper::IObjectWrapperHelper::getSmartPtr(wrapper, ptr, type);
}

DataManager::ParserInitializer::ParserInitializer(const ParserPtr & parser) : parser(parser)
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
    auto it = DataManager::getInstance()->objectsByParsers.find(parser);
    if(it != DataManager::getInstance()->objectsByParsers.end() ){
        for(auto objectIT = it->second.begin(); objectIT != it->second.end(); ++objectIT){
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
	registerObjectFactory(IObjectWrapperFactoryPtr(new core::ObjectWrapperFactory<Metadata>()));
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
        for(auto it = extensions.begin(); it != extensions.end(); ++it){

            if(it->first.empty() == true){
                std::stringstream str;
                str << "Parser ID: "<< parser->getID() << " Trying to register extension of length 0";
                throw std::runtime_error(str.str());
            }

            std::string ext(it->first);
            prepareExtension(ext);

            if(it->second.types.empty() == true){
                std::stringstream str;
                str << "Trying to register extension " << ext << " without supported types for parser " << parser->getID();
                throw std::runtime_error(str.str());                
            }
        }

        for(auto it = extensions.begin(); it != extensions.end(); ++it){
            //w³aœciwe rozszerzenie
            std::string extension(it->first);

            prepareExtension(extension);

            auto extIT = registeredExtensions.find(extension);

            if(extIT != registeredExtensions.end()) {
                LOG_WARNING("There is at least one parser that supports extension " << extension);                
            }else{
                //tworzymy nowy wpis dla nowego rozszerzenia
                extIT = registeredExtensions.insert(SupportedExtensionsPersistenceData::value_type(extension, ExtendedExtensionDescription())).first;
                this->extensions.insert(extension);
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

//core::ObjectWrapperPtr DataManager::getWrapper(void * rawPtr) const
//{
//    ScopedLock lock(stateMutex);
//
//    auto it = rawPointerToObjectWrapper.find(rawPtr);
//    if(it != rawPointerToObjectWrapper.end()){
//        return it->second;
//    }
//
//    return ObjectWrapperPtr();
//}

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

const core::ObjectWrapperPtr & DataManager::getObjectWrapperForRawPtr(const void * ptr) const
{
    ScopedLock lock(stateMutex);

    auto it = rawPointerToObjectWrapper.find(ptr);

    if(it == rawPointerToObjectWrapper.end()){
        throw std::runtime_error("Object not managed by DataManager");
    }

    return it->second;
}

bool DataManager::objectIsManaged(const void * ptr) const
{
    ScopedLock lock(stateMutex);
    return rawPointerToObjectWrapper.find(ptr) != rawPointerToObjectWrapper.end();
}

void DataManager::nonNotifyAddData(const core::ObjectWrapperPtr & data, const core::DataInitializerPtr & initializer)
{
    ScopedLock lock(stateMutex);

    auto it = objects.find(data);

    if(it != objects.end()){
        throw std::runtime_error("Trying to add data already managed by DataManager");
    }

    if(data->isNull() == false || initializer != nullptr){
        //przepakowujemy do naszego wrappera

		//dodaj do grupowania po typach
		core::TypeInfoList types;
		data->getSupportedTypes(types);

		auto it = std::find(types.begin(), types.end(), typeid(Metadata));
		if(it != types.end()){
			MetadataPtr meta = data->get(false);
			auto obj = meta->object().lock();
			if(obj == nullptr || objects.find(obj) == objects.end()){
				throw std::runtime_error("Trying to add metadata for uninitialized object or object not managed by DataManager");
			}

			metadataByObjects[obj].push_back(data);
		}
		
        objects.insert(data);
		
        for(auto it = types.begin(); it != types.end(); ++it){
            objectsByTypes[*it].insert(data);
        }

        if(initializer != nullptr){
            objectsWithInitializers[data] = initializer;
        }

		if(data->isNull() == false){
			rawPointerToObjectWrapper[data->getRawPtr()] = data;
		}
    }else{
        throw std::runtime_error("Attempting to add empty data without initializer");
    }
}


void DataManager::nonNotifyRemoveData(const core::ObjectWrapperPtr & data)
{
    ScopedLock lock(stateMutex);

    auto it = objects.find(data);

    if(it == objects.end()){
        throw std::runtime_error("Trying to remove data not managed by DataManager");
    }

    objects.erase(data);
	metadataByObjects.erase(data);

	//usun z grupowania po typach
	core::TypeInfoList types;
	data->getSupportedTypes(types);

	auto mIT = std::find(types.begin(), types.end(), typeid(Metadata));
	if(mIT != types.end()){
		MetadataPtr meta = data->get(false);
		auto obj = meta->object().lock();
		if(obj != nullptr){
			auto & m = metadataByObjects[obj];
			std::remove(m.begin(), m.end(), data);
		}
	} 

    for(auto it = types.begin(); it != types.end(); ++it){
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
}

void DataManager::getManagedFiles(core::Files & files) const
{
    ScopedLock lock(stateMutex);

    for(auto it = parsersByFiles.begin(); it != parsersByFiles.end(); ++it){
        files.insert(it->first);
    }
}

void DataManager::notify(const core::IFileDataManager * dm)
{
	core::IMemoryDataManager::notify();
	core::IFileDataManager::notify();
}

bool DataManager::isFileManaged(const core::Filesystem::Path & file) const
{
    ScopedLock lock(stateMutex);
    return parsersByFiles.find(file) != parsersByFiles.end();
}

void DataManager::nonNotifyAddFile(const core::Filesystem::Path & file, std::vector<ObjectWrapperPtr> & objects)
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
    for(auto parserIT = extIT->second.parsers.begin(); parserIT != extIT->second.parsers.end(); ++parserIT){
        //twworzymy w³asne opakowanie parsera klienckiego
        ParserPtr parser(new Parser((*parserIT)->create(), file));
        Objects objects;
        //pobieramy udostepniane obiekty
        parser->getObjects(objects);

        Objects verifiedObjects;

        //zarejestrowanie obiektów i ich zwi¹zku z parserem i typami danych
        for(auto objectIT = objects.begin(); objectIT != objects.end(); ++objectIT){

            if(*objectIT == nullptr){

                LOG_DEBUG("Unitialized object " << parser->getPath() << " for parser ID " << parser->getParser()->getID());

            }else{

                if((*objectIT)->getSource().empty() == true){
                    //LOG_DEBUG("Parser ID " << parser->getParser()->getID() << " not initialized properly source for type " << (*objectIT)->getTypeInfo().name() << " while parsing file " << parser->getPath() << " Setting source to file path");
                    (*objectIT)->setSource(file.string());
                }

                if((*objectIT)->getName().empty() == true){
                    //LOG_DEBUG("Parser ID " << parser->getParser()->getID() << " not initialized properly name for type " << (*objectIT)->getTypeInfo().name() << " while parsing file " << parser->getPath() << " Setting source to file name");
                    (*objectIT)->setName(file.filename().string());
                }                

                ObjectWrapperPtr obj(new DMObjectWrapper(*objectIT));

                //ObjectByParsers
                verifiedObjects.insert(obj);
                //ParsersByObjects
                parsersByObjects[obj] = parser;
                
                nonNotifyAddData(obj, core::DataInitializerPtr(new ParserInitializer(parser)));

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

        for(auto it = totalObjects.begin(); it != totalObjects.end(); ++it){
            objects.push_back(*it);
        }
        parsersByFiles.insert(ParsersByFiles::value_type(file, parsers));
        LOG_INFO("File: " << file << " sussesfully loaded to DataManager");
    }
}

void DataManager::nonNotifyRemoveFile(const core::Filesystem::Path & file)
{
    ScopedLock lock(stateMutex);

    auto fileIT = parsersByFiles.find(file);    
    if(fileIT == parsersByFiles.end()){
        throw std::runtime_error("Trying to remove file that is not managed by DataManager");
    }

    Objects totalObjects;

    //zwalniamy zasoby parserów ¿eby potem zwolniæ same parsery
    for(auto parserIT = fileIT->second.begin(); parserIT != fileIT->second.end(); ++parserIT){
        auto objectsByParserIT = objectsByParsers.find(*parserIT);
        
        totalObjects.insert(objectsByParserIT->second.begin(), objectsByParserIT->second.end());

        for(auto objectIT = objectsByParserIT->second.begin(); objectIT != objectsByParserIT->second.end(); ++objectIT){

            nonNotifyRemoveData(*objectIT);

            parsersByObjects.erase(*objectIT);
        }

        //usuwamy ten wpis
        objectsByParsers.erase(objectsByParserIT);
    }

    //usuwamy ten wpis
    parsersByFiles.erase(fileIT);
}

void DataManager::initializeFile(const core::Filesystem::Path & file)
{
    ScopedLock lock(stateMutex);

    std::vector<core::ObjectWrapperPtr> toInitialize;
    
    getObjectsForFile(file, toInitialize);

    Objects invalid;

    for(auto objectIT = toInitialize.begin(); objectIT != toInitialize.end(); ++objectIT){
        initializeData(*objectIT);

        if((*objectIT)->isNull()){
            //nie powiod³a siê inicjalizacja - albo parser rzucil bledem podczas parsowania, albo w danych nie bylo tego czego szukamy i co deklarowal parser
            invalid.insert(*objectIT);
            LOG_DEBUG("Failed to initialize data type " << (*objectIT)->getTypeInfo().name() << " for file " << file);
        }
    }

    // usuniêcie niew³aœciwych wpisów
    if ( invalid.empty() == false ) {
        LOG_DEBUG("Removing " << invalid.size() << " null or untrustfull objects after parsing " << file);
        for(auto it = invalid.begin(); it != invalid.end(); ++it){
            
            /*auto parserIT = parsersByObjects.find(*it);
            objectsByParsers[parserIT->second].erase(*it);
            parsersByObjects.erase(parserIT);        */

            removeData(*it);
        }
    }
}

void DataManager::deinitializeFile(const core::Filesystem::Path & file)
{
    ScopedLock lock(stateMutex);

    std::vector<core::ObjectWrapperPtr> toDeinitialize;
    getObjectsForFile(file, toDeinitialize);

    for(auto objectIT = toDeinitialize.begin(); objectIT != toDeinitialize.end(); ++objectIT){
        deinitializeData(*objectIT);
    }
}

void DataManager::getObjectsForFile(const core::Filesystem::Path & file, std::vector<core::ObjectWrapperPtr> & objects) const
{
    ScopedLock lock(stateMutex);

    auto fileIT = parsersByFiles.find(file);    
    if(fileIT == parsersByFiles.end()){
        throw std::runtime_error("Trying to get object for file that is not managed by DataManager");
    }

    for(auto parserIT = fileIT->second.begin(); parserIT != fileIT->second.end(); ++parserIT){
        auto it = objectsByParsers.find(*parserIT);
        objects.insert(objects.end(), it->second.begin(), it->second.end());
    }
}

const DataManager::Extensions & DataManager::getSupportedFilesExtensions() const
{
	ScopedLock lock(stateMutex);
    return extensions;
}

bool DataManager::isExtensionSupported(const std::string & extension) const
{
	ScopedLock lock(stateMutex);
    std::string ext(extension);

    prepareExtension(ext);

    return extensions.find(ext) != extensions.end();
}

const IFileDataManager::ExtensionDescription & DataManager::getExtensionDescription(const std::string & extension) const
{
	ScopedLock lock(stateMutex);
    std::string ext(extension);

    prepareExtension(ext);

    auto it = registeredExtensions.find(ext);
    if(it == registeredExtensions.end()){
        std::stringstream str;
        str << "Request for description of unsupported/unregistered extension " << ext;
        throw std::runtime_error(str.str());
    }

    return it->second;
}

const core::Types & DataManager::getSupportedTypes() const
{
	ScopedLock lock(stateMutex);
    return registeredTypes;
}

const core::Types & DataManager::getTypeBaseTypes(const core::TypeInfo & type) const
{
	ScopedLock lock(stateMutex);
    auto it = typesHierarchy.find(type);
    if(it == typesHierarchy.end()){
        throw std::runtime_error("Request for description of unsupported type");
    }

    return it->second.first;
}

const core::Types & DataManager::getTypeDerrivedTypes(const core::TypeInfo & type) const
{
	ScopedLock lock(stateMutex);
    auto it = typesHierarchy.find(type);
    if(it == typesHierarchy.end()){
        throw std::runtime_error("Request for description of unsupported type");
    }

    return it->second.second;
}

core::ObjectWrapperPtr DataManager::createObjectWrapper(const TypeInfo & typeInfo) const
{
    auto it = registeredTypesPrototypes.find(typeInfo);
    if(it == registeredTypesPrototypes.end()){
        throw std::runtime_error("Object Wrapper request for undefined data");
    }

    return core::ObjectWrapperPtr(new DMObjectWrapper(it->second->create()));
}

void DataManager::getObjects(core::ObjectWrapperCollection& objects)
{
    ScopedLock lock(stateMutex);
    std::vector<core::ObjectWrapperConstPtr> ob;
    getObjects(ob, objects.getTypeInfo(), objects.exactTypes());
    objects.loadCollectionWithData(ob.begin(), ob.end());
}

void DataManager::getObjects( std::vector<core::ObjectWrapperConstPtr>& objects, const core::TypeInfo& type, bool exact /*= false*/ )
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

    for(auto typeIT = types.begin(); typeIT != types.end(); ++typeIT){

        auto typeObjectsIT = objectsByTypes.find(*typeIT);

        if(typeObjectsIT == objectsByTypes.end()){
            //nie mamy jeszcze zadnego obiektu tego typu wiec idziemyu dalej
            continue;
        }

        for(auto objectIT = typeObjectsIT->second.begin(); objectIT != typeObjectsIT->second.end(); ++objectIT){
            
            core::ObjectWrapperPtr wrapper(*objectIT);

            initializeData(wrapper);

            if(wrapper->isNull() == true){
                //jesli nadal nie udalo sie zainicjalizowac danych to trzeba je usunac
                invalid.insert(wrapper);
            }else{
                objects.push_back(wrapper);
            }
        }
    }

    if ( invalid.empty() == false ) {
        LOG_DEBUG("Removing " << invalid.size() << " null or untrustfull objects after data request of type " << type.name());
        for(auto it = invalid.begin(); it != invalid.end(); ++it){
            
            removeData(*it);
        }
    }
}

bool DataManager::isManaged(const core::ObjectWrapperConstPtr & object) const
{
	ScopedLock lock(stateMutex);
	return objects.find(core::const_pointer_cast<core::IObjectWrapper>(object)) != objects.end();
}

void DataManager::prepareExtension(std::string & extension)
{
    if(extension.empty() == false && extension.front() != '.'){
        extension.insert(0, ".");
        boost::to_lower(extension);
    }
}

void DataManager::registerObjectFactory( const core::IObjectWrapperFactoryPtr & factory )
{
	ScopedLock lock(stateMutex);

    core::TypeInfo type = factory->getType();
    auto it = registeredTypes.find(type);

    if ( it != registeredTypes.end() ) {
        LOG_ERROR("Factory for " << type.name() << " already exists.");
    }else{

        //zapamiêtyjemy fabrykê
        objectFactories[type] = factory;

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

        for(auto typeIT = types.begin(); typeIT != types.end(); ++typeIT){
            typesHierarchy[*typeIT].second.insert(type);
        }
    }
}

//core::ObjectWrapperPtr DataManager::createWrapper( const core::TypeInfo& type )
//{
//	auto found = objectFactories.find(type);
//	if ( found != objectFactories.end() ) {
//		return ObjectWrapperPtr(found->second->createWrapper());
//	} else {
//		// TODO: elaborate
//		throw std::runtime_error("Type not supported.");
//	}
//}

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

bool DataManager::isTypeCompatible(const core::TypeInfo & sourceTypeInfo, const core::TypeInfo & destTypeInfo) const
{
	if(sourceTypeInfo == destTypeInfo){
		return true;
	}

	ScopedLock lock(stateMutex);

	bool ret = false;

	auto const & typesSet = getTypeBaseTypes(sourceTypeInfo);

	if(typesSet.find(destTypeInfo) != typesSet.end()){
		ret = true;
	}

	return ret;
}

void DataManager::getMetadataForObject(const core::ObjectWrapperConstPtr & object, std::vector<core::ObjectWrapperConstPtr> & metadataCollection) const
{
	ScopedLock lock(stateMutex);
	//auto obj = getObjectWrapperForRawPtr(object->getRawPtr());

	auto obj = boost::const_pointer_cast<core::IObjectWrapper>(object);

	auto it = objects.find(obj);

	if(it == objects.end()){
		throw std::runtime_error("Query about metadata of object not managed by DataManager");
	}

	auto metaIT = metadataByObjects.find(obj);

	if(metaIT != metadataByObjects.end()){
		for(auto oIT = metaIT->second.begin(); oIT != metaIT->second.end(); ++oIT){
			metadataCollection.push_back(*oIT);
		}
	}
}

void DataManager::getMetadataForType(const core::TypeInfo & typeInfo, std::vector<core::ObjectWrapperConstPtr> & metadataCollection, bool exact) const
{
	ScopedLock lock(stateMutex);

	if(registeredTypes.find(typeInfo) == registeredTypes.end()){
		std::stringstream str;
		str << "Request for unsupported data type " << typeInfo.name();
		throw std::runtime_error(str.str());
	}

	core::Objects objects;
	core::Types types;
	types.insert(typeInfo);

	if(exact == false){
		auto & derrived = getTypeDerrivedTypes(typeInfo);
		types.insert(derrived.begin(), derrived.end());
	}

	for(auto typeIT = types.begin(); typeIT != types.end(); ++typeIT){

		auto typeObjectsIT = objectsByTypes.find(*typeIT);

		if(typeObjectsIT == objectsByTypes.end()){
			//nie mamy jeszcze zadnego obiektu tego typu wiec idziemyu dalej
			continue;
		}

		objects.insert(typeObjectsIT->second.begin(), typeObjectsIT->second.end());
	}

	auto metaITEnd = metadataByObjects.end();
	for(auto it = objects.begin(); it != objects.end(); ++it){
		auto metaIT = metadataByObjects.find(*it);
		if(metaIT != metaITEnd){
			for(auto oIT = metaIT->second.begin(); oIT != metaIT->second.end(); ++oIT){
				metadataCollection.push_back(*oIT);
			}
		}
	}
}
