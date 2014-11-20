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
	//! \tparam RunnableThread Typ w�tku runnable kt�ry wrapujemy
	//! \tparam CallPolicy Polityka wo�ania zada� w�tk�w
	//! \tparam InterruptPolicy Polityka przerywania dzia�ania w�tku
	//! \tparam InterruptHandlingPolicy Polityka obs�ugi przerwania w�tku
	//! Klasa realizuj�ca funkcjonalno�� puli w�tk�w
	template<class RunnableThread, typename CallPolicy, typename InterruptPolicy, typename InterruptHandlingPolicy>
	class InterruptibleThreadPool
	{
	public:
		//! Typ w�tku wielokrotnego uruchamiania
		typedef InterruptibleMultipleRunThread<RunnableThread, CallPolicy, InterruptHandlingPolicy, InterruptPolicy> InnerThreadType;

		//! Typ listy w�tk�w wielokrotnego uruchamiania
		typedef std::list<InnerThreadType> InnerThreadsList;

		//! Typ opisuj�cy ilo��
		typedef unsigned int size_type;

		//! Klasa w�tku oferowanego z puli
		class Thread
		{
			//! Zaprzyja�nienie z pula w�tku
			friend class MyThreadPoolType;

		public:
			//! Typ puli w�tk�w
			typedef InterruptibleThreadPool<RunnableThread, CallPolicy, InterruptPolicy, InterruptHandlingPolicy> MyThreadPoolType;

			typedef InterruptPolicy InterruptPolicyType;

		public:

			//! \param threadPool Pula w�tk�w do kt�rej nale�a� pierwotnie w�tek
			Thread(MyThreadPoolType * threadPool) : threadPool(threadPool)
			{

			}

			//! \param threadPool Pula w�tk�w do kt�rej nale�a� pierwotnie w�tek
			//! \param thread W�tek wielokrotnego uruchamiania
			Thread(MyThreadPoolType * threadPool, InnerThreadType && thread)
				: threadPool(threadPool), thread(std::move(thread))
			{

			}

		public:

			Thread() : threadPool(nullptr)
			{

			}

			Thread(const Thread & Other) = delete;

			//! \param Other W�tek kt�rego zasoby przejmujemy
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
			//! Pula w�tk�w do kt�rej nale�y w�tek
			MyThreadPoolType * threadPool;
			//! Wewn�trzny w�tek wielokrotnego uruchamiania
			InnerThreadType thread;
			//! Wrapper dla future celem okre�lenia czy w�tek zako�czy� przetwarzanie zleconego zadania
			FutureWrapper futureWrapper;
		};

		//! Zaprzyja�nienie w�tku z pula w�tk�w
		friend class Thread;

		//! Typ agreguj�cy w�tki kt�re pobieraj� klienci
		typedef std::list<Thread> Threads;

	public:

		InterruptibleThreadPool(const InterruptibleThreadPool & Other) = delete;
		InterruptibleThreadPool(InterruptibleThreadPool && Other) = delete;

		//! \param minThreads Minimalna ilo�� w�tk�w do utrzymania
		//! \param maxThreads Maksymalna ilo�� w�tk�w do utrzymania
		InterruptibleThreadPool(const size_type minThreads, const size_type maxThreads) : minThreads_(minThreads), maxThreads_(maxThreads), threadsCount_(0) {}
		//! Domy�lny konstruktor
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

		//! \param maxThreads Maksymalna ilo�� w�tk�w jakie mo�na stworzy�
		void setMaxThreads(size_type maxThreads)
		{
			std::lock_guard<std::mutex> lock(mutex);
			if (maxThreads < minThreads_)
			{
				throw ThreadPoolConfigurationException("Improper max threads value. Must be greater or equal to minimum threads value");
			}

			maxThreads_ = maxThreads;
		}
		//! \param minThreads Minimalna ilo�� w�tk�w jaka b�dzie utrzymywana
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
		//! \return Maksymalna ilo�c w�tk�w jakie mo�na utworzy�
		const size_type maxThreads() const { return maxThreads_; }
		//! \return Minimalna ilo�� w�tk�w utrzymywana przez manager
		const size_type minThreads() const { return minThreads_; }
		//! \return Ilo�� aktualnie zaj�tych w�tk�w
		const size_type threadsCount() const { return threadsCount_; }
		//! \return Nowy w�tek		
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

		//! \param groupSize Ilo�� w�tk�w w grupie
		//! \param threads [out] Lista z nowymi w�tkami, dopisujemy zawsze na ko�cu
		//! \return Ilo�� faktycznie dostarczonych w�tk�w
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

		//! Metoda zmniejsza aktualn� ilo�c w�tk�w w puli
		void detach() { --threadsCount_; }

		//! \param innerThread Wewn�trzny w�tek wielokrotnego uruchamiania, kt�ry pr�bujemy zwr�ci� przy niszczeniu dostarczonego w�tku
		void tryReturn(InnerThreadType & innerThread)
		{
			std::lock_guard<std::mutex> lock(mutex);
			detach();
			if (threadsCount_ < minThreads_){
				innerThreads.push_back(std::move(innerThread));
			}
		}

	private:
		//! Aktualna ilo�� w�tk�w w puli
		std::atomic<size_type> threadsCount_;
		//! Minimalna ilo�c w�tk�w w puli jak� chcemy utrzymywa�
		size_type minThreads_;
		//! Maksymalna ilo�� w�tk�w jakie pozwala stworzy� pula
		size_type maxThreads_;
		//! W�tki wielokrotnego uruchamiania czekaj�ce na ponowne wyko�ystanie
		InnerThreadsList innerThreads;
		//! Obiekt synchronizuj�cy pobieraj�cych
		std::mutex mutex;
	};
}

#endif	// __HEADER_GUARD_THREADINGUTILS__INTERRUPTIBLETHREADPOOL_H__
