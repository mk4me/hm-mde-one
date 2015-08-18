/********************************************************************
	created:  2015/08/12	10:31:41
	filename: ThreadTypes.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_CORELIB__THREADTYPES_H__
#define __HEADER_GUARD_CORELIB__THREADTYPES_H__

#include <threadingUtils/RunnableThread.h>
#include <threadingUtils/InterruptiblePolicy.h>
#include <threadingUtils/InterruptibleThreadPool.h>
#include <threadingUtils/ThreadFactoryT.h>

namespace core
{
	//! Przerywalny w�tek z funkcj� runnable
	typedef threadingUtils::RunnableThread<std::thread, threadingUtils::ConsumeExceptionHandlePolicy> InnerRunnableThread;

	typedef threadingUtils::InterruptibleMultipleRunThread<InnerRunnableThread, threadingUtils::ConsumeExceptionHandlePolicy, threadingUtils::NoInterruptHandlingPolicy, threadingUtils::InterrupltiblePolicy> InnerInterruptibleMultipleRunThread;

	//! Wewn�trza realizacja puli w�tk�w
	typedef threadingUtils::InterruptibleThreadPool<threadingUtils::ThreadFactoryT<InnerInterruptibleMultipleRunThread>> InnerThreadPool;
}

#endif	// __HEADER_GUARD_CORELIB__THREADTYPES_H__