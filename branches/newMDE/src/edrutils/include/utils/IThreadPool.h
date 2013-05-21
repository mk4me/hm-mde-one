/********************************************************************
    created:  2013/03/26
    created:  26:3:2013   12:09
    filename: IThreadPool.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___ITHREADPOOL_H__
#define HEADER_GUARD___ITHREADPOOL_H__

#include <utils/IThread.h>
#include <utils/IThreadGroup.h>
#include <list>

namespace utils {

	//! Wyj¹tek rzucany kiedy chcemy pobraæ w¹tek/grupê z managera a nie ma ju¿ ¿adnych wolnych lub iloœæ wolnych jest niewystarczaj¹ca (dla grupy)
	class NoFreeThreadAvaiable : public std::exception
	{
	public:
		NoFreeThreadAvaiable() {}
		NoFreeThreadAvaiable(const char * msg) : std::exception(msg) {}
	};

	//! Interfejs klasy zarz¹dzaj¹cej w¹tkami. Klasa nadzoruje iloœæ aktualnie utworzonych w¹tków i nie pozwala utworzyæ wiêcej ni¿ zdefiniowano.
	//! Aby zminimalizowaæ koszt tworzenia i niszczenia w¹tków klasa utrzymuje pewn¹ iloœæ w¹tków do ponownego u¿ycia.
	class IThreadPool
	{
	public:
		//! Typ opisuj¹cy iloœæ w¹tków
		typedef unsigned int size_type;
		//! Typ czasu
		typedef double time_type;

	public:
		//! Wirtualny destruktor
		virtual ~IThreadPool() {}
		//! \return Maksymalna iloœc w¹tków jakie mo¿na utworzyæ
		virtual const size_type maxThreads() const = 0;
		//! \return Minimalna iloœæ w¹tków utrzymywana przez manager
		virtual const size_type minThreads() const = 0;
		//! \return Iloœæ aktualnie zajêtych w¹tków
		virtual const size_type threadsCount() const = 0;
		//! \return Nowy w¹tek
		virtual ThreadPtr getOrCreateThread() = 0;
		//! \param groupSize Iloœæ w¹tków w grupie
		//! \param threads [out] Lista z nowymi w¹tkami, dopisujemy zawsze na koñcu
		virtual void getOrCreateThreadsGroup(const size_type groupSize, std::vector<ThreadPtr> & threads) = 0;
	};

	typedef boost::shared_ptr<IThreadPool> ThreadPoolPtr;
	typedef boost::shared_ptr<const IThreadPool> ThreadPoolConstPtr;
	typedef boost::weak_ptr<IThreadPool> ThreadPoolWPtr;
	typedef boost::weak_ptr<const IThreadPool> ThreadPoolWConstPtr;
}

#endif	//	HEADER_GUARD___ITHREADPOOL_H__
