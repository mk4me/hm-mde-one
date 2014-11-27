/********************************************************************
	created:  2014/10/07	10:52:53
	filename: InterruptiblePolicy.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_THREADINGUTILS__INTERRUPTIBLEPOLICY_H__
#define __HEADER_GUARD_THREADINGUTILS__INTERRUPTIBLEPOLICY_H__

#include <mutex>
#include <condition_variable>
#include <exception>
#include <chrono>
#include <threadingUtils/Future.h>
#include <threadingUtils/InterruptFlag.h>
#include <utils/SmartPtr.h>

namespace threadingUtils
{
	//! Wyj¹tek rzucany przy przerywaniu w¹tku
	class ThreadInterruptedException : public std::runtime_error
	{
	public:
		ThreadInterruptedException() : std::runtime_error(std::string()) {}
		explicit ThreadInterruptedException(const char * const & e) : std::runtime_error(e) {}
		//ThreadInterruptedException(const char * const & e, const int s) : std::runtime_error(e, s) {}
		ThreadInterruptedException(const ThreadInterruptedException& e) : std::runtime_error(e) {}
		ThreadInterruptedException& operator=(const ThreadInterruptedException& e) { std::exception::operator=(e); return *this; }
		virtual ~ThreadInterruptedException() {}
	};

	//! \tparam InterruptiblePolicy Polityka przerywalnoœci w¹tku
	//! RAII u¿ywane do inicjalizacji w¹tku wspieraj¹cego przerywanie
	template<typename InterruptiblePolicy>
	struct InterruptibleThreadGuard
	{
	public:
		InterruptibleThreadGuard(InterruptiblePolicy & interrupt) : interrupt(interrupt)
		{
			interrupt.initializeThread();
		}

		~InterruptibleThreadGuard()
		{
			interrupt.deinitializeThread();
		}

	private:
		InterruptiblePolicy & interrupt;
	};

	//! Polityka bez przerywalnoœci
	class NoInterrupltiblePolicy
	{
	public:

		NoInterrupltiblePolicy() = default;
		~NoInterrupltiblePolicy() = default;

		void interrupt() {}
		static void interrupt(void*& privateData) {}
		const bool interuptible() const { return false; }

		static void interruptionPoint() {}
		static void resetInterruption() {}

		void initializeThread() {}
		void deinitializeThread() {}

		static void initializeContext(void*& privateData) {}
		static void deinitializeContext(void*& privateData) {}

		template<class Rep = long long,
		class Period = std::chrono::milliseconds>
			static void wait(std::condition_variable & cv,
			std::unique_lock<std::mutex> & lock,
			const std::chrono::duration<Rep, Period> & timeout = std::chrono::milliseconds(1))
		{
			cv.wait(lock);
		}

		template<typename Predicate, class Rep = long long,
		class Period = std::chrono::milliseconds>
			static void wait(std::condition_variable & cv,
			std::unique_lock<std::mutex> & lock,
			Predicate pred,
			const std::chrono::duration<Rep, Period> & timeout = std::chrono::milliseconds(1))
		{
			cv.wait(lock, pred);
		}

		template<typename Lockable, class Rep = long long,
		class Period = std::chrono::milliseconds>
			static void wait(std::condition_variable_any & cva,
			Lockable & lock,
			const std::chrono::duration<Rep, Period> & timeout = std::chrono::milliseconds(1))
		{
			std::unique_lock<Lockable> l(lock);
			cva.wait(lock);
		}

		template<typename Lockable, typename Predicate, class Rep = long long,
		class Period = std::chrono::milliseconds>
			static void wait(std::condition_variable_any & cva,
			Lockable & lock,
			Predicate pred,
			const std::chrono::duration<Rep, Period> & timeout = std::chrono::milliseconds(1))
		{
			std::unique_lock<Lockable> l(lock);
			cva.wait(lock, pred);
		}

		template<typename T, class Rep = long long,
		class Period = std::chrono::milliseconds>
			static void wait(std::future<T> & f,
			const std::chrono::duration<Rep, Period> & timeout = std::chrono::milliseconds(1))
		{
			f.wait();
		}
	};

	class InterrupltiblePolicy
	{
	private:
		utils::shared_ptr<std::atomic<bool>> interuptible_;
		InterruptFlag flag;	

	public:

		InterrupltiblePolicy()
		{
		
		}

		~InterrupltiblePolicy()
		{

		}

		InterrupltiblePolicy(const InterrupltiblePolicy & Other) = delete;

		void interrupt()
		{
			flag.set();
		}

		static void interrupt(void*& privateData)
		{
			if (privateData != nullptr){
				InterruptFlag * iFlag = static_cast<InterruptFlag*>(privateData);
				iFlag->set();
			}
			else{
				throw std::runtime_error("Uninitialized private data for interruption");
			}
		}

		static void resetInterruption()
		{
			InterruptFlag::threadReset();
		}

		static void interruptionPoint()
		{
			if (InterruptFlag::threadIsSet() == true){
				throw ThreadInterruptedException();
			}
		}

		void initializeThread() { InterruptFlag::threadInterruptFlag() = &flag; interuptible_ = utils::make_shared<std::atomic<bool>>(true); }
		void deinitializeThread() { interuptible_.reset(); InterruptFlag::threadInterruptFlag() = nullptr; }
		const bool interruptible() const { return interuptible_ != nullptr && *interuptible_ == true; }

		static void initializeContext(void*& privateData) { privateData = InterruptFlag::threadInterruptFlag(); }
		static void deinitializeContext(void*& privateData) { privateData = nullptr; }
		static const bool interruptible(void*& privateData)
		{
			bool ret = true;
			if (privateData != nullptr){
				InterruptFlag * iFlag = static_cast<InterruptFlag*>(privateData);
				ret = iFlag->isSet();
			}
			
			return !ret;
		}

		template<typename Duration>
		static void wait(std::condition_variable & cv,
			std::unique_lock<std::mutex> & lock,
			const Duration & timeout = Duration(1))
		{
			interruptionPoint();
			InterruptFlag::threadInterruptFlag()->setConditionVariable(cv);
			InterruptFlag::ClearConditionVariableOnDestruct guard;
			interruptionPoint();
			cv.wait_for(lock, timeout);			
			interruptionPoint();
		}

		template<typename Predicate, typename Duration>
		static void wait(std::condition_variable & cv,
			std::unique_lock<std::mutex> & lock,
			Predicate pred,
			const Duration & timeout = Duration(1))
		{
			bool res = false;
			interruptionPoint();
			InterruptFlag::threadInterruptFlag()->setConditionVariable(cv);
			InterruptFlag::ClearConditionVariableOnDestruct guard;
			while ((InterruptFlag::threadInterruptFlag()->isSet() == false) && ((res = pred()) == false))
			{
				cv.wait_for(lock, timeout);
			}

			if (res == false){
				interruptionPoint();
			}
		}

		template<typename Lockable>
		static void wait(std::condition_variable_any & cva,
			Lockable & lock)
		{
			InterruptFlag::threadInterruptFlag()->wait(cva, lock);
		}

		template<typename Lockable, typename Predicate, typename Duration>
		static void wait(std::condition_variable_any & cva,
			Lockable & lock,Predicate pred,
			const Duration & timeout = Duration(1))
		{
			InterruptFlag::threadInterruptFlag()->wait(cva, lock, pred, timeout);
		}

		template<typename Lockable, typename Duration>
		static void wait(Lockable & lock,
			const Duration & timeout = Duration(1))
		{
			while (InterruptFlag::threadInterruptFlag()->isSet() == false)
			{
				if (lock.try_lock() == true){
					break;
				}

				std::this_thread::sleep_for(timeout);
			}

			interruptionPoint();
		}

		template<typename Predicate, typename Duration>
		static void wait(Predicate pred,
			const Duration & timeout = Duration(1))
		{
			bool res = false;
			while (InterruptFlag::threadInterruptFlag()->isSet() == false && (res = pred()) == false)
			{
				std::this_thread::sleep_for(timeout);
			}

			if (res == false){
				interruptionPoint();
			}
		}

		template<typename T, typename Duration>
		static void wait(std::future<T> & f,
			const Duration & timeout = Duration(1))
		{
			std::mutex mutex;
			std::unique_lock<std::mutex> lk(mutex);
			while (InterruptFlag::threadInterruptFlag()->isSet() == false)
			{
				if (f.wait_for(lk, timeout) == std::future_status::ready){
					break;
				}
			}

			interruptionPoint();
		}
	};
}

#endif	// __HEADER_GUARD_THREADINGUTILS__INTERRUPTIBLEPOLICY_H__
