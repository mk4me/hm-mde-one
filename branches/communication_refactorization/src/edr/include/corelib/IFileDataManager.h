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

	//! Zbi�r operacji na plikach
	class IFileDataManagerOperations
	{
	public:
		//! Destruktor wirtualny
		virtual ~IFileDataManagerOperations() {}

		//! \param file Plik do usuni�cia
		//! Rzuca wyj�tkiem je�li co� si� nie powiedzie
		virtual void removeFile(const Filesystem::Path & file) = 0;

		//! \param file �cie�ka pliku do dodania
		//! Rzuca wyj�tkiem je�li co� si� nie powiedzie
		virtual void addFile(const Filesystem::Path & file) = 0;

		//! \param file �cie�ka pliku do prze�adowania
		//! \param complete Czy plik ma zosta� ca�kowicie prze�adowany czy tylko brakuj�ce dane maj� zosta� do�adowane
		virtual void reloadFile(const Filesystem::Path & file,
			const bool complete = false) = 0;

		//! \param file �cie�ka do dodawanego pliku
		//! \return Prawda je�li plik pomy�lnie dodano
		virtual const bool tryAddFile(const Filesystem::Path & file) = 0;
		//! \param file �cie�ka do usuwanego pliku
		//! \return Prawda je�li plik pomy�lnie usuni�to
		virtual const bool tryRemoveFile(const Filesystem::Path & file) = 0;
		//! \param file �cie�ka pliku do prze�adowania
		//! \param complete Czy plik ma zosta� ca�kowicie prze�adowany czy tylko brakuj�ce dane maj� zosta� do�adowane
		//! \return Prawda je�li plik pomy�lnie prze�adowano
		virtual const bool tryReloadFile(const Filesystem::Path & file,
			const bool complete = false) = 0;
	};

	//! Interfejs dost�pu do danych plikowych i �adowania danych w aplikacji
	class IFileDataManager : public IFileDataManagerOperations
	{
	public:
		//! Interfejs transakcji dla managera plik�w
		class IFileDataManagerTransaction : public ITransaction, public IFileDataManagerOperations, public IFileManagerReaderOperations
		{
		public:
			//! Destruktor wirtualny
			virtual ~IFileDataManagerTransaction() {}
		};

		//! Destruktor wirtualny
		virtual ~IFileDataManager() {}

		//! Typ transakcji - dzia�a jak RAII -> przy niszczeniu pr�buje "commitowa�" zmiany je�li nie by�o wcze�niej �adnych b��d�w
		typedef utils::shared_ptr<IFileDataManagerTransaction> TransactionPtr;
		//! \param mtransaction Transakcja MDM w ramach kt�ej realizujemy funkcjonalno�� FDM
		//! \return Nowa transakcja
		virtual const TransactionPtr transaction() = 0;
	};

}

#endif	//	HEADER_GUARD___IFILEDATAMANAGER_H__
