#include "CorePCH.h"
#include "FileDataManager.h"
#include "MemoryDataManager.h"
#include "ApplicationCommon.h"
#include "ParserManager.h"
#include <utils/Push.h>
#include <boost/bind.hpp>
#include <fstream>

using namespace core;

class FileDataManager::FileTransaction : public IFileDataManager::IFileDataManagerTransaction
{
private:

	struct Modyfication
	{
		IFileManagerReader::ModificationType modyfication;
		VariantsList objects;
	};

	typedef std::map<Filesystem::Path, Modyfication> FileModyfications;

private:
	IMemoryDataManager::TransactionPtr mdmTransaction;
	FileDataManager * fdm;
	FileModyfications modyfications;
	bool transactionRolledback;
	bool oldSkipUpdate;

public:
	FileTransaction(FileDataManager * fdm) : fdm(fdm), transactionRolledback(false),
		mdmTransaction(getMemoryDataManager()->transaction()), oldSkipUpdate(fdm->skipUpdate)
	{
		fdm->sync.lock();
		fdm->skipUpdate = true;
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

			fdm->skipUpdate = oldSkipUpdate;
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
		//odkrecam poprzednia transakcje
		mdmTransaction->rollback();
		//tworze nowa zeby usunac reszte
		mdmTransaction = getMemoryDataManager()->transaction();

		for(auto it = modyfications.begin(); it != modyfications.end(); ++it){
			switch(it->second.modyfication){
			case IFileManagerReader::ADD_FILE:
				{
					VariantsList toRemove;
					fdm->rawGetObjects(it->first, toRemove);
					//usu� obiekty z MemoryDM w transakcji
					for(auto IT = toRemove.begin(); IT != toRemove.end(); ++IT){
						//TODO
						//przywrócić poprzedni jeśli był
						(*IT)->setInitializer(VariantInitializerPtr());
					}
					fdm->objectsByFiles.erase(it->first);
				}
				break;

			case IFileManagerReader::REMOVE_FILE:
				{
					//TODO
					//przywrócić LazyInitializer!!
					auto insRes = fdm->objectsByFiles.insert(FileDataManager::ObjectsByFiles::value_type(it->first, it->second.objects));
					if(insRes.second == false){
						insRes.first->second.insert(insRes.first->second.end(), it->second.objects.begin(), it->second.objects.end());
					}
				}
				break;
			}
		}

		fdm->skipUpdate = oldSkipUpdate;
		fdm->sync.unlock();
	}

	//! \param files Lista plik�w dla kt�rych zostan� utworzone parsery i z kt�rych wyci�gni�te dane
	//! b�da dost�pne poprzez DataMangera LENIWA INICJALIZACJA
	virtual void addFile(const Filesystem::Path & file)
	{
		verifyRollback();

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

	//! \param files Lista plik�w kt�re zostan� usuni�te z aplikacji a wraz z nimi skojarzone parsery i dane
	virtual void removeFile(const Filesystem::Path & file)
	{
		verifyRollback();

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
		verifyRollback();

		fdm->rawGetFiles(files);
	}

	virtual const bool isManaged(const Filesystem::Path & file) const
	{
		verifyRollback();

		return fdm->isManaged(file);
	}

	virtual void getObjects(const Filesystem::Path & file, ConstVariantsList & objects) const
	{
		verifyRollback();

		fdm->getObjects(file, objects);
	}

	virtual void getObjects(const Filesystem::Path & file, VariantsCollection & objects) const
	{
		verifyRollback();

		fdm->getObjects(file, objects);
	}

private:

	inline void verifyRollback() const
	{
		if (isRolledback() == true){
			throw std::runtime_error("File transaction already rolled-back");
		}
	}

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

	virtual void getObjects(const Filesystem::Path & file, ConstVariantsList & objects) const
	{
		fdm->getObjects(file, objects);
	}

	virtual void getObjects(const Filesystem::Path & file, VariantsCollection & objects) const
	{
		fdm->getObjects(file, objects);
	}

private:
	FileDataManager * fdm;
};

//! Wewn�trzna reprezentacja parsera u�ywana przez DataManagera.
class Parser
{
private:
	//! Prawdziwy wewn�trzny parser.
	const plugin::IParserPtr parser;
	//! Opis źródła danych
	const Filesystem::Path path;
	//! Czy przeparsowano plik?
	bool parsed;
	//! Czy u�yto parsera do przeparsowania?
	bool used;
	//! Czy w�a�nie parsujemy?
	bool parsing;

private:
	//! Mo�e rzuca� wyj�tkami!
	virtual void _parseFile() = 0;

public:
	//! \param parser Faktyczny parser. To ten obiekt kontroluje jego
	//!     czas �ycia.
	//! \param resource Czy parser jest zwi�zany z zasobami sta�ymi?
	Parser(plugin::IParser* parser, const Filesystem::Path & path) :
		parser(parser), parsed(false), used(false), path(path)
	{
		UTILS_ASSERT(parser);
		UTILS_ASSERT(!path.empty());
	}

	//! Destruktor drukuj�cy wiadomo�� o wy�adowaniu pliku.
	virtual ~Parser()
	{
		if ( isParsed() ) {
			CORE_LOG_NAMED_DEBUG("parser", "Unloading parser for file: " << path );
		} else if ( isUsed() ) {
			CORE_LOG_NAMED_DEBUG("parser", "Unloading invalid parser for file: " << path );
		} else {
			CORE_LOG_NAMED_DEBUG("parser", "Unloading unused parser for file: " << path );
		}
	}

	//! \return Czy u�yto tego parsera?
	inline const bool isUsed() const
	{
		return used;
	}
	//! \return Czy uda�o si� przeparsowa� plik?
	inline const bool isParsed() const
	{
		return parsed;
	}

	inline void reset()
	{
		parser->reset();
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
		CORE_LOG_NAMED_DEBUG("parser", "File parsed OK: " << path);
	}

	//! Nie rzuca wyj�tkami.
	//! \return Czy uda�o si� przeparsowa�?
	const bool tryParse()
	{
		bool ret = false;
		try {
			parseFile();
			ret = true;
		} catch (const std::exception& e) {
			CORE_LOG_NAMED_ERROR("parser", "Error during parsing file " << path << ": " << e.what());			
		} catch (...) {
			CORE_LOG_NAMED_ERROR("parser", "UNKNOWN Error during parsing file " << path);			
		}

		return ret;
	}

	inline void getObject(Variant& object, const VariantsVector::size_type idx) const
	{
		parser->getObject(object, idx);
	}
};

//! Wewn�trzna reprezentacja parsera u�ywana przez DataManagera.
class FileParser : public Parser
{
public:
	FileParser(plugin::IParser* parser, const Filesystem::Path & path) : Parser(parser, path), fileParser(nullptr)
	{
			
	}

private:
	virtual void _parseFile()
	{
		if(fileParser == nullptr){
			fileParser = dynamic_cast<plugin::ISourceParser*>(getParser().get());
		}

		if(fileParser != nullptr){
			fileParser->parse(getPath().string());
		}else{
			throw std::runtime_error("Parser is expected to implement plugin::ISourceParser interface but failed to cast");
		}
	}

private:
	//! Interfejs parsera plików
	plugin::ISourceParser * fileParser;
};

class StreameFileParser : public Parser
{
public:
	StreameFileParser(plugin::IParser* parser, const std::string & source) : Parser(parser, source), streamFileParser(nullptr)
	{

	}

private:
	virtual void _parseFile()
	{
		if(streamFileParser == nullptr){
			streamFileParser = dynamic_cast<plugin::IStreamParser*>(getParser().get());
		}

		if(streamFileParser != nullptr){

			auto path = getPath().string();
			utils::scoped_ptr<std::ifstream> localStream(new std::ifstream(path));
			if(localStream->good() == true){
				streamFileParser->parse(localStream.get(), path);
			}else{
				throw std::runtime_error("Failed to initialize parser stream");
			}
		}else{			
			throw std::runtime_error("Parser is expected to implement plugin::IStreamParser interface but failed to cast");			
		}
	}

private:
	//! Interfejs parsera plików ze strumieni
	plugin::IStreamParser * streamFileParser;
};


class SimpleInitializer : public IVariantInitializer
{
public:
	SimpleInitializer(const utils::shared_ptr<Parser> & parser, const int idx)
	: parser(parser), idx(idx) {}
	virtual ~SimpleInitializer() {}
	virtual void initialize(Variant * object) {
		parser->parseFile();
		parser->getObject(*object, idx);
		parser->reset();
	}
	virtual SimpleInitializer * clone() const { return new SimpleInitializer(parser, idx); }
	virtual const bool isEqual(const IVariantInitializer &) const { return false; }

private:
	utils::shared_ptr<Parser> parser;
	const int idx;
};

class CompoundInitializer : public IVariantInitializer
{
public:

	struct CompoundData {
		utils::shared_ptr<Parser> parser;
		std::map<int, VariantWeakPtr> objects;
	};

	typedef utils::shared_ptr<CompoundData> CompoundDataPtr;

public:

	CompoundInitializer(const CompoundDataPtr & data, const int idx) : data(data), idx(idx) {}

	virtual ~CompoundInitializer() {}

	virtual void initialize(Variant * object) {
	
		auto mt = getMemoryDataManager()->transaction();
		std::list<int> toDelete;
		if (data->parser->tryParse() == false){
			CORE_LOG_NAMED_INFO("parser", "Parser " << data->parser->getParser()->ID() << " (" << data->parser->getParser()->shortName() << ") failed for file " << data->parser->getPath());
			for (auto it = data->objects.begin(); it != data->objects.end(); ++it){
				auto ow = it->second.lock();
				if (ow != nullptr && ow->data()->getRawPtr() == nullptr){						
					toDelete.push_back(it->first);					
				}				
			}
		}
		else{
			for (auto it = data->objects.begin(); it != data->objects.end(); ++it){
				auto ow = it->second.lock();
				if (ow != nullptr){
					if (ow->initialized() == false){
						data->parser->getObject(*ow, it->first);
						if (ow->data()->getRawPtr() == nullptr){
							CORE_LOG_NAMED_INFO("parser", "Object of type " << ow->data()->getTypeInfo().name()
								<< " not initialized properly by parser ID: " << data->parser->getParser()->ID()
								<< " (" << data->parser->getParser()->shortName() << ") for file " << data->parser->getPath());
							toDelete.push_back(it->first);
						}
					}
				}
				else{
					toDelete.push_back(it->first);
				}
			}
		}

		data->parser->reset();

		for (auto it = toDelete.begin(); it != toDelete.end(); ++it){
			auto oit = data->objects.find(*it);
			mt->tryRemoveData(oit->second.lock());
			data->objects.erase(oit);
		}

		if (data->objects.empty() == true){
			CORE_LOG_NAMED_INFO("parser", "Releasing unused parser ID: " << data->parser->getParser()->ID() << " (" << data->parser->getParser()->shortName() << ") for file " << data->parser->getPath());
		}
	}

	virtual SimpleInitializer * clone() const { return new SimpleInitializer(data->parser, idx); }
	virtual const bool isEqual(const IVariantInitializer &) const { return false; }

	utils::shared_ptr<const Parser> parser() const { return ((data != nullptr) ? data->parser : utils::shared_ptr<const Parser>());  }

private:
	CompoundDataPtr data;
	const int idx;
};

FileDataManager::FileDataManager() : skipUpdate(false)
{

}

FileDataManager::~FileDataManager()
{
	skipUpdate = true;
	auto mt = getMemoryDataManager()->transaction();
	auto tmpObjectsByFiles = objectsByFiles;
	for(auto it = tmpObjectsByFiles.begin(); it != tmpObjectsByFiles.end(); ++it){
		rawRemoveFile(it->first, mt);
	}
}

void FileDataManager::rawRemoveFile(const Filesystem::Path & file, const IMemoryDataManager::TransactionPtr & memTransaction)
{
	bool ok = true;
	VariantsList toRemove;
	//pobieramy obiekty do usuniecia z DM
	rawGetObjects(file, toRemove);
	//usu� obiekty z MemoryDM w transakcji
	for(auto it = toRemove.begin(); it != toRemove.end(); ++it){
		//TODO
		//przywrócić poprzedni jeśli był
		(*it)->setInitializer(VariantInitializerPtr());
		if(memTransaction->tryRemoveData(*it) == false){
			ok = false;
		}
	}

	//usu� plik	
	objectsByFiles.erase(file);
	if(ok == true){
		CORE_LOG_INFO("File " << file << " succesfully removed from manager");
	}else{
		CORE_LOG_DEBUG("There were some problems while removing data from memory data manager for file " << file << ". Not all objects have been removed properly");
	}
}

template<typename ParserT>
void initializeParsers(const IParserManagerReader::ParserPrototypes & parsers, const std::string & path, VariantsList & objects)
{
	auto pm = getParserManager();
	auto hm = getDataHierarchyManager();

	//je�li pliku nie ma dodaj go, stw�rz parsery i rozszerz dost�pne dane wraz z ich opisem
	for(auto parserIT = parsers.begin(); parserIT != parsers.end(); ++parserIT){
		// tworzymy współdzielone dane dla inicjalizatorów
		CompoundInitializer::CompoundDataPtr cid(new CompoundInitializer::CompoundData);
		cid->parser.reset(new ParserT((*parserIT)->create(), path));

		auto parserTypesMap = pm->parserTypes((*parserIT)->ID(), path);

		//zarejestrowanie obiekt�w i ich zwi�zku z parserem i typami danych
		for (auto objectIT = parserTypesMap.begin(); objectIT != parserTypesMap.end(); ++objectIT){
			//towrzymy taki obiekt
			auto ow = hm->createWrapper(objectIT->second);
			//aktualizuje ow dla CompoundInitializer
			cid->objects[objectIT->first] = ow;
			//inicjalizator na bazie parsera
			ow->setInitializer(VariantInitializerPtr(new CompoundInitializer(cid, objectIT->first)));
			objects.push_back(ow);
		}
	}
}

void FileDataManager::rawAddFile(const Filesystem::Path & file, const IMemoryDataManager::TransactionPtr & memTransaction)
{
	IParserManagerReader::ParserPrototypes sourceParsers;
	getParserManager()->sourceParsers(file.string(), sourceParsers);
	IParserManagerReader::ParserPrototypes streamParsers;
	getParserManager()->streamParsers(file.string(), streamParsers);

	//Z listy strumieniowych usuwam te kt�re s� na li�cie z w�asnym I/O - wierze �e zrobia to lepiej
	std::set<plugin::IParserConstPtr> sourceParsersSet;
	std::set<plugin::IParserConstPtr> streamParsersSet;

	sourceParsersSet.insert(sourceParsers.begin(), sourceParsers.end());
	streamParsersSet.insert(streamParsers.begin(), streamParsers.end());
	IParserManagerReader::ParserPrototypes sourcesLeft(sourceParsersSet.size());

	auto lastIT = std::set_difference(sourceParsersSet.begin(), sourceParsersSet.end(), streamParsersSet.begin(), streamParsersSet.end(), sourcesLeft.begin());

	sourcesLeft.erase(lastIT, sourcesLeft.end());
	//obiekty wyciągnięte z parserów
	VariantsList objects;
	//preferuje uzycie parser�w z w�asn� obs�ug� I/O - wierze �e zrobi� to maksymalnie wydajnie wg w�asnych zasad
	initializeParsers<StreameFileParser>(streamParsers, file.string(), objects);
	//teraz uzywam parser�w strumieniowych - sam dostarcz� im strumieni
	initializeParsers<FileParser>(sourcesLeft, file.string(), objects);

	if(objects.empty() == true){
		CORE_LOG_DEBUG("Any of known parsers did not provide any valid data for file: " << file);
	}else{

		int idx = 0;
		VariantsList objectsAdded;

		for(auto it = objects.begin(); it != objects.end(); ++it){
			if((*it)->existMetadata("core/name") == false){
				std::stringstream name;
				name << file.filename().string() << "_" << idx++;
				(*it)->setMetadata("core/name", name.str());
			}

			if((*it)->existMetadata("core/source") == false){
				(*it)->setMetadata("core/source", file.filename().string());
			}

			memTransaction->addData(*it);
			objectsAdded.push_back(*it);
		}

		// jak wszystko ok to zapami�tuje
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

void FileDataManager::rawGetObjects(const Filesystem::Path & file, ConstVariantsList & objects) const
{
	auto it = objectsByFiles.find(file);
    if (it != objectsByFiles.end()) {
	    objects.insert(objects.end(), it->second.begin(), it->second.end());
    } else {
        throw std::runtime_error("File not managed");
    }
}

void FileDataManager::rawGetObjects(const Filesystem::Path & file, VariantsList & objects)
{
	auto it = objectsByFiles.find(file);
    if (it != objectsByFiles.end()) {
	    objects.insert(objects.end(), it->second.begin(), it->second.end());
    } else {
        throw std::runtime_error("File not managed");
    }
}

void FileDataManager::rawGetObjects(const Filesystem::Path & file, VariantsCollection & objects) const
{
	auto it = objectsByFiles.find(file);

	for(auto objectIT = it->second.begin(); objectIT != it->second.end(); ++objectIT){
		if(objects.exactTypes() == true){
			if ((*objectIT)->data()->getTypeInfo() == objects.getTypeInfo()){
				objects.push_back(*objectIT);
			}
		}
		else if ((*objectIT)->data()->isSupported(objects.getTypeInfo()) == true){
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
			//rozwin�� obserwator�w aby si� jako� identyfikowali!! ewentualnie robi� to przez w�asn� implementacj� dostarczan� konretnym obiektom
			//(osobne interfejsy reader�w dla ka�dego elemnentu �adowanego do aplikacji - service, source, datasink, itp)
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

	{
		utils::Push<volatile bool> _skipUpdate(skipUpdate, true);
		//usu� plik
		rawRemoveFile(file, getMemoryDataManager()->transaction());
	}
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

	if(rawIsManaged(file) == true){
		throw std::runtime_error("File already managed");
	}

	if (getParserManager()->sourceIsAccepted(file.string()) == false){
		throw std::runtime_error("Source not accepted by any parser");
	}

	if(!Filesystem::pathExists(file) || !Filesystem::isRegularFile(file)){
		throw std::runtime_error("Source not exists, is not a file or is not accessible");
	}

	{
		utils::Push<volatile bool> _skipUpdate(skipUpdate, true);
		//dodaj plik
		rawAddFile(file, getMemoryDataManager()->transaction());
	}

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

void FileDataManager::observe(const IDataManagerReader::ChangeList & changes)
{

	if (skipUpdate == true){
		return;
	}

	std::set<core::Filesystem::Path> pathsToCheck;
	for (auto it = changes.begin(); it != changes.end(); ++it){
		if ((*it).modyfication == IDataManagerReader::REMOVE_OBJECT &&
			(*it).previousValue != nullptr &&
			(*it).previousValue->initializer() != nullptr){

			auto initializer = utils::dynamic_pointer_cast<const CompoundInitializer>((*it).previousValue->initializer());
			if (initializer != nullptr){
				auto parser = initializer->parser();
				if (parser != nullptr){
					const auto & path = parser->getPath();
					pathsToCheck.insert(path);
					objectsByFiles[path].remove(utils::const_pointer_cast<Variant>((*it).previousValue));
				}
			}
		}
	}

	for (auto it = pathsToCheck.begin(); it != pathsToCheck.end(); ++it){
		tryRemoveUnusedFile(*it);
	}
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

void FileDataManager::getObjects(const Filesystem::Path & file, ConstVariantsList & objects) const
{
	ScopedLock lock(sync);
	rawGetObjects(file, objects);
}

void FileDataManager::getObjects(const Filesystem::Path & file, VariantsCollection & objects) const
{
	ScopedLock lock(sync);
	rawGetObjects(file, objects);
}

void core::FileDataManager::tryRemoveUnusedFile(const core::Filesystem::Path & file)
{
	auto it = objectsByFiles.find(file);
	//sprawdzam czy nie moge juz usunąć pliku i notyfikowć o zmianie
	if (it != objectsByFiles.end() && it->second.empty() == true){
		CORE_LOG_INFO("Removing unused file " << file << " because of lack of delivered objects to memory data manager.");
		ChangeList changes;
		FileChange change;
		change.filePath = it->first;
		change.modyfication = IFileManagerReader::REMOVE_UNUSED_FILE;
		changes.push_back(change);

		objectsByFiles.erase(it);

		updateObservers(changes);
	}
}
