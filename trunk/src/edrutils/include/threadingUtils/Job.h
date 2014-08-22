/********************************************************************
    created:  2012/10/31
    created:  31:10:2012   12:55
    filename: Job.h
    author:   Mateusz Janiak
    
    purpose:  Interfejs klasy realizuj�cej proste, szybkie i niezale�ne
				zadania. Zadania te b�d� realizowane przez niezale�ne w�tki.
				Je�li wyst�puje mi�dzy nimi synchronizacja nale�y samemu
				o to zadba�, maj�c na uwadze mo�liwo�� spowodowania
				dead locka zada�!! Implementuje interfejs IRunnable
				- mo�na wi�c odpala� bezpo�rednio na w�tkach! Mo�e implementowa�
				te� inne interfejsy - np. IProgress
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
		//! Klasa implementuj�ca funkcjonalno��
		class JobImpl;
		//! Wska�nik na implementacje
		typedef shared_ptr<JobImpl> JobImplPtr;

	private:
		//! Implementacja klasy
		JobImplPtr impl_;

	public:

		//! Domy�lny konstruktor
		Job();
		//! Destruktor wirtualny
		virtual ~Job();
		//! \return Stan zadania
		const Status status() const;
		//! Metoda czeka a� zadanie si� sko�czy (z b��dem, ok lub zostanie anulowane)
		void join();
		//! Metoda wykonuj�ca faktycznie zadanie - wo�a IRunnable::run()
		void execute();
		//! \return Opis ostatniego b��du
		const std::string error() const;
	};

	DEFINE_SMART_POINTERS(Job);
}

#endif  //  HEADER_GUARD_UTILS__JOB_H__
