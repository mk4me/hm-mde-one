/********************************************************************
	created:  2014/10/24	09:43:38
	filename: InterruptibleInterruptibleJobManager.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_THREADINGUTILS__INTERRUPTIBLEJOBMANAGER_H__
#define __HEADER_GUARD_THREADINGUTILS__INTERRUPTIBLEJOBMANAGER_H__

#include <threadingUtils/IJob.h>
#include <threadingUtils/FunctionWrapper.h>
#include <future>
#include <atomic>
#include <mutex>
#include <functional>
#include <type_traits>
#include <utils/SmartPtr.h>

namespace threadingUtils
{
	template<typename InterruptibleWorkManager>
	class InterruptibleJobManager
	{
	public:

		template<typename T>
		struct FutureType
		{
			typedef typename InterruptibleWorkManager::template FutureType<T> FTR;
			typedef typename FTR::type type;
		};

		template<typename T>
		class InterruptibleJob : public IJob
		{

			friend class InterruptibleJobManager;

		private:

			struct SharedState
			{
				std::function<T()> functionWrapper;
				std::atomic<Status> status_;
				std::recursive_mutex statusMutex;
			};

			
			typedef typename FutureType<T>::type FutureType;

		private:

			template<typename F, class ...Args>
			InterruptibleJob(InterruptibleWorkManager * workManager, F&& f, Args&& ...arguments) : workManager(workManager),
				sharedState(utils::make_shared<SharedState>()), detached_(false)
			{
				sharedState->functionWrapper = std::bind(std::_Decay_copy(std::forward<F>(f)), std::_Decay_copy(std::forward<Args>(arguments))...);
				sharedState->status_ = Initialized;
			}

		public:

			InterruptibleJob() : workManager(nullptr), detached_(false) {}

			InterruptibleJob(InterruptibleJob && Other) : workManager(std::move(Other.workManager)),
				sharedState(std::move(Other.sharedState)),
				future(std::move(Other.future)),
				detached_(std::move(Other.detached_))
			{

			}

			InterruptibleJob(const InterruptibleJob & Other) = delete;

			virtual ~InterruptibleJob()
			{
				if (detached_ == false && future.valid() == true){
					wait();
				}
			}

			//! \param Other W¹tek którego zasoby przejmujemy
			InterruptibleJob& operator=(InterruptibleJob&& Other) 
			{
				workManager = std::move(Other.workManager);
				sharedState = std::move(Other.sharedState);
				future = std::move(Other.future);
				detached_ = std::move(Other.detached_);
			}

			InterruptibleJob& operator=(const InterruptibleJob&) = delete;

			void swap(InterruptibleJob & Other) 
			{
				std::swap(workManager, Other.workManager);
				std::swap(sharedState, Other.sharedState);
				std::swap(future, Other.future);
				std::swap(detached_, Other.detached_);
			}

			virtual void start()
			{
				if (sharedState == nullptr){
					throw std::runtime_error("No job assigned");
				}else if (workManager == nullptr){
					sharedState->status_ = Failed;
				}				
				else if (sharedState->status_ != Initialized)
				{
					throw std::runtime_error("Job already started");
				}

				sharedState->status_ = Pending;

				try{					
					future = workManager->submit([=]{

						{
							std::lock_guard<std::recursive_mutex> lock(sharedState->statusMutex);
							if (sharedState->status_ == Cancelled){
								throw std::runtime_error("Work canceled");
							}

							sharedState->status_ = Running;
						}
						try{
							T ret(sharedState->functionWrapper());
							{
								std::lock_guard<std::recursive_mutex> lock(sharedState->statusMutex);
								sharedState->status_ = Finished;
							}
							return ret;
						}
						catch (...){
							std::lock_guard<std::recursive_mutex> lock(sharedState->statusMutex);
							sharedState->status_ = Failed;
							throw;
						}
					});
				}
				catch (...){
					sharedState->status_ = Failed;
				}
			}

			virtual void wait() override
			{
				future.wait();
			}

			virtual void cancel()
			{
				std::lock_guard<std::recursive_mutex> lock(sharedState->statusMutex);
				if (cancellable() == true){
					sharedState->status_ = Cancelled;					
					try{
						future.interrupt();
					}
					catch (...)
					{

					}
				}
			}

			virtual const bool cancellable() const
			{
				std::lock_guard<std::recursive_mutex> lock(sharedState->statusMutex);
				if (sharedState == nullptr || sharedState->status_ == Cancelled || sharedState->status_ == Finished || sharedState->status_ == Failed){
					return false;
				}

				return sharedState->status_ == Pending || future.interruptible();
			}

			virtual void detach() override
			{
				std::lock_guard<std::recursive_mutex> lock(sharedState->statusMutex);
				if (sharedState == nullptr || (sharedState != nullptr && sharedState->status_ != Pending && sharedState->status_ != Running)){
					detached_ = true;
				}
			}

			template<class Rep, class Per>
			const std::future_status wait_for(
				const std::chrono::duration<Rep, Per>& _Rel_time) const
			{	// wait for duration
				return future.wait_for(_Rel_time);
			}

			template<class _Clock, class _Dur>
			const std::future_status wait_until(
				const std::chrono::time_point<_Clock, _Dur>& _Abs_time) const
			{	// wait until time point
				return future.wait_until(_Abs_time);
			}

			virtual const Status status() const override
			{
				return (sharedState == nullptr) ? Initialized : sharedState->status_;
			}

			const T get()
			{
				return future.get();
			}

		private:
			InterruptibleWorkManager * workManager;
			bool detached_;
			FutureType future;
			utils::shared_ptr<SharedState> sharedState;
		};

		template<typename T>
		class InterruptibleJob<T&> : public IJob
		{

			friend class InterruptibleJobManager;

		private:

			struct SharedState
			{
				std::function<T&()> functionWrapper;
				std::atomic<Status> status_;
				std::recursive_mutex statusMutex;
			};

			typedef typename FutureType<void>::type FutureType;

		private:

			template<typename F, class ...Args>
			InterruptibleJob(InterruptibleWorkManager * workManager, F&& f, Args&& ...arguments) : workManager(workManager),
				sharedState(utils::make_shared<SharedState>()), detached_(false)
			{
				sharedState->functionWrapper = std::bind(std::_Decay_copy(std::forward<F>(f)), std::_Decay_copy(std::forward<Args>(arguments))...);
				sharedState->status_ = Initialized;
			}

		public:

			InterruptibleJob() : workManager(nullptr), detached_(false) {}

			InterruptibleJob(InterruptibleJob && Other) : workManager(std::move(Other.workManager)),
				sharedState(std::move(Other.sharedState)),
				future(std::move(Other.future)),
				detached_(std::move(Other.detached_))
			{

			}

			InterruptibleJob(const InterruptibleJob & Other) = delete;

			virtual ~InterruptibleJob()
			{
				if (detached_ == false && future.valid() == true){
					wait();
				}
			}

			//! \param Other W¹tek którego zasoby przejmujemy
			InterruptibleJob& operator=(InterruptibleJob&& Other) 
			{
				workManager = std::move(Other.workManager);
				sharedState = std::move(Other.sharedState);
				future = std::move(Other.future);
				detached_ = std::move(Other.detached_);
			}

			InterruptibleJob& operator=(const InterruptibleJob&) = delete;

			void swap(InterruptibleJob & Other) 
			{
				std::swap(workManager, Other.workManager);
				std::swap(sharedState, Other.sharedState);
				std::swap(future, Other.future);
				std::swap(detached_, Other.detached_);
			}

				virtual void start()
			{
				if (sharedState == nullptr){
					throw std::runtime_error("No job assigned");
				}
				else if (workManager == nullptr){
					sharedState->status_ = Failed;
				}
				else if (sharedState->status_ != Initialized)
				{
					throw std::runtime_error("Job already started");
				}

				sharedState->status_ = Pending;

				try{
					future = workManager->submit([=]{

						{
							std::lock_guard<std::recursive_mutex> lock(sharedState->statusMutex);
							if (sharedState->status_ == Cancelled){
								throw std::runtime_error("Work canceled");
							}

							sharedState->status_ = Running;
						}
					try{
						T ret(sharedState->functionWrapper());
						{
							std::lock_guard<std::recursive_mutex> lock(sharedState->statusMutex);
							sharedState->status_ = Finished;
						}
						return ret;
					}
					catch (...){
						std::lock_guard<std::recursive_mutex> lock(sharedState->statusMutex);
						sharedState->status_ = Failed;
						throw;
					}
				});
				}
				catch (...){
					sharedState->status_ = Failed;
				}
			}

			virtual void wait() override
			{
				future.wait();
			}

			virtual void cancel()
			{
				std::lock_guard<std::recursive_mutex> lock(sharedState->statusMutex);
				if (cancellable() == true){
					sharedState->status_ = Cancelled;
					try{
						future.interrupt();
					}
					catch (...)
					{

					}
				}
			}

			virtual const bool cancellable() const
			{
				std::lock_guard<std::recursive_mutex> lock(sharedState->statusMutex);
				if (sharedState == nullptr || sharedState->status_ == Cancelled || sharedState->status_ == Finished || sharedState->status_ == Failed){
					return false;
				}

				return sharedState->status_ == Pending || future.interruptible();
			}

			virtual void detach() override
			{
				std::lock_guard<std::recursive_mutex> lock(sharedState->statusMutex);
				if (sharedState == nullptr || (sharedState != nullptr && sharedState->status_ != Pending && sharedState->status_ != Running)){
					detached_ = true;
				}
			}

			template<class Rep, class Per>
			const std::future_status wait_for(
				const std::chrono::duration<Rep, Per>& _Rel_time) const
			{	// wait for duration
				return future.wait_for(_Rel_time);
			}

			template<class _Clock, class _Dur>
			const std::future_status wait_until(
				const std::chrono::time_point<_Clock, _Dur>& _Abs_time) const
			{	// wait until time point
				return future.wait_until(_Abs_time);
			}

			virtual const Status status() const override
			{
				return (sharedState == nullptr) ? Initialized : sharedState->status_;
			}

			const T& get()
			{
				return future.get();
			}

		private:
			InterruptibleWorkManager * workManager;
			bool detached_;
			FutureType future;
			utils::shared_ptr<SharedState> sharedState;
		};

		template<>
		class InterruptibleJob<void> : public IJob
		{

			friend class InterruptibleJobManager;

		private:

			struct SharedState
			{
				std::function<void()> functionWrapper;
				std::atomic<Status> status_;
				std::recursive_mutex statusMutex;
			};

			typedef typename FutureType<void>::type FutureType;

		private:

			template<typename F, class ...Args>
			InterruptibleJob(InterruptibleWorkManager * workManager, F&& f, Args&& ...arguments) : workManager(workManager),
				sharedState(utils::make_shared<SharedState>()), detached_(false)
			{
				sharedState->functionWrapper = std::bind(std::_Decay_copy(std::forward<F>(f)), std::_Decay_copy(std::forward<Args>(arguments))...);
				sharedState->status_ = Initialized;
			}

		public:

			InterruptibleJob() : workManager(nullptr), detached_(false) {}

			InterruptibleJob(InterruptibleJob && Other) : workManager(std::move(Other.workManager)),
				sharedState(std::move(Other.sharedState)),
				future(std::move(Other.future)),
				detached_(std::move(Other.detached_))
			{

			}

			InterruptibleJob(const InterruptibleJob & Other) = delete;

			virtual ~InterruptibleJob()
			{
				if (detached_ == false && future.valid() == true){
					wait();
				}
			}

			//! \param Other W¹tek którego zasoby przejmujemy
			InterruptibleJob& operator=(InterruptibleJob&& Other) 
			{
				workManager = std::move(Other.workManager);
				sharedState = std::move(Other.sharedState);
				future = std::move(Other.future);
				detached_ = std::move(Other.detached_);
				return *this;
			}

			InterruptibleJob& operator=(const InterruptibleJob&) = delete;

			void swap(InterruptibleJob & Other) 
			{
				std::swap(workManager, Other.workManager);
				std::swap(sharedState, Other.sharedState);
				std::swap(future, Other.future);
				std::swap(detached_, Other.detached_);
			}

				virtual void start()
			{
				if (sharedState == nullptr){
					throw std::runtime_error("No job assigned");
				}
				else if (workManager == nullptr){
					sharedState->status_ = Failed;
				}
				else if (sharedState->status_ != Initialized)
				{
					throw std::runtime_error("Job already started");
				}

				sharedState->status_ = Pending;

				try{
					future = workManager->submit([](utils::shared_ptr<SharedState> sharedState){

						{
							std::lock_guard<std::recursive_mutex> lock(sharedState->statusMutex);
							if (sharedState->status_ == Cancelled){
								throw std::runtime_error("Work canceled");
							}

							sharedState->status_ = Running;
						}
					try{
						sharedState->functionWrapper();
						{
							std::lock_guard<std::recursive_mutex> lock(sharedState->statusMutex);
							sharedState->status_ = Finished;
						}
					}
					catch (...){
						std::lock_guard<std::recursive_mutex> lock(sharedState->statusMutex);
						sharedState->status_ = Failed;
						throw;
					}
				}, sharedState);
				}
				catch (...){
					sharedState->status_ = Failed;
				}
			}

			virtual void wait() override
			{
				future.wait();
			}

			virtual void cancel()
			{
				std::lock_guard<std::recursive_mutex> lock(sharedState->statusMutex);
				if (cancellable() == true){
					sharedState->status_ = Cancelled;
					try{
						future.interrupt();
					}
					catch (...)
					{

					}
				}
			}

			virtual const bool cancellable() const
			{
				std::lock_guard<std::recursive_mutex> lock(sharedState->statusMutex);
				if (sharedState == nullptr || sharedState->status_ == Cancelled || sharedState->status_ == Finished || sharedState->status_ == Failed){
					return false;
				}

				return sharedState->status_ == Pending || future.interruptible();
			}

			virtual void detach() override
			{
				std::lock_guard<std::recursive_mutex> lock(sharedState->statusMutex);
				if (sharedState == nullptr || (sharedState != nullptr && sharedState->status_ != Initialized)){
					detached_ = true;
				}
			}

			template<class Rep, class Per>
			const std::future_status wait_for(
				const std::chrono::duration<Rep, Per>& _Rel_time) const
			{	// wait for duration
				return future.wait_for(_Rel_time);
			}

			template<class _Clock, class _Dur>
			const std::future_status wait_until(
				const std::chrono::time_point<_Clock, _Dur>& _Abs_time) const
			{	// wait until time point
				return future.wait_until(_Abs_time);
			}

			virtual const Status status() const override
			{
				return (sharedState == nullptr) ? Initialized : sharedState->status_;
			}

			void get()
			{
				future.get();
			}

		private:
			InterruptibleWorkManager * workManager;
			bool detached_;
			FutureType future;
			utils::shared_ptr<SharedState> sharedState;
		};

	public:

		InterruptibleJobManager(InterruptibleWorkManager * workManager) : workManager(workManager) {}
		InterruptibleJobManager(InterruptibleJobManager && Other) : workManager(std::move(Other.workManager)) {}
		InterruptibleJobManager(const InterruptibleJobManager & Other) = delete;
		~InterruptibleJobManager() = default;

		template<typename F, class ...Args>
		InterruptibleJob<typename std::result_of<F(Args...)>::type> create(F&& f, Args&& ...arguments)
		{
			return InterruptibleJob<typename std::result_of<F(Args...)>::type>(workManager, std::move(f), std::move(arguments)...);
		}

		//! \tparam T Typ wyniku future
		//! \tparam FuturePolicy Typ future (shared | future)
		//! \param future Future na którego wykonanie czekamy realizuj¹c inne zadania		
		template<typename JobType>
		void waitForOtherJob(JobType & job)
		{
			workManager->waitForOtherTask(job.future);
		}

	private:
		InterruptibleWorkManager * workManager;
	};
}

#endif	// __HEADER_GUARD_THREADINGUTILS__INTERRUPTIBLEJOBMANAGER_H__