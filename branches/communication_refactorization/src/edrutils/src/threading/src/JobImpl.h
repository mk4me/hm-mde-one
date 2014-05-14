/********************************************************************
    created:  2013/06/24
    created:  24:6:2013   10:18
    filename: JobImpl.h
    author:   Mateusz Janiak
    
    purpose:  Implementacja funkcjonalnoœci zadania
*********************************************************************/
#ifndef HEADER_GUARD___JOBIMPL_H__
#define HEADER_GUARD___JOBIMPL_H__

#include <threading/Job.h>
#include "JobManagerImpl.h"
#include <threading/SynchronizationPolicies.h>

namespace utils
{
	class Job::JobImpl
	{
	private:
		//! W¹tek wykonuj¹cy zadanie
		IThreadPtr thread_;
		//! Status zadania
		Job::Status status_;
		//! Komunikat b³êdu
		std::string error_;
		//! Synchronizacja stanu zadania
		mutable RecursiveSyncPolicy sync_;
		//! Obiekt na którym czekaj¹ obserwuj¹cy a¿ zadanie siê zakoñczy
		StrictSyncPolicy waitSync_;

	public:

		//! Domyœlny konstruktor
		JobImpl();
		//! Destruktor
		~JobImpl();
		//! \return Stan zadania
		const Job::Status status() const;
		//! Metoda anuluj¹ca zadanie
		void cancel();
		//! Metoda czeka a¿ zadanie siê skoñczy (z b³êdem, ok lub zostanie anulowane)
		void join();
		//! Metoda wykonuj¹ca faktycznie zadanie - wo³a IRunnable::run()
		void execute(IRunnable * runnable);
		//! \return Opis ostatniego b³êdu
		const std::string error() const;

	private:
		//! \param status Nowy status zadania
		void setStatus(const Job::Status status);
		//! \param thread W¹tek obs³uguj¹cy to zadanie
		void setThread(const IThreadPtr thread);
	};
}

#endif	//	HEADER_GUARD___JOBIMPL_H__
