/********************************************************************
    created:  2012/12/11
    created:  11:12:2012   14:54
    filename: IThread.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___ITHREAD_H__
#define HEADER_GUARD___ITHREAD_H__

#include <stdexcept>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <utils/ObserverPattern.h>

namespace utils {

//! Interfejs obiektu realizuj�cego przetwarzanie w w�tkach
class IRunnable
{
public:
	//! Wirtualny destruktor
	virtual ~IRunnable() {}
	//! Metoda wirtualna realizuj�ca przetwarzanie w w�tku
	virtual void run() = 0;
};

class FunctorRunnable : public IRunnable
{
public:

	typedef boost::function<void()> Functor;

public:

	FunctorRunnable(const Functor & func) : func(func) {}

	virtual void run()
	{
		if(func.empty() == false){
			func();
		}
	}

private:

	Functor func;
};

typedef boost::shared_ptr<IRunnable> RunnablePtr;
typedef boost::shared_ptr<const IRunnable> RunnableConstPtr;
typedef boost::weak_ptr<IRunnable> RunnableWPtr;
typedef boost::weak_ptr<const IRunnable> RunnableWConstPtr;

//! Interfejs pozwalaj�cy okre�li� procentowy stan pracy w�tku
//! Post�p podawany jest w procentach od 0% do 100%;
class IPercentageProgress
{
public:
	//! Destruktor wirtualny
	virtual ~IPercentageProgress() {}
	//! \return Post�p realizacji przetwarzania od 0% do 100%
	virtual float progress() const = 0;
};

//! Interfejs pozwalaj�cy okre�li� szacowany czas do ko�ca zadania oraz czas jaki up�yn�� od momentu jego wystartowania
//! Czas podawany jest w sekundach
class ITimeProgress
{
public:
	//! Destruktor wirtualny
	virtual ~ITimeProgress() {}
	//! \return Czas [s] jaki zadanie jest ju� przetwarzane
	virtual float elapsed() const = 0;
	//! \return Szacowany czas [s] do zako�czenia przetwarzania
	virtual float remaining() const = 0;
};

//! Bazowy interfejs w�tk�w i grup - notyfikuje o zmianach swojego stanu
class IThreadingBase : public Observable<IThreadingBase>
{
public:

	//! Typ wyliczeniowy opisuj�cy stan w�tku lub grupy
	//! Po utworzeniu mamy stan Idle - nic nie robimy
	//! Po uruchomieniu metod� start dla niepustego runnable przejdziemy do stanu running
	//! Po zako�czeniu przetwarzania wracamy do stanu idle
	enum Status {
		Idle,		//! W�tek/grupa utworzony/a, nic nie robi
		Running,	//! W�tek/grupa uruchomiony/a
		Killed		//! W�tek/grupa zostaje zniszczony/a, nie mozna z nim prowadzic dalszej interakcji
	};

	//! Typ wyliczeniowy opisuj�cy stan przetwarzania ostatniego zadania
	enum Result {
		FirstProcessing,	//! W�tek jeszcze nic nie przetwarza�
		Finished,			//! W�tek/Grupa zako�czy�y prac� pomy�lnie
		Error,				//! W�tek/Grupa zako�czy�y prac� b��dem
		Canceled			//! W�tek/grupa anulowany - czekamy na kolejne zadanie
	};

public:
	//! Wirtualny destruktor
	virtual ~IThreadingBase() {}

	//! Metoda zabija w�tek/grupe, nie nadaje si� on/ona ponownie do u�ycia
	virtual void cancel() = 0;
	//! Metoda blokuj�ca a� w�tek/grupa nie zako�czy przetwarzania lub nie zostanie zniszczony/a
	virtual void join() = 0;
	//! \return Stan aktywno�ci w�tku/grupy
	virtual const Status status() const = 0;
	//! \return Stan ostatniego przetwarzania w�tku/grupy
	virtual const Result result() const = 0;
};

//! Wyj�tek rzucany gdy pr�bujemy uruchomi� w�tek ju� uruchomiony
class RunningStartException : public std::runtime_error
{
public:
	RunningStartException(const char * msg) : std::runtime_error(msg) {}
};

//! Wyj�tek rzucany gdy pr�bujemy uruchomi� w�tek ju� uruchomiony
class RunningThreadStartException : public RunningStartException
{
public:
	RunningThreadStartException(const char * msg) : RunningStartException(msg) {}
};

//! Interfejs klasy obs�uguj�cej w�tki
class IThread : public IThreadingBase
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
	virtual void start(const RunnablePtr & runnable, const Priority priority = Inheritate) = 0;
	//! \return Czas aktualnego stanu idle dla w�tku w sekundach
	virtual const float idleTime() const = 0;
	//! \return aktualny priorytet w�tku
	virtual const Priority priority() const = 0;
	//! \return Aktualny rozmiar stosu w�tku
	virtual const size_type stackSize() const = 0;
	//! \param stackSize Rozmiar stosu w�tku
	//! Metoda powinna by� wywo�ywana przed pierwszym uruchomieniem w�tku,
	//! potem nie ma mo�liwo�ci zmiany tego parametru. W przypadku braku
	//! wywo�ania tej metody b�dzie u�yty domy�lny stos
	//! W przypadku wywo�ania tej metody podczas dzia�ania watku b�dzie rzucany wyj�tek
	virtual void setStackSize(size_type stackSize) = 0;
	//! \return Aktualnie wykonywana operacja w w�tku, mo�e by� null jesli nic si� nie dzieje
	virtual RunnableConstPtr runnable() const = 0;
	//! \return Aktualnie wykonywana operacja w w�tku, mo�e by� null jesli nic si� nie dzieje
	virtual RunnablePtr runnable() = 0;
};

typedef boost::shared_ptr<IThread> ThreadPtr;
typedef boost::shared_ptr<const IThread> ThreadConstPtr;
typedef boost::weak_ptr<IThread> ThreadWPtr;
typedef boost::weak_ptr<const IThread> ThreadWConstPtr;

}

#endif	//	HEADER_GUARD___ITHREAD_H__
