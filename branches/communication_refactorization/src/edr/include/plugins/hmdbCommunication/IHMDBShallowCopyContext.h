/********************************************************************
	created:  2014/07/25	13:06:13
	filename: IHMDBShallowCopyContext.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBSHALLOWCOPYCONTEXT_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBSHALLOWCOPYCONTEXT_H__

#include <plugins/hmdbCommunication/ShallowCopy.h>
#include <plugins/hmdbCommunication/IHMDBRemoteContext.h>
#include <plugins/hmdbCommunication/IHMDBStatusManager.h>
#include <corelib/IStreamDataManager.h>
#include <list>

namespace hmdbCommunication
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
		virtual const ShallowCopyConstPtr shallowCopy() const = 0;
		//! \param shallowCopy P�ytka kopia bazy danych
		virtual void setShallowCopy(const ShallowCopyConstPtr shallowCopy) = 0;
		//! \return Manager status�w
		virtual const IHMDBStatusManager * dataStatusManager() const = 0;
		//! \return Manager status�w
		virtual IHMDBStatusManager * dataStatusManager() = 0;
	};

	class IHMDBShallowCopyLocalContext
	{
	public:
		//! Destruktor wirtualny
		virtual ~IHMDBShallowCopyLocalContext() {}
		//! \param type Typ danych o kt�re pytamy
		//! \param id Identyfikator danych
		//! \param recursive Czy pobieramy dane w d�
		//! \return Dane domenowe za�adowane dla tego typu
		virtual const core::ConstVariantsList data(const DataType type,
			const hmdbServices::ID id, const bool recursive = true) const = 0;
		//! \param type Typ danych kt�re chcemy za�adowa�
		//! \param ids Identyfikatory danych
		//! \param shallowCopy P�ytka kopia wzgl�dem kt�rej dzia�amy
		//! \return Identyfikatory plik�w kt�re za�adowano
		virtual const bool load(const DataType type,
			const hmdbServices::ID id) = 0;

		//! Metoda �aduje wszystkie dane
		virtual const bool loadAll() = 0;

		//! \param type Typ danych kt�re chcemy wy�adowa�
		//! \param ids Identyfikatory danych
		//! \param shallowCopy P�ytka kopia wzgl�dem kt�rej wy�adowujemy
		//! \return Identyfikatory plik�w kt�re wy�adowano
		virtual const bool unload(const DataType type,
			const hmdbServices::ID id) = 0;

		//! Metoda wy�adowuje wszystkie dane
		virtual const bool unloadAll() = 0;

		virtual IHMDBShallowCopyDataContext * shallowCopyContext() = 0;
		virtual const IHMDBShallowCopyDataContext * shallowCopyContext() const = 0;

		virtual IHMDBLocalContext * localContext() = 0;
		virtual const IHMDBLocalContext * localContext() const = 0;
	};

	class IHMDBShallowCopyRemoteContext
	{
	public:
		//! Destruktor wirtualny
		virtual ~IHMDBShallowCopyRemoteContext() {}
		//! \param type Typ danych kt�re chcemy za�adowa�
		//! \param id Identyfikator danych	
		//! \param recursive Czy pobieramy wszystkie dane w d� hierarchii
		//! \param forceDownload Czy wymuszamy �ci�ganie wszystkich danych czy tylko brakuj�cych
		//! \return Przygotowany transfer danych
		virtual const IHMDBRemoteContext::CompoundOperationPtr prepareDownload(const DataType type,
			const hmdbServices::ID id, const bool recursive = true, const bool forceDownload = false) = 0;
		//! \param shallowCopy P�ytka kopia bazy danych
		//! \param incrementalBranchShallowCopy Przyrostowa kopia bazy danych
		virtual void synchronize(const ShallowCopyConstPtr shallowCopy,
			const IncrementalBranchShallowCopyConstPtr incrementalBranchShallowCopy = IncrementalBranchShallowCopyConstPtr()) = 0;

		//! \param downloadOperations Operacje �ci�gania
		virtual void synchronize(const IHMDBRemoteContext::CompoundOperationPtr downloadOperations) = 0;

		virtual IHMDBShallowCopyDataContext * shallowCopyContext() = 0;
		virtual const IHMDBShallowCopyDataContext * shallowCopyContext() const = 0;

		virtual IHMDBRemoteContext * remoteContext() = 0;
		virtual const IHMDBRemoteContext * remoteContext() const = 0;
	};

	class IHMDBShallowCopyContext
	{
	public:
		//! Destruktor wirtualny
		virtual ~IHMDBShallowCopyContext() {}

		virtual IHMDBShallowCopyDataContext * shallowCopyDataContext() = 0;
		virtual const IHMDBShallowCopyDataContext * shallowCopyDataContext() const = 0;

		virtual IHMDBShallowCopyLocalContext * shallowCopyLocalContext() = 0;
		virtual const IHMDBShallowCopyLocalContext * shallowCopyLocalContext() const = 0;

		virtual IHMDBShallowCopyRemoteContext * shallowCopyRemoteContext() = 0;
		virtual const IHMDBShallowCopyRemoteContext * shallowCopyRemoteContext() const = 0;
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBSHALLOWCOPYCONTEXT_H__