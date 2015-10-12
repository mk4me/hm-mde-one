/********************************************************************
created:  2014/10/26	16:22:44
filename: IThreadPool.h
author:	  Mateusz Janiak

purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_CORELIB__ITHREADPOOL_H__
#define __HEADER_GUARD_CORELIB__ITHREADPOOL_H__

#include <utils/SmartPtr.h>
#include <corelib/Thread.h>

namespace core
{
	class IThreadPool
	{
	public:
		//! 
		typedef InnerThreadPool::size_type size_type;		

		typedef std::list<Thread> Threads;		

	public:
		//! Desturktor
		virtual ~IThreadPool() {}

		//! \return Maksymalna ilo�c w�tk�w jakie mo�na utworzy�
		virtual const size_type maxThreads() const = 0;
		//! \return Minimalna ilo�� w�tk�w utrzymywana przez manager
		virtual const size_type minThreads() const = 0;
		//! \return Ilo�� aktualnie zaj�tych w�tk�w
		virtual const size_type threadsCount() const = 0;
		//! \return Nowy w�tek		
		virtual Thread get(const std::string & who, const std::string & destination) = 0;

		//! \param groupSize Ilo�� w�tk�w w grupie
		//! \param threads [out] Lista z nowymi w�tkami, dopisujemy zawsze na ko�cu
		//! \return Ilo�� faktycznie dostarczonych w�tk�w
		virtual const size_type get(const size_type groupSize, Threads & threads, const bool exact,
			const std::string & who, const std::string & destination = std::string()) = 0;

		//! \param groupSize Ilość wątków jakie tworzymy poza ThreadPoolem
		virtual InnerThreadPool::CustomThreadProxy getCustom(const size_type groupSize = 1) = 0;
	};	
}

#endif	// __HEADER_GUARD_CORELIB__THREADPOOL_H__
