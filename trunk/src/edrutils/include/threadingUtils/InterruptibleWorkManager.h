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
	//! \tparam WorkQueue Typ kolejki zada�
	//! \tparam InterruptibleThread Typ w�tku kt�ry mo�na uruchamia� (run) oraz przerywa� (interrupt)
	//! \tparam CallPolicy Typ polityki wo�ania zada�
	//! Klasa realizuj�ca funkcjonalno�� managera prac, zarz�dzaj�cego zleconymi zadaniami i ich realizacj�
	//! na zarz�dzanych w�tkach
	template<class WorkQueue, class InterruptibleThread, typename WorkExceptionHandlePolicy, typename InterruptHandlePolicy>
	class InterruptibleWorkManager
	{
	private:

		//! RAII pomocne przy wyznaczaniu ilo�ci bezrobotnych, wisz�cych na zmiennej warunkowej
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

		//! Klasa obs�uguj�ca przetwarzanie zleconych zada� w pojedynczym w�tku
		class WorkExecutor
		{
		private:

			//! RAII do obs�ugi inicjalizacji/deinicjalizacji obs�ugi w�tku przez kolejk� zada�
			struct ThreadLocalWorkQueueController
			{
			public:

				//! \param workQueue Kolejka zada� z kt�rej ko�ysta aktualny w�tek
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
				//! Kolejka zada� z kt�rej ko�ysta aktualny w�tek
				WorkQueue & workQueue;
			};

			//! Struktura opisuj�ca wsp�lny stan w�tku i obiektu zarz�dzaj�cego jego prac� przy przetwarzaniu zleconych zada�
			struct SharedState
			{
				SharedState() : workManager(nullptr), forceFinalize(false)
				{

				}

				SharedState(InterruptibleWorkManager * workManager,
					bool forceFinalize) : workManager(workManager),
					forceFinalize(forceFinalize)
				{

				}

				~SharedState()
				{

				}

				//! Manager zada�
				InterruptibleWorkManager * workManager;
				//! Czy w�tek ma zako�czy� przetwarzanie zada�
				volatile bool forceFinalize;
			};

		public:
			//! \param jobManager Manager zada�
			//! \param thread W�tek kt�ry przejmujemy na potrzeby prztwarzania zleconych zada�
			WorkExecutor(InterruptibleWorkManager * workManager, InterruptibleThread && thread)
				: sharedState(utils::make_shared<SharedState>(workManager, false)), thread(std::move(thread))
			{				
				this->thread.run([](utils::shared_ptr<SharedState> sharedState){
					LocalThreadGuard localThreadGuard;
					ThreadGuard<std::atomic<size_type>> guard(sharedState->workManager->activeCounter);
					ThreadLocalWorkQueueController tlwqc(sharedState->workManager->workQueue);
					while (sharedState->forceFinalize == false && sharedState->workManager->forceFinalize_ == false){
						sharedState->workManager->runPendingTask();
					}
				}, sharedState);
			}

			//! \param Other Obiekt kt�rego zasoby przejmujemy
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
				if (sharedState != nullptr){
					sharedState->forceFinalize = true;
				}
			}

			//! Metoda ko�czy dzia�anie przetwarzania zada� przez obs�ugiwany w�tek
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

			//! Metoda czeka na zako��zenie w�tku
			void join()
			{
				thread.join();
			}

			//! Metoda przerywa dzia�anie watku
			void interrupt()
			{
				thread.interrupt();
			}

			const bool interruptible() const
			{
				return thread.interruptible();
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

		private:
			//! Wsp�dzielony stan przez kt�ry kontrolujemy w�tek obs�uguj�cy zlecone zadania
			utils::shared_ptr<SharedState> sharedState;
			//! W�tek obs�uguj�cy zlecone zadania
			InterruptibleThread thread;
		};

		typedef std::map<std::thread::id, WorkExecutor> WorkExecutorsMap;

		typedef typename WorkExecutorsMap::size_type size_type;

		struct WorkSharedState
		{
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

	public:

		//! Konstruktor domy�lny
		InterruptibleWorkManager() : finalize_(false), forceFinalize_(false),
			activeCounter(0) {};
		//! Destruktor
		~InterruptibleWorkManager()
		{
			join(true);
		}

		//! \tparam F Typ funkcji jak� chcemy wykona�
		//! \tparam Args Argumenty funkcji
		//! \param f Funkcja kt�r� chcemy wykona�
		//! \param arguments Argumenty z jakimi chcemy wykona� funkcj�
		//! \return Future pozwalaj�cy czeka� na rezultat zleconego zadania
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

			return ret;
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

		//! \param thread Dodawany w�tek realizuj�cy zadania
		//! \return Identyfikator w�tku
		const std::thread::id addWorkerThread(InterruptibleThread && thread)
		{
			if (finalize_ == true)
			{
				throw std::runtime_error("Operation not permitted");
			}

			WorkExecutor workExecutor(this, std::move(thread));
			const auto ret = workExecutor.get_id();
			auto pair = std::make_pair(ret, std::move(workExecutor) );
			workExecutors.insert(std::move(pair));
			++activeCounter;
			return ret;
		}

		//! \param threadID Identyfikator usuwanego w�teku realizuj�cego zadania
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
			for (auto & we : workExecutors){
				if (we.second.joinable() == true){
					we.second.join();
				}
			}
		}

		//! Metoda sygnalizuje koniec przetwarzania dla wszystkich w�tk�w przetwarzaj�cych
		void forceFinalize()
		{
			for (auto & we : workExecutors){
				we.second.forceFinalize();
			}
		}

		//! Metoda pobieraj�ca i przetwarzaj�ca kolejne zadania z kolejki zada�
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
		//! Mapa identyfikator�w w�tk�w i obiekt� obs�uguj�cych ich przetwarzanie
		WorkExecutorsMap workExecutors;
		//! Ilo�� aktualnie przetwarzaj�cych w�tk�w
		std::atomic<size_type> activeCounter;
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

	template<class WorkQueue, class InterruptibleThread, typename WorkExceptionHandlePolicy, typename InterruptHandlePolicy>
	thread_local bool InterruptibleWorkManager<WorkQueue, InterruptibleThread, WorkExceptionHandlePolicy, InterruptHandlePolicy>::isLocalThread = false;
}

#endif	// __HEADER_GUARD_THREADINGUTILS__INTERRUPTIBLEWORKMANAGER_H__
