/********************************************************************
	created:  2014/10/22	10:14:31
	filename: WorkManager.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_THREADINGUTILS__WORKMANAGER_H__
#define __HEADER_GUARD_THREADINGUTILS__WORKMANAGER_H__

#include <type_traits>
#include <map>
#include <threadingUtils/Future.h>
#include <threadingUtils/Macros.h>
#include <threadingUtils/FunctionWrapper.h>
#include <threadingUtils/LogPolicy.h>

namespace threadingUtils {

	//! \tparam WorkQueue Typ kolejki zada�
	//! \tparam RunnableThread Typ w�tku kt�ry mo�na uruchamia� (run)
	//! \tparam WorkExceptionHandlePolicy
	//! \tparam LogPolicy
	//! Klasa realizuj�ca funkcjonalno�� managera prac, zarz�dzaj�cego zleconymi zadaniami i ich realizacj�
	//! na zarz�dzanych w�tkach
	template<class WorkQueue, class RunnableThread, typename WorkExceptionHandlePolicy = ConsumeExceptionHandlePolicy, typename LogPolicy = EmptyLogPolicy>
	class WorkManager : private WorkExceptionHandlePolicy, private LogPolicy
	{
	private:

		//! RAII pomocnicze przy rozr�nianiu w�tk�w worker�w od innych zlecaj�cych zadania
		struct WorkerThreadGuard
		{
			WorkerThreadGuard()
			{
				WorkManager::isLocalThread = true;
			}

			~WorkerThreadGuard()
			{
				WorkManager::isLocalThread = false;
			}
		};

		friend class WorkerThreadGuard;

		//! Klasa obs�uguj�ca przetwarzanie zleconych zada� w pojedynczym w�tku
		class WorkExecutor
		{
		private:

			//! RAII do obs�ugi inicjalizacji/deinicjalizacji obs�ugi w�tku przez kolejk� zada�
			struct LocalWorkQueueGuard
			{
			public:

				//! \param workQueue Kolejka zada� z kt�rej ko�ysta aktualny w�tek
				LocalWorkQueueGuard(WorkQueue & workQueue) : workQueue(workQueue)
				{
					workQueue.initializeThread();
				}

				//! Destruktor
				~LocalWorkQueueGuard()
				{
					workQueue.deinitializeThread();
				}

			private:
				//! Kolejka zada� z kt�rej ko�ysta aktualny w�tek
				WorkQueue & workQueue;
			};

			//! Struktura opisuj�ca wsp�lny stan w�tku i obiektu zarz�dzaj�cego jego prac� przy przetwarzaniu zleconych zada�
			struct SharedState
			{
				//! Manager zada�
				WorkManager * workManager;
				//! Czy w�tek ma zako�czy� przetwarzanie zada�
				volatile bool forceFinalize;
			};

		public:
			//! \param jobManager Manager zada�
			//! \param thread W�tek kt�ry przejmujemy na potrzeby prztwarzania zleconych zada�
			WorkExecutor(WorkManager * workManager, RunnableThread && thread)
				: sharedState(utils::make_shared<SharedState>(workManager, false), thread(std::move(thread))
			{
				this->thread.run([](utils::shared_ptr<SharedState> sharedState) {
					WorkerThreadGuard localThreadGuard;
					LocalWorkQueueGuard lqqg(sharedState->workManager->workQueue);
					while (sharedState->forceFinalize == false && sharedState->workManager->forceFinalize == false){
						sharedState->workManager->runPendingTask(std::chrono::milliseconds(100), false);
					}
				}, sharedState);
			}

			//! \param Other Obiekt kt�rego zasoby przejmujemy
			WorkExecutor(WorkExecutor && Other) : sharedState(std::move(Other.sharedState)), thread(std::move(Other.thread))
			{

			}

			//! Destruktor
			~WorkExecutor() {}

			//! Metoda ko�czy dzia�anie przetwarzania zada� przez obs�ugiwany w�tek
			void forceFinalize() { if (sharedState != nullptr) sharedState->forceFinalize = true; }

			//! Metoda czeka na zako��zenie w�tku
			void join()
			{
				thread.join();
			}

			//! \return Czy na w�tek mo�na czeka�
			bool joinable()
			{
				return thread.joinable();
			}

			//! \return Identyfikator w�tku
			std::thread::id get_id() const
			{
				return thread.get_id();
			}

			//! \return Identyfikator w�tku
			void detach()
			{
				thread.detach();
			}

		private:
			//! Wsp�dzielony stan przez kt�ry kontrolujemy w�tek obs�uguj�cy zlecone zadania
			utils::shared_ptr<SharedState> sharedState;
			//! W�tek obs�uguj�cy zlecone zadania
			RunnableThread thread;
		};

		friend class WorkExector;

		using WorkExecutorsMap = std::map<std::thread::id, WorkExecutor>;

		using size_type = WorkExecutorsMap::size_type;

	public:

		template<typename T>
		struct FutureType
		{
			using type = std::future<T>;
		};

	public:

		//! Konstruktor domy�lny
		WorkManager(const WorkExceptionHandlePolicy & wehp = WorkExceptionHandlePolicy(), const LogPolicy & logPolicy = LogPolicy())
			: WorkExceptionHandlePolicy(wehp), LogPolicy(logPolicy)
		{

		}
		//! Destruktor
		~WorkManager()
		{
			LogPolicy::log("Work manager destructor: joining");
			join(true);
			LogPolicy::log("Work manager destructor: finished");
		}

		//! \tparam F Typ funkcji jak� chcemy wykona�
		//! \tparam Args Argumenty funkcji
		//! \param f Funkcja kt�r� chcemy wykona�
		//! \param arguments Argumenty z jakimi chcemy wykona� funkcj�
		//! \return Future pozwalaj�cy czeka� na rezultat zleconego zadania
		template<typename F, class ...Args>
		typename FutureType<typename std::result_of<F(Args)>::type>::type submit(F&& f, Args&& arguments...)
		{
			LogPolicy::log("Work manager: submiting work");
			using result_type = typename std::result_of<F(Args)>::type;

			std::lock_guard<std::mutex> lock(taskMutex);
			if (isLocalThread == false && finalize_ == true){
				throw std::runtime_error("Operation not permitted");				
			}

			std::function<result_type()> ff = std::bind(utils::decay_copy(std::forward<F>(f)), utils::decay_copy(std::forward<Args>(arguments))...);
			LogPolicy::log("Work queue: submiting task");
			auto ret = workQueue.submit([=, this]()->result_type {
				this->LogPolicy::log("Work manager task running");
				try{
					return ff();
				}
				catch (...){
					auto e = std::current_exception();
					WorkExceptionHandlePolicy::handle(e);
					std::rethrow_exception(e);
				}
			});

			auto s = workQueue.size();
			LogPolicy::log("Work manager waking work executors");
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
			LogPolicy::log("Work manager submit finished");
			return ret;
		}

		//! \param thread Dodawany w�tek realizuj�cy zadania
		//! \return Identyfikator w�tku
		std::thread::id addWorkerThread(RunnableThread && thread)
		{
			if (finalize_ == true)
			{
				throw std::runtime_error("Operation not permitted");
			}

			WorkExecutor workExecutor(this, std::move(thread));
			const auto ret = workExecutor.get_id();
			workExecutors.insert({ ret, std::move(workExecutor) });
			//++activeCounter;
			return ret;
		}

		//! \param threadID Identyfikator usuwanego w�teku realizuj�cego zadania
		void removeWorkerThread(const std::thread::id threadID)
		{
			auto it = workExecutors.find(threadID);
			if (it != workExecutors.end()){
				it->second.forceFinalize();
				taskCV.notify_all();
				if (it->second.joinable() == true){
					it->second.join();
				}

				workExecutors.erase(it);
			}
		}

		//! \return Ilo�� w�tk�w realizuj�cych zadania
		unsigned int workerThreadsCount() const
		{
			return workExecutors.size();
		}

		//! \tparam T Typ wyniku future
		//! \tparam Rep Typ reprezentacji czasu
		//! \tparam Per Typ okresu czasu		
		template<typename Future, class Rep = long long, class Per = std::milli>
		//! \param future Future na kt�rego wykonanie czekamy realizuj�c inne zadania
		//! \param timeout Czas uspania podczas czekania
		void waitForOtherTask(Future & future, const std::chrono::duration<Rep, Per> & timeout = std::chrono::duration<Rep, Per>(std::chrono::milliseconds(100)))
		{
			while (future.wait_for(std::chrono::seconds(0)) == std::future_status::timeout) { runPendingTask(timeout, true); }
		}

		//! Metoda czeka na zako�czenie przetwarzanych zada�
		//! \param force Czy wymyszamy koniec obs�ugi zleconych zada�
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
			for (auto we : workExecutors){
				if (we.second.joinable() == true){
					we.second.join();
				}
			}
		}

		//! Metoda sygnalizuje koniec przetwarzania dla wszystkich w�tk�w przetwarzaj�cych
		void forceFinalize()
		{
			for (auto we : workExecutors){
				we.second.forceFinalize();
			}
		}

		//! Metoda pobieraj�ca i przetwarzaj�ca kolejne zadania z kolejki zada�
		//! \tparam Rep Typ reprezentacji czasu
		//! \tparam Per Typ okresu czasu	
		template<class Rep = long long, class Per = std::milli>
		//! \param timeout Czas uspania podczas czekania
		//! \param waitForOther Czy oczekujemy na inne zadanie - wtedy nie mozemy sie uspa�
		void runPendingTask(const std::chrono::duration<Rep, Per> & timeout = std::chrono::duration<Rep, Per>(std::chrono::milliseconds(100)),
			const bool waitForOther = false)
		{
			LogPolicy::log("Work manager in runPendingTask");
			Task task;
			if (workQueue.tryGet(task) == true){
				LogPolicy::log("Work manager running task");
				task.functionWrapper();
			}
			else if (isLocalThread == true){
				std::unique_lock<std::mutex> lock(taskMutex);
				if ((forceFinalize_ == true) || ((finalize_ == true) && (workQueue.empty() == true))){
					LogPolicy::log("Work manager finalizing work executor");
					forceFinalize_ = true;
					lock.unlock();
					taskCV.notify_all();
				}
				else if (waitForOther == true){
					LogPolicy::log("Work manager waiting for others");
					std::this_thread::sleep_for(timeout);
				}
				else{
					LogPolicy::log("Work manager waiting on CV");
					taskCV.wait(lock);
				}
			}
			else{
				LogPolicy::log("Work manager external waiting");
				std::this_thread::sleep_for(timeout);
			}
		}

	private:
		//! Mapa identyfikator�w w�tk�w i obiekt� obs�uguj�cych ich przetwarzanie
		WorkExecutorsMap workExecutors;
		//! Ilo�� aktualnie przetwarzaj�cych w�tk�w
		//std::atomic<WorkExecutorsMap::size_type> activeCounter;
		//! Kolejka obs�uguj�ca zlecane zadania
		WorkQueue workQueue;
		//! Czy ko�czymy dzia�anie managera zada� - dodawanie nowych nie b�dzie mo�liwe
		volatile bool finalize_;
		//! Czy ko�czymy dzia�anie managera zada� - dodawanie nowych nie b�dzie mo�liwe
		volatile bool forceFinalize_;
		//! Zmienna warunkowa do czekania na zadanie
		std::condition_variable taskCV;
		//! Obiekt synchronizuj�cy
		std::mutex taskMutex;
		//! Info czy w�tek jest workerem czy nie
		static thread_local bool isLocalThread;
	};

	template<class WorkQueue, class InterruptibleThread, typename WorkExceptionHandlePolicy, typename InterruptHandlePolicy, typename LogPolicy>
	thread_local bool InterruptibleWorkManager<WorkQueue, InterruptibleThread, WorkExceptionHandlePolicy, InterruptHandlePolicy, LogPolicy>::isLocalThread = false;
}

#endif	// __HEADER_GUARD_THREADINGUTILS__WORKMANAGER_H__