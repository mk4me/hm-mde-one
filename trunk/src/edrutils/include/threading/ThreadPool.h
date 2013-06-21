/********************************************************************
    created:  2013/03/26
    created:  26:3:2013   15:30
    filename: ThreadPool.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___THREADPOOL_H__
#define HEADER_GUARD___THREADPOOL_H__

#include <threading/IThreadPool.h>
#include <threading/IThreadFactory.h>
#include <boost/shared_ptr.hpp>

namespace utils {

	class ThreadPool : public IThreadPool
	{
	private:
		//! Typ implementuj�cy faktyczn� funkcjonalno��
		class ThreadPoolImpl;

	private:
		//! Obiekt realizujacy funkcjonalno��
		boost::shared_ptr<ThreadPoolImpl> impl_;

	public:
		//! \param minThreads Minimalna ilo�� w�tk�w do utrzymania
		//! \param maxThreads Maksymalna ilo�� w�tk�w do utrzymania
		ThreadPool(IThreadFactoryPtr threadFactory, size_type minThreads = 4, size_type maxThreads = 16);
		//! Wirtualny desturktor
		virtual ~ThreadPool();

		//! \return Maksymalna ilo�c w�tk�w jakie mo�na utworzy�
		virtual const size_type maxThreads() const;
		//! \return Minimalna ilo�� w�tk�w utrzymywana przez manager
		virtual const size_type minThreads() const;
		//! \return Ilo�� aktualnie zaj�tych w�tk�w
		virtual const size_type threadsCount() const;
		//! \return Nowy w�tek
		virtual IThreadPtr getThread();
		//! \param groupSize Ilo�� w�tk�w w grupie
		//! \param threads [out] Lista z nowymi w�tkami, dopisujemy zawsze na ko�cu
		virtual void getThreads(const size_type groupSize, std::vector<IThreadPtr> & threads, const bool exact);
		//! \param maxThreads Maksymalna ilo�� w�tk�w jakie mo�na stworzy�
		void setMaxThreads(size_type maxThreads);
		//! \param minThreads Minimalna ilo�� w�tk�w jaka b�dzie utrzymywana
		void setMinThreads(size_type minThreads);
	};

}

#endif	//	HEADER_GUARD___THREADPOOL_H__
