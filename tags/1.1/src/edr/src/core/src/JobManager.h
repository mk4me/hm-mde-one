/********************************************************************
    created:  2013/07/05
    created:  5:7:2013   11:42
    filename: JobManager.h
    author:   Mateusz Janiak
    
    purpose:  Implementacja managera zadañ
*********************************************************************/
#ifndef HEADER_GUARD_CORE__JOBMANAGER_H__
#define HEADER_GUARD_CORE__JOBMANAGER_H__

#include <corelib/IJobManager.h>
#include <threading/IThread.h>
#include <threading/JobManager.h>
#include <threading/SynchronizationPolicies.h>
#include <corelib/BaseDataTypes.h>

namespace core
{

	class JobManager : public IJobManager
	{
	public:

		//! Konstruktor
		JobManager(utils::IThreadPtr managmentThread);

		//! Wirualny destruktor
		virtual ~JobManager();

		//! Metoda dodaje zadanie wraz z opisem zlecaj¹cego i opisem zadania do
		//! kolejki przetwarzania zwracaj¹c uchwyt do zadania
		virtual IJobPtr addJob(const std::string & who, const std::string & name,
			utils::IRunnablePtr runnable);

		//! \param workerThread W¹tek wykonuj¹cy czynnoœci w poolu
		void addWorkerThread(utils::IThreadPtr workerThread);
		//! \param workerThread Usuwany w¹tek wykonuj¹cy czynnoœci w poolu
		void removeWorkerThread(utils::IThreadPtr workerThread);

	private:
		//! Faktyczny jobManager
		utils::shared_ptr<utils::JobManager> jobManager_;
		//! Obiekt synchronizuj¹cy
		mutable utils::StrictSyncPolicy synch_;
	};

}

#endif	//	HEADER_GUARD_CORE__JOBMANAGER_H__
