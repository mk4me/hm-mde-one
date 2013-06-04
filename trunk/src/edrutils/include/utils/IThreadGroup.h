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

	//! Wyj�tek rzucany gdy pr�bujemy uruchomi� w�tek ju� uruchomiony
	class RunningGroupStartException : public RunningStartException
	{
	public:
		RunningGroupStartException(const char * msg) : RunningStartException(msg) {}
	};

	//! Interfejs klasy obs�uguj�cej w�tki
	class IThreadGroup : public IThreadingBase
	{
	public:
		//! Struktura opisuj�ca parametry przetwarzania
		struct RunnableDescription
		{
			RunnablePtr runnable;		//! Przetwarzany obiekt
			IThread::Priority priority;	//! Priorytet jego przetwarzania
		};
		//! Typ grupy funktor�w wykonywanych w w�tkach grupy
		typedef std::vector<RunnableDescription> Runnables;
		//! Typ ilo�ci w�tk�w w grupie
		typedef Runnables::size_type size_type;

	public:
		//! Wirtualny destruktor
		virtual ~IThreadGroup() {}

		//! Metoda uruchamia przetwarzanie przez grup�
		//! \param funcs Funktory wykonywane w grupie - ich rozmiar musi si� zgadza� rozmiarowi grupy, w przeciwnym wypadku dostaniemy wyj�tek
		virtual void start(const Runnables & funcs) = 0;
		//! \return Rozmiar grupy
		virtual const size_type size() const = 0;
		//! \param idx Indeks w�tku
		//! \return W�tek pod zadanym indeksem
		virtual ThreadConstPtr thread(size_type idx) const = 0;
		//! \param stacks Wektor opisuj�cy rozmiary stos�w poszczeg�lnych watk�e w grupie
		//! Wolno wo�a� t� metode przed pierwszym przetwarzaniem lub bezpo�rednio po kill
		virtual void setStacksSizes(const std::vector<IThread::size_type> & stacks) = 0;
		//! \param idx Indeks w�tku kt�ry chcemy obserwowa� w grupie
		//! \param observer Obiekt obserwuj�cy w�tek
		virtual void attachToThread(size_type idx, Observer<IThreadingBase> * observer) = 0;
		//! \param idx Indeks w�tku kt�ry chcemy przesta� obserwowa� w grupie
		//! \param observer Obiekt obserwuj�cy w�tek
		virtual void detachFromThread(size_type idx, Observer<IThreadingBase> * observer) = 0;
	};

	typedef boost::shared_ptr<IThreadGroup> ThreadGroupPtr;
	typedef boost::shared_ptr<const IThreadGroup> ThreadGroupConstPtr;
	typedef boost::weak_ptr<IThreadGroup> ThreadGroupWPtr;
	typedef boost::weak_ptr<const IThreadGroup> ThreadGroupWConstPtr;

}

#endif	//	HEADER_GUARD___ITHREADGROUP_H__
