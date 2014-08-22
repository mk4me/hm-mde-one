/********************************************************************
    created:  2013/01/08
    created:  8:1:2013   22:51
    filename: IFileManagerReader.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IFILEMANAGERREADER_H__
#define HEADER_GUARD___IFILEMANAGERREADER_H__

#include <threadingUtils/ITTransaction.h>
#include <corelib/Filesystem.h>
#include <corelib/BaseDataTypes.h>
#include <corelib/VariantsCollection.h>

namespace core {

	class IFileManagerReaderOperations
	{
	public:
		//! Destruktor wirtualny
		virtual ~IFileManagerReaderOperations() {}

		//! \param files Zbi�r plik�w ktrymi aktualnie zarz�dza ten DataManager
		virtual void getFiles(Files & files) const = 0;

		//! \param file Plik k�try weryfikujemy czy jest zarz�dzany przez DM
		//! \return Prawda je�li plik jest zarz�dzany przez ten DM
		virtual const bool isManaged(const Filesystem::Path & file) const = 0;

		//! \param file Plik k�try weryfikujemy czy jest w pe�ni za�adowany
		//! \return Prawda je�li plik jest w pe�ni za�adowany
		virtual const bool isLoadedCompleately(const Filesystem::Path & file) const = 0;

		//! \param file Plik dla kt�ego pobieramy dane
		//! \param objects [out] Lista danych dla pliku
		virtual void getObjects(const Filesystem::Path & file, ConstVariantsList & objects) const = 0;

		//! \param file Plik dla kt�ego pobieramy dane
		//! \param objects [out] Kolekcja danych pliku
		virtual void getObjects(const Filesystem::Path & file, VariantsCollection & objects) const = 0;
	};

	//! Zapewnia dost�p do danych plikowych aplikacji, pozwala obserwowa� zmiany z nimi zwi�zane
	class IFileManagerReader : public threadingUtils::ITReadableTransaction<IFileManagerReaderOperations>
	{
	public:
		//! Typ operacji na plikach
		enum ModificationType {
			ADD_FILE,				//! Operacja dodawania pliku
			REMOVE_FILE,			//! Operacja usuni�cia pliku	
			RELOAD_FILE,			//! Operacja prze�adowania pliku (ca�ego lub dodania tylko brakujacych danych wy�adowanych z poziomu MemoryDM)
			REMOVE_UNUSED_FILE		//! Operacja usuni�cia pliku w wyniku usuni�cia jego wszystkich obiekt�w ( w wyniku niepomy�lnych inicjalizacji, innych operacji u�ytkownik�w na MemoryDataManager)			
		};

		//! Obiekt opisuj�cy zmian� w DM
		struct FileChange
		{
			Filesystem::Path filePath;		//! Plik kt�ry zmieniamy
			ModificationType modyfication;	//! Typ zmiany na plikach
			ConstVariantsList previousData;
			ConstVariantsList currentData;
		};

		//! Agregat zmian w DM
		typedef std::list<FileChange> ChangeList;

		//! Interfejs obserwatora zmian
		typedef IChangesObserver<ChangeList> IFileObserver;

		//! Wska�nik na obiek obserwuj�cy zmiany
		typedef utils::shared_ptr<IFileObserver> FileObserverPtr;

	public:

		//! Destrutkor wirtualny
		virtual ~IFileManagerReader() {}

		//! \param fileWatcher Dodawany obserwator managera plik�w
		virtual void addObserver(const FileObserverPtr & fileWatcher) = 0;
		//! \param fileWatcher Usuwany obserwator managera plik�w
		virtual void removeObserver(const FileObserverPtr & fileWatcher) = 0;
	};

}

#endif	//	HEADER_GUARD___IFILEMANAGERREADER_H__
