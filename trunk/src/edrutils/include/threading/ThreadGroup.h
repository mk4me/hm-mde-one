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
		//! Domy�lny kosntruktor
		ThreadGroup();

		//! Destruktor wirtualny
		virtual ~ThreadGroup();

		//! Metoda zabija grupe, w�tki nie nadaj� si� ponownie do u�ycia
		virtual void cancel();
		//! Metoda blokuj�ca a� w�tek/grupa nie zako�czy przetwarzania lub nie zostanie zniszczony/a
		virtual void join();
		//! \param threads W�tki obs�ugiwane przez grup�
		//! \param priority Priorytet na jakim b�d� uruchomione w�tki grupy
		virtual void start(const Threads & threads, const IThreadGroup::StartPolicy startPolicy);
		//! \return Aktualna polityka ko�czenia grupy w�tk�w
		virtual const StartPolicy startPolicy() const;
		//! \return Czy grupa dzia�a
		virtual const bool running() const;
		//! \return Rozmiar grupy
		virtual const size_type size() const;
		//! \param idx Indeks w�tku
		//! \return W�tek pod zadanym indeksem
		virtual IThreadConstPtr thread(const size_type idx) const;
	};

}

#endif	//	HEADER_GUARD___THREADGROUP_H__
