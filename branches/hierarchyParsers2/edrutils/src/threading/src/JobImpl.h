/********************************************************************
    created:  2013/06/24
    created:  24:6:2013   10:18
    filename: JobImpl.h
    author:   Mateusz Janiak
    
    purpose:  Implementacja funkcjonalno�ci zadania
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
		//! W�tek wykonuj�cy zadanie
		IThreadPtr thread_;
		//! Status zadania
		Job::Status status_;
		//! Komunikat b��du
		std::string error_;
		//! Synchronizacja stanu zadania
		mutable RecursiveSyncPolicy sync_;
		//! Obiekt na kt�rym czekaj� obserwuj�cy a� zadanie si� zako�czy
		StrictSyncPolicy waitSync_;

	public:

		//! Domy�lny konstruktor
		JobImpl();
		//! Destruktor
		~JobImpl();
		//! \return Stan zadania
		const Job::Status status() const;
		//! Metoda anuluj�ca zadanie
		void cancel();
		//! Metoda czeka a� zadanie si� sko�czy (z b��dem, ok lub zostanie anulowane)
		void join();
		//! Metoda wykonuj�ca faktycznie zadanie - wo�a IRunnable::run()
		void execute(IRunnable * runnable);
		//! \return Opis ostatniego b��du
		const std::string error() const;

	private:
		//! \param status Nowy status zadania
		void setStatus(const Job::Status status);
		//! \param thread W�tek obs�uguj�cy to zadanie
		void setThread(const IThreadPtr thread);
	};
}

#endif	//	HEADER_GUARD___JOBIMPL_H__
