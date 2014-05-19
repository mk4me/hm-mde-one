/********************************************************************
	created:  2013/03/26
	created:  26:3:2013   15:30
	filename: ThreadPool.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD_THREADINGUTILS__THREADPOOL_H__
#define HEADER_GUARD_THREADINGUTILS__THREADPOOL_H__

#include <threadingUtils/Export.h>
#include <threadingUtils/IThreadPool.h>
#include <threadingUtils/IThreadFactory.h>

namespace threadingUtils {
	class THREADINGUTILS_EXPORT ThreadPool : public IThreadPool
	{
	private:
		//! Typ implementujący faktyczną funkcjonalność
		class ThreadPoolImpl;

	private:
		//! Obiekt realizujacy funkcjonalność
		utils::shared_ptr<ThreadPoolImpl> impl_;

	public:
		//! \param minThreads Minimalna ilość wątków do utrzymania
		//! \param maxThreads Maksymalna ilość wątków do utrzymania
		ThreadPool(IThreadFactoryPtr threadFactory, size_type minThreads = 4, size_type maxThreads = 16);
		//! Wirtualny desturktor
		virtual ~ThreadPool();

		//! \return Maksymalna ilośc wątków jakie można utworzyć
		virtual const size_type maxThreads() const;
		//! \return Minimalna ilość wątków utrzymywana przez manager
		virtual const size_type minThreads() const;
		//! \return Ilość aktualnie zajętych wątków
		virtual const size_type threadsCount() const;
		//! \return Nowy wątek
		virtual IThreadPtr getThread();
		//! \param groupSize Ilość wątków w grupie
		//! \param threads [out] Lista z nowymi wątkami, dopisujemy zawsze na końcu
		virtual void getThreads(const size_type groupSize, Threads & threads, const bool exact);
		//! \param maxThreads Maksymalna ilość wątków jakie można stworzyć
		void setMaxThreads(size_type maxThreads);
		//! \param minThreads Minimalna ilość wątków jaka będzie utrzymywana
		void setMinThreads(size_type minThreads);
	};
}

#endif	//	HEADER_GUARD_THREADINGUTILS__THREADPOOL_H__
