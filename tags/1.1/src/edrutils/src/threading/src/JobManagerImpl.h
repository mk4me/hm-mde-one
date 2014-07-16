/********************************************************************
    created:  2013/06/24
    created:  24:6:2013   11:53
    filename: JobManagerImpl.h
    author:   Mateusz Janiak
    
    purpose:  Implementacja managera zadañ
*********************************************************************/
#ifndef HEADER_GUARD___JOBMANAGERIMPL_H__
#define HEADER_GUARD___JOBMANAGERIMPL_H__

#include <threading/JobManager.h>
#include <list>
#include <threading/SynchronizationPolicies.h>
#include <utils/SmartPtr.h>
#include <QtCore/QMutex>
#include <QtCore/QWaitCondition>

namespace utils
{

	class JobManager::JobManagerImpl
	{
	private:
		//! Klasa realizuj¹ca logikê pobierania jobów, przetwarzania ich,
		// koñczenia dzia³ania przetwarzania w ramach danego w¹tku
		class ThreadWorker : public IRunnable
		{
		public:
			//! \param jobManager Manager jobów
			ThreadWorker(JobManagerImpl * jobManager);
			//! Destruktor
			~ThreadWorker();

			//! Metoda ustawia ¿¹danie zakoñczenia przetwarzanie w w¹tku po zakoñczeniu
			//! aktualnego zadania, NIE zwalnia pauzy dla job managera ani uspania
			//! z powodu braku jobów!!
			void finish();
			//! \return Czy koñczymy przetwarzanie dla aktualnego w¹tku
			const bool finalizing() const;
			//! Metoda anuluje ¿¹danie zakoñczenia przetwarzania w danym w¹tku aby mo¿na by³o je wznowiæ
			void reset();
			//! Logika przetwarzania jobów w ramach jednego w¹tku
			virtual void run();

		private:
			//! Info czy koñczymy przetwarzanie w ramach danego w¹tku
			volatile bool finish_;
			//! JobManager
			JobManagerImpl * jobManager_;
		};

		friend class ThreadWorker;

		DEFINE_SMART_POINTERS(ThreadWorker)

		//! Wszystkie dane niezbêdne do obs³ugi przetwarzania zadañ
		struct WorkerDescription
		{
			//! W¹tek przetwarzaj¹cy
			IThreadPtr thread;
			//! Obiekt obs³uguj¹cy logikê przetwarzania
			ThreadWorkerPtr threadWorker;
		};

	private:
		//! Agregat trzymaj¹cy w¹tki realizuj¹ce zadania
		typedef std::list<WorkerDescription> Threads;	
		//! Typ agreguj¹cy zadania
		typedef std::list<IRunnablePtr> Jobs;

	public:
		//! \param workerThread W¹tek realizuj¹cy zadania
		JobManagerImpl(IThreadPtr workerThread);
		//! Destruktor
		~JobManagerImpl();

		//! \param job Zadanie dodawane do kolejki
		void addJob(IRunnablePtr job);
		//! \param thread Dodawany w¹tek realizuj¹cy zadania
		void addWorkerThread(IThreadPtr thread);
		//! \param thread Usuwany w¹tek realizuj¹cy zadania
		void removeWorkerThread(IThreadPtr thread);
		//! \return Iloœæ w¹tków realizuj¹cych zadania
		const unsigned int workerThreadsCount() const;
		//! \param idx Indeks w¹tku realizuj¹cego zadania
		//! \return W¹tek realizuj¹cy zadania
		IThreadConstPtr workerThread(const unsigned int idx) const;
		//! \param idx Indeks w¹tku realizuj¹cego zadania
		//! \return W¹tek realizuj¹cy zadania
		IThreadPtr workerThread(const unsigned int idx);

	private:
		//! Metoda realizuj¹ca nadzorowanie w¹tków przetwarzj¹cych
		void maintainerRunner();
		//! \return Job do przetworzenia lub nullptr jeœli nie ma ¿adnych jobów w kolejce
		IRunnablePtr getJob(ThreadWorker * threadWorker);
		//! Sprawdza koniec przetwarzania dla w¹tków
		const bool workerFinish() const;

	private:
		//! W¹tki do usuniêcia
		Threads workersToRemove_;
		//! W¹tki realizujace przetwarzanie zadañ
		Threads workerThreads_;
		//! W¹tkek nadzoruj¹cy pracê pozosta³ych w¹tków
		IThreadPtr maintainerThread_;
		//! Zadania
		Jobs jobs_;
		//! Synchronizacja stanu managera
		mutable StrictSyncPolicy synch_;
		//! Czy koñczymy dzia³anie managera jobów
		volatile bool finish_;
		//! Warunek oczekiwania na joby w kolejce
		QWaitCondition jobWait_;
		//! Obiekt synchronizuj¹cy kolejkê jobów
		QMutex jobsSynch_;
	};

}

#endif	//	HEADER_GUARD___JOBMANAGERIMPL_H__
