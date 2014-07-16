/********************************************************************
    created:  2013/07/05
    created:  5:7:2013   13:53
    filename: Thread.h
    author:   Mateusz Janiak
    
    purpose:  Implementacja wątku
*********************************************************************/
#ifndef HEADER_GUARD_CORE__THREAD_H__
#define HEADER_GUARD_CORE__THREAD_H__

#include <corelib/IThread.h>
#include <threading/SynchronizationPolicies.h>

namespace core
{

	class ThreadPool;

	class Thread : public IThread
	{

		friend class ThreadPool;

	private:
		//! \param who Właściciel wątku
		//! \param destination Przeznaczenie wątku
		//! \param thread Faktyczny wątek przetwarzający
		Thread(const std::string & who, const std::string & destination,
			utils::IThreadPtr thread);

		//! \param who Właściciel wątku		
		//! \param thread Faktyczny wątek przetwarzający
		Thread(const std::string & who, utils::IThreadPtr thread);

	public:

		//! Destruktor wirtualny
		virtual ~Thread();
		//! \return Właściciel wątku	
		virtual const std::string & who() const;
		//! \return Przeznaczenie wątku
		virtual const std::string & destination() const;
		//! \param destination Przeznaczenie wątku, co realizuje
		virtual void setDestination(const std::string & destination);

		virtual void start(const utils::IRunnablePtr & runnable, const Priority priority);
		//! Metoda zabija w�tek/grupe, nie nadaje si� on/ona ponownie do u�ycia
		virtual void cancel();
		//! Metoda blokuj�ca a� w�tek/grupa nie zako�czy przetwarzania lub nie zostanie zniszczony/a
		virtual void join();
		//! \return Czy wątek wciąż działa
		virtual const bool running() const;
		//! \return aktualny priorytet w�tku
		virtual const Priority priority() const;
		//! \return Aktualny rozmiar stosu w�tku
		virtual const size_type stackSize() const;
		//! \param stackSize Rozmiar stosu w�tku
		//! Metoda powinna by� wywo�ywana przed pierwszym uruchomieniem w�tku,
		//! potem nie ma mo�liwo�ci zmiany tego parametru. W przypadku braku
		//! wywo�ania tej metody b�dzie u�yty domy�lny stos
		//! W przypadku wywo�ania tej metody podczas dzia�ania watku powinien być rzucany wyj�tek
		virtual void setStackSize(const size_type stackSize);
		//! \param priority Priorytet wątku
		virtual void setPriority(const Priority priority);
		//! \return Aktualnie wykonywana operacja w w�tku, mo�e by� null jesli nic si� nie dzieje
		virtual const utils::IRunnableConstPtr runnable() const;
		//! \return Aktualnie wykonywana operacja w w�tku, mo�e by� null jesli nic si� nie dzieje
		virtual utils::IRunnablePtr runnable();

	private:
		//! Właściciel wątku
		std::string who_;
		//! Przeznaczenie wątku
		std::string destination_;
		//! Właściwy wątek
		utils::IThreadPtr thread_;
		//! Obiekt synchronizujący
		mutable utils::StrictSyncPolicy synch_;
	};

}

#endif	//	HEADER_GUARD_CORE__THREAD_H__
