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

	//! Wyj�tek rzucany kiedy chcemy pobra� w�tek/grup� z managera a nie ma ju� �adnych wolnych lub ilo�� wolnych jest niewystarczaj�ca (dla grupy)
	class NoFreeThreadAvaiable : public std::runtime_error
	{
	public:
		NoFreeThreadAvaiable(const char * msg) : std::runtime_error(msg) {}
	};

	//! Interfejs klasy zarz�dzaj�cej w�tkami. Klasa nadzoruje ilo�� aktualnie utworzonych w�tk�w i nie pozwala utworzy� wi�cej ni� zdefiniowano.
	//! Aby zminimalizowa� koszt tworzenia i niszczenia w�tk�w klasa utrzymuje pewn� ilo�� w�tk�w do ponownego u�ycia.
	class IThreadPool
	{
	public:
		//! Typ opisuj�cy ilo�� w�tk�w
		typedef unsigned int size_type;
		//! Typ czasu
		typedef double time_type;

	public:
		//! Wirtualny destruktor
		virtual ~IThreadPool() {}
		//! \return Maksymalna ilo�c w�tk�w jakie mo�na utworzy�
		virtual const size_type maxThreads() const = 0;
		//! \return Minimalna ilo�� w�tk�w utrzymywana przez manager
		virtual const size_type minThreads() const = 0;
		//! \return Ilo�� aktualnie zaj�tych w�tk�w
		virtual const size_type threadsCount() const = 0;
		//! \return Nowy w�tek
		virtual IThreadPtr getThread() = 0;
		//! \param groupSize Ilo�� w�tk�w w grupie
		//! \param threads [out] Lista z nowymi w�tkami, dopisujemy zawsze na ko�cu
		//! \param exact Czy ma być dokładnie tyle wątków czy może być mniej
		virtual void getThreads(const size_type groupSize, std::vector<IThreadPtr> & threads, const bool exact = true) = 0;
	};

	typedef boost::shared_ptr<IThreadPool> ThreadPoolPtr;
	typedef boost::shared_ptr<const IThreadPool> ThreadPoolConstPtr;
	typedef boost::weak_ptr<IThreadPool> ThreadPoolWPtr;
	typedef boost::weak_ptr<const IThreadPool> ThreadPoolWConstPtr;
}

#endif	//	HEADER_GUARD___ITHREADPOOL_H__
