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
#include <mutex>
#include <corelib/IFileManagerReader.h>
#include <corelib/IFileDataManager.h>
#include <corelib/IDataManagerReader.h>
#include <corelib/IParserManagerReader.h>

namespace core {

	class FileDataManager : public IFileDataManager, public IFileManagerReader, public IDataManagerReader::IObjectObserver
	{
	private:
		//! Forward declaration
		class FileTransaction;
		friend class FileTransaction;
		//! Forward declaration
		class FileReaderTransaction;
		friend class FileReaderTransaction;

		//! Forward declaration
		class CompoundInitializer;		

		//! S�ownik aktualnie obsługiwanych plików i skojarzonych z nimi parserów
		typedef std::map<Filesystem::Path, ConstVariantsList> ObjectsByFiles;
		//! Mapowanie indeksu danych parsera do typu pod tym indeksem
		typedef std::map<int, utils::TypeInfo> ParserTypes;
		//! Mapowanie inicjalizatora do brakujących danych
		typedef std::map<utils::shared_ptr<CompoundInitializer>, ParserTypes> ObjectsByParser;
		//! Mapowanie plików do brakujących danych
		typedef std::map<Filesystem::Path, ObjectsByParser> MissingObjects;

		//! Polityka synchronizacji
		typedef std::recursive_mutex SyncPolicy;
		typedef std::lock_guard<SyncPolicy> ScopedLock;

	private:

		//! Mapa parserów wg plików
		ObjectsByFiles objectsByFiles;
		//! Dane wyładowane przez Memory DM
		MissingObjects missingObjects;

		//! Obiekt na potrzeby synchronizacji
		mutable SyncPolicy sync;

		//! Obiekty obserwujące stan DM
		std::list<FileObserverPtr> observers;

		//! Zmienna pozwalająca szybko opuscic update memory data managera
		volatile bool skipUpdate;

	private:

		template<typename ParserT>
		void initializeParsers(const IParserManagerReader::ParserPrototypes & parsers, const std::string & path, VariantsList & objects);

		void rawRemoveFile(const Filesystem::Path & file, const IMemoryDataManager::TransactionPtr & memTransaction);

		void rawAddFile(const Filesystem::Path & file, const IMemoryDataManager::TransactionPtr & memTransaction);

		void rawReloadFile(const Filesystem::Path & file, const bool compleately, const IMemoryDataManager::TransactionPtr & memTransaction);

		const bool rawIsManaged(const Filesystem::Path & file) const;

		const bool rawIsLoadedCompleately(const Filesystem::Path & file) const;

		void updateObservers(const ChangeList & changes);

		void rawGetFiles(Files & files) const;

		void rawGetObjects(const Filesystem::Path & file, ConstVariantsList & objects) const;

		void rawGetObjects(const Filesystem::Path & file, VariantsCollection & objects) const;

		void rawGetObjects(const Filesystem::Path & file, VariantsList & objects);

	public:
		//IFileDataManager API

		//! \param files Lista plik�w kt�re zostan� usuni�te z aplikacji a wraz z nimi skojarzone parsery i dane
		virtual void removeFile(const Filesystem::Path & file);

		//! \param files Lista plik�w dla kt�rych zostan� utworzone parsery i z kt�rych wyci�gni�te dane
		//! b�da dost�pne poprzez DataMangera LENIWA INICJALIZACJA
		virtual void addFile(const Filesystem::Path & file);

		//! \param file Ścieżka pliku do przeładowania
		//! \param complete Czy plik ma zostać całkowicie przeładowany czy tylko brakujące dane mają zostać doładowane
		virtual void reloadFile(const Filesystem::Path & file,
			const bool complete);


		virtual const bool tryAddFile(const core::Filesystem::Path & file);

		virtual const bool tryRemoveFile(const core::Filesystem::Path & file);

		//! \param file Ścieżka pliku do przeładowania
		//! \param complete Czy plik ma zostać całkowicie przeładowany czy tylko brakujące dane mają zostać doładowane
		//! \return Prawda jeśli plik pomyślnie przeładowano
		virtual const bool tryReloadFile(const Filesystem::Path & file,
			const bool complete);

		//! \return Nowa transakcja
		virtual const TransactionPtr transaction();

	public:

		//IFileManagerReader API

		virtual void addObserver(const FileObserverPtr & fileWatcher);
		virtual void removeObserver(const FileObserverPtr & fileWatcher);

		//! \param files Zbi�r plik�w ktrymi aktualnie zarz�dza ten DataManager
		virtual void getFiles(Files & files) const;

		//! \param file Plik k�try weryfikujemy czy jest zarz�dzany przez DM
		//! \return Prawda je�li plik jest zarz�dzany przez ten DM
		virtual const bool isManaged(const Filesystem::Path & file) const;

		//! \param file Plik kótry weryfikujemy czy jest w pełni załadowany
		//! \return Prawda jeśli plik jest w pełni załadowany
		virtual const bool isLoadedCompleately(const Filesystem::Path & file) const;

		//! \param files Zbior plik�w dla kt�rych chcemy pobra� list� obiekt�w
		//! \return Mapa obiekt�w wzgl�dem plik�w z kt�rych pochodza
		virtual void getObjects(const Filesystem::Path & file, ConstVariantsList & objects) const;

		//! \param files Zbior plik�w dla kt�rych chcemy pobra� list� obiekt�w
		//! \return Mapa obiekt�w wzgl�dem plik�w z kt�rych pochodza
		virtual void getObjects(const Filesystem::Path & file, VariantsCollection & objects) const;

		virtual const TransactionConstPtr transaction() const;

	public:

		//IDataManagerReader::IObjectObserver API
		virtual void observe(const IDataManagerReader::ChangeList & changes);

	public:

		FileDataManager();

		virtual ~FileDataManager();

	private:
		//! \param path Ścieżka pliku który weryfikujemy pod kątem dostępności danych,
		//! jeśli brak to usuwamy nieużywany plik
		void tryRemoveUnusedFile(const core::Filesystem::Path & file,
			ChangeList & changes);
	};


	template<typename ParserT>
	void FileDataManager::initializeParsers(const IParserManagerReader::ParserPrototypes & parsers, const std::string & path, VariantsList & objects)
	{
		auto pm = getParserManager();
		auto hm = getDataHierarchyManager();

		//je�li pliku nie ma dodaj go, stw�rz parsery i rozszerz dost�pne dane wraz z ich opisem
		for (auto parserIT = parsers.begin(); parserIT != parsers.end(); ++parserIT){
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
}

#endif	//	HEADER_GUARD___FILEDATAMANAGER_H__
