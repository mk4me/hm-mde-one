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

	//! Klasa w¹tku implementuj¹ca interfejs IThread
	class Thread : public IThread
	{
	private:
		//! Wewnêtrzna implementacja w¹tku
		class ThreadImpl;

	private:
		//! WskaŸnik na obiekt z wewnêtrzn¹ implementacj¹
		boost::shared_ptr<ThreadImpl> impl_;

	public:
		//! Publiczny konstruktor
		Thread(IThreadPtr innerThread);
		//! Wirtualny destruktor
		virtual ~Thread();

		//! Metoda zabija w¹tek/grupe, nie nadaje siê on/ona ponownie do u¿ycia
		virtual void cancel();
		//! Metoda blokuj¹ca a¿ w¹tek/grupa nie zakoñczy przetwarzania lub nie zostanie zniszczony/a
		virtual void join();
		//! \return Czy w¹tek wci¹¿ dzia³a
		virtual const bool running() const;
		//! Metoda uruchamia przetwarzanie przez w¹tek, rzuca wyj¹tkiem kiedy w¹tek jeszcze dzia³¹ lub zosta³ zabity
		//! \param priority Priorytet w¹tku		
		//! \param runnable Obiekt wykonuj¹cy pracê w w¹tku, musi byæ kopiowalny
		virtual void start(const RunnablePtr & runnable, const Priority priority);
		//! \return Czas aktualnego stanu idle dla w¹tku w sekundach
		virtual const float idleTime() const;
		//! \return aktualny priorytet w¹tku
		virtual const Priority priority() const;
		//! \return Aktualny rozmiar stosu w¹tku
		virtual const size_type stackSize() const;
		//! \param stackSize Rozmiar stosu w¹tku
		//! Metoda powinna byæ wywo³ywana przed pierwszym uruchomieniem w¹tku, lub po wykonaniu metody cancel
		//! potem nie ma mo¿liwoœci zmiany tego parametru. W przypadku braku
		//! wywo³ania tej metody domyœlny stos bêdzie u¿yty
		virtual void setStackSize(const size_type stackSize);
		//! \param priority Priorytet w¹tku
		virtual void setPriority(const Priority priority);
		//! \return Aktualnie wykonywana operacja w w¹tku, mo¿e byæ null jesli nic siê nie dzieje
		virtual const RunnableConstPtr runnable() const;
		//! \return Aktualnie wykonywana operacja w w¹tku, mo¿e byæ null jesli nic siê nie dzieje
		virtual RunnablePtr runnable();
	};

	typedef boost::shared_ptr<Thread> ThreadPtr;
	typedef boost::shared_ptr<const Thread> ThreadConstPtr;
	typedef boost::weak_ptr<Thread> ThreadWPtr;
	typedef boost::weak_ptr<const Thread> ThreadWConstPtr;

}

#endif	//	HEADER_GUARD___THREAD_H__
