/********************************************************************
    created:  2013/01/14
    created:  14:1:2013   16:13
    filename: FileDataManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___FILEDATAMANAGER_H__
#define HEADER_GUARD___FILEDATAMANAGER_H__

#include <map>
#include <list>
#include <core/IFileManagerReader.h>
#include <core/IFileDataManager.h>
#include <utils/SynchronizationPolicies.h>
#include <core/IParserManagerReader.h>

namespace core {

class FileDataManager : public IFileDataManager, public IFileManagerReader
{
private:

	class FileTransaction;
	friend class FileTransaction;

	class FileReaderTransaction;
	friend class FileReaderTransaction;

	//! Deklaracja wewnêtrznej reprezentacji parsera, obudowauj¹cej core::IParser
	class Parser;
	class FileParser;
	class StreameFileParser;

	friend class FileParser;
	friend class StreameFileParser;

	//! WskaŸnik na parser.
	typedef shared_ptr<Parser> ParserPtr;
	//! S³aby wskaŸnik na parser
	typedef weak_ptr<Parser> ParserWeakPtr;
	//! S³ownik aktualnie obs³ugiwanych plików i skojarzonych z nimi parserów
	typedef std::map<Filesystem::Path, ObjectsList> ObjectsByFiles;

	typedef utils::RecursiveSyncPolicy SyncPolicy;
	typedef utils::ScopedLock<SyncPolicy> ScopedLock;

	typedef boost::function<ParserPtr(plugin::IParser*, const Filesystem::Path &)> ParserCreator;

private:

	//! Mapa parserów wg plików
	ObjectsByFiles objectsByFiles;

	//! Obiekt na potrzeby synchronizacji
	mutable SyncPolicy sync;

	//! Obiekty obserwuj¹ce stan DM
	std::list<FileObserverPtr> observers;

	const static ParserCreator streamParserCreator;
	const static ParserCreator sourceParserCreator;

private:

	static ParserPtr createSourceParser(plugin::IParser * parser, const Filesystem::Path & path);
	static ParserPtr createStreamParser(plugin::IParser * parser, const Filesystem::Path & path);

	void initializeDataWithParser(ObjectWrapper & object, const ParserPtr & parser);
	void initializeDataWithExternalInitializer(ObjectWrapper & object, const ObjectWrapper::LazyInitializer & li, const ParserPtr & parser);
	void verifyAndRemoveUninitializedParserObjects(const ParserPtr & parser);

	void rawRemoveFile(const Filesystem::Path & file, const IMemoryDataManager::TransactionPtr & memTransaction);

	void rawAddFile(const Filesystem::Path & file, const IMemoryDataManager::TransactionPtr & memTransaction);

	const bool rawIsManaged(const Filesystem::Path & file) const;

	void updateObservers(const ChangeList & changes );

	void rawGetFiles(Files & files) const;

	void rawGetObjects(const Filesystem::Path & file, ConstObjectsList & objects) const;

	void rawGetObjects(const Filesystem::Path & file, ObjectWrapperCollection & objects) const;

	void rawGetObjects(const Filesystem::Path & file, ObjectsList & objects);

	void initializeParsers(const IParserManagerReader::ParserPrototypes & parsers, const Filesystem::Path & path, const ParserCreator & parserCreator, ObjectsList & objects);

public:
	//IFileDataManager API

	//! \param files Lista plików które zostan¹ usuniête z aplikacji a wraz z nimi skojarzone parsery i dane
	virtual void removeFile(const Filesystem::Path & file);

	//! \param files Lista plików dla których zostan¹ utworzone parsery i z których wyci¹gniête dane
	//! bêda dostêpne poprzez DataMangera LENIWA INICJALIZACJA
	virtual void addFile(const Filesystem::Path & file);

	virtual const bool core::IFileDataManagerOperations::tryAddFile(const core::Filesystem::Path & file);

	virtual const bool core::IFileDataManagerOperations::tryRemoveFile(const core::Filesystem::Path & file);

	//! \return Nowa transakcja
	virtual IFileDataManager::TransactionPtr transaction();

public:

	//IFileManagerReader API

	virtual void addObserver(const FileObserverPtr & fileWatcher);
	virtual void removeObserver(const FileObserverPtr & fileWatcher);

	//! \param files Zbiór plików ktrymi aktualnie zarz¹dza ten DataManager
	virtual void getFiles(Files & files) const;

	//! \param file Plik kótry weryfikujemy czy jest zarz¹dzany przez DM
	//! \return Prawda jeœli plik jest zarz¹dzany przez ten DM
	virtual const bool isManaged(const Filesystem::Path & file) const;

	//! \param files Zbior plików dla których chcemy pobraæ listê obiektów
	//! \return Mapa obiektów wzglêdem plików z których pochodza
	virtual void getObjects(const Filesystem::Path & file, ConstObjectsList & objects) const;

	//! \param files Zbior plików dla których chcemy pobraæ listê obiektów
	//! \return Mapa obiektów wzglêdem plików z których pochodza
	virtual void getObjects(const Filesystem::Path & file, ObjectWrapperCollection & objects) const;

	virtual IFileManagerReader::TransactionPtr transaction() const;
};

}

#endif	//	HEADER_GUARD___FILEDATAMANAGER_H__