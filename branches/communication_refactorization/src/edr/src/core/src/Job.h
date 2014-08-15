/********************************************************************
	created:  2013/07/05
	created:  5:7:2013   10:34
	filename: Job.h
	author:   Mateusz Janiak

	purpose:  Faktyczna implementacja zadañ do przetworzenia
	*********************************************************************/
#ifndef HEADER_GUARD_CORE__JOB_H__
#define HEADER_GUARD_CORE__JOB_H__

#include <corelib/IJob.h>
#include <threadingUtils/SynchronizationPolicies.h>
#include <boost/atomic.hpp>

namespace core
{
	class JobManager;

	class Job : public IJob, public threadingUtils::IRunnable
	{
		friend class JobManager;

	public:

		//! Destruktor wirtualny
		virtual ~Job();

		//! \return Zadanie do przetworzenia
		virtual threadingUtils::IRunnablePtr runnable();
		//! \return Zadanie do przetworzenia
		virtual threadingUtils::IRunnableConstPtr runnable() const;
		//! \return Nazwa zleceniodawcy
		virtual const std::string & who() const;
		//! \return Nazwa zadania
		virtual const std::string & name() const;
		//! \param Status zadania
		virtual const Status status() const;
		//! Metoda czeka a¿ zadanie nie dobiegnie koñca lub zostanie anulowane
		virtual void wait();
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
			const threadingUtils::IRunnablePtr runnable);

		//! Metoda pozwala managerowi zwolnic czekanie na joba jesli go anuluje
		void unlock();
		//! Metoda pozwala managerowi zwolnic czekanie na joba jesli go anuluje
		void lock();
	private:

		threadingUtils::StrictSyncPolicy sync;
		//! Obiekt do czekania
		threadingUtils::ConditionVariable wait_;
		//! Zleceniodawca
		std::string who_;
		//! Nazwa zadania
		std::string name_;
		//! Status zadania
		boost::atomic<Status> status_;
		//! Faktyczne zadanie
		threadingUtils::IRunnablePtr runnable_;
	};
}

#endif	//	HEADER_GUARD_CORE__JOB_H__
