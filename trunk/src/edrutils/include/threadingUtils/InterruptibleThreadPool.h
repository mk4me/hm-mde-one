/********************************************************************
	created:  2014/10/24	09:43:55
	filename: InterruptibleThreadPool.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_THREADINGUTILS__INTERRUPTIBLETHREADPOOL_H__
#define __HEADER_GUARD_THREADINGUTILS__INTERRUPTIBLETHREADPOOL_H__

#include <list>
#include <utils/Debug.h>
#include <threadingUtils/InterruptibleThread.h>
#include <threadingUtils/FutureWrapper.h>

namespace threadingUtils
{
	//! \tparam RunnableThread Typ w¹tku runnable który wrapujemy
	//! \tparam CallPolicy Polityka wo³ania zadañ w¹tków
	//! \tparam InterruptPolicy Polityka przerywania dzia³ania w¹tku
	//! \tparam InterruptHandlingPolicy Polityka obs³ugi przerwania w¹tku
	//! Klasa realizuj¹ca funkcjonalnoœæ puli w¹tków
	template<class RunnableThread, typename CallPolicy, typename InterruptPolicy, typename InterruptHandlingPolicy>
	class InterruptibleThreadPool
	{
	public:
		//! Typ w¹tku wielokrotnego uruchamiania
		typedef InterruptibleMultipleRunThread<RunnableThread, CallPolicy, InterruptHandlingPolicy, InterruptPolicy> InnerThreadType;

		//! Typ listy w¹tków wielokrotnego uruchamiania
		typedef std::list<InnerThreadType> InnerThreadsList;

		//! Typ opisuj¹cy iloœæ
		typedef unsigned int size_type;

		//! Klasa w¹tku oferowanego z puli
		class Thread
		{
			//! ZaprzyjaŸnienie z pula w¹tku
			friend class MyThreadPoolType;

		public:
			//! Typ puli w¹tków
			typedef InterruptibleThreadPool<RunnableThread, CallPolicy, InterruptPolicy, InterruptHandlingPolicy> MyThreadPoolType;

			typedef InterruptPolicy InterruptPolicyType;

		public:

			//! \param threadPool Pula w¹tków do której nale¿a³ pierwotnie w¹tek
			Thread(MyThreadPoolType * threadPool) : threadPool(threadPool)
			{

			}

			//! \param threadPool Pula w¹tków do której nale¿a³ pierwotnie w¹tek
			//! \param thread W¹tek wielokrotnego uruchamiania
			Thread(MyThreadPoolType * threadPool, InnerThreadType && thread)
				: threadPool(threadPool), thread(std::move(thread))
			{

			}

		public:

			Thread() : threadPool(nullptr)
			{

			}

			Thread(const Thread & Other) = delete;

			//! \param Other W¹tek którego zasoby przejmujemy
			Thread(Thread && Other) : threadPool(std::move(Other.threadPool)),
				thread(std::move(Other.thread)), futureWrapper(std::move(Other.futureWrapper))
			{
				Other.threadPool = nullptr;
			}

			//! Destruktor
			~Thread()
			{
				if (joinable() == true){
					join();
				}

				if (threadPool != nullptr){
					threadPool->tryReturn(thread);
				}

				if (thread.joinable() == true){
					thread.join();
				}
			}

			Thread& operator=(Thread&& Other) { threadPool = std::move(Other.threadPool); Other.threadPool = nullptr; thread = std::move(Other.thread); futureWrapper = std::move(Other.futureWrapper); return *this; }
			Thread& operator=(const Thread&) = delete;

			template<typename F, class ...Args>
			void run(F&& f, Args&& ...arguments)
			{
				futureWrapper = std::move(thread.run(std::move(f), std::move(arguments)...));
			}

			void swap(Thread& Other) { std::swap(threadPool, Other.threadPool); std::swap(thread, Other.thread); std::swap(futureWrapper, Other.futureWrapper); }
			const bool joinable() const { return futureWrapper.valid() && thread.joinable(); }
			void join() { futureWrapper.wait(); }
			void detach() { threadPool->detach(); threadPool = nullptr; futureWrapper.reset(); thread.detach(); }
			std::thread::id get_id() const { return thread.get_id(); }
			std::thread::native_handle_type native_handle() { return thread.native_handle(); }
			void interrupt() { if (futureWrapper.valid() == false || thread.interruptible() == false) { throw std::logic_error("Operation not permitted"); } thread.interrupt(); }
			const bool interruptible() const { return futureWrapper.valid() && thread.interruptible(); }
			static void interruptionPoint() { InnerThreadType::interruptionPoint(); }
			static void resetInterruption() { InnerThreadType::resetInterruption(); }

		private:
			//! Pula w¹tków do której nale¿y w¹tek
			MyThreadPoolType * threadPool;
			//! Wewnêtrzny w¹tek wielokrotnego uruchamiania
			InnerThreadType thread;
			//! Wrapper dla future celem okreœlenia czy w¹tek zakoñczy³ przetwarzanie zleconego zadania
			FutureWrapper futureWrapper;
		};

		//! ZaprzyjaŸnienie w¹tku z pula w¹tków
		friend class Thread;

		//! Typ agreguj¹cy w¹tki które pobieraj¹ klienci
		typedef std::list<Thread> Threads;

	public:

		InterruptibleThreadPool(const InterruptibleThreadPool & Other) = delete;
		InterruptibleThreadPool(InterruptibleThreadPool && Other) = delete;

		//! \param minThreads Minimalna iloœæ w¹tków do utrzymania
		//! \param maxThreads Maksymalna iloœæ w¹tków do utrzymania
		InterruptibleThreadPool(const size_type minThreads, const size_type maxThreads) : minThreads_(minThreads), maxThreads_(maxThreads), threadsCount_(0) {}
		//! Domyœlny konstruktor
		InterruptibleThreadPool() : InterruptibleThreadPool(std::max<unsigned int>(std::thread::hardware_concurrency(), 1), std::thread::hardware_concurrency() == 0 ? 1 : std::thread::hardware_concurrency() * 8) {}
		
		//! Desturktor
		~InterruptibleThreadPool()
		{
			std::lock_guard<std::mutex> lock(mutex);
			UTILS_ASSERT((threadsCount_ == 0), "ThreadPool finished while some threads are still alive");
			for (auto & t : innerThreads){
				if (t.joinable() == true){
					t.join();
				}
			}
		}

		//! \param maxThreads Maksymalna iloœæ w¹tków jakie mo¿na stworzyæ
		void setMaxThreads(size_type maxThreads)
		{
			std::lock_guard<std::mutex> lock(mutex);
			if (maxThreads < minThreads_)
			{
				throw ThreadPoolConfigurationException("Improper max threads value. Must be greater or equal to minimum threads value");
			}

			maxThreads_ = maxThreads;
		}
		//! \param minThreads Minimalna iloœæ w¹tków jaka bêdzie utrzymywana
		void setMinThreads(size_type minThreads)
		{
			std::lock_guard<std::mutex> lock(mutex);
			if (maxThreads < minThreads_)
			{
				throw ThreadPoolConfigurationException("Improper min threads value. Must be less or equal to maximum threads value");
			}

			if (minThreads < 0)
			{
				throw ThreadPoolConfigurationException("Improper min threads value. Must be greater or equal 0");
			}

			minThreads_ = minThreads;
			while (innerThreads.size() > minThreads_)
			{
				auto t = std::move(innerThreads.front());
				innerThreads.pop_front();
				if (t.joinable() == true){
					try{
						t.join();
					}
					catch (...){

					}
				}
			}
		}
		//! \return Maksymalna iloœc w¹tków jakie mo¿na utworzyæ
		const size_type maxThreads() const { return maxThreads_; }
		//! \return Minimalna iloœæ w¹tków utrzymywana przez manager
		const size_type minThreads() const { return minThreads_; }
		//! \return Iloœæ aktualnie zajêtych w¹tków
		const size_type threadsCount() const { return threadsCount_; }
		//! \return Nowy w¹tek		
		Thread get()
		{
			std::lock_guard<std::mutex> lock(mutex);
			if (threadsCount_ < maxThreads_){
				InnerThreadType ith;
				if (innerThreads.empty() == false){
					ith = std::move(innerThreads.front());
					innerThreads.pop_front();
				}

				Thread th(this, std::move(ith));
				++threadsCount_;
				return std::move(th);
			}
			else{
				throw std::runtime_error("ThreadPool empty");
			}
		}

		//! \param groupSize Iloœæ w¹tków w grupie
		//! \param threads [out] Lista z nowymi w¹tkami, dopisujemy zawsze na koñcu
		//! \return Iloœæ faktycznie dostarczonych w¹tków
		const size_type get(const size_type groupSize, Threads & threads, const bool exact)
		{
			std::lock_guard<std::mutex> lock(mutex);

			if (exact == true && groupSize + threadsCount_ >= maxThreads_){
				throw std::runtime_error("ThreadPool limited resources");
			}

			const auto toDeliver = std::min(maxThreads_ - threadsCount_, groupSize);

			if (toDeliver > 0){

				size_type size = 0;
				while (innerThreads.empty() == false && size < toDeliver)
				{
					threads.push_back(Thread(this, std::move(innerThreads.front())));
					innerThreads.pop_front();
					++size;
				}

				while (size < toDeliver)
				{
					threads.push_back(Thread(this));
					++size;
				}

				threadsCount_ += toDeliver;
			}

			return toDeliver;
		}

	private:

		//! Metoda zmniejsza aktualn¹ iloœc w¹tków w puli
		void detach() { --threadsCount_; }

		//! \param innerThread Wewnêtrzny w¹tek wielokrotnego uruchamiania, który próbujemy zwróciæ przy niszczeniu dostarczonego w¹tku
		void tryReturn(InnerThreadType & innerThread)
		{
			std::lock_guard<std::mutex> lock(mutex);
			detach();
			if (threadsCount_ < minThreads_){
				innerThreads.push_back(std::move(innerThread));
			}
		}

	private:
		//! Aktualna iloœæ w¹tków w puli
		std::atomic<size_type> threadsCount_;
		//! Minimalna iloœc w¹tków w puli jak¹ chcemy utrzymywaæ
		size_type minThreads_;
		//! Maksymalna iloœæ w¹tków jakie pozwala stworzyæ pula
		size_type maxThreads_;
		//! W¹tki wielokrotnego uruchamiania czekaj¹ce na ponowne wyko¿ystanie
		InnerThreadsList innerThreads;
		//! Obiekt synchronizuj¹cy pobieraj¹cych
		std::mutex mutex;
	};
}

#endif	// __HEADER_GUARD_THREADINGUTILS__INTERRUPTIBLETHREADPOOL_H__
