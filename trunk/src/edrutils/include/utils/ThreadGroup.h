/********************************************************************
    created:  2013/03/27
    created:  27:3:2013   17:56
    filename: ThreadGroup.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___THREADGROUP_H__
#define HEADER_GUARD___THREADGROUP_H__

#include <utils/IThreadGroup.h>

namespace utils {

	//! Klasa reprezentuj�ca grup� w�tk�w
	class ThreadGroup : public IThreadGroup
	{
	private:
		//! Prywatna klasa realizuj�ca ca�� funkcjonalno�� grupy
		class ThreadGroupImpl;

	private:
		//! Obiekt klasy realizuj�cej funkcjonalno��
		boost::shared_ptr<ThreadGroupImpl> impl;

	public:

		//! \param size Rozmiar grupy/Ilo�� w�tk�w w grupie
		ThreadGroup(size_type size);

		//! \param threads W�tki wchodz�ce w sk�ad grupy
		ThreadGroup(const std::vector<ThreadPtr> & threads);

		//! Destruktor wirtualny
		virtual ~ThreadGroup();

		//! Metoda zabija w�tek/grupe, nie nadaje si� on/ona ponownie do u�ycia
		virtual void cancel();
		//! Metoda blokuj�ca a� w�tek/grupa nie zako�czy przetwarzania lub nie zostanie zniszczony/a
		virtual void join();
		//! \return Stan w�tku/grupy
		virtual const Status status() const;
		//! \return Rezultat w�tku/grupy
		virtual const Result result() const;

		//! Metoda uruchamia przetwarzanie przez grup�
		//! \param funcs Funktory wykonywane w grupie - ich rozmiar musi si� zgadza� rozmiarowi grupy, w przeciwnym wypadku dostaniemy wyj�tek
		virtual void start(const Runnables & funcs);

		//! \param runnables Obiekty przetwarzane w grupie
		//! \param priority Priotytet w�tk� w grupie
		void start(const std::vector<RunnablePtr> & runnables, IThread::Priority priority);

		//! \param stacks Wektor opisuj�cy rozmiary stos�w poszczeg�lnych watk�e w grupie
		//! Wolno wo�a� t� metode przed pierwszym przetwarzaniem lub bezpo�rednio po cancel
		virtual void setStacksSizes(const std::vector<IThread::size_type> & stacks);

		//! \return Rozmiar grupy
		virtual const size_type size() const;
		//! \param idx Indeks w�tku
		//! \return W�tek pod zadanym indeksem
		virtual ThreadConstPtr thread(size_type idx) const;
		//! \param idx Indeks w�tku kt�ry chemy obserwowa� w grupie
		//! \param observer Obiekt obserwujacy w�tek
		virtual void attachToThread(size_type idx, Observer<IThreadingBase> * observer);
		//! \param idx Indeks w�tku kt�ry chemy przestac obserwowa�
		//! \param observer Obiekt obserwujacy w�tek
		virtual void detachFromThread(size_type idx, Observer<IThreadingBase> * observer);
	};

}

#endif	//	HEADER_GUARD___THREADGROUP_H__
