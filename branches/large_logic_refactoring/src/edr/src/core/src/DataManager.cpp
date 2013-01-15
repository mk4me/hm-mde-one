#include "CorePCH.h"
#include "DataManager.h"

#include "Log.h"
#include <core/Filesystem.h>
#include <boost/algorithm/string.hpp>
#include <algorithm>
#include <utils/Push.h>
#include "ServiceManager.h"

using namespace core;
using namespace std;

//! Wewnętrzna reprezentacja parsera używana przez DataManagera.
class DataManager::Parser
{
private:
    //! Prawdziwy wewnętrzny parser.
    const IParserPtr parser;
    //! Parsowany plik.
    const Filesystem::Path filePath;
    //! Czy przeparsowano plik?
    bool parsed;
    //! Czy użyto parsera do przeparsowania?
    bool used;
    //! Czy właśnie parsujemy?
    bool parsing;

public:
    //! \param parser Faktyczny parser. To ten obiekt kontroluje jego
    //!     czas życia.
    //! \param resource Czy parser jest związany z zasobami stałymi?
    Parser(IParser* parser, const Filesystem::Path& path) :
      parser(parser), parsed(false), used(false), filePath(path), parsing(false)
      {
          UTILS_ASSERT(parser);
          UTILS_ASSERT(!filePath.empty());
      }
      //! Destruktor drukujący wiadomość o wyładowaniu pliku.
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
    //! \return Czy użyto tego parsera?
    inline bool isUsed() const
    {
        return used;
    }
    //! \return Czy udało się przeparsować plik?
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

    //! \return Ścieżka do pliku.
    inline const Filesystem::Path& getPath() const
    {
        return filePath;
    }
    //! \return
    inline IParserPtr getParser() const
    {
        return parser;
    }

    //! Może rzucać wyjątkami!
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
    //! Nie rzuca wyjątkami.
    //! \return Czy udało się przeparsować?
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
    //!         bądź nie.
    inline void getObjects(core::Objects& objects)
    {
        parser->getObjects(objects);
    }
};

void DataManager::initializeDataWithParser(core::ObjectWrapper & object, const ParserPtr & parser)
{
	if(parser->isUsed() == true){
		parser->reset();
	}

    if(parser->tryParse() == true){
        LOG_DEBUG("Parser ID " << parser->getParser()->getID() << " successfully initialized object of type " << object.getTypeInfo().name() << " parsing file " << parser->getPath());
		core::Objects objects;
		parser->getObjects(objects);

		for(auto it = objects.begin(); it != objects.end(); ++it){
			if((*it)->isNull() == true){
				LOG_DEBUG("Data type initialization failed: " << (*it)->getTypeInfo().name() << " using Parser ID " << parser->getParser()->getID() << " for file " << parser->getPath());
			}
		}

    }else if(object.isNull() == true){
        LOG_DEBUG("Parser ID " << parser->getParser()->getID() << " failed to initialized object of type " << object.getTypeInfo().name() << " parsing file " << parser->getPath());
    }
}

template <>
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
        // uzupełnienie słownika rozszerzeń
        core::IParser::Extensions extensions;
        // pobranie słownika rozszerzeń z parsera
        parser->getSupportedExtensions(extensions);

        if(extensions.empty() == true){
            std::stringstream str;
            str << "Parser ID: "<< parser->getID() << " Trying to register parser with 0 supported extensions";
            throw std::runtime_error(str.str());
        }

        //weryfikacja rozszerzeń
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
            //właściwe rozszerzenie
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
            //prubujemy aktualizaować opisy tego rozszerzenia
            if(it->second.description.empty() == false){
                extIT->second.descriptions.push_back(it->second.description);
            }
            //aktualizujemy typy tego rozszerzenia
            extIT->second.types.insert(it->second.types.begin(), it->second.types.end());

            LOG_INFO("Parser for extension " << extension << " registered. Parser ID: " << parser->getID());
        }
        // uzupełnienie pozostałych kolekcji
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

void DataManager::nonNotifyAddData(const core::ObjectWrapperPtr & data, const core::ObjectWrapper::LazyInitializer & initializer)
{
    ScopedLock lock(stateMutex);

    auto it = objects.find(data);

    if(it != objects.end()){
        throw std::runtime_error("Trying to add data already managed by DataManager");
    }

    if(data->isNull() == false || initializer.empty() == false){

		//dodaj do grupowania po typach
		core::TypeInfoList types;
		data->getSupportedTypes(types);

        objects.insert(data);

        for(auto it = types.begin(); it != types.end(); ++it){
            objectsByTypes[*it].insert(data);
        }

    }else{
        throw std::runtime_error("Attempting to add empty data without initializer");
    }
}

void DataManager::removeDataImpl(const core::ObjectWrapperPtr & data)
{
	objects.erase(data);

	//usun z grupowania po typach
	core::TypeInfoList types;
	data->getSupportedTypes(types);

	for(auto it = types.begin(); it != types.end(); ++it){
		auto IT = objectsByTypes.find(*it);
		IT->second.erase(data);
		if(IT->second.empty() == true){
			objectsByTypes.erase(IT);
		}
	}
}


void DataManager::nonNotifyRemoveData(const core::ObjectWrapperPtr & data)
{
    ScopedLock lock(stateMutex);

    auto it = objects.find(data);

    if(it == objects.end()){
        throw std::runtime_error("Trying to remove data not managed by DataManager");
    }

    removeDataImpl(data);
}

void DataManager::getManagedFiles(core::Files & files) const
{
    ScopedLock lock(stateMutex);

    for(auto it = parsersByFiles.begin(); it != parsersByFiles.end(); ++it){
        files.insert(it->first);
    }
}

const bool DataManager::isFileManaged(const core::Filesystem::Path & file) const
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

    //jeśli pliku nie ma dodaj go, stwórz parsery i rozszerz dostępne dane wraz z ich opisem
    for(auto parserIT = extIT->second.parsers.begin(); parserIT != extIT->second.parsers.end(); ++parserIT){
        //twworzymy własne opakowanie parsera klienckiego
        ParserPtr parser(new Parser((*parserIT)->create(), file));
        Objects objects;
        //pobieramy udostępniane obiekty
        parser->getObjects(objects);

        Objects verifiedObjects;

        //zarejestrowanie obiektów i ich związku z parserem i typami danych
        for(auto objectIT = objects.begin(); objectIT != objects.end(); ++objectIT){

            if(*objectIT == nullptr){

                LOG_DEBUG("Unitialized object " << parser->getPath() << " for parser ID " << parser->getParser()->getID());

            }else{

				//TODO
				//dodac inicjalizator, jesli jest to warning!!

                //ObjectByParsers
                verifiedObjects.insert(*objectIT);
                //ParsersByObjects
                parsersByObjects[*objectIT] = parser;

                nonNotifyAddData(*objectIT, boost::bind(&DataManager::initializeDataWithParser, this, _1, parser));

                //UWAGA!!
                //mapowanie surowego wskaźnika do ObjectWrappera jest robione podczas parsowania!!
                //teraz mamy leniwą inicjalizację
            }
        }

        if(verifiedObjects.empty() == false){
            //pomocnicza zmienna - żebyt nie czytać mapy za każdym razem dla tego samego argumentu
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

    //zwalniamy zasoby parserów żeby potem zwolnić same parsery
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

void DataManager::getObjects(core::ObjectWrapperCollection& objects)
{
    ScopedLock lock(stateMutex);
    std::vector<core::ObjectWrapperConstPtr> ob;
    getObjects(ob, objects.getTypeInfo(), objects.exactTypes());
    objects.nonCheckInsert(objects.end(), ob.begin(), ob.end());
}

void DataManager::getObjects( std::vector<core::ObjectWrapperConstPtr>& objects, const core::TypeInfo& type, bool exact /*= false*/)
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

			if(wrapper->isNull() == false){
				objects.push_back(wrapper);
			}
        }
    }

	//TODO
	//move to initializer
	/*if ( invalid.empty() == false ) {

	NotifyBlocker<core::IMemoryDataManager> blocker(*this);

	LOG_DEBUG("Removing " << invalid.size() << " null or untrustfull objects after data request of type " << type.name());
	for(auto it = invalid.begin(); it != invalid.end(); ++it){

	removeData(*it);
	}
	}*/
}

void DataManager::prepareExtension(std::string & extension)
{
    if(extension.empty() == false && extension.front() != '.'){
        extension.insert(0, ".");
        boost::to_lower(extension);
    }
}

void DataManager::registerObjectWrapperPrototype( const core::ObjectWrapperConstPtr & prototype )
{
	ScopedLock lock(stateMutex);

    core::TypeInfo type = prototype->getTypeInfo();
    auto it = registeredTypes.find(type);

    if ( it != registeredTypes.end() ) {
        LOG_ERROR("Prototype for " << type.name() << " already exists.");
    }else{

        //rejestrujemy typ i jego prototyp
        registeredTypesPrototypes.insert(std::make_pair(type, prototype));

        //aktualizujemy hierarchię typów
        registeredTypes.insert(type);

        //hierarchia typów
        core::TypeInfoList types;
        prototype->getSupportedTypes(types);

        types.remove(type);

        typesHierarchy[type].first.insert(types.begin(), types.end());

        for(auto typeIT = types.begin(); typeIT != types.end(); ++typeIT){
            typesHierarchy[*typeIT].second.insert(type);
        }
    }
}

core::ObjectWrapperCollectionPtr DataManager::createWrapperCollection(const core::TypeInfo& typeInfo)
{
    auto found = registeredTypesPrototypes.find(typeInfo);
    if ( found != registeredTypesPrototypes.end() ) {
        //return ObjectWrapperCollectionPtr(found->second->createWrapperCollection());
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
