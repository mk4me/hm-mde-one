/********************************************************************
    created:  2013/03/27
    created:  27:3:2013   17:56
    filename: ThreadGroup.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___THREADGROUP_H__
#define HEADER_GUARD___THREADGROUP_H__

#include <threading/IThreadGroup.h>

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
		//! Domyœlny kosntruktor
		ThreadGroup();

		//! Destruktor wirtualny
		virtual ~ThreadGroup();

		//! Metoda zabija grupe, w¹tki nie nadaj¹ siê ponownie do u¿ycia
		virtual void cancel();
		//! Metoda blokuj¹ca a¿ w¹tek/grupa nie zakoñczy przetwarzania lub nie zostanie zniszczony/a
		virtual void join();
		//! \param threads W¹tki obs³ugiwane przez grupê
		//! \param priority Priorytet na jakim bêd¹ uruchomione w¹tki grupy
		virtual void start(const Threads & threads, const IThreadGroup::StartPolicy startPolicy);
		//! \return Aktualna polityka koñczenia grupy w¹tków
		virtual const StartPolicy startPolicy() const;
		//! \return Czy grupa dzia³a
		virtual const bool running() const;
		//! \return Rozmiar grupy
		virtual const size_type size() const;
		//! \param idx Indeks w¹tku
		//! \return W¹tek pod zadanym indeksem
		virtual IThreadConstPtr thread(const size_type idx) const;
	};

}

#endif	//	HEADER_GUARD___THREADGROUP_H__
