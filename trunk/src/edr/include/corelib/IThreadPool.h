/********************************************************************
    created:  2013/07/04
    created:  4:7:2013   17:14
    filename: IThreadPool.h
    author:   Mateusz Janiak
    
    purpose:  Interfejs puli w�tk�w
*********************************************************************/
#ifndef HEADER_GUARD_CORE__ITHREADPOOL_H__
#define HEADER_GUARD_CORE__ITHREADPOOL_H__

#include <corelib/IThread.h>
#include <list>

namespace core
{

	class IThreadPool
	{
	public:
		//! Typ agreguj�cy w�tki
		typedef std::list<IThreadPtr> Threads;
		//! Typ opisuj�cy rozmiar/ilo��
		typedef Threads::size_type size_type;

	public:
		//! Destruktor wirtualny
		virtual ~IThreadPool() {}

		//! \return Maksymalna ilo�� w�tk�w jak� oferuje pula
		virtual const size_type max() const = 0;
		//! \return Minimalna ilo�� utrzymywanych w�tk�w jak� utrzymuje pula
		virtual const size_type min() const = 0;
		//! \return Ilo�� wolnych aktualnie w�tk�w utrzymywanych przez pul�
		virtual const size_type free() const = 0;
		//! \return Aktualna ilo�� w�tk�w
		virtual const size_type count() const = 0;

		//! Metoda pobiera w�tki z puli podaj�c kto i po co pobiera
		//! \param who Kto pobiera w�tki		
		//! \param[out] threads Agregat ��dowany pobieranymi w�tkami
		//! \param count Ilo�� w�tk�w do pobrania
		//! \param exact Czy mo�na pobra� mniej w�tk�w ni� ��dano
		virtual void getThreads(const std::string & who,
			Threads & threads, const size_type count, const bool exact = true) = 0;
	};

	DEFINE_SMART_POINTERS(IThreadPool)
}

#endif	//	HEADER_GUARD_CORE__ITHREADPOOL_H__
