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

class FileTransaction;

class FileDataManager : public plugin::IFileDataManager, public plugin::IFileManagerReader
{

	friend class FileTransaction;

private:

	//! Deklaracja wewnêtrznej reprezentacji parsera, obudowauj¹cej core::IParser
	class Parser;

	//! WskaŸnik na parser.
	typedef shared_ptr<Parser> ParserPtr;
	//! S³aby wskaŸnik na parser
	typedef weak_ptr<Parser> ParserWeakPtr;
	//! S³ownik aktualnie obs³ugiwanych plików i skojarzonych z nimi parserów
	typedef std::map<Filesystem::Path, ObjectsList> ObjectsByFiles;

	typedef utils::RecursiveSyncPolicy SyncPolicy;
	typedef utils::ScopedLock<SyncPolicy> ScopedLock;

	typedef boost::function<ObjectWrapper::LazyInitializer(const ParserPtr &)> ParserInitializer;

private:

	//! Mapa parserów wg plików
	ObjectsByFiles objectsByFiles;

	//! Obiekt na potrzeby synchronizacji
	mutable SyncPolicy sync;

	//! Obiekty obserwuj¹ce stan DM
	std::list<FileObserverPtr> observers;

private:

	core::ObjectWrapper::LazyInitializer initStreamParser(const ParserPtr & streamParser);
	core::ObjectWrapper::LazyInitializer initSourceParser(const ParserPtr & sourceParser);

	void initializeDataWithStreamParser(ObjectWrapper & object, const ParserPtr & parser, plugin::IStreamParserCapabilities * streamCapabilities);
	void initializeDataWithSourceParser(ObjectWrapper & object, const ParserPtr & parser, plugin::ISourceParserCapabilities * sourceCapabilities);
	void initializeDataWithExternalInitializer(ObjectWrapper & object, const ObjectWrapper::LazyInitializer & li);

	void rawRemoveFile(const Filesystem::Path & file);

	void rawAddFile(const Filesystem::Path & file);

	const bool rawIsManaged(const Filesystem::Path & file) const;

	void updateObservers(const ChangeList & changes );

	void rawGetFiles(Files & files) const;

	void rawGetObjects(const Filesystem::Path & file, ConstObjectsList & objects) const;

	void rawGetObjects(const Filesystem::Path & file, ObjectWrapperCollection & objects) const;

	void initializeParsers(const plugin::IParserManagerReader::ParserPrototypes & parsers, const std::string & source, const ParserInitializer & parserInitializer, ObjectsList & objects);

public:
	//IFileDataManager API

	//! \param files Lista plików które zostan¹ usuniête z aplikacji a wraz z nimi skojarzone parsery i dane
	virtual void removeFile(const Filesystem::Path & file);

	//! \param files Lista plików dla których zostan¹ utworzone parsery i z których wyci¹gniête dane
	//! bêda dostêpne poprzez DataMangera LENIWA INICJALIZACJA
	virtual void addFile(const Filesystem::Path & file);
	//! \return Nowa transakcja
	virtual const TransactionPtr transaction();
	//! \return Nowa transakcja
	virtual const TransactionPtr transaction(const TransactionPtr & memoryTransaction);

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
};

}

#endif	//	HEADER_GUARD___FILEDATAMANAGER_H__