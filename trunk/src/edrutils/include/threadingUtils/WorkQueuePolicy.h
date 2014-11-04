/********************************************************************
	created:  2014/10/06	18:51:19
	filename: WorkQueuePolicy.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_THREADINGUTILS__WORKQUEUEPOLICY_H__
#define __HEADER_GUARD_THREADINGUTILS__WORKQUEUEPOLICY_H__

#include <threadingUtils/Export.h>
#include <map>
#include <mutex>
#include <condition_variable>
#include <type_traits>
#include <thread>
#include <threadingUtils/Macros.h>
#include <threadingUtils/Future.h>
#include <threadingUtils/FunctionWrapper.h>
#include <threadingUtils/Queue.h>
#include <threadingUtils/StealingQueue.h>
#include <utils/SmartPtr.h>

namespace threadingUtils
{

	struct THREADINGUTILS_EXPORT Task
	{
		std::thread::id id;
		FunctionWrapper functionWrapper;
	};

	class THREADINGUTILS_EXPORT StealingMultipleWorkQueuePolicy
	{
		typedef StealingQueue<FunctionWrapper> WorkStealingQueue;
		typedef std::map<std::thread::id, utils::shared_ptr<WorkStealingQueue>> LocalQueues;

		Queue<FunctionWrapper> commonQueue;
		LocalQueues localQueues;
		mutable std::mutex mutex;

	public:

		typedef Queue<FunctionWrapper>::size_type size_type;

	public:
		
		void initializeThread();

		void deinitializeThread();

		const size_type size() const;

		template<typename FunctionType>
		std::future<typename std::result_of<FunctionType()>::type> submit(FunctionType f)
		{
			typedef typename std::result_of<FunctionType()>::type result_type;
			std::packaged_task<result_type()> innerTask(f);
			std::future<result_type> ret(innerTask.get_future());

			if (localWorkQueue() != nullptr){
				localWorkQueue()->push(std::move(innerTask));
			}
			else{
				commonQueue.push(std::move(innerTask));
			}

			return std::move(ret);
		}

		const bool tryGet(Task & task);

		void clearLocalQueue();

		const bool empty() const;

	private:

		void rescheduleLocalQueue();
		
		const bool localQueuesEmpty() const;

		const bool tryPopWorkFromLocalQueue(Task & task);

		const bool tryPopWorkFromCommonQueue(Task & task);

		const bool tryPopWorkFromOtherLocalQueue(Task & task);

		static StealingQueue<FunctionWrapper> *& localWorkQueue();
	};	
}

#endif	// __HEADER_GUARD_THREADINGUTILS__WORKQUEUEPOLICY_H__