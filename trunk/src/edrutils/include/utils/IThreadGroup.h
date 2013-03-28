/********************************************************************
    created:  2013/03/26
    created:  26:3:2013   12:31
    filename: IThreadGroup.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___ITHREADGROUP_H__
#define HEADER_GUARD___ITHREADGROUP_H__

#include <vector>
#include <utils/IThread.h>

namespace utils {

	//! Wyj¹tek rzucany gdy próbujemy uruchomiæ w¹tek po wykonaniu na nim kill
	class KilledGroupReStartException : public KilledReStartException
	{
	public:
		KilledGroupReStartException() {}
		KilledGroupReStartException(const char * msg) : KilledReStartException(msg) {}
	};

	//! Wyj¹tek rzucany gdy próbujemy uruchomiæ w¹tek ju¿ uruchomiony
	class RunningGroupStartException : public RunningStartException
	{
	public:
		RunningGroupStartException() {}
		RunningGroupStartException(const char * msg) : RunningStartException(msg) {}
	};

	//! Interfejs klasy obs³uguj¹cej w¹tki
	class IThreadGroup : public IThreadingBase
	{
	public:
		//! Struktura opisuj¹ca parametry przetwarzania
		struct RunnableDescription
		{
			RunnablePtr runnable;		//! Przetwarzany obiekt
			IThread::Priority priority;	//! Priorytet jego przetwarzania
		};
		//! Typ grupy funktorów wykonywanych w w¹tkach grupy
		typedef std::vector<RunnableDescription> Runnables;
		//! Typ iloœci w¹tków w grupie
		typedef Runnables::size_type size_type;

	public:
		//! Metoda uruchamia przetwarzanie przez grupê
		//! \param funcs Funktory wykonywane w grupie - ich rozmiar musi siê zgadzaæ rozmiarowi grupy, w przeciwnym wypadku dostaniemy wyj¹tek
		virtual void start(const Runnables & funcs) = 0;
		//! \return Rozmiar grupy
		virtual const size_type size() const = 0;
		//! \param idx Indeks w¹tku
		//! \return W¹tek pod zadanym indeksem
		virtual ThreadConstPtr thread(size_type idx) const = 0;
		//! \param stacks Wektor opisuj¹cy rozmiary stosów poszczególnych watkóe w grupie
		//! Wolno wo³aæ t¹ metode przed pierwszym przetwarzaniem lub bezpoœrednio po kill
		virtual void setStacksSizes(const std::vector<IThread::size_type> & stacks) = 0;
		//! \param idx Indeks w¹tku który chcemy obserwowaæ w grupie
		//! \param observer Obiekt obserwuj¹cy w¹tek
		virtual void attachToThread(size_type idx, Observer<IThreadingBase> * observer) = 0;
		//! \param idx Indeks w¹tku który chcemy przestaæ obserwowaæ w grupie
		//! \param observer Obiekt obserwuj¹cy w¹tek
		virtual void detachFromThread(size_type idx, Observer<IThreadingBase> * observer) = 0;
	};

	typedef boost::shared_ptr<IThreadGroup> ThreadGroupPtr;
	typedef boost::shared_ptr<const IThreadGroup> ThreadGroupConstPtr;
	typedef boost::weak_ptr<IThreadGroup> ThreadGroupWPtr;
	typedef boost::weak_ptr<const IThreadGroup> ThreadGroupWConstPtr;

}

#endif	//	HEADER_GUARD___ITHREADGROUP_H__
