/********************************************************************
	created:  2014/07/04	13:00:31
	filename: HMDBStatusManager.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__HMDBSTATUSMANAGER_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__HMDBSTATUSMANAGER_H__

#include <plugins/hmdbCommunication/IHMDBStatusManager.h>
#include <threadingUtils/SynchronizationPolicies.h>
#include <map>

namespace hmdbCommunication
{

	class IHMDBStorage;

	class HMDBStatusManager : public IHMDBStatusManager
	{
	private:

		typedef threadingUtils::ScopedLock<threadingUtils::RecursiveSyncPolicy> ScopedLock;

		class HMDBStatusManagerTransaction;
		friend class HMDBStatusManagerTransaction;

	public:

		typedef std::map<hmdbServices::ID, DataStatus> StatusByID;
		typedef std::map<DataType, StatusByID> Statuses;
		typedef std::map<DataType, std::set<hmdbServices::ID>> UpdateData;

	public:
		HMDBStatusManager();
		virtual ~HMDBStatusManager();

		virtual const TransactionPtr transaction();
		virtual const TransactionConstPtr transaction() const;

		//! \param dataType Typ danych
		//! \param id Identyfikator danych
		//! \return Status danych
		virtual const DataStatus dataStatus(const DataType dataType,
			const hmdbServices::ID id) const;

		//! \param dataType Typ danych
		//! \param id Identyfikator danych
		//! \param dataStatus Status danych
		virtual void setDataStatus(const DataType dataType,
			const hmdbServices::ID id, const DataStatus dataStatus);

		virtual void updateDataStatus(const DataType dataType,
			const hmdbServices::ID id, const DataStatus dataStatus);

		//! \param dataType Typ danych
		//! \param id Identyfikator danych		
		virtual void removeDataStatus(const DataType dataType,
			const hmdbServices::ID id);


		virtual void removeDataStatus(const DataType dataType);

		virtual void tryUpdate(const ShallowCopyConstPtr shallowCopy);

		virtual void rebuild(const IHMDBStorageConstPtr storage, const ShallowCopyConstPtr shallowCopy);

	private:

		const UpdateData fullUpdateData(const ShallowCopyConstPtr shallowCopy);

		//! \param dataType Typ danych
		//! \param id Identyfikator danych
		//! \return Status danych
		const DataStatus rawDataStatus(const DataType dataType,
			const hmdbServices::ID id) const;

		//! \param dataType Typ danych
		//! \param id Identyfikator danych
		//! \param dataStatus Status danych
		void rawSetDataStatus(const DataType dataType,
			const hmdbServices::ID id, const DataStatus dataStatus);

		void rawUpdateDataStatus(const DataType dataType,
			const hmdbServices::ID id, const DataStatus dataStatus);

		//! \param dataType Typ danych
		//! \param id Identyfikator danych		
		void rawRemoveDataStatus(const DataType dataType,
			const hmdbServices::ID id);

		void rawRemoveDataStatus(const DataType dataType);

		void refresh(const DataType type, hmdbServices::ID id);

		void rawTryUpdate(const ShallowCopyConstPtr shallowCopy);

		void rawRebuild(const IHMDBStorageConstPtr storage, const ShallowCopyConstPtr shallowCopy);

	private:
		//! Statusy danych
		Statuses statuses;
		//! Obiekt synchronizuj�cy
		mutable threadingUtils::RecursiveSyncPolicy synch_;
		//! Mapa aktualizacji
		UpdateData updateData;
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__HMDBSTATUSMANAGER_H__