/********************************************************************
    created:  2012/12/11
    created:  11:12:2012   14:54
    filename: IThread.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___ITHREAD_H__
#define HEADER_GUARD___ITHREAD_H__

#include <exception>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <utils/ObserverPattern.h>

namespace utils {

//! Interfejs obiektu realizuj¹cego przetwarzanie w w¹tkach
class IRunnable
{
public:
	//! Wirtualny destruktor
	virtual ~IRunnable() {}
	//! Metoda wirtualna realizuj¹ca przetwarzanie w w¹tku
	virtual void run() = 0;
};

typedef boost::shared_ptr<IRunnable> RunnablePtr;
typedef boost::shared_ptr<const IRunnable> RunnableConstPtr;
typedef boost::weak_ptr<IRunnable> RunnableWPtr;
typedef boost::weak_ptr<const IRunnable> RunnableWConstPtr;

//! Interfejs pozwalaj¹cy okreœliæ procentowy stan pracy w¹tku
//! Postêp podawany jest w procentach od 0% do 100%;
class IPercentageProgress
{
public:
	//! Destruktor wirtualny
	virtual ~IPercentageProgress() {}
	//! \return Postêp realizacji przetwarzania od 0% do 100%
	virtual float progress() const = 0;
};

//! Interfejs pozwalaj¹cy okreœliæ szacowany czas do koñca zadania oraz czas jaki up³yn¹³ od momentu jego wystartowania
//! Czas podawany jest w sekundach
class ITimeProgress
{
public:
	//! Destruktor wirtualny
	virtual ~ITimeProgress() {}
	//! \return Czas [s] jaki zadanie jest ju¿ przetwarzane
	virtual float elapsed() const = 0;
	//! \return Szacowany czas [s] do zakoñczenia przetwarzania
	virtual float remaining() const = 0;
};

//! Bazowy interfejs w¹tków i grup - notyfikuje o zmianach swojego stanu
class IThreadingBase : public Observable<IThreadingBase>
{
public:

	//! Typ wyliczeniowy opisuj¹cy stan w¹tku lub grupy
	//! Po utworzeniu mamy stan Idle - nic nie robimy
	//! Po uruchomieniu metod¹ start dla niepustego runnable przejdziemy do stanu running
	//! Po zakoñczeniu przetwarzania wracamy do stanu idle
	enum Status {
		Idle,		//! W¹tek/grupa utworzony/a, nic nie robi
		Running,	//! W¹tek/grupa uruchomiony/a
		Killed		//! W¹tek/grupa zostaje zniszczony/a, nie mozna z nim prowadzic dalszej interakcji
	};

	//! Typ wyliczeniowy opisuj¹cy stan przetwarzania ostatniego zadania
	enum Result {
		FirstProcessing,	//! W¹tek jeszcze nic nie przetwarza³
		Finished,			//! W¹tek/Grupa zakoñczy³y pracê pomyœlnie
		Error,				//! W¹tek/Grupa zakoñczy³y pracê b³êdem
		Canceled			//! W¹tek/grupa anulowany - czekamy na kolejne zadanie
	};

public:
	//! Metoda zabija w¹tek/grupe, nie nadaje siê on/ona ponownie do u¿ycia
	virtual void cancel() = 0;
	//! Metoda blokuj¹ca a¿ w¹tek/grupa nie zakoñczy przetwarzania lub nie zostanie zniszczony/a
	virtual void join() = 0;
	//! \return Stan aktywnoœci w¹tku/grupy
	virtual const Status status() const = 0;
	//! \return Stan ostatniego przetwarzania w¹tku/grupy
	virtual const Result result() const = 0;
};

//! Wyj¹tek rzucany gdy próbujemy uruchomiæ w¹tek po wykonaniu na nim cancel
class KilledReStartException : public std::exception
{
public:
	KilledReStartException() {}
	KilledReStartException(const char * msg) : std::exception(msg) {}
};

//! Wyj¹tek rzucany gdy próbujemy uruchomiæ w¹tek ju¿ uruchomiony
class RunningStartException : public std::exception
{
public:
	RunningStartException() {}
	RunningStartException(const char * msg) : std::exception(msg) {}
};

//! Wyj¹tek rzucany gdy próbujemy uruchomiæ w¹tek po wykonaniu na nim cancel
class KilledThreadReStartException : public KilledReStartException
{
public:
	KilledThreadReStartException() {}
	KilledThreadReStartException(const char * msg) : KilledReStartException(msg) {}
};

//! Wyj¹tek rzucany gdy próbujemy uruchomiæ w¹tek ju¿ uruchomiony
class RunningThreadStartException : public RunningStartException
{
public:
	RunningThreadStartException() {}
	RunningThreadStartException(const char * msg) : RunningStartException(msg) {}
};

//! Interfejs klasy obs³uguj¹cej w¹tki
class IThread : public IThreadingBase
{
public:
	//! Typ rozmiaru stosu w bajtach
	typedef unsigned int size_type;

	//! Priorytet w¹tku
	enum Priority {
		Idle,			//! Wznawiaj tylko wtedy gdy nie ma innych w¹tków
		Lowest,			//! Najni¿szy priorytet
		Low,			//! Niski priorytet
		Normal,			//! Œredni priorytet
		High,			//! Wysoki priorytet
		Highest,		//! Najwy¿szy priorytet
		TimeCritical,	//! Wznawiaj tak czêsto jak to mo¿liwe
		Inheritate		//! Priorytet w¹tku two¿¹cego - domyœlna wartoœæ
	};

public:
	//! Metoda uruchamia przetwarzanie przez w¹tek, rzuca wyj¹tkiem kiedy w¹tek jeszcze dzia³¹ lub zosta³ zabity
	//! \param priority Priorytet w¹tku
	//! \param stackSize Rozmiar stosu dla w¹tku - jeœli zbyt du¿y w¹tek mo¿e siê nie uruchomiæ ze wzglêdu na ograniczenia systemu operacyjnego
	//! \param runnable Obiekt wykonuj¹cy pracê w w¹tku, musi byæ kopiowalny
	virtual void start(const Priority priority, const RunnablePtr & runnable) = 0;
	//! \return Czas aktualnego stanu idle dla w¹tku w sekundach
	virtual const float idleTime() const = 0;
	//! \return aktualny priorytet w¹tku
	virtual const Priority priority() const = 0;
	//! \return Aktualny rozmiar stosu w¹tku
	virtual const size_type stackSize() const = 0;
	//! \param stackSize Rozmiar stosu w¹tku
	//! Metoda powinna byæ wywo³ywana przed pierwszym uruchomieniem w¹tku,
	//! potem nie ma mo¿liwoœci zmiany tego parametru. W przypadku braku
	//! wywo³ania tej metody bêdzie u¿yty domyœlny stos
	//! W przypadku wywo³ania tej metody podczas dzia³ania watku bêdzie rzucany wyj¹tek
	virtual void setStackSize(size_type stackSize) = 0;
	//! \return Aktualnie wykonywana operacja w w¹tku, mo¿e byæ null jesli nic siê nie dzieje
	virtual RunnableConstPtr runnable() const = 0;
	//! \return Aktualnie wykonywana operacja w w¹tku, mo¿e byæ null jesli nic siê nie dzieje
	virtual RunnablePtr runnable() = 0;
};

typedef boost::shared_ptr<IThread> ThreadPtr;
typedef boost::shared_ptr<const IThread> ThreadConstPtr;
typedef boost::weak_ptr<IThread> ThreadWPtr;
typedef boost::weak_ptr<const IThread> ThreadWConstPtr;

}

#endif	//	HEADER_GUARD___ITHREAD_H__
