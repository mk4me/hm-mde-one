/********************************************************************
	created:  2014/07/25	13:06:13
	filename: IHMDBShallowCopyContext.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBSHALLOWCOPYCONTEXT_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBSHALLOWCOPYCONTEXT_H__

#include <plugins/hmdbCommunication/ShallowCopy.h>
#include <plugins/hmdbCommunication/IHMDBLocalContext.h>
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
		//! \return Aktualna p³ytka kopia bazy danych
		virtual const ShallowCopyConstPtr shallowCopy() const = 0;
		//! \param shallowCopy P³ytka kopia bazy danych
		virtual void setShallowCopy(const ShallowCopyConstPtr shallowCopy) = 0;
		//! \return Manager statusów
		virtual const IHMDBStatusManagerConstPtr dataStatusManager() const = 0;
		//! \return Manager statusów
		virtual const IHMDBStatusManagerPtr dataStatusManager() = 0;
	};

	DEFINE_SMART_POINTERS(IHMDBShallowCopyDataContext);

	class IHMDBShallowCopyLocalContext
	{
	public:
		//! Destruktor wirtualny
		virtual ~IHMDBShallowCopyLocalContext() {}
		//! \param type Typ danych o które pytamy
		//! \param id Identyfikator danych
		//! \param recursive Czy pobieramy dane w dó³
		//! \return Dane domenowe za³adowane dla tego typu
		virtual const core::ConstVariantsList data(const DataType type,
			const hmdbServices::ID id, const bool recursive = true) const = 0;
		//! \param type Typ danych które chcemy za³adowaæ
		//! \param ids Identyfikatory danych
		//! \param shallowCopy P³ytka kopia wzglêdem której dzia³amy
		//! \return Identyfikatory plików które za³adowano
		virtual const bool load(const DataType type,
			const hmdbServices::ID id) = 0;

		//! Metoda ³aduje wszystkie dane
		virtual const bool loadAll() = 0;

		//! \param type Typ danych które chcemy wy³adowaæ
		//! \param ids Identyfikatory danych
		//! \param shallowCopy P³ytka kopia wzglêdem której wy³adowujemy
		//! \return Identyfikatory plików które wy³adowano
		virtual const bool unload(const DataType type,
			const hmdbServices::ID id) = 0;

		//! Metoda wy³adowuje wszystkie dane
		virtual const bool unloadAll() = 0;

		virtual const IHMDBShallowCopyDataContextPtr shallowCopyContext() = 0;
		virtual const IHMDBShallowCopyDataContextConstPtr shallowCopyContext() const = 0;

		virtual const IHMDBLocalContextPtr localContext() = 0;
		virtual const IHMDBLocalContextConstPtr localContext() const = 0;
	};

	DEFINE_SMART_POINTERS(IHMDBShallowCopyLocalContext);

	class IHMDBShallowCopyRemoteContext
	{
	public:
		//! Destruktor wirtualny
		virtual ~IHMDBShallowCopyRemoteContext() {}
		//! \param type Typ danych które chcemy za³adowaæ
		//! \param id Identyfikator danych	
		//! \param recursive Czy pobieramy wszystkie dane w dó³ hierarchii
		//! \param forceDownload Czy wymuszamy œci¹ganie wszystkich danych czy tylko brakuj¹cych
		//! \return Przygotowany transfer danych
		virtual const std::list<IHMDBRemoteContext::DownloadOperationPtr> prepareDownload(const DataType type,
			const hmdbServices::ID id, const bool recursive = true, const bool forceDownload = false) = 0;
		//! \param shallowCopy P³ytka kopia bazy danych
		//! \param incrementalBranchShallowCopy Przyrostowa kopia bazy danych
		virtual void synchronize(const ShallowCopyConstPtr shallowCopy,
			const IncrementalBranchShallowCopyConstPtr incrementalBranchShallowCopy = IncrementalBranchShallowCopyConstPtr()) = 0;

		//! \param downloadOperations Operacje œci¹gania
		virtual void synchronize(const IHMDBRemoteContext::SynchronizeOperationPtr downloadOperations) = 0;

		virtual const IHMDBShallowCopyDataContextPtr shallowCopyContext() = 0;
		virtual const IHMDBShallowCopyDataContextConstPtr shallowCopyContext() const = 0;

		virtual const IHMDBRemoteContextPtr remoteContext() = 0;
		virtual const IHMDBRemoteContextConstPtr remoteContext() const = 0;
	};

	DEFINE_SMART_POINTERS(IHMDBShallowCopyRemoteContext);

	class IHMDBShallowCopyContext
	{
	public:
		//! Destruktor wirtualny
		virtual ~IHMDBShallowCopyContext() {}

		virtual const IHMDBShallowCopyDataContextPtr shallowCopyDataContext() = 0;
		virtual const IHMDBShallowCopyDataContextConstPtr shallowCopyDataContext() const = 0;

		virtual const IHMDBShallowCopyLocalContextPtr shallowCopyLocalContext() = 0;
		virtual const IHMDBShallowCopyLocalContextConstPtr shallowCopyLocalContext() const = 0;

		virtual const IHMDBShallowCopyRemoteContextPtr shallowCopyRemoteContext() = 0;
		virtual const IHMDBShallowCopyRemoteContextConstPtr shallowCopyRemoteContext() const = 0;
	};

	DEFINE_SMART_POINTERS(IHMDBShallowCopyContext);
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBSHALLOWCOPYCONTEXT_H__