#include "CorePCH.h"
#include "FileDataManager.h"
#include "DataManager.h"
#include "RegisteredDataTypesManager.h"
#include "ApplicationCommon.h"
#include "ParserManager.h"
#include <utils/Push.h>
#include <atomic>
#include <fstream>

using namespace core;

class FileDataManager::FileTransaction : public IFileDataManager::ITransaction
{
private:

	typedef std::map<Filesystem::Path, IFileManagerReader::Change> FileModyfications;

private:
	IDataManager::TransactionPtr mdmTransaction;
	FileDataManager * fdm;
	std::atomic<bool> transactionRolledback;
	utils::shared_ptr<std::lock_guard<std::recursive_mutex>> lock;
	const bool oldSkipUpdate;
	FileModyfications modyfications;

public:
	FileTransaction(FileDataManager * fdm)
		: mdmTransaction(getDataManager()->transaction()),
		fdm(fdm), transactionRolledback(false),
		lock(new std::lock_guard<std::recursive_mutex>(fdm->sync)),
		oldSkipUpdate(fdm->skipUpdate)
	{
		fdm->skipUpdate = true;
	}

	~FileTransaction()
	{
		//wymuszam najpierw commit transakcji po stronie MemoryDataManagera
		//potem sam notyfikuje o zmianach w plikach
		mdmTransaction.reset();

		if (isRolledback() == false) {

			if (modyfications.empty() == false) {
				FileDataManager::ChangeList changes;
				for (auto it = modyfications.begin(); it != modyfications.end(); ++it) {
					FileDataManager::Change change;
					change.filePath = it->first;
					change.modyfication = it->second.modyfication;
					changes.push_back(change);
				}

				fdm->updateObservers(changes);
			}

			fdm->skipUpdate = oldSkipUpdate;
		}
	}

	virtual const bool isRolledback() const
	{
		return transactionRolledback;
	}

	virtual void rollback()
	{
		transactionRolledback = true;
		mdmTransaction->rollback();

		for (auto it = modyfications.begin(); it != modyfications.end(); ++it) {
			switch (it->second.modyfication) {
			case IFileManagerReader::ADD_FILE:
			{
												 VariantsList toRemove;
												 fdm->rawGetObjects(it->first, toRemove);
												 //usu� obiekty z MemoryDM w transakcji
												 for (auto IT = toRemove.begin(); IT != toRemove.end(); ++IT) {
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
													auto insRes = fdm->objectsByFiles.insert(FileDataManager::ObjectsByFiles::value_type(it->first, it->second.previousData));

													if (insRes.second == false) {
														insRes.first->second.insert(insRes.first->second.end(), it->second.previousData.begin(), it->second.previousData.end());
													}
			}
				break;
			}
		}

		fdm->skipUpdate = oldSkipUpdate;
		mdmTransaction.reset();
		lock.reset();
	}

	//! \param files Lista plik�w dla kt�rych zostan� utworzone parsery i z kt�rych wyci�gni�te dane
	//! b�da dost�pne poprzez DataMangera LENIWA INICJALIZACJA
	virtual void addFile(const Filesystem::Path & file)
	{
		verifyRollback();

		if (fdm->rawIsManaged(file) == true) {
			rollback();
			throw std::runtime_error("File transaction tried to add file already managed by manager");
		}

		if (!Filesystem::pathExists(file) || !Filesystem::isRegularFile(file)) {
			rollback();
			throw std::runtime_error("File transaction tried to add not existing or invalid file");
		}

		rawAddFile(file);
	}

	//! \param files Lista plik�w kt�re zostan� usuni�te z aplikacji a wraz z nimi skojarzone parsery i dane
	virtual void removeFile(const Filesystem::Path & file)
	{
		verifyRollback();

		if (fdm->rawIsManaged(file) == false) {
			rollback();
			throw std::runtime_error("File transaction tried to modify file not managed by manager");
		}

		rawRemoveFile(file);
	}

	virtual void reloadFile(const Filesystem::Path & file,
							const bool complete)
	{
		verifyRollback();

		if (fdm->rawIsManaged(file) == false) {
			rollback();
			throw std::runtime_error("File transaction tried to modify file not managed by manager");
		}

		rawReloadFile(file, complete);
	}


	virtual const bool tryAddFile(const Filesystem::Path & file)
	{
		if (transactionRolledback == true || fdm->rawIsManaged(file) == true || !Filesystem::pathExists(file) || !Filesystem::isRegularFile(file)) {
			return false;
		}

		rawAddFile(file);

		return true;
	}

	virtual const bool tryRemoveFile(const Filesystem::Path & file)
	{
		if (transactionRolledback == true || fdm->rawIsManaged(file) == false) {
			return false;
		}

		rawRemoveFile(file);

		return true;
	}

	virtual const bool tryReloadFile(const Filesystem::Path & file,
									 const bool complete)
	{
		if (transactionRolledback == true || fdm->rawIsManaged(file) == false) {
			return false;
		}

		rawReloadFile(file, complete);

		return true;
	}

	virtual void getFiles(Filesystem::FilesSet & files) const
	{
		verifyRollback();

		fdm->rawGetFiles(files);
	}

	virtual const bool isManaged(const Filesystem::Path & file) const
	{
		verifyRollback();

		return fdm->rawIsManaged(file);
	}

	virtual const bool isLoadedCompleately(const Filesystem::Path & file) const
	{
		verifyRollback();

		return fdm->rawIsLoadedCompleately(file);
	}

	virtual void getObjects(const Filesystem::Path & file, ConstVariantsList & objects) const
	{
		verifyRollback();

		fdm->rawGetObjects(file, objects);
	}

	virtual void getObjects(const Filesystem::Path & file, VariantsCollection & objects) const
	{
		verifyRollback();

		fdm->rawGetObjects(file, objects);
	}

private:

	inline void verifyRollback() const
	{
		if (isRolledback() == true) {
			throw std::runtime_error("File transaction already rolled-back");
		}
	}

	void rawAddFile(const Filesystem::Path & file)
	{
		//dodajemy dane do dm
		fdm->rawAddFile(file, mdmTransaction);
		//aktualizujemy liste zmian
		IFileManagerReader::Change mod;
		mod.modyfication = IFileManagerReader::ADD_FILE;
		fdm->getObjects(file, mod.currentData);
		modyfications.insert(FileModyfications::value_type(file, mod));
	}

	void rawRemoveFile(const Filesystem::Path & file)
	{
		IFileManagerReader::Change mod;
		mod.modyfication = IFileManagerReader::REMOVE_FILE;
		fdm->rawGetObjects(file, mod.previousData);
		//dodajemy dane do dm
		fdm->rawRemoveFile(file, mdmTransaction);
		//aktualizujemy liste zmian
		modyfications.insert(FileModyfications::value_type(file, mod));
	}

	void rawReloadFile(const Filesystem::Path & file, const bool complete)
	{
		//sprawdzam czy jest co robic
		if (complete == false && fdm->rawIsLoadedCompleately(file) == true) {
			return;
		}

		IFileManagerReader::Change mod;
		mod.modyfication = IFileManagerReader::RELOAD_FILE;
		fdm->rawGetObjects(file, mod.previousData);

		for (auto it = mod.previousData.begin(); it != mod.previousData.end(); ++it) {
			*it = (*it)->clone();
		}

		//przeładowuje dane
		fdm->rawReloadFile(file, complete, mdmTransaction);
		//pobieram dane po zmianach
		fdm->rawGetObjects(file, mod.currentData);
		//aktualizujemy liste zmian
		modyfications.insert(FileModyfications::value_type(file, mod));
	}
};

class FileDataManager::FileReaderTransaction : public IFileManagerReader::IOperations
{
public:
	FileReaderTransaction(FileDataManager * fdm) : fdm(fdm),
		lock(new std::lock_guard<std::recursive_mutex>(fdm->sync))
	{

	}

	~FileReaderTransaction()
	{

	}

public:

	virtual void getFiles(Filesystem::FilesSet & files) const
	{
		fdm->rawGetFiles(files);
	}

	virtual const bool isManaged(const Filesystem::Path & file) const
	{
		return fdm->rawIsManaged(file);
	}

	virtual const bool isLoadedCompleately(const Filesystem::Path & file) const
	{
		return fdm->rawIsLoadedCompleately(file);
	}

	virtual void getObjects(const Filesystem::Path & file, ConstVariantsList & objects) const
	{
		fdm->rawGetObjects(file, objects);
	}

	virtual void getObjects(const Filesystem::Path & file, VariantsCollection & objects) const
	{
		fdm->rawGetObjects(file, objects);
	}

private:
	FileDataManager * fdm;
	utils::shared_ptr<std::lock_guard<std::recursive_mutex>> lock;
};

namespace core {
//! Wewn�trzna reprezentacja parsera u�ywana przez DataManagera.
class FParser
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

private:
	//! Mo�e rzuca� wyj�tkami!
	virtual void _parseFile() = 0;

public:
	//! \param parser Faktyczny parser. To ten obiekt kontroluje jego
	//!     czas �ycia.
	//! \param resource Czy parser jest zwi�zany z zasobami sta�ymi?
	FParser(plugin::IParser* parser, const Filesystem::Path & path) :
		parser(parser), parsed(false), used(false), path(path)
	{
		UTILS_ASSERT(parser);
		UTILS_ASSERT(!path.empty());
	}

	//! Destruktor drukuj�cy wiadomo�� o wy�adowaniu pliku.
	virtual ~FParser()
	{
		if (isParsed()) {
			CORE_LOG_NAMED_DEBUG("parser", "Unloading parser for file: " << path);
		} else if (isUsed()) {
			CORE_LOG_NAMED_DEBUG("parser", "Unloading invalid parser for file: " << path);
		} else {
			CORE_LOG_NAMED_DEBUG("parser", "Unloading unused parser for file: " << path);
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

		if (core::Filesystem::pathExists(path) == false) {
			throw std::runtime_error("File not exist or not available to read");
		}

		CORE_LOG_NAMED_DEBUG("parser", "Parsing file: " << path);
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
		}
		catch (const std::exception& e) {
			CORE_LOG_NAMED_ERROR("parser", "Error during parsing file " << path << ": " << e.what());
		}
		catch (...) {
			CORE_LOG_NAMED_ERROR("parser", "UNKNOWN Error during parsing file " << path);
		}

		return ret;
	}

	inline void getObject(Variant& object, const VariantsVector::size_type idx) const
	{
		parser->getObject(object, idx);
	}
};
}

//! Wewn�trzna reprezentacja parsera u�ywana przez DataManagera.
class FileParser : public FParser
{
public:
	FileParser(plugin::IParser* parser, const Filesystem::Path & path) : FParser(parser, path), fileParser(nullptr)
	{

	}

	virtual ~FileParser()
	{

	}

private:
	virtual void _parseFile()
	{
		if (fileParser == nullptr) {
			fileParser = dynamic_cast<plugin::ISourceParser*>(getParser().get());
		}

		if (fileParser != nullptr) {
			fileParser->parse(getPath().string());
		} else {
			throw std::runtime_error("Parser is expected to implement plugin::ISourceParser interface but failed to cast");
		}
	}

private:
	//! Interfejs parsera plików
	plugin::ISourceParser * fileParser;
};

class StreameFileParser : public FParser
{
public:
	StreameFileParser(plugin::IParser* parser, const std::string & source) : FParser(parser, source), streamFileParser(nullptr)
	{

	}

	virtual ~StreameFileParser()
	{

	}

private:
	virtual void _parseFile()
	{
		if (streamFileParser == nullptr) {
			streamFileParser = dynamic_cast<plugin::IStreamParser*>(getParser().get());
		}

		if (streamFileParser != nullptr) {

			auto path = getPath().string();
			utils::shared_ptr<std::ifstream> localStream(new std::ifstream(path, std::ifstream::in | std::ifstream::binary));
			if (localStream->good() == true) {
				streamFileParser->parse(localStream, path);
			} else {
				throw std::runtime_error("Failed to initialize parser stream");
			}
		} else {
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
	SimpleInitializer(const utils::shared_ptr<FParser> & parser, const int idx)
		: parser_(parser), idx(idx) {}
	virtual ~SimpleInitializer() {}
	virtual void initialize(Variant * object) {
		parser_->parseFile();
		parser_->getObject(*object, idx);
		parser_->reset();
	}
	virtual IVariantInitializer * clone() const { return new SimpleInitializer(parser_, idx); }
	virtual const bool isEqual(const IVariantInitializer &) const { return false; }

private:
	utils::shared_ptr<FParser> parser_;
	const int idx;
};



FileDataManager::FileDataManager() : skipUpdate(false)
{

}

FileDataManager::~FileDataManager()
{
	skipUpdate = true;
	auto mt = getDataManager()->transaction();
	auto tmpObjectsByFiles = objectsByFiles;
	for (auto it = tmpObjectsByFiles.begin(); it != tmpObjectsByFiles.end(); ++it) {
		rawRemoveFile(it->first, mt);
	}
}

void FileDataManager::rawRemoveFile(const Filesystem::Path & file, const IDataManager::TransactionPtr & memTransaction)
{
	bool ok = true;
	VariantsList toRemove;
	//pobieramy obiekty do usuniecia z DM
	rawGetObjects(file, toRemove);
	//usu� obiekty z MemoryDM w transakcji
	for (auto it = toRemove.begin(); it != toRemove.end(); ++it) {
		//TODO
		//przywrócić poprzedni jeśli był
		(*it)->setInitializer(VariantInitializerPtr());
		if (memTransaction->tryRemoveData(*it) == false) {
			ok = false;
		}
	}

	//usu� plik	
	objectsByFiles.erase(file);
	missingObjects.erase(file);
	if (ok == true) {
		CORE_LOG_INFO("File " << file << " succesfully removed from manager");
	} else {
		CORE_LOG_DEBUG("There were some problems while removing data from memory data manager for file " << file << ". Not all objects have been removed properly");
	}
}

void FileDataManager::rawAddFile(const Filesystem::Path & file, const IDataManager::TransactionPtr & memTransaction)
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
	//obiekty wycią	gnięte z parserów
		VariantsList objects;
	//preferuje uzycie parser�w z w�asn� obs�ug� I/O - wierze �e zrobi� to maksymalnie wydajnie wg w�asnych zasad
	if (streamParsers.empty() == false) {
		initializeParsers<StreameFileParser>(streamParsers, file.string(), objects);
	}
	//teraz uzywam parser�w strumieniowych - sam dostarcz� im strumieni
	if (sourceParsers.empty() == false) {
		initializeParsers<FileParser>(sourcesLeft, file.string(), objects);
	}

	if (objects.empty() == true) {
		CORE_LOG_DEBUG("None of known parsers provide any valid data for file: " << file);
	} else {

		int idx = 0;
		ConstVariantsList objectsAdded;

		for (auto it = objects.begin(); it != objects.end(); ++it) {
			if ((*it)->existMetadata("core/name") == false) {
				std::stringstream name;
				name << file.filename().string() << "_" << idx++;
				(*it)->setMetadata("core/name", name.str());
			}

			if ((*it)->existMetadata("core/source") == false) {
				(*it)->setMetadata("core/source", file.filename().string());
			}

			memTransaction->addData(*it);
			objectsAdded.push_back(*it);
		}

		// jak wszystko ok to zapami�tuje
		objectsByFiles.insert(ObjectsByFiles::value_type(file, objectsAdded));
		CORE_LOG_INFO("File: " << file << " successfully loaded to manager. Extracted data : " << objects.size() << "| data loaded to memory data manager : " << objectsAdded.size());
	}
}

void FileDataManager::rawReloadFile(const Filesystem::Path & file, const bool compleately, const IDataManager::TransactionPtr & memTransaction)
{
	if (compleately == true) {

		auto it = objectsByFiles.find(file);

		for (auto oIT = it->second.begin(); oIT != it->second.end(); ++oIT) {
			auto ow = *oIT;
			if (ow->getRawPtr() != nullptr) {
				auto newOW = ow->clone();
				newOW->forceInitialize();
				memTransaction->updateData(ow, newOW);
			}
		}
	}

	auto it = missingObjects.find(file);

	if (it != missingObjects.end()) {

		auto dhm = core::getRegisteredDataTypesManager();

		VariantsList objectsAdded;

		for (auto iIT = it->second.begin(); iIT != it->second.end(); ++iIT) {
			//inicjalizator
			auto init = iIT->first;
			auto initDetails = init->details();
			for (auto dIT = iIT->second.begin(); dIT != iIT->second.end(); ++dIT) {
				//idx + typ
				auto ow = dhm->createWrapper(dIT->second);
				initDetails->objects[dIT->first] = ow;
				ow->setInitializer(init);
				memTransaction->addData(ow);
				objectsAdded.push_back(ow);
			}
		}

		objectsByFiles[file].insert(objectsByFiles[file].end(), objectsAdded.begin(), objectsAdded.end());
	}
}

const bool FileDataManager::rawIsManaged(const Filesystem::Path & file) const
{
	return objectsByFiles.find(file) != objectsByFiles.end();
}

const bool FileDataManager::rawIsLoadedCompleately(const Filesystem::Path & file) const
{
	return missingObjects.find(file) == missingObjects.end();
}

void FileDataManager::rawGetFiles(Filesystem::FilesSet & files) const
{
	for (const auto & obf : objectsByFiles) {
		files.insert(obf.first);
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

		for (auto fIT = it->second.begin(); fIT != it->second.end(); ++fIT) {
			objects.push_back(utils::const_pointer_cast<Variant>(*fIT));
		}

	} else {
		throw std::runtime_error("File not managed");
	}
}

void FileDataManager::rawGetObjects(const Filesystem::Path & file, VariantsCollection & objects) const
{
	auto it = objectsByFiles.find(file);

	for (auto objectIT = it->second.begin(); objectIT != it->second.end(); ++objectIT) {
		if (objects.exactTypes() == true) {
			if ((*objectIT)->data()->getTypeInfo() == objects.getTypeInfo()) {
				objects.push_back(*objectIT);
			}
		} else if ((*objectIT)->data()->isSupported(objects.getTypeInfo()) == true) {
			objects.push_back(*objectIT);
		}
	}
}

void FileDataManager::updateObservers(const ChangeList & changes)
{
	for (auto it = observers.begin(); it != observers.end(); ++it) {
		try {
			(*it)->observe(changes);
		}
		catch (...) {
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

	if (rawIsManaged(file) == false) {
		throw std::runtime_error("File not managed");
	}

	{
		utils::Push _skipUpdate(skipUpdate, true);
		//usu� plik
		rawRemoveFile(file, getDataManager()->transaction());
	}
	//notyfikuj
	ChangeList changes;
	Change change;
	change.filePath = file;
	change.modyfication = IFileManagerReader::REMOVE_FILE;
	changes.push_back(change);
	updateObservers(changes);
}

void FileDataManager::addFile(const Filesystem::Path & file)
{
	ScopedLock lock(sync);

	if (rawIsManaged(file) == true) {
		throw std::runtime_error("File already managed");
	}

	if (getParserManager()->sourceIsAccepted(file.string()) == false) {
		throw std::runtime_error("Source not accepted by any parser");
	}

	if (!Filesystem::pathExists(file) || !Filesystem::isRegularFile(file)) {
		throw std::runtime_error("Source not exists, is not a file or is not accessible");
	}

	{
		utils::Push _skipUpdate(skipUpdate, true);
		//dodaj plik
		rawAddFile(file, getDataManager()->transaction());
	}

	//notyfikuj
	ChangeList changes;
	Change change;
	change.filePath = file;
	change.modyfication = IFileManagerReader::ADD_FILE;
	changes.push_back(change);
	updateObservers(changes);
}

void FileDataManager::reloadFile(const Filesystem::Path & file, const bool complete)
{
	ScopedLock lock(sync);

	if (rawIsManaged(file) == false) {
		throw std::runtime_error("File not managed");
	}

	Change change;

	rawGetObjects(file, change.previousData);

	for (auto it = change.previousData.begin(); it != change.previousData.end(); ++it) {
		*it = (*it)->clone();
	}

	{
		utils::Push _skipUpdate(skipUpdate, true);
		//dodaj plik
		rawReloadFile(file, complete, getDataManager()->transaction());
	}

	rawGetObjects(file, change.currentData);

	//notyfikuj
	ChangeList changes;

	change.filePath = file;
	change.modyfication = IFileManagerReader::RELOAD_FILE;
	changes.push_back(change);
	updateObservers(changes);
}

const bool FileDataManager::tryAddFile(const core::Filesystem::Path & file)
{
	bool ret = true;
	try {
		addFile(file);
	}
	catch (...) {
		ret = false;
	}

	return ret;
}

const bool FileDataManager::tryRemoveFile(const core::Filesystem::Path & file)
{
	bool ret = true;
	try {
		removeFile(file);
	}
	catch (...) {
		ret = false;
	}

	return ret;
}

const bool FileDataManager::tryReloadFile(const core::Filesystem::Path & file,
										  const bool complete)
{
	bool ret = true;
	try {
		reloadFile(file, complete);
	}
	catch (...) {
		ret = false;
	}

	return ret;
}

const IFileDataManager::TransactionPtr FileDataManager::transaction()
{
	return IFileDataManager::TransactionPtr(new FileTransaction(this));
}

const IFileManagerReader::TransactionConstPtr FileDataManager::transaction() const
{
	return IFileManagerReader::TransactionConstPtr(new FileReaderTransaction(const_cast<FileDataManager*>(this)));
}

void FileDataManager::observe(const IDataManagerReader::ChangeList & changes)
{

	if (skipUpdate == true) {
		return;
	}

	std::set<core::Filesystem::Path> pathsToCheck;
	for (auto it = changes.begin(); it != changes.end(); ++it) {
		if ((*it).modyfication == IDataManagerReader::REMOVE_OBJECT &&
			(*it).previousValue != nullptr &&
			(*it).previousValue->initializer() != nullptr) {

			auto cinitializer = utils::dynamic_pointer_cast<const CompoundInitializer>((*it).previousValue->initializer());
			if (cinitializer != nullptr) {
				auto initializer = utils::const_pointer_cast<CompoundInitializer>(cinitializer);
				auto parser = initializer->innerParser();
				if (parser != nullptr) {
					const auto & path = parser->getPath();
					pathsToCheck.insert(path);
					objectsByFiles[path].remove(utils::const_pointer_cast<Variant>((*it).previousValue));

					const auto idx = initializer->objectIdx((*it).previousValue);

					if ((*it).previousValue->initialized() == false ||
						(*it).previousValue->getRawPtr() != nullptr) {

						missingObjects[path][initializer][idx] = (*it).previousValue->data()->getTypeInfo();
					} else {
						missingObjects[path][initializer].erase(idx);

						if (missingObjects[path][initializer].empty() == true) {
							missingObjects[path].erase(initializer);
							if (missingObjects[path].empty() == true) {
								missingObjects.erase(path);
							}
						}
					}
				}
			}
		}
	}

	ChangeList fchanges;

	for (auto it = pathsToCheck.begin(); it != pathsToCheck.end(); ++it) {
		tryRemoveUnusedFile(*it, fchanges);
	}

	if (changes.empty() == false) {
		updateObservers(fchanges);
	}
}

void FileDataManager::addObserver(const ObserverPtr & fileWatcher)
{
	ScopedLock lock(sync);
	if (std::find(observers.begin(), observers.end(), fileWatcher) != observers.end()) {
		throw std::runtime_error("Watcher already registered");
	}

	observers.push_back(fileWatcher);
}

void FileDataManager::removeObserver(const ObserverPtr & fileWatcher)
{
	ScopedLock lock(sync);
	auto it = std::find(observers.begin(), observers.end(), fileWatcher);
	if (it == observers.end()) {
		throw std::runtime_error("Watcher not registered");
	}

	observers.erase(it);
}

void FileDataManager::getFiles(Filesystem::FilesList & files) const
{
	ScopedLock lock(sync);
	for (const auto & obf : objectsByFiles) {
		files.push_back(obf.first);
	}
}

const bool FileDataManager::isManaged(const Filesystem::Path & file) const
{
	ScopedLock lock(sync);
	return rawIsManaged(file);
}

const bool FileDataManager::isLoadedCompleately(const Filesystem::Path & file) const
{
	ScopedLock lock(sync);
	return rawIsLoadedCompleately(file);
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

void core::FileDataManager::tryRemoveUnusedFile(const core::Filesystem::Path & file,
												ChangeList & changes)
{
	auto it = objectsByFiles.find(file);
	//sprawdzam czy nie moge juz usun�� pliku i notyfikowć o zmianie
	if (it != objectsByFiles.end() && it->second.empty() == true) {
		CORE_LOG_INFO("Removing unused file " << file << " because of lack of delivered objects to memory data manager.");
		Change change;
		change.filePath = it->first;
		change.modyfication = IFileManagerReader::REMOVE_UNUSED_FILE;
		changes.push_back(change);

		objectsByFiles.erase(it);
	}
}

core::FileDataManager::CompoundInitializer::CompoundDataPtr core::FileDataManager::CompoundInitializer::details()
{
	return data;
}
	utils::shared_ptr<const FParser> core::FileDataManager::CompoundInitializer::innerParser() const
	{
		return ((data != nullptr) ? data->parser : utils::shared_ptr<const FParser>());
	}

	const int core::FileDataManager::CompoundInitializer::objectIdx(const VariantConstPtr object) const
	{
		int ret = -1;

		if (data != nullptr) {

			for (auto it = data->objects.begin(); it != data->objects.end(); ++it) {
				if (it->second.lock() == object) {
					ret = it->first;
					break;
				}
			}
		}

		return ret;
	}

	const plugin::IParserPtr core::FileDataManager::CompoundInitializer::parser() const
	{
		plugin::IParserPtr ret;

		if (data != nullptr && data->parser != nullptr) {
			ret = data->parser->getParser();
		}

		return ret;
	}

	const bool core::FileDataManager::CompoundInitializer::isEqual(const IVariantInitializer &) const
	{
		return false;
	}

	IVariantInitializer * core::FileDataManager::CompoundInitializer::clone() const
	{
		return new ::SimpleInitializer(data->parser, idx);
	}

	void core::FileDataManager::CompoundInitializer::initialize(Variant * object)
	{
		auto mt = getDataManager()->transaction();
		std::list<int> toDelete;
		if (data->parser->tryParse() == false) {
			CORE_LOG_NAMED_INFO("parser", "Parser " << data->parser->getParser()->ID() << " (" << data->parser->getParser()->shortName() << ") failed for file " << data->parser->getPath());
			for (auto it = data->objects.begin(); it != data->objects.end(); ++it) {
				auto ow = it->second.lock();
				if (ow != nullptr && ow->data()->getRawPtr() == nullptr) {
					toDelete.push_back(it->first);
				}
			}
		} else {
			for (auto it = data->objects.begin(); it != data->objects.end(); ++it) {
				auto ow = it->second.lock();
				if (ow != nullptr) {
					if (ow->initialized() == false) {
						data->parser->getObject(*ow, it->first);
						if (ow->data()->getRawPtr() == nullptr) {
							CORE_LOG_NAMED_INFO("parser", "Object of type " << ow->data()->getTypeInfo().name()
												<< " not initialized properly by parser ID: " << data->parser->getParser()->ID()
												<< " (" << data->parser->getParser()->shortName() << ") for file " << data->parser->getPath());
							toDelete.push_back(it->first);
						}
					}
				} else {
					toDelete.push_back(it->first);
				}
			}
		}

		data->parser->reset();

		for (auto it = toDelete.begin(); it != toDelete.end(); ++it) {
			auto oit = data->objects.find(*it);
			mt->tryRemoveData(oit->second.lock());
			data->objects.erase(oit);
		}

		if (data->objects.empty() == true) {
			CORE_LOG_NAMED_INFO("parser", "Releasing unused parser ID: " << data->parser->getParser()->ID() << " (" << data->parser->getParser()->shortName() << ") for file " << data->parser->getPath());
		}
	}

	core::FileDataManager::CompoundInitializer::~CompoundInitializer()
	{

	}

	core::FileDataManager::CompoundInitializer::CompoundInitializer(const CompoundDataPtr & data, const int idx) : data(data), idx(idx)
	{

	}

