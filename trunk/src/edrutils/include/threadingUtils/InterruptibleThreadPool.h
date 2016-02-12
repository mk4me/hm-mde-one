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
	template<class InterruptibleMultipleRunThreadFactory>
	class InterruptibleThreadPool : private InterruptibleMultipleRunThreadFactory
	{
	public:		

		typedef decltype(std::declval<InterruptibleMultipleRunThreadFactory>().create()) InterruptibleMultipleRunThread;

		//! Typ listy w¹tków wielokrotnego uruchamiania
		typedef std::list<InterruptibleMultipleRunThread> InnerThreadsList;

		//! Typ opisuj¹cy iloœæ
		typedef unsigned int size_type;

		//! Klasa w¹tku oferowanego z puli
		class Thread
		{
			//! ZaprzyjaŸnienie z pula w¹tku
			friend class MyThreadPoolType;

		public:
			//! Typ puli w¹tków
			typedef InterruptibleThreadPool<InterruptibleMultipleRunThreadFactory> MyThreadPoolType;
			typedef typename InterruptibleMultipleRunThread::InterruptiblePolicy InterruptiblePolicy;

		private:

			void finalize()
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

		public:

			//! \param threadPool Pula w¹tków do której nale¿a³ pierwotnie w¹tek
			Thread(MyThreadPoolType * threadPool) : threadPool(threadPool)
			{

			}

			//! \param threadPool Pula w¹tków do której nale¿a³ pierwotnie w¹tek
			//! \param thread W¹tek wielokrotnego uruchamiania
			Thread(MyThreadPoolType * threadPool, InterruptibleMultipleRunThread && thread)
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
				finalize();
			}

			Thread& operator=(Thread&& Other)
			{
				finalize();
				threadPool = std::move(Other.threadPool);
				Other.threadPool = nullptr;
				thread = std::move(Other.thread);
				futureWrapper = std::move(Other.futureWrapper);
				return *this;
			}

			Thread& operator=(const Thread&) = delete;

			template<typename F, class ...Args>
			void run(F&& f, Args&& ...arguments)
			{
				futureWrapper = std::move(thread.run(std::forward<F>(f), std::forward<Args>(arguments)...));
			}

			void swap(Thread& Other)
			{
				std::swap(threadPool, Other.threadPool);
				std::swap(thread, Other.thread);
				std::swap(futureWrapper, Other.futureWrapper);
			}

			bool joinable() const { return futureWrapper.valid() && thread.joinable(); }
			void join() { futureWrapper.wait(); }
			void detach() { threadPool->detach(); threadPool = nullptr; futureWrapper.reset(); thread.detach(); }
			std::thread::id get_id() const { return thread.get_id(); }
			std::thread::native_handle_type native_handle() { return thread.native_handle(); }
			void interrupt() { if (futureWrapper.valid() == false || thread.interruptible() == false) { throw std::logic_error("Operation not permitted"); } thread.interrupt(); }
			bool interruptible() const { return futureWrapper.valid() && thread.interruptible(); }
			static void interruptionPoint() { InterruptibleMultipleRunThread::interruptionPoint(); }
			static void resetInterruption() { InterruptibleMultipleRunThread::resetInterruption(); }

		private:
			//! Pula w¹tków do której nale¿y w¹tek
			MyThreadPoolType * threadPool;
			//! Wewnêtrzny w¹tek wielokrotnego uruchamiania
			InterruptibleMultipleRunThread thread;
			//! Wrapper dla future celem okreœlenia czy w¹tek zakoñczy³ przetwarzanie zleconego zadania
			FutureWrapper futureWrapper;
		};

		//! ZaprzyjaŸnienie w¹tku z pula w¹tków
		friend class Thread;

		class CustomThread
		{
			friend class InterruptibleThreadPool;
		private:
			CustomThread(InterruptibleThreadPool * tp, size_type size)
				: tp(tp), size_(size)
			{
				if (tp == nullptr){
					throw std::runtime_error("Invalid custom threads configuration");
				}
			}

		public:

			~CustomThread()
			{
				tp->returnCustom(size_);
			}

			size_type size() const { return size_; }

		private:
			InterruptibleThreadPool * tp;
			const size_type size_;
		};

		friend class CustomThread;

		typedef utils::unique_ptr<CustomThread> CustomThreadProxy;

		//! Typ agreguj¹cy w¹tki które pobieraj¹ klienci
		typedef std::list<Thread> Threads;

	public:

		InterruptibleThreadPool(const InterruptibleThreadPool & Other) = delete;
		InterruptibleThreadPool(InterruptibleThreadPool && Other) = delete;

		//! \param minThreads Minimalna iloœæ w¹tków do utrzymania
		//! \param maxThreads Maksymalna iloœæ w¹tków do utrzymania
		InterruptibleThreadPool(const size_type minThreads, const size_type maxThreads,
			const InterruptibleMultipleRunThreadFactory & imrtf = InterruptibleMultipleRunThreadFactory())
			: InterruptibleMultipleRunThreadFactory(imrtf), minThreads_(minThreads),
			maxThreads_(maxThreads), threadsCount_(0) {}
		
		//! Domyœlny konstruktor
		InterruptibleThreadPool(const InterruptibleMultipleRunThreadFactory & imrtf = InterruptibleMultipleRunThreadFactory())
			: InterruptibleThreadPool((std::thread::hardware_concurrency() > 1 ? std::thread::hardware_concurrency() - 1 : 1), std::max<unsigned int>(std::thread::hardware_concurrency() * 8, 4), imrtf) {}
		
		//! Desturktor
		~InterruptibleThreadPool()
		{
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
		size_type maxThreads() const { return maxThreads_; }
		//! \return Minimalna iloœæ w¹tków utrzymywana przez manager
		size_type minThreads() const { return minThreads_; }
		//! \return Iloœæ aktualnie zajêtych w¹tków
		size_type threadsCount() const { return threadsCount_; }
		//! \return Iloœæ w¹tków w cache
		size_type cachedThreadsCount() const
		{
			std::lock_guard<std::mutex> lock(mutex);
			return innerThreads.size();
		}
		//! \return Nowy w¹tek		
		Thread get()
		{
			std::lock_guard<std::mutex> lock(mutex);
			if (threadsCount_ < maxThreads_){
				Thread th;
				if (innerThreads.empty() == false){
					th = Thread(this, std::move(innerThreads.front()));
					innerThreads.pop_front();
				}
				else{
					th = Thread(this, InterruptibleMultipleRunThreadFactory::create());
				}				
				++threadsCount_;
				return th;
			}
			else{
				throw std::runtime_error("ThreadPool empty");
			}
		}

		//! \param groupSize Iloœæ w¹tków w grupie
		//! \param threads [out] Lista z nowymi w¹tkami, dopisujemy zawsze na koñcu
		//! \return Iloœæ faktycznie dostarczonych w¹tków
		size_type get(const size_type groupSize, Threads & threads, const bool exact)
		{
			std::lock_guard<std::mutex> lock(mutex);

			if (exact == true && ((groupSize + threadsCount_) > maxThreads_)){
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
					threads.push_back(Thread(this, InterruptibleMultipleRunThreadFactory::create()));
					++size;
				}

				threadsCount_ += toDeliver;
			}

			return toDeliver;
		}

		CustomThreadProxy getCustom(const size_type size, bool exact = true)
		{
			if (size <= 0){
				throw std::invalid_argument("Invalid number of custom threads");
			}

			std::lock_guard<std::mutex> lock(mutex);

			if (exact == true && ((size + threadsCount_) > maxThreads_)){
				throw std::runtime_error("ThreadPool limited resources");
			}

			auto s = std::min(maxThreads_ - threadsCount_, size);

			threadsCount_ += s;

			return CustomThreadProxy(new CustomThread(this, s));
		}

	private:

		//! Metoda zmniejsza aktualn¹ iloœc w¹tków w puli
		void detach() { --threadsCount_; }

		//! Metoda zwraca customowe w¹tki tworzone poza thread pool
		void returnCustom(const size_type size)
		{			
			std::lock_guard<std::mutex> lock(mutex);
			threadsCount_ -= size;		
		}

		//! \param innerThread Wewnêtrzny w¹tek wielokrotnego uruchamiania, który próbujemy zwróciæ przy niszczeniu dostarczonego w¹tku
		void tryReturn(InterruptibleMultipleRunThread & innerThread)
		{
			std::lock_guard<std::mutex> lock(mutex);
			detach();
			if (threadsCount_ < minThreads_ && innerThread.joinable() == true){
				innerThreads.push_back(std::move(innerThread));
			}
		}

	private:
		//! Aktualna iloœæ w¹tków w puli
		volatile size_type threadsCount_;
		//! Minimalna iloœc w¹tków w puli jak¹ chcemy utrzymywaæ
		size_type minThreads_;
		//! Maksymalna iloœæ w¹tków jakie pozwala stworzyæ pula
		size_type maxThreads_;
		//! W¹tki wielokrotnego uruchamiania czekaj¹ce na ponowne wyko¿ystanie
		InnerThreadsList innerThreads;
		//! Obiekt synchronizuj¹cy pobieraj¹cych
		mutable std::mutex mutex;
	};
}

#endif	// __HEADER_GUARD_THREADINGUTILS__INTERRUPTIBLETHREADPOOL_H__
