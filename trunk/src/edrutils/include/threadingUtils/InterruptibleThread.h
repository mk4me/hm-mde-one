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
	template<typename RunnableThread, typename InterruptHandlingPolicy = NoInterruptHandlingPolicy, typename InteruptiblePolicy = NoInterrupltiblePolicy>
	class InterrubtibleThread
	{
	private:

		struct SharedState
		{
			InterrupltiblePolicy interruptible;			
		};

	public:

		typedef InteruptiblePolicy InterruptPolicy;

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
					InteruptiblePolicy::handle(e);
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
		static void interruptionPoint() { InterrupltiblePolicy::interruptionPoint(); }
		static void resetInterruption() { InterrupltiblePolicy::resetInterruption(); }

	protected:
		RunnableThread thread;

	private:
		utils::shared_ptr<SharedState> sharedState;
	};

	template<typename RunnableThread, typename CallPolicy = RawCallPolicy, typename InterruptHandlingPolicy = NoInterruptHandlingPolicy, typename InteruptiblePolicy = NoInterrupltiblePolicy>
	class InterruptibleMultipleRunThread
	{
	private:

		typedef InterruptibleMultipleRunThread<RunnableThread, CallPolicy, InterruptHandlingPolicy, InteruptiblePolicy> MyThreadType;

		struct SharedState
		{
			std::atomic<bool> finalize;
			std::mutex functionMutex;
			std::condition_variable functionCondition;
			utils::shared_ptr<FunctionWrapper> functionWrapper;
			InterrupltiblePolicy interruptible;			
		};

	public:

		typedef InteruptiblePolicy InterruptPolicy;	

	public:

		InterruptibleMultipleRunThread() {}
		InterruptibleMultipleRunThread(InterruptibleMultipleRunThread&& Other) : sharedState(std::move(Other.sharedState)), thread(std::move(Other.thread)) {}
		InterruptibleMultipleRunThread(const InterruptibleMultipleRunThread&) = delete;
		virtual ~InterruptibleMultipleRunThread() { tryFinalize(); }


		InterruptibleMultipleRunThread& operator=(InterruptibleMultipleRunThread&& Other) { sharedState = std::move(Other.sharedState); thread = std::move(Other.thread); return *this; }
		InterruptibleMultipleRunThread& operator=(const InterruptibleMultipleRunThread&) = delete;

		template<typename F, class ...Args>
		InterruptibleFuture<typename std::result_of<F(Args...)>::type, InteruptiblePolicy> run(F&& f, Args&& ...arguments)
		{
			typedef typename std::result_of<F(Args...)>::type result_type;

			if (sharedState != nullptr && sharedState->finalize == true){
				throw std::logic_error("Operation not permitted");
			}
			else if (sharedState == nullptr){

				sharedState = utils::make_shared<SharedState>();
				sharedState->finalize = false;

				thread.run([](utils::shared_ptr<SharedState> sharedState){

					InterruptibleThreadGuard<InterrupltiblePolicy> guard(sharedState->interruptible);

					while (sharedState->finalize == false){
						std::unique_lock<std::mutex> lock(sharedState->functionMutex);
						if (sharedState->finalize == true){
							break;
						}
						sharedState->functionCondition.wait(lock);
						if (sharedState->functionWrapper != nullptr){

							auto fw = sharedState->functionWrapper;
							sharedState->functionWrapper.reset();

							(*fw)();

							InterrupltiblePolicy::resetInterruption();

							//TODO
							//dodaæ RAII dla resetu przerwania i uchwytu funktora
							
						}
					}
				}, sharedState);
			}

			InterruptibleFuture<result_type, InteruptiblePolicy> ret;

			{
				std::lock_guard<std::mutex> lock(sharedState->functionMutex);

				if (sharedState->functionWrapper != nullptr){
					if (sharedState->functionWrapper->valid() == true){
						throw std::logic_error("Operation not permitted");
					}
				}

				std::function<result_type()> intf = std::bind(utils::decay_copy(std::forward<F>(f)), utils::decay_copy(std::forward<Args>(arguments))...);
				
				InterruptiblePackagedTask<InteruptiblePolicy, result_type()> innerTask(intf);
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
		static void interruptionPoint() { InterrupltiblePolicy::interruptionPoint(); }
		static void resetInterruption() { InterrupltiblePolicy::resetInterruption(); }

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
		RunnableThread thread;

	private:
		utils::shared_ptr<SharedState> sharedState;
	};
}

#endif	// __HEADER_GUARD_THREADINGUTILS__INTERRUPTIBLETHREAD_H__
