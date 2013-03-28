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

	//! Klasa reprezentuj¹ca grupê w¹tków
	class ThreadGroup : public IThreadGroup
	{
	private:
		//! Prywatna klasa realizuj¹ca ca³¹ funkcjonalnoœæ grupy
		class ThreadGroupImpl;

	private:
		//! Obiekt klasy realizuj¹cej funkcjonalnoœæ
		boost::shared_ptr<ThreadGroupImpl> impl;

	public:

		//! \param size Rozmiar grupy/Iloœæ w¹tków w grupie
		ThreadGroup(size_type size);

		//! \param threads W¹tki wchodz¹ce w sk³ad grupy
		ThreadGroup(const std::vector<ThreadPtr> & threads);

		//! Destruktor wirtualny
		virtual ~ThreadGroup();

		//! Metoda zabija w¹tek/grupe, nie nadaje siê on/ona ponownie do u¿ycia
		virtual void cancel();
		//! Metoda blokuj¹ca a¿ w¹tek/grupa nie zakoñczy przetwarzania lub nie zostanie zniszczony/a
		virtual void join();
		//! \return Stan w¹tku/grupy
		virtual const Status status() const;
		//! \return Rezultat w¹tku/grupy
		virtual const Result result() const;

		//! Metoda uruchamia przetwarzanie przez grupê
		//! \param funcs Funktory wykonywane w grupie - ich rozmiar musi siê zgadzaæ rozmiarowi grupy, w przeciwnym wypadku dostaniemy wyj¹tek
		virtual void start(const Runnables & funcs);

		//! \param runnables Obiekty przetwarzane w grupie
		//! \param priority Priotytet w¹tkó w grupie
		void start(const std::vector<RunnablePtr> & runnables, IThread::Priority priority);

		//! \param stacks Wektor opisuj¹cy rozmiary stosów poszczególnych watkóe w grupie
		//! Wolno wo³aæ t¹ metode przed pierwszym przetwarzaniem lub bezpoœrednio po cancel
		virtual void setStacksSizes(const std::vector<IThread::size_type> & stacks);

		//! \return Rozmiar grupy
		virtual const size_type size() const;
		//! \param idx Indeks w¹tku
		//! \return W¹tek pod zadanym indeksem
		virtual ThreadConstPtr thread(size_type idx) const;
		//! \param idx Indeks w¹tku który chemy obserwowaæ w grupie
		//! \param observer Obiekt obserwujacy w¹tek
		virtual void attachToThread(size_type idx, Observer<IThreadingBase> * observer);
		//! \param idx Indeks w¹tku który chemy przestac obserwowaæ
		//! \param observer Obiekt obserwujacy w¹tek
		virtual void detachFromThread(size_type idx, Observer<IThreadingBase> * observer);
	};

}

#endif	//	HEADER_GUARD___THREADGROUP_H__
