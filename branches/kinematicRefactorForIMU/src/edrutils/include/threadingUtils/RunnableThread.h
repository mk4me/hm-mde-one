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
#include <utils/Utils.h>
namespace threadingUtils
{
	//! \tparam Thread Typ w�tku jaki wrapujemy do postaci runnable
	//! \tparam CallPolicy Spos�b wo�ania metod przez w�tek
	template<typename Thread, typename ExceptionHandlePolicy = RawCallPolicy>
	class RunnableThread
	{
	public:

		//! Konstruktor domy�lny
		RunnableThread()  : launched(false) {}
		//! \param Other W�tek kt�rego zasoby przejmujemy
		RunnableThread(RunnableThread&& Other)  : launched(std::move(Other.launched)), thread(std::move(Other.thread)) {}
		RunnableThread(const RunnableThread&) = delete;
		//! Destruktor
		~RunnableThread() {}
		
		//! \param Other W�tek kt�rego zasoby przejmujemy
		RunnableThread& operator=(RunnableThread&& Other) { launched = std::move(Other.launched); thread = std::move(Other.thread); return *this; }
		RunnableThread& operator=(const RunnableThread&) = delete;

		template<class F, class... Args>
		void run(F&& f, Args&&... arguments)
		{
			if (launched == true){
				throw std::logic_error("Operation not permitted");
			}
			else{				

				std::function<void()> ff = std::bind(utils::decay_copy(std::forward<F>(f)), utils::decay_copy(std::forward<Args>(arguments))...);

				thread = Thread([=]{

					try{
						ff();
					}
					catch (...){
						ExceptionHandlePolicy::handle(std::current_exception());
					}
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

	template<typename Thread, typename ExceptionHandlePolicy = RawCallPolicy>
	class MultipleRunThread
	{
	private:

		typedef MultipleRunThread<Thread, ExceptionHandlePolicy> MyMultipleRunThread;

		struct SharedState
		{
			SharedState()
			{

			}

			~SharedState()
			{

			}

			std::atomic<bool> finalize;
			std::mutex functionMutex;
			std::condition_variable functionCondition;
			utils::shared_ptr<FunctionWrapper> functionWrapper;			
		};

	public:

		MultipleRunThread() = default;
		MultipleRunThread(MultipleRunThread&& Other) : sharedState(std::move(Other.sharedState)), thread(std::move(Other.thread)) {}
		MultipleRunThread(const MultipleRunThread&) = delete;
		virtual ~MultipleRunThread() { }


		MultipleRunThread& operator=(MultipleRunThread&& Other) { sharedState = std::move(Other.sharedState); thread = std::move(Other.thread); }
		MultipleRunThread& operator=(const MultipleRunThread&) = delete;

		template<typename F, class ...Args>
		std::future<typename std::result_of<F(Args...)>::type> run(F&& f, Args&& ...arguments)
		{
			typedef typename std::result_of<F(Args...)>::type result_type;

			if (sharedState != nullptr && sharedState->finalize == true){
				throw std::logic_error("Operation not permitted");
			}else if (joinable() == false){

				sharedState = utils::make_shared<SharedState>();
				sharedState->finalize = false;

				thread = Thread([](utils::shared_ptr<SharedState> sharedState){

					while (true){
						std::unique_lock<std::mutex> lock(sharedState->functionMutex);

						while (sharedState->functionCondition.wait_for(lock,
							std::chrono::milliseconds(100),
							[=]() { return (sharedState->functionWrapper != nullptr) || (sharedState->finalize == true); }) == false) {}

						if (sharedState->finalize == true){
							break;
						}

						if (sharedState->functionWrapper != nullptr){
							auto fw = sharedState->functionWrapper;
							sharedState->functionWrapper.reset();

							(*fw)();
						}
					}
				}, sharedState);
			}

			std::future<result_type> ret;

			{
				std::lock_guard<std::mutex> lock(sharedState->functionMutex);

				if (sharedState->functionWrapper != nullptr && sharedState->functionWrapper->valid() == true){
					throw std::logic_error("Operation not permitted");
				}

				std::function<result_type()> ff = std::bind(utils::decay_copy(std::forward<F>(f)), utils::decay_copy(std::forward<Args>(arguments))...);

				auto l = [=]()->result_type
				{
					try{
						return ff();
					}
					catch (...){
						auto e = std::current_exception();
						ExceptionHandlePolicy::handle(e);
						std::rethrow_exception(e);
					}
				};

				std::packaged_task<result_type()> innerTask(l);
				ret = innerTask.get_future();
				sharedState->functionWrapper.reset(new FunctionWrapper(std::move(innerTask)));
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
			if (sharedState != nullptr && sharedState->finalize == true){
				throw std::logic_error("Operation not permitted");
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
				sharedState->finalize = true;
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
