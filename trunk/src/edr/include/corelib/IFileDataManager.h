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

namespace core {

	//! Zbi�r operacji na plikach
	class IFileDataManagerOperations
	{
	public:
		//! Destruktor wirtualny
		virtual ~IFileDataManagerOperations() {}

		//! \param files Lista plik�w kt�re zostan� usuni�te z aplikacji a wraz z nimi skojarzone parsery i dane
		//! Rzuca wyj�tkiem je�li co� si� nie powiedzie
		virtual void removeFile(const Filesystem::Path & file) = 0;

		//! \param files Lista plik�w dla kt�rych zostan� utworzone parsery i z kt�rych wyci�gni�te dane
		//! b�da dost�pne poprzez DataMangera LENIWA INICJALIZACJA
		//! Rzuca wyj�tkiem je�li co� si� nie powiedzie
		virtual void addFile(const Filesystem::Path & file) = 0;
		//! \param file �cie�ka do dodawanego pliku
		//! \return Prawda je�li plik pomy�lnie dodano
		virtual const bool tryAddFile(const Filesystem::Path & file) = 0;
		//! \param file �cie�ka do usuwanego pliku
		//! \return Prawda je�li plik pomy�lnie usuni�to
		virtual const bool tryRemoveFile(const Filesystem::Path & file) = 0;
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
		//! \return Nowa transakcja
		virtual TransactionPtr transaction() = 0;
	};

}

#endif	//	HEADER_GUARD___IFILEDATAMANAGER_H__
