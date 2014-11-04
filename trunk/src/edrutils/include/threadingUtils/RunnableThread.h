/********************************************************************
	created:  2014/10/02	10:04:35
	filename: RunnableThread.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_THREADINGUTILS__RUNNABLETHREAD_H__
#define __HEADER_GUARD_THREADINGUTILS__RUNNABLETHREAD_H__

#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <type_traits>
#include <threadingUtils/Future.h>
#include <threadingUtils/CallPolicy.h>
#include <threadingUtils/FunctionWrapper.h>

namespace threadingUtils
{
	//! \tparam Thread Typ w¹tku jaki wrapujemy do postaci runnable
	//! \tparam CallPolicy Sposób wo³ania metod przez w¹tek
	template<typename Thread, typename CallPolicy = RawCallPolicy>
	class RunnableThread
	{
	public:

		//! Konstruktor domyœlny
		RunnableThread()  : launched(false) {}
		//! \param Other W¹tek którego zasoby przejmujemy
		RunnableThread(RunnableThread&& Other)  : launched(std::move(Other.launched)), thread(std::move(Other.thread)) {}
		RunnableThread(const RunnableThread&) = delete;
		//! Destruktor
		~RunnableThread() {}
		
		//! \param Other W¹tek którego zasoby przejmujemy
		RunnableThread& operator=(RunnableThread&& Other) { launched = std::move(Other.launched); thread = std::move(Other.thread); return *this; }
		RunnableThread& operator=(const RunnableThread&) = delete;

		template<class F, class... Args>
		void run(F&& f, Args&&... arguments)
		{
			if (launched == true){
				std::_Throw_Cpp_error(std::_OPERATION_NOT_PERMITTED);
			}
			else{
				thread = Thread([=]
				{
					CallPolicy::call(std::move(f), std::move(arguments)...);
				});
				launched = true;
			}
		}

		void swap(RunnableThread & Other) { std::swap(launched, Other.launched); std::swap(thread, Other.thread); }
		bool joinable() const  { return thread.joinable(); }
		void join() { thread.join(); }
		void detach() { thread.detach(); }
		std::thread::id get_id() const  { return thread.get_id(); }
		std::thread::native_handle_type native_handle() { return thread.native_handle(); }

	private:
		bool launched;
		Thread thread;
	};

	template<typename Thread, typename CallPolicy = RawCallPolicy>
	class MultipleRunThread
	{
	private:

		typedef MultipleRunThread<Thread, CallPolicy> MyMultipleRunThread;

		struct SharedState
		{
			std::atomic<bool> finalize;
			std::mutex functionMutex;
			std::condition_variable functionCondition;
			utils::shared_ptr<FunctionWrapper> functionWrapper;			
		};

	public:

		MultipleRunThread() = default;
		MultipleRunThread(MultipleRunThread&& Other)  : sharedState(std::move(Other.sharedState)), thread(std::move(Other.thread)) {}
		MultipleRunThread(const MultipleRunThread&) = delete;
		virtual ~MultipleRunThread() { tryFinalize(); }


		MultipleRunThread& operator=(MultipleRunThread&& Other) { sharedState = std::move(Other.sharedState); thread = std::move(Other.thread); }
		MultipleRunThread& operator=(const MultipleRunThread&) = delete;

		template<typename F, class ...Args>
		std::future<typename std::result_of<F(Args...)>::type> run(F&& f, Args&& ...arguments)
		{
			typedef typename std::result_of<F(Args...)>::type result_type;

			if (sharedState != nullptr && sharedState->finalize == true){
				std::_Throw_Cpp_error(std::_OPERATION_NOT_PERMITTED);				
			}else if (joinable() == false){

				sharedState = utils::make_shared<SharedState>();
				sharedState->finalize = false;

				thread = Thread([=]{

					while (sharedState->finalize == false){
						std::unique_lock<std::mutex> lock(sharedState->functionMutex);
						if (sharedState->finalize == true){
							break;
						}
						sharedState->functionCondition.wait(lock);
						if (sharedState->functionWrapper != nullptr){
							auto fw = sharedState->functionWrapper;
							sharedState->functionWrapper.reset();

							fw();
						}
					}
				});
			}

			std::future<result_type> ret;

			{
				std::lock_guard<std::mutex> lock(sharedState->functionMutex);

				if (sharedState->functionWrapper != nullptr && sharedState->functionWrapper->valid() == true){
					std::_Throw_Cpp_error(std::_OPERATION_NOT_PERMITTED);
				}

				std::packaged_task<result_type()> innerTask(std::bind(std::_Decay_copy(std::forward<F>(f)), std::_Decay_copy(std::forward<Args>(arguments))...););
				ret = innerTask.get_future();
				sharedState->functionWrapper.reset(new FunctionWrapper(std::move(innerTask), std::move(arguments)...));
			}

			sharedState->functionCondition.notify_one();
			return ret;
		}

		void swap(MultipleRunThread& Other) 
		{
			std::swap(sharedState, Other.sharedState);
			std::swap(thread, Other.thread);
		}

		const bool joinable() const 
		{
			if (sharedState != nullptr && sharedState->finalize_ == true){
				std::_Throw_Cpp_error(std::_OPERATION_NOT_PERMITTED);
			}

			return thread.joinable();
		}

		void join() {

			tryFinalize();
			thread.join();
		}

		void detach() {

			tryFinalize();
			thread.detach();
		}

		std::thread::id get_id() const { return thread.get_id(); }
		std::thread::native_handle_type native_handle() { return thread.native_handle(); }

	private:

		void tryFinalize()
		{
			if (sharedState != nullptr){
				{
					std::lock_guard<std::mutex> lock(sharedState->functionMutex);
					sharedState->finalize = true;
				}
				sharedState->functionCondition.notify_one();
			}
		}

	protected:
		Thread thread;

	private:
		utils::shared_ptr<SharedState> sharedState;
	};
}

#endif	// __HEADER_GUARD_THREADINGUTILS__RUNNABLETHREAD_H__