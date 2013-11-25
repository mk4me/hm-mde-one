/********************************************************************
    created:  2013/07/05
    created:  5:7:2013   10:34
    filename: Job.h
    author:   Mateusz Janiak
    
    purpose:  Faktyczna implementacja zada� do przetworzenia
*********************************************************************/
#ifndef HEADER_GUARD_CORE__JOB_H__
#define HEADER_GUARD_CORE__JOB_H__

#include <corelib/IJob.h>
#include <threading/SynchronizationPolicies.h>

namespace core
{

	class JobManager;

	class Job : public IJob, public utils::IRunnable
	{

		friend class JobManager;

	public:

		//! Destruktor wirtualny
		virtual ~Job();

		//! \return Zadanie do przetworzenia
		virtual utils::IRunnablePtr runnable();
		//! \return Zadanie do przetworzenia
		virtual utils::IRunnableConstPtr runnable() const;
		//! \return Nazwa zleceniodawcy
		virtual const std::string & who() const;
		//! \return Nazwa zadania
		virtual const std::string & name() const;
		//! \param Status zadania
		virtual const Status status() const;
		//! \param status Nowy status zadania
		void setStatus(const Status status);

		//! Wykonanie zadania
		virtual void run();

	private:

		//! Konstruktor
		//! \param who Zleceniodawca
		//! \param name Nazwa zadania
		//! \param runnable Faktyczne zadanie do wykonania
		Job(const std::string & who, const std::string & name,
			const utils::IRunnablePtr runnable);

	private:
		//! Obiekt synchronizuj�cy
		mutable utils::StrictSyncPolicy synch_;
		//! Zleceniodawca
		std::string who_;
		//! Nazwa zadania
		std::string name_;
		//! Status zadania
		Status status_;
		//! Faktyczne zadanie
		utils::IRunnablePtr runnable_;
	};

}

#endif	//	HEADER_GUARD_CORE__JOB_H__