/********************************************************************
    created:  2013/03/27
    created:  27:3:2013   8:42
    filename: Thread.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___THREAD_H__
#define HEADER_GUARD___THREAD_H__

#include <threading/IThread.h>
#include <boost/shared_ptr.hpp>

namespace utils {

	//! Klasa w�tku implementuj�ca interfejs IThread
	class Thread : public IThread
	{
	private:
		//! Wewn�trzna implementacja w�tku
		class ThreadImpl;

	private:
		//! Wska�nik na obiekt z wewn�trzn� implementacj�
		boost::shared_ptr<ThreadImpl> impl_;

	public:
		//! Publiczny konstruktor
		Thread(IThreadPtr innerThread);
		//! Wirtualny destruktor
		virtual ~Thread();

		//! Metoda zabija w�tek/grupe, nie nadaje si� on/ona ponownie do u�ycia
		virtual void cancel();
		//! Metoda blokuj�ca a� w�tek/grupa nie zako�czy przetwarzania lub nie zostanie zniszczony/a
		virtual void join();
		//! \return Czy w�tek wci�� dzia�a
		virtual const bool running() const;
		//! Metoda uruchamia przetwarzanie przez w�tek, rzuca wyj�tkiem kiedy w�tek jeszcze dzia�� lub zosta� zabity
		//! \param priority Priorytet w�tku		
		//! \param runnable Obiekt wykonuj�cy prac� w w�tku, musi by� kopiowalny
		virtual void start(const RunnablePtr & runnable, const Priority priority);
		//! \return Czas aktualnego stanu idle dla w�tku w sekundach
		virtual const float idleTime() const;
		//! \return aktualny priorytet w�tku
		virtual const Priority priority() const;
		//! \return Aktualny rozmiar stosu w�tku
		virtual const size_type stackSize() const;
		//! \param stackSize Rozmiar stosu w�tku
		//! Metoda powinna by� wywo�ywana przed pierwszym uruchomieniem w�tku, lub po wykonaniu metody cancel
		//! potem nie ma mo�liwo�ci zmiany tego parametru. W przypadku braku
		//! wywo�ania tej metody domy�lny stos b�dzie u�yty
		virtual void setStackSize(const size_type stackSize);
		//! \param priority Priorytet w�tku
		virtual void setPriority(const Priority priority);
		//! \return Aktualnie wykonywana operacja w w�tku, mo�e by� null jesli nic si� nie dzieje
		virtual const RunnableConstPtr runnable() const;
		//! \return Aktualnie wykonywana operacja w w�tku, mo�e by� null jesli nic si� nie dzieje
		virtual RunnablePtr runnable();
	};

	typedef boost::shared_ptr<Thread> ThreadPtr;
	typedef boost::shared_ptr<const Thread> ThreadConstPtr;
	typedef boost::weak_ptr<Thread> ThreadWPtr;
	typedef boost::weak_ptr<const Thread> ThreadWConstPtr;

}

#endif	//	HEADER_GUARD___THREAD_H__
