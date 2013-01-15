#include "CorePCH.h"
#include "FileDataManager.h"
#include "ApplicationCommon.h"
#include "ParserManager.h"

namespace core {

//! Wewnêtrzna reprezentacja parsera u¿ywana przez DataManagera.
class FileDataManager::Parser
{
private:
	//! Prawdziwy wewnêtrzny parser.
	const plugin::IParserPtr parser;
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
	Parser(plugin::IParser* parser, const Filesystem::Path& path) :
	  parser(parser), parsed(false), used(false), filePath(path)
	  {
		  UTILS_ASSERT(parser);
		  UTILS_ASSERT(!filePath.empty());
	  }
	  //! Destruktor drukuj¹cy wiadomoœæ o wy³adowaniu pliku.
	  ~Parser()
	  {
		  if ( isParsed() ) {
			  CORE_LOG_NAMED_DEBUG("parser", "Unloading parser for file: " << getPath() );
		  } else if ( isUsed() ) {
			  CORE_LOG_NAMED_DEBUG("parser", "Unloading invalid parser for file: " << getPath() );
		  } else {
			  CORE_LOG_NAMED_DEBUG("parser", "Unloading unused parser for file: " << getPath() );
		  }
	  }

public:
	//! \return Czy u¿yto tego parsera?
	inline const bool isUsed() const
	{
		return used;
	}
	//! \return Czy uda³o siê przeparsowaæ plik?
	inline const bool isParsed() const
	{
		return parsed;
	}

	inline void reset()
	{
		used = false;
		parsed = false;
	}

	//! \return Œcie¿ka do pliku.
	inline const Filesystem::Path& getPath() const
	{
		return filePath;
	}
	//! \return
	inline plugin::IParserPtr getParser() const
	{
		return parser;
	}

	//! Mo¿e rzucaæ wyj¹tkami!
	void parseFile()
	{
		UTILS_ASSERT(!isUsed());
		UTILS_ASSERT(!filePath.empty());
		CORE_LOG_NAMED_DEBUG("parser", "Parsing file: " << getPath() );
		used = true;
		//parser->parseFile(filePath.string());
		parsed = true;
	}
	//! Nie rzuca wyj¹tkami.
	//! \return Czy uda³o siê przeparsowaæ?
	const bool tryParse()
	{
		bool ret = true;
		try {
			parseFile();
		} catch (const std::exception& ex) {
			CORE_LOG_NAMED_ERROR("parser", "Error during parsing file " << getPath() << ": " << ex.what());
			ret = false;
		}

		return ret;
	}
	//! \param objects Lista wrappowanych obiektów, zainicjowanych (przeparsowany parser)
	//!         b¹dŸ nie.
	inline void getObjects(Objects& objects)
	{
		parser->getObjects(objects);
	}
};

ObjectWrapper::LazyInitializer FileDataManager::initStreamParser(const ParserPtr & streamParser)
{

}

ObjectWrapper::LazyInitializer FileDataManager::initSourceParser(const ParserPtr & sourceParser)
{

}

void FileDataManager::initializeDataWithStreamParser(ObjectWrapper & object, const ParserPtr & parser, plugin::IStreamParserCapabilities * streamCapabilities)
{

}

void FileDataManager::initializeDataWithSourceParser(ObjectWrapper & object, const ParserPtr & parser, plugin::ISourceParserCapabilities * sourceCapabilities)
{

}

void FileDataManager::initializeDataWithExternalInitializer(ObjectWrapper & object, const ObjectWrapper::LazyInitializer & li)
{

}

void FileDataManager::rawRemoveFile(const Filesystem::Path & file)
{
	objectsByFiles.erase(file);
}

void FileDataManager::initializeParsers(const plugin::IParserManagerReader::ParserPrototypes & parsers, const std::string & source, const ParserInitializer & parserInitializer, ObjectsList & objects)
{
	//jeœli pliku nie ma dodaj go, stwórz parsery i rozszerz dostêpne dane wraz z ich opisem
	for(auto parserIT = parsers.begin(); parserIT != parsers.end(); ++parserIT){
		//twworzymy w³asne opakowanie parsera klienckiego
		ParserPtr parser(new Parser((*parserIT)->create(), source));
		Objects objectsToVerify;
		//pobieramy udostêpniane obiekty - powinno dawaæ tylko obiekty zarejestrowane w DM
		parser->getObjects(objectsToVerify);

		//zarejestrowanie obiektów i ich zwi¹zku z parserem i typami danych
		for(auto objectIT = objectsToVerify.begin(); objectIT != objectsToVerify.end(); ++objectIT){

			if(*objectIT == nullptr){

				CORE_LOG_DEBUG("Unitialized object " << parser->getPath() << " for parser ID " << parser->getParser()->getID());

			}else{
				//dodac inicjalizator, jesli jest to warning!!
				if((*objectIT)->initializer().empty() == false){
					CORE_LOG_WARNING("Object " << (*objectIT)->getTypeInfo().name() << " from " << parser->getPath() << " for parser ID " << parser->getParser()->getID() << " loaded with custom initializer. Supplying with compound initializer");
					//inicjalizator na bazie inicjalizatora:)
					(*objectIT)->set(boost::bind(&FileDataManager::initializeDataWithExternalInitializer, this, _1, (*objectIT)->initializer()));
				}else{
					if((*objectIT)->getRawPtr() != nullptr ){
						CORE_LOG_WARNING("Object " << (*objectIT)->getTypeInfo().name() << " from " << parser->getPath() << " for parser ID " << parser->getParser()->getID() << " loaded already initialized. Reseting object and supplying initializer");
					}

					//inicjalizator na bazie parsera
					(*objectIT)->set(parserInitializer(parser));
				}

				//ObjectByParsers
				objects.push_back(*objectIT);

				//UWAGA!!
				//mapowanie surowego wskaŸnika do ObjectWrappera jest robione podczas parsowania!!
				//teraz mamy leniw¹ inicjalizacjê
			}
		}
	}
}

void FileDataManager::rawAddFile(const Filesystem::Path & file)
{
	if(!Filesystem::pathExists(file) || !Filesystem::isRegularFile(file)){
		throw std::runtime_error("Source not exist or is not a file");
	}

	plugin::IParserManagerReader::ParserPrototypes sourceParsers;
	getParserManager()->sourceParsers(sourceParsers, file.string());
	plugin::IParserManagerReader::ParserPrototypes streamParsers;
	getParserManager()->streamParsers(streamParsers, file.string());

	//Z listy strumieniowych usuwam te które s¹ na liœcie z w³asnym I/O - wierze ¿e zrobia to lepiej
	std::set<plugin::IParserConstPtr> sourceParsersSet;
	std::set<plugin::IParserConstPtr> streamParsersSet;

	sourceParsersSet.insert(sourceParsers.begin(), sourceParsers.end());
	streamParsersSet.insert(streamParsers.begin(), streamParsers.end());
	plugin::IParserManagerReader::ParserPrototypes sourcesLeft;

	std::set_difference(sourceParsersSet.begin(), sourceParsersSet.end(), streamParsersSet.begin(), streamParsersSet.end(), sourcesLeft);

	//Czêœciowo zachowuje siê jak manager strumieni - sam nimi zarz¹dzam dla plików
	ObjectsList objects;

	initializeParsers(streamParsers, , objects);

	initializeParsers(sourcesLeft, , objects);

	//jeœli pliku nie ma dodaj go, stwórz parsery i rozszerz dostêpne dane wraz z ich opisem
	for(auto parserIT = sourcesLeft.begin(); parserIT != sourcesLeft.end(); ++parserIT){
		//twworzymy w³asne opakowanie parsera klienckiego
		ParserPtr parser(new Parser((*parserIT)->create(), file.string()));
		Objects objectsToVerify;
		//pobieramy udostêpniane obiekty - powinno dawaæ tylko obiekty zarejestrowane w DM
		parser->getObjects(objectsToVerify);

		Objects verifiedObjects;

		//zarejestrowanie obiektów i ich zwi¹zku z parserem i typami danych
		for(auto objectIT = objectsToVerify.begin(); objectIT != objectsToVerify.end(); ++objectIT){

			if(*objectIT == nullptr){

				CORE_LOG_DEBUG("Unitialized object " << parser->getPath() << " for parser ID " << parser->getParser()->getID());

			}else{
				//dodac inicjalizator, jesli jest to warning!!
				if((*objectIT)->initializer().empty() == false){
					CORE_LOG_WARNING("Object " << (*objectIT)->getTypeInfo().name() << " from " << parser->getPath() << " for parser ID " << parser->getParser()->getID() << " loaded with custom initializer. Supplying with compound initializer");
					//inicjalizator na bazie inicjalizatora:)
					(*objectIT)->set(boost::bind(&FileDataManager::initializeDataWithExternalInitializer, this, _1, (*objectIT)->initializer()));
				}else{
					if((*objectIT)->getRawPtr() != nullptr ){
						CORE_LOG_WARNING("Object " << (*objectIT)->getTypeInfo().name() << " from " << parser->getPath() << " for parser ID " << parser->getParser()->getID() << " loaded already initialized. Reseting object and supplying initializer");
					}

					//inicjalizator na bazie parsera
					(*objectIT)->set(boost::bind(&FileDataManager::initializeDataWithSourceParser, this, _1, parser));
				}

				//ObjectByParsers
				objects.insert(*objectIT);

				//UWAGA!!
				//mapowanie surowego wskaŸnika do ObjectWrappera jest robione podczas parsowania!!
				//teraz mamy leniw¹ inicjalizacjê
			}
		}
	}

	if(objects.empty() == true){
		CORE_LOG_DEBUG("Any of known parsers did not provide any valid data for file: " << file);
	}else{
		objectsByFiles.insert(ObjectsByFiles::value_type(file, objects));
		CORE_LOG_INFO("File: " << file << " sussesfully loaded to DataManager");
	}
}

const bool FileDataManager::rawIsManaged(const Filesystem::Path & file) const
{
	return parsersByFiles.find(file) != parsersByFiles.end();
}

void FileDataManager::rawGetFiles(Filesystem::Files & files) const
{
	for(auto it = objectsByFiles.begin(); it != objectsByFiles.end(); ++it){
		files.insert(it->first);
	}
}

void FileDataManager::rawGetObjects(const Filesystem::Path & file, ConstObjectsList & objects) const
{
	auto it = objectsByFiles.find(file);

	for(auto objectIT = it->second.begin(); objectIT != it->second.end(); ++objectIT){
		objects.push_back(*objectIT);
	}
}

void FileDataManager::rawGetObjects(const Filesystem::Path & file, ObjectWrapperCollection & objects) const
{
	auto it = objectsByFiles.find(file);

	for(auto objectIT = it->second.begin(); objectIT != it->second.end(); ++objectIT){
		if(objects.exactTypes() == true){
			if((*objectIT)->getTypeInfo() == objects.getTypeInfo()){
				objects.push_back(*objectIT);
			}
		}else if((*objectIT)->isSupported(objects.getTypeInfo()) == true){
			objects.push_back(*objectIT);
		}
	}
}

void FileDataManager::updateObservers(const ChangeList & changes )
{
	for(auto it = observers.begin(); it != observers.end(); ++it){
		try{
			(*it)->observe(changes);
		}catch(...){
			//TODO
			//rozwin¹æ obserwatorów aby siê jakoœ identyfikowali!! ewentualnie robiæ to przez w³asn¹ implementacjê dostarczan¹ konretnym obiektom
			//(osobne interfejsy readerów dla ka¿dego elemnentu ³adowanego do aplikacji - service, source, datasink, itp)
			CORE_LOG_WARNING("Error while updating file data manager observer");
		}
	}
}

void FileDataManager::removeFile(const Filesystem::Path & file)
{
	ScopedLock lock(sync);

	if(rawIsManaged(file) == false){
		throw std::runtime_error("File not managed");
	}

	ConstObjectsList toRemove;
	//pobieramy obiekty do usuniecia z DM
	rawGetObjects(file, toRemove);
	//usuñ obiekty z MemoryDM w transakcji
	{
		auto transaction = getMemoryDataManager()->transaction();

		for(auto it = toRemove.begin(); it != toRemove.end(); ++it){
			transaction->tryRemoveData(*it);
		}
	}

	//usuñ plik
	rawRemoveFile(file);
	//notyfikuj
	ChangeList changes;
	FileChange change;
	change.filePath = file;
	change.modyfication = plugin::IFileManagerReader::REMOVE_FILE;
	changes.push_back(change);
	updateObservers(changes);
}

void FileDataManager::addFile(const Filesystem::Path & file)
{
	ScopedLock lock(sync);

	if(rawIsManaged(file) == false){
		throw std::runtime_error("File already managed");
	}

	//dodaj plik
	rawAddFile(file);
	//pobierz obiekty dla pliku
	ConstObjectsList toAdd;
	rawGetObjects(file, toAdd);
	//dodaj obiekty do Memory DM w transakcji
	{
		auto transaction = getMemoryDataManager()->transaction();

		for(auto it = toAdd.begin(); it != toAdd.end(); ++it){
			transaction->tryAddData(*it);
		}
	}

	//notyfikuj
	ChangeList changes;
	FileChange change;
	change.filePath = file;
	change.modyfication = plugin::IFileManagerReader::ADD_FILE;
	changes.push_back(change);
	updateObservers(changes);
}

const FileDataManager::TransactionPtr FileDataManager::transaction()
{

}

const FileDataManager::TransactionPtr FileDataManager::transaction(const plugin::IMemoryDataManager::TransactionPtr & memoryTransaction)
{

}

void FileDataManager::addObserver(const FileObserverPtr & fileWatcher)
{
	ScopedLock lock(sync);
	if(std::find(observers.begin(), observers.end(), fileWatcher) != observers.end()){
		throw std::runtime_error("Watcher already registered");
	}

	observers.push_back(fileWatcher);
}

void FileDataManager::removeObserver(const FileObserverPtr & fileWatcher)
{
	ScopedLock lock(sync);
	auto it = std::find(observers.begin(), observers.end(), fileWatcher);
	if(it == observers.end()){
		throw std::runtime_error("Watcher not registered");
	}

	observers.erase(it);
}

void FileDataManager::getFiles(Files & files) const
{
	ScopedLock lock(sync);
}

const bool FileDataManager::isManaged(const Filesystem::Path & file) const
{
	ScopedLock lock(sync);
	return rawIsManaged(file);
}

void FileDataManager::getObjects(const Filesystem::Path & file, ConstObjectsList & objects) const
{
	ScopedLock lock(sync);
}

void FileDataManager::getObjects(const Filesystem::Path & file, ObjectWrapperCollection & objects) const
{
	ScopedLock lock(sync);
}

}