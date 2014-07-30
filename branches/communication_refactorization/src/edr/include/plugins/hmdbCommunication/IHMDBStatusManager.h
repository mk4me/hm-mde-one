/********************************************************************
	created:  2014/07/04	11:37:47
	filename: IHMDBStatusManager.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBSTATUSMANAGER_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBSTATUSMANAGER_H__

#include <plugins/hmdbCommunication/Types.h>
#include <plugins/hmdbCommunication/DataStatus.h>
#include <plugins/hmdbCommunication/ShallowCopy.h>
#include <utils/ITTransaction.h>

namespace hmdbCommunication
{

	class IHMDBStorage;
	class IHMDBStatusManager;

	class IHMDBStatusManagerOperations
	{
	public:
		//! Destruktor wirtualny
		virtual ~IHMDBStatusManagerOperations() {}

		//! \param dataType Typ danych
		//! \param id Identyfikator danych
		//! \return Status danych
		virtual const DataStatus dataStatus(const DataType dataType,
			const hmdbServices::ID id) const = 0;

		//! \param dataType Typ danych
		//! \param id Identyfikator danych
		//! \param dataStatus Status danych
		virtual void setDataStatus(const DataType dataType,
			const hmdbServices::ID id, const DataStatus dataStatus) = 0;

		//! \param dataType Typ danych
		//! \param id Identyfikator danych		
		virtual void removeDataStatus(const DataType dataType,
			const hmdbServices::ID id) = 0;

		//! \param dataType Typ danych
		virtual void removeDataStatus(const DataType dataType) = 0;
		
		//! Metoda próbuje aktualizaocwaæ statusu jeœli by³u jakieœ zmiany
		//! \param shallowCopy P³ytka kopia bêd¹ca map¹ hierarchii statusów
		virtual void tryUpdate(const ShallowCopyConstPtr shallowCopy) = 0;
		//! Metoda buduje od zera statusy na bazie p³ytkiej kopii bazy danych, StreamDataManager i Storage
		//! \param storage Storage z danymi
		//! \param shallowCopy P³ytka kopia dla której budujemy statusu
		virtual void rebuild(const IHMDBStorage * storage, const ShallowCopyConstPtr shallowCopy) = 0;
		//! \param shallowCopy P³ytka kopia bazy dnaych dla której budujemy statusy na bazie aktualnych statusów
		//! \return Manager statusów
		virtual IHMDBStatusManager * create(const ShallowCopyConstPtr shallowCopy) const = 0;
	};

	//! Interfejs udostêpniaj¹cy opis statusu danych
	class IHMDBStatusManager : public utils::ITTransaction<IHMDBStatusManagerOperations>
	{
	public:
		//! Destruktor wirtualny
		virtual ~IHMDBStatusManager() {}	
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBSTATUSMANAGER_H__