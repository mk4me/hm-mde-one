/********************************************************************
    created:  2013/01/08
    created:  8:1:2013   22:51
    filename: IFileManagerReader.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IFILEMANAGERREADER_H__
#define HEADER_GUARD___IFILEMANAGERREADER_H__

#include <core/Filesystem.h>
#include <core/ObjectWrapper.h>
#include <core/ObjectWrapperCollection.h>

namespace plugin {

	class IFileManagerReaderOperations
	{
	public:
		//! \param files Zbiór plików ktrymi aktualnie zarz¹dza ten DataManager
		virtual void getFiles(core::Files & files) const = 0;

		//! \param file Plik kótry weryfikujemy czy jest zarz¹dzany przez DM
		//! \return Prawda jeœli plik jest zarz¹dzany przez ten DM
		virtual const bool isManaged(const core::Filesystem::Path & file) const = 0;

		//! \param files Zbior plików dla których chcemy pobraæ listê obiektów
		//! \return Mapa obiektów wzglêdem plików z których pochodza
		virtual void getObjects(const core::Filesystem::Path & file, core::ConstObjectsList & objects) const = 0;

		//! \param files Zbior plików dla których chcemy pobraæ listê obiektów
		//! \return Mapa obiektów wzglêdem plików z których pochodza
		virtual void getObjects(const core::Filesystem::Path & file, core::ObjectWrapperCollection & objects) const = 0;
	};

	//! Zapewnia dostêp do danych plikowych aplikacji, pozwala obserwowaæ zmiany z nimi zwi¹zane
	class IFileManagerReader : public IFileManagerReaderOperations
	{
	public:
		//! Typ operacji na plikach
		enum ModificationType {
			ADD_FILE,		//! Operacja dodawania pliku
			REMOVE_FILE,		//! Operacja usuniêcia pliku			
			REMOVE_UNUSED_FILE		//! Operacja usuniêcia pliku w wyniku usuniêcia jego wszystkich obiektów ( w wyniku niepomyœlnych inicjalizacji, innych operacji u¿ytkowników na MemoryDataManager)			
		};

		//! Obiekt opisuj¹cy zmianê w DM
		struct FileChange
		{
			core::Filesystem::Path filePath;		//! Plik który zmieniamy
			ModificationType modyfication;	//! Typ zmiany na plikach
		};

		//! Agregat zmian w DM
		typedef std::list<FileChange> ChangeList;

		//! Interfejs obserwatora zmian
		class IFileObserver
		{
		public:
			//! \param changes Lista zmian na plikach
			virtual void observe(const ChangeList & changes) = 0;
		};

		//! WskaŸnik na obiek obserwuj¹cy zmiany
		typedef core::shared_ptr<IFileObserver> FileObserverPtr;

		typedef core::shared_ptr<IFileManagerReaderOperations> TransactionPtr;

	public:

		virtual void addObserver(const FileObserverPtr & fileWatcher) = 0;
		virtual void removeObserver(const FileObserverPtr & fileWatcher) = 0;

		virtual TransactionPtr transaction() const = 0;
	};

}

#endif	//	HEADER_GUARD___IFILEMANAGERREADER_H__
