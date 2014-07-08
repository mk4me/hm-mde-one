/********************************************************************
    created:  2013/07/04
    created:  4:7:2013   17:14
    filename: IThreadPool.h
    author:   Mateusz Janiak
    
    purpose:  Interfejs puli w¹tków
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
		//! Typ agreguj¹cy w¹tki
		typedef std::list<IThreadPtr> Threads;
		//! Typ opisuj¹cy rozmiar/iloœæ
		typedef Threads::size_type size_type;

	public:
		//! Destruktor wirtualny
		virtual ~IThreadPool() {}

		//! \return Maksymalna iloœæ w¹tków jak¹ oferuje pula
		virtual const size_type max() const = 0;
		//! \return Minimalna iloœæ utrzymywanych w¹tków jak¹ utrzymuje pula
		virtual const size_type min() const = 0;
		//! \return Iloœæ wolnych aktualnie w¹tków utrzymywanych przez pulê
		virtual const size_type free() const = 0;
		//! \return Aktualna iloœæ w¹tków
		virtual const size_type count() const = 0;

		//! Metoda pobiera w¹tki z puli podaj¹c kto i po co pobiera
		//! \param who Kto pobiera w¹tki		
		//! \param[out] threads Agregat ³¹dowany pobieranymi w¹tkami
		//! \param count Iloœæ w¹tków do pobrania
		//! \param exact Czy mo¿na pobraæ mniej w¹tków ni¿ ¿¹dano
		virtual void getThreads(const std::string & who,
			Threads & threads, const size_type count, const bool exact = true) = 0;
	};

	DEFINE_SMART_POINTERS(IThreadPool)
}

#endif	//	HEADER_GUARD_CORE__ITHREADPOOL_H__
