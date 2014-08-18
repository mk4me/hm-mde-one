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
#include <threadingUtils/IThread.h>
#include <threadingUtils/JobManager.h>
#include <threadingUtils/SynchronizationPolicies.h>
#include <corelib/BaseDataTypes.h>

namespace core
{
	class JobManager : public IJobManager
	{
	public:

		//! Konstruktor
		JobManager(threadingUtils::IThreadPtr managmentThread);

		//! Wirualny destruktor
		virtual ~JobManager();

		//! Metoda dodaje zadanie wraz z opisem zlecaj¹cego i opisem zadania do
		//! kolejki przetwarzania zwracaj¹c uchwyt do zadania
		virtual IJobPtr addJob(const std::string & who, const std::string & name,
			threadingUtils::IRunnablePtr runnable);

		virtual const bool execute(const std::string & who, const std::string & name,
			threadingUtils::FunctorRunnable::Functor f);

		//! \param workerThread W¹tek wykonuj¹cy czynnoœci w poolu
		void addWorkerThread(threadingUtils::IThreadPtr workerThread);
		//! \param workerThread Usuwany w¹tek wykonuj¹cy czynnoœci w poolu
		void removeWorkerThread(threadingUtils::IThreadPtr workerThread);

	private:
		//! Faktyczny jobManager
		utils::shared_ptr<threadingUtils::JobManager> jobManager_;
		//! Obiekt synchronizuj¹cy
		mutable threadingUtils::StrictSyncPolicy synch_;
	};
}

#endif	//	HEADER_GUARD_CORE__JOBMANAGER_H__
