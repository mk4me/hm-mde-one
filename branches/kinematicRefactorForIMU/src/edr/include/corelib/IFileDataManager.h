/********************************************************************
    created:  2013/01/08
    created:  8:1:2013   20:10
    filename: IFileDataManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IFILEDATAMANAGER_H__
#define HEADER_GUARD___IFILEDATAMANAGER_H__

#include <corelib/ITransaction.h>
#include <corelib/IFileManagerReader.h>
#include <corelib/IMemoryDataManager.h>
#include <corelib/BaseDataTypes.h>
#include <corelib/Filesystem.h>
#include <corelib/IMemoryDataManager.h>

namespace core {

	//! Zbiór operacji na plikach
	class IFileDataManagerOperations
	{
	public:
		//! Destruktor wirtualny
		virtual ~IFileDataManagerOperations() {}

		//! \param file Plik do usuniêcia
		//! Rzuca wyj¹tkiem jeœli coœ siê nie powiedzie
		virtual void removeFile(const Filesystem::Path & file) = 0;

		//! \param file Œcie¿ka pliku do dodania
		//! Rzuca wyj¹tkiem jeœli coœ siê nie powiedzie
		virtual void addFile(const Filesystem::Path & file) = 0;

		//! \param file Œcie¿ka pliku do prze³adowania
		//! \param complete Czy plik ma zostaæ ca³kowicie prze³adowany czy tylko brakuj¹ce dane maj¹ zostaæ do³adowane
		virtual void reloadFile(const Filesystem::Path & file,
			const bool complete = false) = 0;

		//! \param file Œcie¿ka do dodawanego pliku
		//! \return Prawda jeœli plik pomyœlnie dodano
		virtual const bool tryAddFile(const Filesystem::Path & file) = 0;
		//! \param file Œcie¿ka do usuwanego pliku
		//! \return Prawda jeœli plik pomyœlnie usuniêto
		virtual const bool tryRemoveFile(const Filesystem::Path & file) = 0;
		//! \param file Œcie¿ka pliku do prze³adowania
		//! \param complete Czy plik ma zostaæ ca³kowicie prze³adowany czy tylko brakuj¹ce dane maj¹ zostaæ do³adowane
		//! \return Prawda jeœli plik pomyœlnie prze³adowano
		virtual const bool tryReloadFile(const Filesystem::Path & file,
			const bool complete = false) = 0;
	};

	//! Interfejs dostêpu do danych plikowych i ³adowania danych w aplikacji
	class IFileDataManager : public IFileDataManagerOperations
	{
	public:
		//! Interfejs transakcji dla managera plików
		class IFileDataManagerTransaction : public ITransaction, public IFileDataManagerOperations, public IFileManagerReaderOperations
		{
		public:
			//! Destruktor wirtualny
			virtual ~IFileDataManagerTransaction() {}
		};

		//! Destruktor wirtualny
		virtual ~IFileDataManager() {}

		//! Typ transakcji - dzia³a jak RAII -> przy niszczeniu próbuje "commitowaæ" zmiany jeœli nie by³o wczeœniej ¿adnych b³êdów
		typedef utils::shared_ptr<IFileDataManagerTransaction> TransactionPtr;
		//! \param mtransaction Transakcja MDM w ramach któej realizujemy funkcjonalnoœæ FDM
		//! \return Nowa transakcja
		virtual const TransactionPtr transaction() = 0;
	};

}

#endif	//	HEADER_GUARD___IFILEDATAMANAGER_H__
