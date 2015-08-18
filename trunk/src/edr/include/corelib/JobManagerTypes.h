/********************************************************************
	created:  2015/08/12	15:34:52
	filename: JobManagerTypes.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_CORELIB__JOBMANAGERTYPES_H__
#define __HEADER_GUARD_CORELIB__JOBMANAGERTYPES_H__

#include <corelib/Thread.h>
#include <threadingUtils/InterruptibleWorkManager.h>
#include <threadingUtils/InterruptibleJobManager.h>

namespace core
{
	typedef threadingUtils::InterruptibleWorkManager<threadingUtils::StealingMultipleWorkQueuePolicy, Thread, threadingUtils::ConsumeExceptionHandlePolicy, threadingUtils::NoInterruptHandlingPolicy> InnerWorkManager;
	typedef threadingUtils::InterruptibleJobManager<InnerWorkManager> InnerJobManager;
}

#endif	// __HEADER_GUARD_CORELIB__JOBMANAGERTYPES_H__