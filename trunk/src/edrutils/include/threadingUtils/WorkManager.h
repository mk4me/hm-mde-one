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

namespace threadingUtils {

	//! \tparam WorkQueue Typ kolejki zada�
	//! \tparam RunnableThread Typ w�tku kt�ry mo�na uruchamia� (run)
	//! Klasa realizuj�ca funkcjonalno�� managera prac, zarz�dzaj�cego zleconymi zadaniami i ich realizacj�
	//! na zarz�dzanych w�tkach
	template<class WorkQueue, class RunnableThread, typename CallPolicy>
	class WorkManager
	{

	private:

		//! RAII pomocne przy wyznaczaniu ilo�ci bezrobotnych, wisz�cych na zmiennej warunkowej
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
					try{
						WorkExecutor::run(sharedState);
						--(sharedState->workManager->activeCounter);
					}
					catch (...){
						--(sharedState->workManager->activeCounter);
						throw;
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
			void forceFinalize() { if (sharedState == nullptr) { throw std::runtime_error("Operation not permitted"); sharedState->forceFinalize = true; }

			//! Metoda czeka na zako��zenie w�tku
			void join()
			{
				thread.join();
			}

			//! \return Czy na w�tek mo�na czeka�
			const bool joinable()
			{
				return thread.joinable();
			}

			//! \return Identyfikator w�tku
			const std::thread::id get_id() const
			{
				return thread.get_id();
			}

			//! \return Identyfikator w�tku
			void detach()
			{
				thread.detach();
			}

		private:
			//! Metoda przetwarzaj�ca zlecone zadania
			static void run(std::shared_ptr<SharedState> sharedState)
			{
				LocalWorkQueueGuard(sharedState->workManager->workQueue);
				while (sharedState->forceFinalize == false && sharedState->workManager->forceFinalize == false){					
					sharedState->workManager->runPendingTask();
				}				
			}

		private:
			//! Wsp�dzielony stan przez kt�ry kontrolujemy w�tek obs�uguj�cy zlecone zadania
			utils::shared_ptr<SharedState> sharedState;
			//! W�tek obs�uguj�cy zlecone zadania
			RunnableThread thread;
		};

		typedef std::map<std::thread::id, WorkExecutor> WorkExecutorsMap;

		typedef WorkExecutorsMap::size_type size_type;

	public:

		template<typename T>
		struct FutureType
		{
			typedef std::future<T> type;
		};

	public:

		//! Konstruktor domy�lny
		WorkManager() = default;
		//! Destruktor
		~WorkManager()
		{
			join(true);
		}

		//! \tparam F Typ funkcji jak� chcemy wykona�
		//! \tparam Args Argumenty funkcji
		//! \param f Funkcja kt�r� chcemy wykona�
		//! \param arguments Argumenty z jakimi chcemy wykona� funkcj�
		//! \return Future pozwalaj�cy czeka� na rezultat zleconego zadania
		template<typename F, class ...Args>
		typename FutureType<typename std::result_of<F(Args)>::type>::type submit(F&& f, Args&& arguments...)
		{
			std::lock_guard<std::mutex> lock(taskMutex);
			if (isLocalThread == false && finalize_ == true){
				throw std::runtime_error("Operation not permitted");				
			}

			auto ret = workQueue.submit(std::bind(std::_Decay_copy(std::forward<F>(f)), std::_Decay_copy(std::forward<Args>(arguments))...));

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

		//! \param thread Dodawany w�tek realizuj�cy zadania
		//! \return Identyfikator w�tku
		const std::thread::id addWorkerThread(RunnableThread && thread)
		{
			if (finalize_ == true)
			{
				throw std::runtime_error("Operation not permitted");
			}

			WorkExecutor workExecutor(this, std::move(thread));
			const auto ret = workExecutor.get_id();
			workExecutors.insert(WorkExecutorsMap::value_type(ret, std::move(workExecutor)));
			++activeCounter;
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
		const unsigned int workerThreadsCount() const
		{
			return workExecutors.size();
		}

		//! \tparam T Typ wyniku future
		//! \tparam FuturePolicy Typ future (shared | future)
		//! \param future Future na kt�rego wykonanie czekamy realizuj�c inne zadania
		template<typename Future>
		void waitForOtherTask(Future & future)
		{
			while (future.wait_for(std::chrono::seconds(0)) == std::future_status::timeout) { runPendingTask(); }
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
		void runPendingTask()
		{
			Task task;
			if (workQueue.tryGet(task) == true){
				try{
					CallPolicy::call([&]{
						task.functionWrapper();
					});
				}
				catch (...)
				{

				}
			}
			else if (isLocalThread == true){
				std::unique_lock<std::mutex> lock(taskMutex);
				ActiveThreadGuard(activeCounter);
				if ((activeCounter == 0) && ((forceFinalize_ == true) || ((finalize_ == true) && (workQueue.empty() == true))){
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
		//! Mapa identyfikator�w w�tk�w i obiekt� obs�uguj�cych ich przetwarzanie
		WorkExecutorsMap workExecutors;
		//! Ilo�� aktualnie przetwarzaj�cych w�tk�w
		std::atomic<WorkExecutorsMap::size_type> activeCounter;
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

	template<class WorkQueue, class RunnableThread, typename CallPolicy>
	thread_local bool WorkManager<WorkQueue, RunnableThread, CallPolicy>::isLocalThread = false;
}

#endif	// __HEADER_GUARD_THREADINGUTILS__WORKMANAGER_H__