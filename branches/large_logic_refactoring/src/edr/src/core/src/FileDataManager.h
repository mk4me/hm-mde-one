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

	//! Deklaracja wewn�trznej reprezentacji parsera, obudowauj�cej core::IParser
	class Parser;

	//! Wska�nik na parser.
	typedef shared_ptr<Parser> ParserPtr;
	//! S�aby wska�nik na parser
	typedef weak_ptr<Parser> ParserWeakPtr;
	//! S�ownik aktualnie obs�ugiwanych plik�w i skojarzonych z nimi parser�w
	typedef std::map<Filesystem::Path, ObjectsList> ObjectsByFiles;

	typedef utils::RecursiveSyncPolicy SyncPolicy;
	typedef utils::ScopedLock<SyncPolicy> ScopedLock;

	typedef boost::function<ObjectWrapper::LazyInitializer(const ParserPtr &)> ParserInitializer;

private:

	//! Mapa parser�w wg plik�w
	ObjectsByFiles objectsByFiles;

	//! Obiekt na potrzeby synchronizacji
	mutable SyncPolicy sync;

	//! Obiekty obserwuj�ce stan DM
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

	//! \param files Lista plik�w kt�re zostan� usuni�te z aplikacji a wraz z nimi skojarzone parsery i dane
	virtual void removeFile(const Filesystem::Path & file);

	//! \param files Lista plik�w dla kt�rych zostan� utworzone parsery i z kt�rych wyci�gni�te dane
	//! b�da dost�pne poprzez DataMangera LENIWA INICJALIZACJA
	virtual void addFile(const Filesystem::Path & file);
	//! \return Nowa transakcja
	virtual const TransactionPtr transaction();
	//! \return Nowa transakcja
	virtual const TransactionPtr transaction(const TransactionPtr & memoryTransaction);

public:

	//IFileManagerReader API

	virtual void addObserver(const FileObserverPtr & fileWatcher);
	virtual void removeObserver(const FileObserverPtr & fileWatcher);

	//! \param files Zbi�r plik�w ktrymi aktualnie zarz�dza ten DataManager
	virtual void getFiles(Files & files) const;

	//! \param file Plik k�try weryfikujemy czy jest zarz�dzany przez DM
	//! \return Prawda je�li plik jest zarz�dzany przez ten DM
	virtual const bool isManaged(const Filesystem::Path & file) const;

	//! \param files Zbior plik�w dla kt�rych chcemy pobra� list� obiekt�w
	//! \return Mapa obiekt�w wzgl�dem plik�w z kt�rych pochodza
	virtual void getObjects(const Filesystem::Path & file, ConstObjectsList & objects) const;

	//! \param files Zbior plik�w dla kt�rych chcemy pobra� list� obiekt�w
	//! \return Mapa obiekt�w wzgl�dem plik�w z kt�rych pochodza
	virtual void getObjects(const Filesystem::Path & file, ObjectWrapperCollection & objects) const;
};

}

#endif	//	HEADER_GUARD___FILEDATAMANAGER_H__