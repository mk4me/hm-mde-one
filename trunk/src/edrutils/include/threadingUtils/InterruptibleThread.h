/********************************************************************
	created:  2014/10/07	12:55:02
	filename: InterruptibleThread.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_THREADINGUTILS__INTERRUPTIBLETHREAD_H__
#define __HEADER_GUARD_THREADINGUTILS__INTERRUPTIBLETHREAD_H__

#include <utils/Utils.h>
#include <threadingUtils/FunctionWrapper.h>
#include <threadingUtils/InterruptiblePolicy.h>
#include <threadingUtils/InterruptHandlingPolicy.h>
#include <threadingUtils/InterruptibleFuture.h>
#include <threadingUtils/InterruptiblePackagedTask.h>


namespace threadingUtils
{
	//! \tparam RunnableThread Polityka w¹tku z metod¹ uruchamiania zadañ run
	//! \tparam InterruptHandlingPolicy Polityka obs³ugi przerwania w w¹tku
	//! \tparam InteruptiblePolicy Polityka realizuj¹ca przerywanie w¹tku
	template<typename RunnableThread, typename ExceptionHandlePolicy = RawCallPolicy, typename InterruptHandlingPolicy = NoInterruptHandlingPolicy, typename InterruptiblePolicy = NoInterruptiblePolicy>
	class InterrubtibleThread
	{
	private:

		struct SharedState
		{
			SharedState()
			{

			}

			~SharedState()
			{

			}

			InterruptiblePolicy interruptible;
		};

	public:

		typedef InterruptiblePolicy InterruptiblePolicy;

	public:

		InterrubtibleThread() : sharedState(utils::make_shared<SharedState>())  {}
		InterrubtibleThread(InterrubtibleThread&& Other)  : sharedState(std::move(Other.sharedState)), thread(std::move(Other.thread)) {}
		InterrubtibleThread(const InterrubtibleThread&) = delete;
		virtual ~InterrubtibleThread() {}


		InterrubtibleThread& operator=(InterrubtibleThread&& Other) { sharedState = std::move(Other.sharedState); thread = std::move(Other.thread); }
		InterrubtibleThread& operator=(const InterrubtibleThread&) = delete;

		template<class F, class... Args>
		void run(F&& f, Args &&... arguments)
		{
			std::function<void()> ff(std::bind(utils::decay_copy(std::forward<F>(f)), utils::decay_copy(std::forward<Args>(arguments))...));

			thread.run([=](utils::shared_ptr<SharedState> sharedState)
			{
				InterruptibleThreadGuard<InterrupltiblePolicy> guard(sharedState->interruptible);
				try{
					ff();
				}
				catch (ThreadInterruptedException & e)
				{
					InterruptHandlingPolicy::handle(e);
				}
				catch (...){
					ExceptionHandlePolicy::handle(std::current_exception());
				}
			}, sharedState);
		}

		void swap(InterrubtibleThread& Other) { std::swap(sharedState, Other.sharedState); std::swap(thread, Other.thread); }
		const bool joinable() const { return thread.joinable(); }
		void join() { thread.join(); }
		void detach() { thread.detach(); }
		std::thread::id get_id() const { return thread.get_id(); }
		std::thread::native_handle_type native_handle() { return thread.native_handle(); }
		void interrupt() { interruptible.interrupt(); }
		const bool interruptible() const { return sharedState->interruptible.interruptible(); }
		static void interruptionPoint() { InterruptiblePolicy::interruptionPoint(); }
		static void resetInterruption() { InterruptiblePolicy::resetInterruption(); }

	protected:
		RunnableThread thread;

	private:
		utils::shared_ptr<SharedState> sharedState;
	};

	template<typename RunnableThread, typename ExceptionHandlePolicy = RawCallPolicy, typename InterruptHandlingPolicy = NoInterruptHandlingPolicy, typename InterruptiblePolicy = NoInterruptiblePolicy>
	class InterruptibleMultipleRunThread
	{
	private:

		typedef InterruptibleMultipleRunThread<RunnableThread, ExceptionHandlePolicy, InterruptHandlingPolicy, InterruptiblePolicy> MyThreadType;

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
			InterruptiblePolicy interruptible;
		};

	public:

		typedef InterruptiblePolicy InterruptiblePolicy;

	public:

		InterruptibleMultipleRunThread() {}
		InterruptibleMultipleRunThread(InterruptibleMultipleRunThread&& Other) : sharedState(std::move(Other.sharedState)), thread(std::move(Other.thread)) {}
		InterruptibleMultipleRunThread(const InterruptibleMultipleRunThread&) = delete;
		virtual ~InterruptibleMultipleRunThread() { }


		InterruptibleMultipleRunThread& operator=(InterruptibleMultipleRunThread&& Other) { sharedState = std::move(Other.sharedState); thread = std::move(Other.thread); return *this; }
		InterruptibleMultipleRunThread& operator=(const InterruptibleMultipleRunThread&) = delete;

		template<typename F, class ...Args>
		InterruptibleFuture<typename std::result_of<F(Args...)>::type, InterruptiblePolicy> run(F&& f, Args&& ...arguments)
		{
			typedef typename std::result_of<F(Args...)>::type result_type;

			if (sharedState != nullptr && sharedState->finalize == true){
				throw std::logic_error("Operation not permitted");
			}
			else if (sharedState == nullptr){

				sharedState = utils::make_shared<SharedState>();
				sharedState->finalize = false;

				thread.run([](utils::shared_ptr<SharedState> sharedState){

					InterruptibleThreadGuard<InterruptiblePolicy> guard(sharedState->interruptible);

					while (true){
						std::unique_lock<std::mutex> lock(sharedState->functionMutex);

						while (sharedState->functionCondition.wait_for(lock,
							std::chrono::milliseconds(100),
							[=]() { return (sharedState->functionWrapper != nullptr) || (sharedState->finalize == true); }) == false) {
						}

						if (sharedState->finalize == true){
							break;
						}

						if (sharedState->functionWrapper != nullptr){

							auto fw = sharedState->functionWrapper;
							sharedState->functionWrapper.reset();

							(*fw)();

							InterruptiblePolicy::resetInterruption();
						}
					}
				}, sharedState);
			}

			InterruptibleFuture<result_type, InterruptiblePolicy> ret;

			{
				std::lock_guard<std::mutex> lock(sharedState->functionMutex);

				if (sharedState->functionWrapper != nullptr){
					if (sharedState->functionWrapper->valid() == true){
						throw std::logic_error("Operation not permitted");
					}
				}

				std::function<result_type()> ff = std::bind(utils::decay_copy(std::forward<F>(f)), utils::decay_copy(std::forward<Args>(arguments))...);

				auto l = [=]()->result_type
				{
					try{
						return ff();
					}
					catch (ThreadInterruptedException & e){
						InterruptHandlingPolicy::handle(e);
						throw;
					}
					catch (...){
						auto e = std::current_exception();
						ExceptionHandlePolicy::handle(e);
						std::rethrow_exception(e);
					}					
				};
				
				InterruptiblePackagedTask<InterruptiblePolicy, result_type()> innerTask(l);
				ret = innerTask.get_future();
				sharedState->functionWrapper.reset(new FunctionWrapper(std::move(innerTask)));
			}

			sharedState->functionCondition.notify_one();
			return ret;
		}

		void swap(InterruptibleMultipleRunThread& Other) {
			std::swap(sharedState,Other.sharedState);
			std::swap(thread, Other.thread);
		}

		const bool joinable() const  {

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
		void interrupt() { if (sharedState == nullptr) { throw std::logic_error("Operation not permitted"); } sharedState->interruptible.interrupt(); }
		const bool interruptible() const { return sharedState != nullptr && sharedState->interruptible.interruptible(); }
		static void interruptionPoint() { InterruptiblePolicy::interruptionPoint(); }
		static void resetInterruption() { InterruptiblePolicy::resetInterruption(); }

	private:

		void tryFinalize()
		{
			if (sharedState != nullptr){
				sharedState->finalize = true;
				sharedState->functionCondition.notify_one();
			}
		}

	protected:
		RunnableThread thread;

	private:
		utils::shared_ptr<SharedState> sharedState;
	};
}

#endif	// __HEADER_GUARD_THREADINGUTILS__INTERRUPTIBLETHREAD_H__
