/********************************************************************
	created:  2014/10/26	16:22:44
	filename: ThreadPool.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_CORELIB__THREADPOOL_H__
#define __HEADER_GUARD_CORELIB__THREADPOOL_H__

#include <corelib/IThreadPool.h>
#include <loglib/ILog.h>

namespace core
{
	class ThreadPool : public IThreadPool
	{
	public:
		//! Domy�lny konstruktor
		ThreadPool(InnerThreadPool * tp);
		//! Desturktor
		~ThreadPool();

		//! \return Maksymalna ilo�c w�tk�w jakie mo�na utworzy�
		const size_type maxThreads() const override;
		//! \return Minimalna ilo�� w�tk�w utrzymywana przez manager
		const size_type minThreads() const override;
		//! \return Ilo�� aktualnie zaj�tych w�tk�w
		const size_type threadsCount() const override;
		//! \return Nowy w�tek		
		Thread get(const std::string & who, const std::string & destination) override;

		//! \param groupSize Ilo�� w�tk�w w grupie
		//! \param threads [out] Lista z nowymi w�tkami, dopisujemy zawsze na ko�cu
		//! \return Ilo�� faktycznie dostarczonych w�tk�w
		const size_type get(const size_type groupSize, Threads & threads, const bool exact,
			const std::string & who, const std::string & destination = std::string()) override;

		//! \param groupSize Ilość wątków jakie tworzymy poza ThreadPoolem
		virtual InnerThreadPool::CustomThreadProxy getCustom(const size_type groupSize = 1) override;

	private:
		
		Thread create(InnerThreadPool::Thread && innerThread,
			const std::string & who, const std::string & destination);

	private:
		//! Wewn�trzny tp
		InnerThreadPool * tp;	
		loglib::ILogPtr logger;
	};	
}

#endif	// __HEADER_GUARD_CORELIB__THREADPOOL_H__
