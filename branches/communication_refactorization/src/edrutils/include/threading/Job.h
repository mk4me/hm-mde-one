/********************************************************************
    created:  2012/10/31
    created:  31:10:2012   12:55
    filename: Job.h
    author:   Mateusz Janiak
    
    purpose:  Interfejs klasy realizuj¹cej proste, szybkie i niezale¿ne
				zadania. Zadania te bêd¹ realizowane przez niezale¿ne w¹tki.
				Jeœli wystêpuje miêdzy nimi synchronizacja nale¿y samemu
				o to zadbaæ, maj¹c na uwadze mo¿liwoœæ spowodowania
				dead locka zadañ!! Implementuje interfejs IRunnable
				- mo¿na wiêc odpalaæ bezpoœrednio na w¹tkach! Mo¿e implementowaæ
				te¿ inne interfejsy - np. IProgress
*********************************************************************/
#ifndef HEADER_GUARD_UTILS__JOB_H__
#define HEADER_GUARD_UTILS__JOB_H__

#include <threading/IThread.h>
#include <utils/SmartPtr.h>

namespace utils{

	class JobManager;

	class Job : private IRunnable
	{
		friend class JobManager;

	private:
		//! Klasa implementuj¹ca funkcjonalnoœæ
		class JobImpl;
		//! WskaŸnik na implementacje
		typedef shared_ptr<JobImpl> JobImplPtr;

	private:
		//! Implementacja klasy
		JobImplPtr impl_;

	public:

		//! Domyœlny konstruktor
		Job();
		//! Destruktor wirtualny
		virtual ~Job();
		//! \return Stan zadania
		const Status status() const;
		//! Metoda czeka a¿ zadanie siê skoñczy (z b³êdem, ok lub zostanie anulowane)
		void join();
		//! Metoda wykonuj¹ca faktycznie zadanie - wo³a IRunnable::run()
		void execute();
		//! \return Opis ostatniego b³êdu
		const std::string error() const;
	};

	DEFINE_SMART_POINTERS(Job);
}

#endif  //  HEADER_GUARD_UTILS__JOB_H__
