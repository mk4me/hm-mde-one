/********************************************************************
	created:  2014/10/26	16:22:44
	filename: ThreadPool.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_CORELIB__THREADPOOL_H__
#define __HEADER_GUARD_CORELIB__THREADPOOL_H__

#include <thread>
#include <iostream>
#include <utils/Macros.h>
#include <threadingUtils/RunnableThread.h>
#include <threadingUtils/InterruptiblePolicy.h>
#include <threadingUtils/InterruptibleThreadPool.h>
#include <utils/SmartPtr.h>
#include <boost/thread/tss.hpp>
#include <corelib/Export.h>
#include <corelib/ILog.h>

namespace core
{
	class CORELIB_EXPORT ThreadPool
	{
	public:

		class Thread;

	private:

		struct ErrorLogCallPolicy
		{
			//! \tparam F Typ metody jak� wo�amy
			//! \tparam Args Typy argument�w z jakimi wo�amy metod�
			//! \param f Wo�ana metoda
			//! \param arguments Argumenty metody
			template<class F, class... Args>
			static void call(F&& f, Args&&... arguments)
			{
				try{
					f(arguments...);
				}
				catch (std::exception & e){
					ThreadPool::logError(UTILS_FORMAT_STRING(" failed while calling work with error: " << e.what()));
				}
				catch (...){
					ThreadPool::logError(" failed with UNKNOWN error.");
				}
			}			
		};

		//! Polityka braku obs�ugi przerwania - ponownie rzuca ten sam wyj�tek
		struct LogResetableInterruptHandlingPolicy
		{
			template<typename InterruptPolicy>
			static void handle(const threadingUtils::ThreadInterruptedException & e)
			{
				ThreadPool::logError(" canceled.");
				InterruptPolicy::resetInterrupt();
			}			
		};

		//! Przerywalny w�tek z funkcj� runnable
		typedef threadingUtils::RunnableThread<std::thread, ErrorLogCallPolicy> InnerRunnableThread;

	public:

		//! Wewn�trza realizacja puli w�tk�w
		typedef threadingUtils::InterruptibleThreadPool<InnerRunnableThread, ErrorLogCallPolicy, threadingUtils::InterrupltiblePolicy, LogResetableInterruptHandlingPolicy> InnerThreadPool;

		//! 
		typedef InnerThreadPool::size_type size_type;

		//! Klasa w�tku oferowanego z puli
		class CORELIB_EXPORT Thread
		{

			friend class ThreadPool;

		private:

			//! Struktura opisuj�ca w�tek
			struct Description
			{
				//! Kto zarz�dza w�tkiem
				std::string owner;
				//! Co jest realizowane w w�tku
				std::string destination;
			};

			typedef boost::thread_specific_ptr<utils::shared_ptr<Description>> TLS;

			struct CORELIB_EXPORT DescriptionLocalGuard
			{
				DescriptionLocalGuard(utils::shared_ptr<Description> td);

				~DescriptionLocalGuard();
			};

			friend struct DescriptionLocalGuard;

		public:

			typedef InnerThreadPool::Thread::InterruptPolicyType InterruptPolicy;

		private:

			//! \param threadPool Pula w�tk�w do kt�rej nale�a� pierwotnie w�tek
			Thread(InnerThreadPool::Thread && innerThread, const std::string & owner,
				const std::string & destination);

		public:

			//! Domy�lny konstruktor
			Thread();

			//! \param Other W�tek kt�rego zasoby przejmujemy
			Thread(Thread && Other);

			//! Destruktor
			~Thread();

			Thread& operator=(Thread&& Other) NOEXCEPT;
			Thread& operator=(const Thread&) = delete;

			template<typename F, class ...Args>
			void run(F&& f, Args&& ...arguments)
			{				
				std::function<void()> ff = std::bind(utils::decay_copy(std::forward<F>(f)),
					utils::decay_copy(std::forward<Args>(arguments))...);

				this->innerThread.run([ff](utils::shared_ptr<Description> description)
				{
					DescriptionLocalGuard dlg(description);
					ff();
				}, description);
			}

			void swap(Thread& Other) NOEXCEPT ;
			const bool joinable() const NOEXCEPT ;
			void join();
			void detach();
			std::thread::id get_id() const NOEXCEPT ;
			std::thread::native_handle_type native_handle();
			void interrupt();
			const bool interruptible() const;
			static void interruptionPoint();
			static void resetInterruption();
			const std::string owner() const;
			const std::string destination() const;

			static const std::string currentOwner();
			static const std::string currentDestination();

		private:
			//! Wewn�trzny w�tek wielokrotnego uruchamiania
			InnerThreadPool::Thread innerThread;
			//! Opis w�tku
			utils::shared_ptr<Description> description;
			//! Opis w�tku
			static TLS tlsTD;
		};

		typedef std::list<Thread> Threads;

	public:
		//! Domy�lny konstruktor
		ThreadPool(InnerThreadPool * tp);
		//! Desturktor
		~ThreadPool();

		//! \return Maksymalna ilo�c w�tk�w jakie mo�na utworzy�
		const size_type maxThreads() const;
		//! \return Minimalna ilo�� w�tk�w utrzymywana przez manager
		const size_type minThreads() const;
		//! \return Ilo�� aktualnie zaj�tych w�tk�w
		const size_type threadsCount() const;
		//! \return Nowy w�tek		
		Thread get(const std::string & who, const std::string & destination);

		//! \param groupSize Ilo�� w�tk�w w grupie
		//! \param threads [out] Lista z nowymi w�tkami, dopisujemy zawsze na ko�cu
		//! \return Ilo�� faktycznie dostarczonych w�tk�w
		const size_type get(const size_type groupSize, Threads & threads, const bool exact,
			const std::string & who, const std::string & destination = std::string());

		//! \param log Logger kt�rego u�ywamy do logowania
		static void setLog(LogPtr log);

	private:

		static void logError(const std::string & message);

	private:
		//! Wewn�trzny tp
		InnerThreadPool * tp;
		//! Logger
		static LogPtr log_;
	};

	typedef ThreadPool::Thread Thread;
}

#endif	// __HEADER_GUARD_CORELIB__THREADPOOL_H__
