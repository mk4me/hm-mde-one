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
		//! \param files Zbi�r plik�w ktrymi aktualnie zarz�dza ten DataManager
		virtual void getFiles(core::Files & files) const = 0;

		//! \param file Plik k�try weryfikujemy czy jest zarz�dzany przez DM
		//! \return Prawda je�li plik jest zarz�dzany przez ten DM
		virtual const bool isManaged(const core::Filesystem::Path & file) const = 0;

		//! \param files Zbior plik�w dla kt�rych chcemy pobra� list� obiekt�w
		//! \return Mapa obiekt�w wzgl�dem plik�w z kt�rych pochodza
		virtual void getObjects(const core::Filesystem::Path & file, core::ConstObjectsList & objects) const = 0;

		//! \param files Zbior plik�w dla kt�rych chcemy pobra� list� obiekt�w
		//! \return Mapa obiekt�w wzgl�dem plik�w z kt�rych pochodza
		virtual void getObjects(const core::Filesystem::Path & file, core::ObjectWrapperCollection & objects) const = 0;
	};

	//! Zapewnia dost�p do danych plikowych aplikacji, pozwala obserwowa� zmiany z nimi zwi�zane
	class IFileManagerReader : public IFileManagerReaderOperations
	{
	public:
		//! Typ operacji na plikach
		enum ModificationType {
			ADD_FILE,		//! Operacja dodawania pliku
			REMOVE_FILE,		//! Operacja usuni�cia pliku			
			REMOVE_UNUSED_FILE		//! Operacja usuni�cia pliku w wyniku usuni�cia jego wszystkich obiekt�w ( w wyniku niepomy�lnych inicjalizacji, innych operacji u�ytkownik�w na MemoryDataManager)			
		};

		//! Obiekt opisuj�cy zmian� w DM
		struct FileChange
		{
			core::Filesystem::Path filePath;		//! Plik kt�ry zmieniamy
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

		//! Wska�nik na obiek obserwuj�cy zmiany
		typedef core::shared_ptr<IFileObserver> FileObserverPtr;

		typedef core::shared_ptr<IFileManagerReaderOperations> TransactionPtr;

	public:

		virtual void addObserver(const FileObserverPtr & fileWatcher) = 0;
		virtual void removeObserver(const FileObserverPtr & fileWatcher) = 0;

		virtual TransactionPtr transaction() const = 0;
	};

}

#endif	//	HEADER_GUARD___IFILEMANAGERREADER_H__
