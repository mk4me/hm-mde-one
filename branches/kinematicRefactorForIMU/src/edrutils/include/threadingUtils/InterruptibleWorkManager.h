/********************************************************************
	created:  2014/10/24	09:44:12
	filename: InterruptibleWorkManager.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_THREADINGUTILS__INTERRUPTIBLEWORKMANAGER_H__
#define __HEADER_GUARD_THREADINGUTILS__INTERRUPTIBLEWORKMANAGER_H__

#include <type_traits>
#include <map>
#include <utils/Utils.h>
#include <threadingUtils/Macros.h>
#include <threadingUtils/WorkQueuePolicy.h>
#include <threadingUtils/FunctionWrapper.h>
#include <threadingUtils/InterruptibleFuture.h>
#include <threadingUtils/InterruptiblePolicy.h>
#include <threadingUtils/InterruptiblePackagedTask.h>

namespace threadingUtils
{
	//! \tparam WorkQueue Typ kolejki zadañ
	//! \tparam InterruptibleThread Typ w¹tku który mo¿na uruchamiaæ (run) oraz przerywaæ (interrupt)
	//! \tparam CallPolicy Typ polityki wo³ania zadañ
	//! Klasa realizuj¹ca funkcjonalnoœæ managera prac, zarz¹dzaj¹cego zleconymi zadaniami i ich realizacj¹
	//! na zarz¹dzanych w¹tkach
	template<class WorkQueue, class InterruptibleThread, typename WorkExceptionHandlePolicy, typename InterruptHandlePolicy>
	class InterruptibleWorkManager
	{
	private:

		//! RAII pomocne przy wyznaczaniu iloœci bezrobotnych, wisz¹cych na zmiennej warunkowej
		template<typename T>
		struct ThreadGuard
		{
		public:
			ThreadGuard(T & counter) : counter(counter)
			{

			}

			~ThreadGuard()
			{
				--counter;
			}
		private:
			T & counter;
		};

		//! RAII pomocne przy wyznaczaniu iloœci bezrobotnych, wisz¹cych na zmiennej warunkowej
		template<typename T>
		struct ActiveThreadGuard
		{
		public:
			ActiveThreadGuard(T & counter) : counter(counter)
			{
				--counter;
			}

			~ActiveThreadGuard()
			{
				++counter;
			}
		private:
			T & counter;
		};

		//! RAII pomocnicze przy rozró¿nianiu w¹tków workerów od innych zlecaj¹cych zadania
		struct LocalThreadGuard
		{
			LocalThreadGuard()
			{
				InterruptibleWorkManager::isLocalThread = true;
			}

			~LocalThreadGuard()
			{
				InterruptibleWorkManager::isLocalThread = false;
			}
		};

		friend struct LocalThreadGuard;

		//! Klasa obs³uguj¹ca przetwarzanie zleconych zadañ w pojedynczym w¹tku
		class WorkExecutor
		{
		private:

			//! RAII do obs³ugi inicjalizacji/deinicjalizacji obs³ugi w¹tku przez kolejkê zadañ
			struct ThreadLocalWorkQueueController
			{
			public:

				//! \param workQueue Kolejka zadañ z której ko¿ysta aktualny w¹tek
				ThreadLocalWorkQueueController(WorkQueue & workQueue) : workQueue(workQueue)
				{
					workQueue.initializeThread();
				}

				//! Destruktor
				~ThreadLocalWorkQueueController()
				{
					workQueue.deinitializeThread();
				}

			private:
				//! Kolejka zadañ z której ko¿ysta aktualny w¹tek
				WorkQueue & workQueue;
			};

			//! Struktura opisuj¹ca wspólny stan w¹tku i obiektu zarz¹dzaj¹cego jego prac¹ przy przetwarzaniu zleconych zadañ
			struct SharedState
			{
				SharedState()
				{

				}

				~SharedState()
				{

				}

				//! Manager zadañ
				InterruptibleWorkManager * workManager;
				//! Czy w¹tek ma zakoñczyæ przetwarzanie zadañ
				volatile bool forceFinalize;
			};

		public:
			//! \param jobManager Manager zadañ
			//! \param thread W¹tek który przejmujemy na potrzeby prztwarzania zleconych zadañ
			WorkExecutor(InterruptibleWorkManager * workManager, InterruptibleThread && thread)
				: sharedState(utils::make_shared<SharedState>()), thread(std::move(thread))
			{
				sharedState->workManager = workManager;
				sharedState->forceFinalize = false;
				this->thread.run([](utils::shared_ptr<SharedState> sharedState){
					LocalThreadGuard localThreadGuard;
					ThreadGuard<std::atomic<size_type>> guard(sharedState->workManager->activeCounter);
					ThreadLocalWorkQueueController tlwqc(sharedState->workManager->workQueue);
					while (sharedState->forceFinalize == false && sharedState->workManager->forceFinalize_ == false){
						sharedState->workManager->runPendingTask();
					}
				}, sharedState);
			}

			//! \param Other Obiekt którego zasoby przejmujemy
			WorkExecutor(WorkExecutor && Other) : sharedState(std::move(Other.sharedState)), thread(std::move(Other.thread))
			{

			}

			WorkExecutor(const WorkExecutor & Other) = delete;

			//! Destruktor
			~WorkExecutor()
			{
			
			}

			void finalize()
			{
				if (sharedState == nullptr)
				{
					throw std::runtime_error("Operation not permitted");
				}

				sharedState->forceFinalize = true;
			}

			//! Metoda koñczy dzia³anie przetwarzania zadañ przez obs³ugiwany w¹tek
			void forceFinalize()
			{
				finalize();
				
				if (interruptible() == true){
					try{
						interrupt();
					}
					catch (...){

					}
				}
			}

			//! Metoda czeka na zakoñæzenie w¹tku
			void join()
			{
				thread.join();
			}

			//! Metoda przerywa dzia³anie watku
			void interrupt()
			{
				thread.interrupt();
			}

			const bool interruptible() const
			{
				return thread.interruptible();
			}

			//! \return Czy na w¹tek mo¿na czekaæ
			const bool joinable()
			{
				return thread.joinable();
			}

			//! \return Identyfikator w¹tku
			const std::thread::id get_id() const
			{
				return thread.get_id();
			}

		private:
			//! Wspó³dzielony stan przez który kontrolujemy w¹tek obs³uguj¹cy zlecone zadania
			utils::shared_ptr<SharedState> sharedState;
			//! W¹tek obs³uguj¹cy zlecone zadania
			InterruptibleThread thread;
		};

		typedef std::map<std::thread::id, WorkExecutor> WorkExecutorsMap;

		typedef typename WorkExecutorsMap::size_type size_type;

		struct WorkSharedState
		{
			WorkSharedState()
			{

			}

			~WorkSharedState()
			{

			}

			std::promise<void*> interruptPrivateData;
			utils::shared_ptr<bool> interrupt;
		};

	public:

		struct InteruptGuard
		{
			~InteruptGuard()
			{
				InterruptibleThread::resetInterruption();
			}
		};

		template<typename T>
		using FutureType = InterruptibleFuture < T, typename InterruptibleThread::InterruptiblePolicy >;

		/*
		template<typename T>
		struct FutureType = InterruptibleFuture < T, typename InterruptibleThread::InterruptPolicy > ;
		{
			typedef InterruptibleFuture<T, typename InterruptibleThread::InterruptPolicy> type;
		};
		*/

	public:

		//! Konstruktor domyœlny
		InterruptibleWorkManager() : finalize_(false), forceFinalize_(false),
			activeCounter(0) {};
		//! Destruktor
		~InterruptibleWorkManager()
		{
			join(true);
		}

		//! \tparam F Typ funkcji jak¹ chcemy wykonaæ
		//! \tparam Args Argumenty funkcji
		//! \param f Funkcja któr¹ chcemy wykonaæ
		//! \param arguments Argumenty z jakimi chcemy wykonaæ funkcjê
		//! \return Future pozwalaj¹cy czekaæ na rezultat zleconego zadania
		template<typename F, class ...Args>
		FutureType<typename std::result_of<F(Args...)>::type> submit(F&& f, Args&& ...arguments)
		{
			typedef typename std::result_of<F(Args...)>::type result_type;

			std::lock_guard<std::mutex> lock(taskMutex);
			if (isLocalThread == false && finalize_ == true){
				throw std::runtime_error("Operation not permitted");
			}
			
			std::function<result_type()> intf = std::bind(utils::decay_copy(std::forward<F>(f)), utils::decay_copy(std::forward<Args>(arguments))...);

			auto ss = utils::make_shared<WorkSharedState>();
			ss->interrupt = utils::make_shared<bool>(false);
			
			auto innerFuture = submitWork(ss, intf);

			InterruptibleFuture<result_type, typename InterruptibleThread::InterruptiblePolicy> ret(std::move(innerFuture), ss->interruptPrivateData.get_future(), ss->interrupt);

			auto s = workQueue.size();

			if (isLocalThread == false || s > 1){

				if (s > workExecutors.size()){
					taskCV.notify_all();
				}
				else{
					taskCV.notify_one();
					if (s > 0){
						--s;
						while (s-- > 0)	{ taskCV.notify_one(); };
					}
				}
			}

			return std::move(ret);
		}
		
		template<typename Ret>
		std::future<Ret> submitWork(utils::shared_ptr<WorkSharedState> sharedState, std::function<Ret()> intf)
		{
			return workQueue.submit([sharedState, intf]
			{
				if (*(sharedState->interrupt) == true){
					throw ThreadInterruptedException();
				}

				InteruptGuard intGuard;
				InterruptibleTaskGuard<typename InterruptibleThread::InterruptiblePolicy> taskGuard(sharedState->interruptPrivateData);
				try{
					return intf();
				}
				catch (ThreadInterruptedException & e){
					InterruptHandlePolicy::handle(e);
					throw;
				}
				catch (...){
					auto e = std::current_exception();
					WorkExceptionHandlePolicy::handle(e);
					std::rethrow_exception(e);
				}
			});
		}

		//! \param thread Dodawany w¹tek realizuj¹cy zadania
		//! \return Identyfikator w¹tku
		const std::thread::id addWorkerThread(InterruptibleThread && thread)
		{
			if (finalize_ == true)
			{
				throw std::runtime_error("Operation not permitted");
			}

			WorkExecutor workExecutor(this, std::move(thread));
			const auto ret = workExecutor.get_id();
			workExecutors.insert(std::make_pair(ret, std::move(workExecutor)));
			++activeCounter;
			return ret;
		}

		//! \param threadID Identyfikator usuwanego w¹teku realizuj¹cego zadania
		void removeWorkerThread(const std::thread::id threadID)
		{
			auto it = workExecutors.find(threadID);
			if (it != workExecutors.end()){
				it->second.finalize();
				taskCV.notify_all();
				if (it->second.joinable() == true){
					it->second.join();
				}

				workExecutors.erase(it);
			}
		}

		//! \return Iloœæ w¹tków realizuj¹cych zadania
		const unsigned int workerThreadsCount() const
		{
			return workExecutors.size();
		}

		//! \tparam T Typ wyniku future
		//! \tparam FuturePolicy Typ future (shared | future)
		//! \param future Future na którego wykonanie czekamy realizuj¹c inne zadania
		template<typename Future>
		void waitForOtherTask(Future & future)
		{			
			while (future.wait_for(std::chrono::seconds(0)) == std::future_status::timeout) { runPendingTask(); }			
		}

		//! Metoda czeka na zakoñczenie przetwarzanych zadañ
		//! \param force Czy wymyszamy koniec obs³ugi zleconych zadañ
		void join(const bool force = false)
		{
			{
				std::lock_guard<std::mutex> lock(taskMutex);
				finalize_ = true;
				if (force == true){
					forceFinalize();					
				}
			}

			taskCV.notify_all();

			innerJoin();
			WorkExecutorsMap().swap(workExecutors);
		}

	private:

		void innerJoin()
		{
			for (auto & we : workExecutors){
				if (we.second.joinable() == true){
					we.second.join();
				}
			}
		}

		//! Metoda sygnalizuje koniec przetwarzania dla wszystkich w¹tków przetwarzaj¹cych
		void forceFinalize()
		{
			for (auto & we : workExecutors){
				we.second.forceFinalize();
			}
		}

		//! Metoda pobieraj¹ca i przetwarzaj¹ca kolejne zadania z kolejki zadañ
		void runPendingTask()
		{
			Task task;
			if (workQueue.tryGet(task) == true){
				task.functionWrapper();
			}
			else if (isLocalThread == true){
				std::unique_lock<std::mutex> lock(taskMutex);
				ActiveThreadGuard<std::atomic<size_type>> guard(activeCounter);
				if ((activeCounter == 0) && ((forceFinalize_ == true) || ((finalize_ == true) && (workQueue.empty() == true)))){
					forceFinalize_ = true;
					lock.unlock();
					taskCV.notify_all();
				}
				else{
					taskCV.wait(lock);
				}				
			}
		}

	private:
		//! Mapa identyfikatorów w¹tków i obiektó obs³uguj¹cych ich przetwarzanie
		WorkExecutorsMap workExecutors;
		//! Iloœæ aktualnie przetwarzaj¹cych w¹tków
		std::atomic<size_type> activeCounter;
		//! Kolejka obs³uguj¹ca zlecane zadania
		WorkQueue workQueue;
		//! Czy koñczymy dzia³anie managera zadañ - dodawanie nowych nie bêdzie mo¿liwe
		volatile bool finalize_;
		//! Czy koñczymy dzia³anie managera zadañ - dodawanie nowych nie bêdzie mo¿liwe
		volatile bool forceFinalize_;
		//! Zmienna warunkowa do czekania na zadanie
		std::condition_variable taskCV;
		//! Obiekt synchronizuj¹cy
		std::mutex taskMutex;
		//! Info czy w¹tek jest workerem czy nie
		static thread_local bool isLocalThread;
	};

	template<class WorkQueue, class InterruptibleThread, typename WorkExceptionHandlePolicy, typename InterruptHandlePolicy>
	thread_local bool InterruptibleWorkManager<WorkQueue, InterruptibleThread, WorkExceptionHandlePolicy, InterruptHandlePolicy>::isLocalThread = false;
}

#endif	// __HEADER_GUARD_THREADINGUTILS__INTERRUPTIBLEWORKMANAGER_H__
