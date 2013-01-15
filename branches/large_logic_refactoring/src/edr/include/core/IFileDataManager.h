/********************************************************************
    created:  2013/01/08
    created:  8:1:2013   20:10
    filename: IFileDataManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IFILEDATAMANAGER_H__
#define HEADER_GUARD___IFILEDATAMANAGER_H__

#include <vector>
#include <set>

#include <core/IMemoryDataManager.h>
#include <core/ObjectWrapper.h>
#include <core/Filesystem.h>

namespace plugin {

	//! Zbi�r operacji na plikach
	class IFileDataManagerOperations
	{
	public:
		//! \param files Lista plik�w kt�re zostan� usuni�te z aplikacji a wraz z nimi skojarzone parsery i dane
		virtual void removeFile(const core::Filesystem::Path & file) = 0;

		//! \param files Lista plik�w dla kt�rych zostan� utworzone parsery i z kt�rych wyci�gni�te dane
		//! b�da dost�pne poprzez DataMangera LENIWA INICJALIZACJA
		virtual void addFile(const core::Filesystem::Path & file) = 0;

		const bool tryAddFile(const core::Filesystem::Path & file)
		{
			bool ret = true;
			try{
				addFile(file);
			}catch(...){
				ret = false;
			}

			return ret;
		}

		const bool tryRemoveFile(const core::Filesystem::Path & file)
		{
			bool ret = true;
			try{
				removeFile(file);
			}catch(...){
				ret = false;
			}

			return ret;
		}
	};

	//! Interfejs dost�pu do danych plikowych i �adowania danych w aplikacji
	class IFileDataManager : public IFileDataManagerOperations
	{
	public:
		//! Typ transakcji - dzia�a jak RAII -> przy niszczeniu pr�buje "commitowa�" zmiany je�li nie by�o wcze�niej �adnych b��d�w
		typedef core::shared_ptr<IFileDataManagerOperations> TransactionPtr;
		//! \return Nowa transakcja
		virtual const TransactionPtr transaction() = 0;
		//! \return Nowa transakcja
		virtual const TransactionPtr transaction(const IMemoryDataManager::TransactionPtr & memoryTransaction) = 0;
	};

}

#endif	//	HEADER_GUARD___IFILEDATAMANAGER_H__
