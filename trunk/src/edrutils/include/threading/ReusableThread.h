/********************************************************************
    created:  2013/06/18
    created:  18:6:2013   10:56
    filename: ReusableThread.h
    author:   Mateusz Janiak
    
    purpose:  Klasa wątku pozwalająca na jego ponowne użycie do momentu
				wystąpienia błędu lub anulowania, działa w oparciu o
				klasyczny wątek nadający się do jednorazowego uzytku.
				Klasa ta stanowi nakładkę na klasyczny wątek
*********************************************************************/
#ifndef HEADER_GUARD___REUSABLETHREAD_H__
#define HEADER_GUARD___REUSABLETHREAD_H__

#include <threading/IThread.h>
#include <threading/IThreadFactory.h>

namespace utils {

	class ReusableThread : public IThread
	{
	private:
		//! Implementacja klasy ReusableRunnable
		class ReusableThreadImpl;

	private:
		//! Implementacja
		shared_ptr<ReusableThreadImpl> impl_;

	public:
		//! Konstruktor
		//! \param thread Wątek w ramach którego realizujemy funkcjonalność
		ReusableThread(IThreadPtr thread);
		//! Wirtualny destruktor
		~ReusableThread();

		//! Metoda uruchamia przetwarzanie przez w�tek, rzuca wyj�tkiem kiedy w�tek jeszcze dzia�� lub zosta� zabity
		//! \param priority Priorytet w�tku
		//! \param stackSize Rozmiar stosu dla w�tku - je�li zbyt du�y w�tek mo�e si� nie uruchomi� ze wzgl�du na ograniczenia systemu operacyjnego
		//! \param runnable Obiekt wykonuj�cy prac� w w�tku, musi by� kopiowalny
		virtual void start(const RunnablePtr & runnable, const Priority priority);
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
		virtual const RunnableConstPtr runnable() const;
		//! \return Aktualnie wykonywana operacja w w�tku, mo�e by� null jesli nic si� nie dzieje
		virtual RunnablePtr runnable();
	};

}

#endif	//	HEADER_GUARD___REUSABLETHREAD_H__
