/********************************************************************
	created:  2012/12/11
	created:  11:12:2012   14:54
	filename: IThread.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD_THREADINGUTILS__ITHREAD_H__
#define HEADER_GUARD_THREADINGUTILS__ITHREAD_H__

#include <threadingUtils/Export.h>
#include <utils/SmartPtr.h>
#include <threadingUtils/Runnable.h>

namespace threadingUtils {
	//! Interfejs klasy obs�uguj�cej w�tki
	class THREADINGUTILS_EXPORT IThread
	{
	public:
		//! Typ rozmiaru stosu w bajtach
		typedef unsigned int size_type;

		//! Priorytet w�tku
		enum Priority {
			Idle,			//! Wznawiaj tylko wtedy gdy nie ma innych w�tk�w
			Lowest,			//! Najni�szy priorytet
			Low,			//! Niski priorytet
			Normal,			//! �redni priorytet
			High,			//! Wysoki priorytet
			Highest,		//! Najwy�szy priorytet
			TimeCritical,	//! Wznawiaj tak cz�sto jak to mo�liwe
			Inheritate		//! Priorytet w�tku two��cego - domy�lna warto��
		};

	public:
		//! Wirtualny destruktor
		virtual ~IThread() {}
		//! Metoda uruchamia przetwarzanie przez w�tek, rzuca wyj�tkiem kiedy w�tek jeszcze dzia�� lub zosta� zabity
		//! \param priority Priorytet w�tku
		//! \param stackSize Rozmiar stosu dla w�tku - je�li zbyt du�y w�tek mo�e si� nie uruchomi� ze wzgl�du na ograniczenia systemu operacyjnego
		//! \param runnable Obiekt wykonuj�cy prac� w w�tku, musi by� kopiowalny
		virtual void start(const IRunnablePtr & runnable, const Priority priority = Inheritate) = 0;
		//! Metoda zabija w�tek/grupe, nie nadaje si� on/ona ponownie do u�ycia
		virtual void cancel() = 0;
		//! Metoda blokuj�ca a� w�tek/grupa nie zako�czy przetwarzania lub nie zostanie zniszczony/a
		virtual void join() = 0;
		//! \return Czy wątek wciąż działa
		virtual const bool running() const = 0;
		//! \return aktualny priorytet w�tku
		virtual const Priority priority() const = 0;
		//! \return Aktualny rozmiar stosu w�tku
		virtual const size_type stackSize() const = 0;
		//! \param stackSize Rozmiar stosu w�tku
		//! Metoda powinna by� wywo�ywana przed pierwszym uruchomieniem w�tku,
		//! potem nie ma mo�liwo�ci zmiany tego parametru. W przypadku braku
		//! wywo�ania tej metody b�dzie u�yty domy�lny stos
		//! W przypadku wywo�ania tej metody podczas dzia�ania watku powinien być rzucany wyj�tek
		virtual void setStackSize(const size_type stackSize) = 0;
		//! \param priority Priorytet wątku
		virtual void setPriority(const Priority priority) = 0;
		//! \return Aktualnie wykonywana operacja w w�tku, mo�e by� null jesli nic si� nie dzieje
		virtual const IRunnableConstPtr runnable() const = 0;
		//! \return Aktualnie wykonywana operacja w w�tku, mo�e by� null jesli nic si� nie dzieje
		virtual IRunnablePtr runnable() = 0;
	};

	DEFINE_SMART_POINTERS(IThread)
}

#endif	//	HEADER_GUARD_THREADINGUTILS__ITHREAD_H__
