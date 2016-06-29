/********************************************************************
	created:  2014/07/25	13:06:13
	filename: IHMDBShallowCopyContext.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_medusaDatabase__IHMDBSHALLOWCOPYCONTEXT_H__
#define __HEADER_GUARD_medusaDatabase__IHMDBSHALLOWCOPYCONTEXT_H__

#include <plugins/medusaDatabase/ShallowCopy.h>
#include <plugins/medusaDatabase/IHMDBLocalContext.h>
#include <plugins/medusaDatabase/IHMDBRemoteContext.h>
#include <plugins/medusaDatabase/IHMDBStatusManager.h>
#include <corelib/IStreamDataManager.h>
#include <list>

namespace medusaDatabase
{
	class IHMDBStatusManager;
	class IHMDBLocalContext;
	class IHMDBRemoteContext;

	class IHMDBShallowCopyDataContext
	{
	public:
		//! Destruktor wirtualny
		virtual ~IHMDBShallowCopyDataContext() {}
		//! \return Aktualna p�ytka kopia bazy danych
		virtual ShallowCopyConstPtr shallowCopy() const = 0;
		//! \param shallowCopy P�ytka kopia bazy danych
		virtual void setShallowCopy(const ShallowCopyConstPtr shallowCopy) = 0;
		//! \return Manager status�w
		virtual IHMDBStatusManagerConstPtr dataStatusManager() const = 0;
		//! \return Manager status�w
		virtual IHMDBStatusManagerPtr dataStatusManager() = 0;
	};

	DEFINE_SMART_POINTERS(IHMDBShallowCopyDataContext);

	class IHMDBShallowCopyLocalContext
	{
	public:
		//! Destruktor wirtualny
		virtual ~IHMDBShallowCopyLocalContext() {}
		//! \param type Typ danych o kt�re pytamy
		//! \param id Identyfikator danych
		//! \param recursive Czy pobieramy dane w d�
		//! \return Dane domenowe za�adowane dla tego typu
		virtual core::ConstVariantsList data(const DataType type,
			const hmdbServices::ID id, const bool recursive = true) const = 0;
		//! \param type Typ danych kt�re chcemy za�adowa�
		//! \param ids Identyfikatory danych		
		//! \return Czy uda�o si� za�adowa�
		virtual bool load(const DataType type,
			const hmdbServices::ID id) = 0;

		//! Metoda �aduje wszystkie dane
		virtual bool loadAll() = 0;

		//! \param type Typ danych kt�re chcemy wy�adowa�
		//! \param id Identyfikator danych		
		//! \return Czy uda�o si� wy�adowa�
		virtual bool unload(const DataType type,
			const hmdbServices::ID id) = 0;

		//! Metoda wy�adowuje wszystkie dane
		virtual bool unloadAll() = 0;

		//! \param type Typ danych kt�re chcemy wy�adowa�
		//! \param id Identyfikator danych		
		//! \return Czy dane za�adwoane
		/*
		virtual const bool isLoaded(const DataType type,
			const hmdbServices::ID id) const = 0;*/

		virtual IHMDBShallowCopyDataContextPtr shallowCopyContext() = 0;
		virtual IHMDBShallowCopyDataContextConstPtr shallowCopyContext() const = 0;

		virtual IHMDBLocalContextPtr localContext() = 0;
		virtual IHMDBLocalContextConstPtr localContext() const = 0;
	};

	DEFINE_SMART_POINTERS(IHMDBShallowCopyLocalContext);

	class IHMDBShallowCopyRemoteContext
	{
	public:

		//! Interfejs realizuj�cy operacj� synchronizacji perspektywy danych
		class ISynchronizeOperation : public IHMDBRemoteContext::IOperation
		{
		public:
			//! Destruktor wirtualny
			virtual ~ISynchronizeOperation() {}
			//! \return Pe�na p�ytka kopia bazy danych - ca�a perspektywa dostepnych danych
			virtual ShallowCopyConstPtr shallowCopy() const = 0;
			//! \return Pe�na p�ytka kopia bazy danych - ca�a perspektywa dostepnych danych
			virtual IncrementalBranchShallowCopyConstPtr incrementalBranchShallowCopy() const = 0;
		};

		//! Wska�nik dla operacji synchronizacji
		typedef utils::shared_ptr<ISynchronizeOperation> SynchronizeOperationPtr;
		//! Wska�nik dla operacji synchronizacji
		typedef utils::shared_ptr<const ISynchronizeOperation> SynchronizeOperationConstPtr;

	public:
		//! Destruktor wirtualny
		virtual ~IHMDBShallowCopyRemoteContext() {}
		//! \param type Typ danych kt�re chcemy za�adowa�
		//! \param id Identyfikator danych	
		//! \param recursive Czy pobieramy wszystkie dane w d� hierarchii
		//! \param forceDownload Czy wymuszamy �ci�ganie wszystkich danych czy tylko brakuj�cych
		//! \return Przygotowany transfer danych
		virtual std::list<IHMDBRemoteContext::DownloadOperationPtr> prepareDownload(const DataType type,
			const hmdbServices::ID id, const bool recursive = true, const bool forceDownload = false) = 0;
		//! \return Operacja synchronizacji
		virtual SynchronizeOperationPtr prepareSynchronization(IHMDBStoragePtr storage) = 0;
		//! \param shallowCopy P�ytka kopia bazy danych
		//! \param incrementalBranchShallowCopy Przyrostowa kopia bazy danych
		virtual void synchronize(const ShallowCopyConstPtr shallowCopy,
			const IncrementalBranchShallowCopyConstPtr incrementalBranchShallowCopy = IncrementalBranchShallowCopyConstPtr()) = 0;

		//! \param downloadOperations Operacje �ci�gania
		virtual void synchronize(const SynchronizeOperationPtr downloadOperations) = 0;

		virtual IHMDBShallowCopyDataContextPtr shallowCopyContext() = 0;
		virtual IHMDBShallowCopyDataContextConstPtr shallowCopyContext() const = 0;

		virtual IHMDBRemoteContextPtr remoteContext() = 0;
		virtual IHMDBRemoteContextConstPtr remoteContext() const = 0;
	};

	DEFINE_SMART_POINTERS(IHMDBShallowCopyRemoteContext);

	class IHMDBShallowCopyContext
	{
	public:
		//! Destruktor wirtualny
		virtual ~IHMDBShallowCopyContext() {}

		virtual IHMDBShallowCopyDataContextPtr shallowCopyDataContext() = 0;
		virtual IHMDBShallowCopyDataContextConstPtr shallowCopyDataContext() const = 0;

		virtual IHMDBShallowCopyLocalContextPtr shallowCopyLocalContext() = 0;
		virtual IHMDBShallowCopyLocalContextConstPtr shallowCopyLocalContext() const = 0;

		virtual IHMDBShallowCopyRemoteContextPtr shallowCopyRemoteContext() = 0;
		virtual IHMDBShallowCopyRemoteContextConstPtr shallowCopyRemoteContext() const = 0;
	};

	DEFINE_SMART_POINTERS(IHMDBShallowCopyContext);
}

#endif	// __HEADER_GUARD_medusaDatabase__IHMDBSHALLOWCOPYCONTEXT_H__
