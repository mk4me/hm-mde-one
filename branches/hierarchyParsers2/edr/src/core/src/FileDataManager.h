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
#include <corelib/IFileManagerReader.h>
#include <corelib/IFileDataManager.h>
#include <threading/SynchronizationPolicies.h>
#include <corelib/IParserManagerReader.h>

namespace core {

class FileDataManager : public IFileDataManager, public IFileManagerReader
{
private:

	class FileTransaction;
	friend class FileTransaction;

	class FileReaderTransaction;
	friend class FileReaderTransaction;

	//! Deklaracja wewn�trznej reprezentacji parsera, obudowauj�cej core::IParser
	class Parser;
	class FileParser;
	class StreameFileParser;

	friend class FileParser;
	friend class StreameFileParser;

	//! Wska�nik na parser.
	typedef utils::shared_ptr<Parser> ParserPtr;
	//! S�aby wska�nik na parser
	typedef utils::weak_ptr<Parser> ParserWeakPtr;

    typedef plugin::IParser::ParsedObjectsPtr HierarchyWithObjectsPtr;
	//! S�ownik aktualnie obs�ugiwanych plik�w i skojarzonych z nimi parser�w
	typedef std::multimap<Filesystem::Path, HierarchyWithObjectsPtr> ObjectsByFiles;

	typedef utils::RecursiveSyncPolicy SyncPolicy;
	typedef utils::ScopedLock<SyncPolicy> ScopedLock;

	typedef boost::function<ParserPtr(plugin::IParser*, const Filesystem::Path &)> ParserCreator;

private:

	//! Mapa parser�w wg plik�w
	ObjectsByFiles objectsByFiles;

	//! Obiekt na potrzeby synchronizacji
	mutable SyncPolicy sync;

	//! Obiekty obserwuj�ce stan DM
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

	/*void rawGetObjects(const Filesystem::Path & file, IHierarchyItemConstPtr& itm, ConstObjectsList & objects) const;

	void rawGetObjects(const Filesystem::Path & file, IHierarchyItemConstPtr& itm, ObjectWrapperCollection & objects) const;*/

	ParsedCollection rawGetObjects(const Filesystem::Path & file);
    ParsedConstCollection rawGetObjects(const Filesystem::Path & file) const;

	ObjectsByFiles initializeParsers(const IParserManagerReader::ParserPrototypes & parsers, const Filesystem::Path & path, const ParserCreator & parserCreator);

public:
	//IFileDataManager API

	//! \param files Lista plik�w kt�re zostan� usuni�te z aplikacji a wraz z nimi skojarzone parsery i dane
	virtual void removeFile(const Filesystem::Path & file);

	//! \param files Lista plik�w dla kt�rych zostan� utworzone parsery i z kt�rych wyci�gni�te dane
	//! b�da dost�pne poprzez DataMangera LENIWA INICJALIZACJA
	virtual void addFile(const Filesystem::Path & file);

	virtual const bool tryAddFile(const core::Filesystem::Path & file);

	virtual const bool tryRemoveFile(const core::Filesystem::Path & file);

	//! \return Nowa transakcja
	virtual IFileDataManager::TransactionPtr transaction();

public:

	//IFileManagerReader API

	virtual void addObserver(const FileObserverPtr & fileWatcher);
	virtual void removeObserver(const FileObserverPtr & fileWatcher);

	//! \param files Zbi�r plik�w ktrymi aktualnie zarz�dza ten DataManager
	virtual void getFiles(Files & files) const;

	//! \param file Plik k�try weryfikujemy czy jest zarz�dzany przez DM
	//! \return Prawda je�li plik jest zarz�dzany przez ten DM
	virtual const bool isManaged(const Filesystem::Path & file) const;

	////! \param files Zbior plik�w dla kt�rych chcemy pobra� list� obiekt�w
	////! \return Mapa obiekt�w wzgl�dem plik�w z kt�rych pochodza
	//virtual void getObjects(const Filesystem::Path & file, IHierarchyItemConstPtr& itm, ConstObjectsList & objects) const;

	////! \param files Zbior plik�w dla kt�rych chcemy pobra� list� obiekt�w
	////! \return Mapa obiekt�w wzgl�dem plik�w z kt�rych pochodza
	//virtual void getObjects(const Filesystem::Path & file, IHierarchyItemConstPtr& itm, ObjectWrapperCollection & objects) const;


    ////! \param files Zbior plik�w dla kt�rych chcemy pobra� list� obiekt�w
    ////! \return Mapa obiekt�w wzgl�dem plik�w z kt�rych pochodza
    virtual ParsedConstCollection getObjects(const Filesystem::Path & file) const;
    
	virtual IFileManagerReader::TransactionPtr transaction() const;
};

}

#endif	//	HEADER_GUARD___FILEDATAMANAGER_H__
