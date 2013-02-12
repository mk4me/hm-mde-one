#include "CorePCH.h"
#include "FileDataManager.h"
#include "MemoryDataManager.h"
#include "ApplicationCommon.h"
#include "ParserManager.h"

using namespace core;

const FileDataManager::ParserCreator FileDataManager::streamParserCreator = boost::bind(&FileDataManager::createStreamParser, _1, _2);
const FileDataManager::ParserCreator FileDataManager::sourceParserCreator = boost::bind(&FileDataManager::createSourceParser, _1, _2);

class FileDataManager::FileTransaction : public IFileDataManager::IFileDataManagerTransaction
{
private:

	struct Modyfication
	{
		IFileManagerReader::ModificationType modyfication;
		ObjectsList objects;
	};

	typedef std::map<Filesystem::Path, Modyfication> FileModyfications;

private:
	IMemoryDataManager::TransactionPtr mdmTransaction;
	FileDataManager * fdm;
	FileModyfications modyfications;
	bool transactionRolledback;

public:
	FileTransaction(FileDataManager * fdm) : fdm(fdm), transactionRolledback(false),  mdmTransaction(getMemoryDataManager()->transaction())
	{
		fdm->sync.lock();
	}

	~FileTransaction()
	{
		//wymuszam najpierw commit transakcji po stronie MemoryDataManagera
		//potem sam notyfikuje o zmianach w plikach
		mdmTransaction.reset();

		if(transactionRolledback == false){
			if(modyfications.empty() == false){
				FileDataManager::ChangeList changes;
				for(auto it = modyfications.begin(); it != modyfications.end(); ++it){
					FileDataManager::FileChange change;
					change.filePath = it->first;
					change.modyfication = it->second.modyfication;
					changes.push_back(change);
				}

				fdm->updateObservers(changes);
			}

			fdm->sync.unlock();
		}
	}

public:

	virtual const bool isRolledback() const
	{
		return transactionRolledback;
	}

	virtual void rollback()
	{
		transactionRolledback = true;

		mdmTransaction->rollback();

		for(auto it = modyfications.begin(); it != modyfications.end(); ++it){
			switch(it->second.modyfication){
			case IFileManagerReader::ADD_FILE:
				fdm->objectsByFiles.erase(it->first);
				break;

			case IFileManagerReader::REMOVE_FILE:
				{
					auto insRes = fdm->objectsByFiles.insert(FileDataManager::ObjectsByFiles::value_type(it->first, it->second.objects));
					if(insRes.second == false){
						insRes.first->second.insert(insRes.first->second.end(), it->second.objects.begin(), it->second.objects.end());
					}
				}
				break;
			}
		}

		fdm->sync.unlock();
	}

	//! \param files Lista plikÛw dla ktÛrych zostanπ utworzone parsery i z ktÛrych wyciπgniÍte dane
	//! bÍda dostÍpne poprzez DataMangera LENIWA INICJALIZACJA
	virtual void addFile(const Filesystem::Path & file)
	{
		if(transactionRolledback == true){
			throw std::runtime_error("File transaction already rolled-back");
		}

		if(fdm->rawIsManaged(file) == true){
			rollback();
			throw std::runtime_error("File transaction tried to add file already managed by manager");
		}

		if(!Filesystem::pathExists(file) || !Filesystem::isRegularFile(file)){
			rollback();
			throw std::runtime_error("File transaction tried to add not existing file");
		}

		rawAddFile(file);
	}

	//! \param files Lista plikÛw ktÛre zostanπ usuniÍte z aplikacji a wraz z nimi skojarzone parsery i dane
	virtual void removeFile(const Filesystem::Path & file)
	{
		if(transactionRolledback == true){
			throw std::runtime_error("File transaction already rolled-back");
		}

		if(fdm->rawIsManaged(file) == false){
			rollback();
			throw std::runtime_error("File transaction tried to modify file not managed by manager");
		}

		rawRemoveFile(file);
	}

	virtual const bool tryAddFile(const Filesystem::Path & file)
	{
		if(transactionRolledback == true || fdm->rawIsManaged(file) == true || !Filesystem::pathExists(file) || !Filesystem::isRegularFile(file)){
			return false;
		}

		rawAddFile(file);

		return true;
	}

	virtual const bool tryRemoveFile(const Filesystem::Path & file)
	{
		if(transactionRolledback == true || fdm->rawIsManaged(file) == false){
			return false;
		}

		rawRemoveFile(file);

		return true;
	}

	virtual void getFiles(Files & files) const
	{
		if(transactionRolledback == true){
			throw std::runtime_error("File transaction already rolled-back");
		}

		fdm->rawGetFiles(files);
	}

	virtual const bool isManaged(const Filesystem::Path & file) const
	{
		if(transactionRolledback == true){
			throw std::runtime_error("File transaction already rolled-back");
		}

		return fdm->isManaged(file);
	}

	virtual void getObjects(const Filesystem::Path & file, ConstObjectsList & objects) const
	{
		if(transactionRolledback == true){
			throw std::runtime_error("File transaction already rolled-back");
		}

		fdm->getObjects(file, objects);
	}

	virtual void getObjects(const Filesystem::Path & file, ObjectWrapperCollection & objects) const
	{
		if(transactionRolledback == true){
			throw std::runtime_error("File transaction already rolled-back");
		}

		fdm->getObjects(file, objects);
	}

private:

	void rawAddFile(const Filesystem::Path & file)
	{
		//dodajemy dane do dm
		fdm->rawAddFile(file, mdmTransaction);
		//aktualizujemy liste zmian
		Modyfication mod;
		mod.modyfication = IFileManagerReader::ADD_FILE;
		modyfications.insert(FileModyfications::value_type(file,mod));
	}

	void rawRemoveFile(const Filesystem::Path & file)
	{
		Modyfication mod;
		mod.modyfication = IFileManagerReader::REMOVE_FILE;
		fdm->rawGetObjects(file, mod.objects);
		//dodajemy dane do dm
		fdm->rawRemoveFile(file, mdmTransaction);
		//aktualizujemy liste zmian
		modyfications.insert(FileModyfications::value_type(file,mod));
	}
};

class FileDataManager::FileReaderTransaction : public IFileManagerReaderOperations
{
public:
	FileReaderTransaction(FileDataManager * fdm) : fdm(fdm)
	{
		fdm->sync.lock();
	}

	~FileReaderTransaction()
	{
		fdm->sync.unlock();
	}

public:

	virtual void getFiles(Files & files) const
	{
		fdm->rawGetFiles(files);
	}

	virtual const bool isManaged(const Filesystem::Path & file) const
	{
		return fdm->isManaged(file);
	}

	virtual void getObjects(const Filesystem::Path & file, ConstObjectsList & objects) const
	{
		fdm->getObjects(file, objects);
	}

	virtual void getObjects(const Filesystem::Path & file, ObjectWrapperCollection & objects) const
	{
		fdm->getObjects(file, objects);
	}

private:
	FileDataManager * fdm;
};

//! WewnÍtrzna reprezentacja parsera uøywana przez DataManagera.
class FileDataManager::Parser
{
private:
	//! Prawdziwy wewnÍtrzny parser.
	const plugin::IParserPtr parser;
	//! èrÛd≥a danych
	const Filesystem::Path path;
	//! Czy przeparsowano plik?
	bool parsed;
	//! Czy uøyto parsera do przeparsowania?
	bool used;
	//! Czy w≥aúnie parsujemy?
	bool parsing;

public:
	//! \param parser Faktyczny parser. To ten obiekt kontroluje jego
	//!     czas øycia.
	//! \param resource Czy parser jest zwiπzany z zasobami sta≥ymi?
	Parser(plugin::IParser* parser, const Filesystem::Path & path) :
		parser(parser), parsed(false), used(false), path(path)
		{
			UTILS_ASSERT(parser);
			UTILS_ASSERT(!path.empty());
		}
		//! Destruktor drukujπcy wiadomoúÊ o wy≥adowaniu pliku.
		~Parser()
		{
			if ( isParsed() ) {
				CORE_LOG_NAMED_DEBUG("parser", "Unloading parser for file: " << path );
			} else if ( isUsed() ) {
				CORE_LOG_NAMED_DEBUG("parser", "Unloading invalid parser for file: " << path );
			} else {
				CORE_LOG_NAMED_DEBUG("parser", "Unloading unused parser for file: " << path );
			}
		}

public:
	//! \return Czy uøyto tego parsera?
	inline const bool isUsed() const
	{
		return used;
	}
	//! \return Czy uda≥o siÍ przeparsowaÊ plik?
	inline const bool isParsed() const
	{
		return parsed;
	}

	inline void reset()
	{
		used = false;
		parsed = false;
	}

	const Filesystem::Path & getPath() const
	{
		return path;
	}

	//! \return
	inline plugin::IParserPtr getParser() const
	{
		return parser;
	}

	void parseFile()
	{
		UTILS_ASSERT(!isUsed());
		CORE_LOG_NAMED_DEBUG("parser", "Parsing file: " << path );
		used = true;
		_parseFile();
		parsed = true;
	}

private:
	//! Moøe rzucaÊ wyjπtkami!
	virtual void _parseFile() = 0;

public:
	//! Nie rzuca wyjπtkami.
	//! \return Czy uda≥o siÍ przeparsowaÊ?
	const bool tryParse()
	{
		bool ret = true;
		try {
			parseFile();
		} catch (const std::exception& ex) {
			CORE_LOG_NAMED_ERROR("parser", "Error during parsing file " << path << ": " << ex.what());
			ret = false;
		}

		return ret;
	}
	//! \param objects Lista wrappowanych obiektÛw, zainicjowanych (przeparsowany parser)
	//!         bπdü nie.
	inline void getObjects(Objects& objects)
	{
		parser->getObjects(objects);
	}
};


//! WewnÍtrzna reprezentacja parsera uøywana przez DataManagera.
class FileDataManager::FileParser : public FileDataManager::Parser
{
public:
	FileParser(plugin::IParser* parser, const Filesystem::Path & path) : FileDataManager::Parser(parser, path), fileParser(nullptr)
	{
			
	}

private:
	virtual void _parseFile()
	{
		if(fileParser == nullptr){
			fileParser = dynamic_cast<plugin::ISourceParserCapabilities*>(getParser().get());
		}

		if(fileParser != nullptr){
			fileParser->parse(getPath().string());
		}else{
			throw std::runtime_error("Parser is expected to implement plugin::ISourceParserCapabilities interface but failed to cast");
		}
	}

private:
	plugin::ISourceParserCapabilities * fileParser;
};

class FileDataManager::StreameFileParser : public FileDataManager::Parser
{
public:
	StreameFileParser(plugin::IParser* parser, const Filesystem::Path & path) : FileDataManager::Parser(parser, path), streamFileParser(nullptr)
	{

	}

private:
	virtual void _parseFile()
	{
		if(streamFileParser == nullptr){
			streamFileParser = dynamic_cast<plugin::IStreamParserCapabilities*>(getParser().get());
		}

		if(streamFileParser != nullptr){

			scoped_ptr<std::ifstream> localStream(new std::ifstream(getPath().string()));
			if(localStream->good() == true){
				streamFileParser->parse(localStream.get(), getPath().string());
			}else{
				throw std::runtime_error("Failed to initialize parser stream");
			}
		}else{			
			throw std::runtime_error("Parser is expected to implement plugin::IStreamParserCapabilities interface but failed to cast");			
		}
	}

private:
	plugin::IStreamParserCapabilities * streamFileParser;
};

void FileDataManager::initializeDataWithParser(ObjectWrapper & object, const ParserPtr & parser)
{
	ScopedLock lock(sync);
	if(parser->isUsed() == false){
		if(parser->tryParse() == false){
			CORE_LOG_DEBUG("Parser " << parser->getParser()->getName() << " ID " << parser->getParser()->getID() << " failed during initialization of object " << object.getClassName() << " with type info " << object.getTypeInfo().name() << " for file " << parser->getPath());
		}

		verifyAndRemoveUninitializedParserObjects(parser);
	}
}

void FileDataManager::initializeDataWithExternalInitializer(ObjectWrapper & object, const ObjectWrapper::LazyInitializer & li, const ParserPtr & parser)
{
	ScopedLock lock(sync);
	try{
		li(object);
	}catch(std::exception & e){
		CORE_LOG_DEBUG("Error while running compound initializer delivered from external parser " << parser->getParser()->getName() << " with ID " << parser->getParser()->getID() << " for file " << parser->getPath() << " : " << e.what());
	}catch(...){
		CORE_LOG_DEBUG("UNKNOWN Error while running compound initializer delivered from external parser " << parser->getParser()->getName() << " with ID " << parser->getParser()->getID() << " for file " << parser->getPath());
	}

	verifyAndRemoveUninitializedParserObjects(parser);
}

void FileDataManager::verifyAndRemoveUninitializedParserObjects(const ParserPtr & parser)
{
	auto it = objectsByFiles.find(parser->getPath());
	if(it != objectsByFiles.end()){
		Objects parserObjects;
		parser->getObjects(parserObjects);
		std::set<ObjectWrapperPtr> parsersObjectSet(parserObjects.begin(), parserObjects.end());
		std::set<ObjectWrapperPtr> fileObjectSet(it->second.begin(), it->second.end());
		std::vector<ObjectWrapperPtr> toVerify(std::min(parserObjects.size(), it->second.size()));
		std::set_intersection(parsersObjectSet.begin(), parsersObjectSet.end(), fileObjectSet.begin(), fileObjectSet.end(), toVerify.begin());

		if(toVerify.empty() == false){

			ObjectsList toRemove;

			//transakcja w MemoryDataManager
			{
				auto mdmTransaction = getMemoryDataManager()->transaction();
		
				for(auto itV = toVerify.begin(); itV != toVerify.end(); ++itV){
					if((*itV)->getRawPtr() == nullptr){
						CORE_LOG_DEBUG("Object of type " << (*itV)->getTypeInfo().name() << " not initialized properly by parser ID " << parser->getParser()->getID() <<
							" named "  << parser->getParser()->getName() << " for file " << parser->getPath());
						toRemove.push_back(*itV);
						it->second.remove(*itV);
						if(mdmTransaction->isManaged(*itV) == true){
							mdmTransaction->removeData(*itV);
						}
					}
				}
			}

			//sprawdzam czy nie moge juz usunπÊ pliku i notyfikowaÊ o zmianie
			if(it->second.empty() == true){
				CORE_LOG_INFO("Removing unused file " << parser->getPath() << " because of lack of derived objects in memory data manager. Some of them were corrupted while parsing - not initialized properly.");
				ChangeList changes;
				FileChange change;
				change.filePath = it->first;
				change.modyfication = IFileManagerReader::REMOVE_UNUSED_FILE;
				changes.push_back(change);

				objectsByFiles.erase(it);
				
				updateObservers(changes);
			}
		}
	}	
}

void FileDataManager::rawRemoveFile(const Filesystem::Path & file, const IMemoryDataManager::TransactionPtr & memTransaction)
{
	bool ok = true;
	ObjectsList toRemove;
	//pobieramy obiekty do usuniecia z DM
	rawGetObjects(file, toRemove);
	//usuÒ obiekty z MemoryDM w transakcji
	for(auto it = toRemove.begin(); it != toRemove.end(); ++it){
		if(memTransaction->tryRemoveData(*it) == false){
			ok = false;
		}
	}

	//usuÒ plik	
	objectsByFiles.erase(file);
	if(ok == true){
		CORE_LOG_INFO("File: " << file << " succesfully removed from manager");
	}else{
		CORE_LOG_DEBUG("There were some problems while removing data from memory data manager for file " << file << ". Not all objects have been removed properly");
	}
}

void FileDataManager::initializeParsers(const IParserManagerReader::ParserPrototypes & parsers, const Filesystem::Path & path, const ParserCreator & parserCreator, ObjectsList & objects)
{
	//jeúli pliku nie ma dodaj go, stwÛrz parsery i rozszerz dostÍpne dane wraz z ich opisem
	for(auto parserIT = parsers.begin(); parserIT != parsers.end(); ++parserIT){
		//twworzymy w≥asne opakowanie parsera klienckiego
		ParserPtr parser(parserCreator((*parserIT)->create(), path));
		Objects objectsToVerify;
		//pobieramy udostÍpniane obiekty - powinno dawaÊ tylko obiekty zarejestrowane w DM
		parser->getObjects(objectsToVerify);

		//zarejestrowanie obiektÛw i ich zwiπzku z parserem i typami danych
		for(auto objectIT = objectsToVerify.begin(); objectIT != objectsToVerify.end(); ++objectIT){

			if(*objectIT == nullptr){

				CORE_LOG_DEBUG("Unitialized object " << parser->getPath() << " for parser ID " << parser->getParser()->getID());

			}else{
				//dodac inicjalizator, jesli jest to warning!!
				if((*objectIT)->initializer().empty() == false){
					CORE_LOG_WARNING("Object " << (*objectIT)->getTypeInfo().name() << " from " << parser->getPath() << " for parser ID " << parser->getParser()->getID() << " loaded with custom initializer. Supplying with compound initializer");
					//inicjalizator na bazie inicjalizatora:)
					(*objectIT)->set(ObjectWrapper::LazyInitializer(boost::bind(&FileDataManager::initializeDataWithExternalInitializer, this, _1, (*objectIT)->initializer(), parser)));
				}else{
					if((*objectIT)->getRawPtr() != nullptr ){
						CORE_LOG_WARNING("Object " << (*objectIT)->getTypeInfo().name() << " from " << parser->getPath() << " for parser ID " << parser->getParser()->getID() << " loaded already initialized. Reseting object and supplying initializer");
					}

					//inicjalizator na bazie parsera
					(*objectIT)->set(ObjectWrapper::LazyInitializer(boost::bind(&FileDataManager::initializeDataWithParser, this, _1, parser)));
				}

				objects.push_back(*objectIT);
			}
		}
	}
}

void FileDataManager::rawAddFile(const Filesystem::Path & file, const IMemoryDataManager::TransactionPtr & memTransaction)
{
	ObjectsList objects;

	IParserManagerReader::ParserPrototypes sourceParsers;
	getParserManager()->sourceParsers(file.string(), sourceParsers);
	IParserManagerReader::ParserPrototypes streamParsers;
	getParserManager()->streamParsers(file.string(), streamParsers);

	//Z listy strumieniowych usuwam te ktÛre sπ na liúcie z w≥asnym I/O - wierze øe zrobia to lepiej
	std::set<plugin::IParserConstPtr> sourceParsersSet;
	std::set<plugin::IParserConstPtr> streamParsersSet;

	sourceParsersSet.insert(sourceParsers.begin(), sourceParsers.end());
	streamParsersSet.insert(streamParsers.begin(), streamParsers.end());
	IParserManagerReader::ParserPrototypes sourcesLeft(sourceParsersSet.size());

	auto lastIT = std::set_difference(sourceParsersSet.begin(), sourceParsersSet.end(), streamParsersSet.begin(), streamParsersSet.end(), sourcesLeft.begin());

	sourcesLeft.erase(lastIT, sourcesLeft.end());

	//preferuje uzycie parserÛw z w≥asnπ obs≥ugπ I/O - wierze øe zrobiπ to maksymalnie wydajnie wg w≥asnych zasad
	initializeParsers(streamParsers, file, streamParserCreator, objects);
	//teraz uzywam parserÛw strumieniowych - sam dostarczÍ im strumieni
	initializeParsers(sourcesLeft, file, sourceParserCreator, objects);

	if(objects.empty() == true){
		CORE_LOG_DEBUG("Any of known parsers did not provide any valid data for file: " << file);
	}else{

		//TODO
		//metadane: name i source -> dodaÊ jeúli brakuje

		ObjectsList objectsAdded;

		for(auto it = objects.begin(); it != objects.end(); ++it){
			if(memTransaction->isManaged(*it) == false){
				memTransaction->addData(*it);
				objectsAdded.push_back(*it);
			}
		}

		// jak wszystko ok to zapamiÍtuje
		objectsByFiles.insert(ObjectsByFiles::value_type(file, objectsAdded));
		CORE_LOG_INFO("File: " << file << " succesfully loaded to manager. Extracted data : " << objects.size() << "| data loaded to memory data manager : " << objectsAdded.size());
	}
}

const bool FileDataManager::rawIsManaged(const Filesystem::Path & file) const
{
	return objectsByFiles.find(file) != objectsByFiles.end();
}

void FileDataManager::rawGetFiles(Files & files) const
{
	for(auto it = objectsByFiles.begin(); it != objectsByFiles.end(); ++it){
		files.insert(it->first);
	}
}

void FileDataManager::rawGetObjects(const Filesystem::Path & file, ConstObjectsList & objects) const
{
	auto it = objectsByFiles.find(file);

	objects.insert(objects.end(), it->second.begin(), it->second.end());
}

void FileDataManager::rawGetObjects(const Filesystem::Path & file, ObjectsList & objects)
{
	auto it = objectsByFiles.find(file);

	objects.insert(objects.end(), it->second.begin(), it->second.end());
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
			//rozwinπÊ obserwatorÛw aby siÍ jakoú identyfikowali!! ewentualnie robiÊ to przez w≥asnπ implementacjÍ dostarczanπ konretnym obiektom
			//(osobne interfejsy readerÛw dla kaødego elemnentu ≥adowanego do aplikacji - service, source, datasink, itp)
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

	//usuÒ plik
	rawRemoveFile(file, getMemoryDataManager()->transaction());
	//notyfikuj
	ChangeList changes;
	FileChange change;
	change.filePath = file;
	change.modyfication = IFileManagerReader::REMOVE_FILE;
	changes.push_back(change);
	updateObservers(changes);
}

void FileDataManager::addFile(const Filesystem::Path & file)
{
	ScopedLock lock(sync);

	if(rawIsManaged(file) == false){
		throw std::runtime_error("File already managed");
	}

	if(!Filesystem::pathExists(file) || !Filesystem::isRegularFile(file)){
		throw std::runtime_error("Source not exist or is not a file");
	}

	//dodaj plik
	rawAddFile(file, getMemoryDataManager()->transaction());

	//notyfikuj
	ChangeList changes;
	FileChange change;
	change.filePath = file;
	change.modyfication = IFileManagerReader::ADD_FILE;
	changes.push_back(change);
	updateObservers(changes);
}

const bool FileDataManager::tryAddFile(const core::Filesystem::Path & file)
{
	bool ret = true;
	try{
		addFile(file);
	}catch(...){
		ret = false;
	}

	return ret;
}

const bool FileDataManager::tryRemoveFile(const core::Filesystem::Path & file)
{
	bool ret = true;
	try{
		removeFile(file);
	}catch(...){
		ret = false;
	}

	return ret;
}

IFileDataManager::TransactionPtr FileDataManager::transaction()
{
	return IFileDataManager::TransactionPtr(new FileTransaction(this));
}

IFileManagerReader::TransactionPtr FileDataManager::transaction() const
{
	return IFileManagerReader::TransactionPtr(new FileReaderTransaction(const_cast<FileDataManager*>(this)));
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
	for(auto it = objectsByFiles.begin(); it != objectsByFiles.end(); ++it){
		files.insert(it->first);
	}
}

const bool FileDataManager::isManaged(const Filesystem::Path & file) const
{
	ScopedLock lock(sync);
	return rawIsManaged(file);
}

void FileDataManager::getObjects(const Filesystem::Path & file, ConstObjectsList & objects) const
{
	ScopedLock lock(sync);
	rawGetObjects(file, objects);
}

void FileDataManager::getObjects(const Filesystem::Path & file, ObjectWrapperCollection & objects) const
{
	ScopedLock lock(sync);
	rawGetObjects(file, objects);
}

FileDataManager::ParserPtr FileDataManager::createSourceParser(plugin::IParser * parser, const Filesystem::Path & path)
{
	return ParserPtr(new FileParser(parser, path));
}

FileDataManager::ParserPtr FileDataManager::createStreamParser(plugin::IParser * parser, const Filesystem::Path & path)
{
	return ParserPtr(new StreameFileParser(parser, path));
}
