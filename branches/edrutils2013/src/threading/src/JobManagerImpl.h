/********************************************************************
    created:  2013/06/24
    created:  24:6:2013   11:53
    filename: JobManagerImpl.h
    author:   Mateusz Janiak
    
    purpose:  Implementacja managera zada�
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
		//! Klasa realizuj�ca logik� pobierania job�w, przetwarzania ich,
		// ko�czenia dzia�ania przetwarzania w ramach danego w�tku
		class ThreadWorker : public IRunnable
		{
		public:
			//! \param jobManager Manager job�w
			ThreadWorker(JobManagerImpl * jobManager);
			//! Destruktor
			~ThreadWorker();

			//! Metoda ustawia ��danie zako�czenia przetwarzanie w w�tku po zako�czeniu
			//! aktualnego zadania, NIE zwalnia pauzy dla job managera ani uspania
			//! z powodu braku job�w!!
			void finish();
			//! \return Czy ko�czymy przetwarzanie dla aktualnego w�tku
			const bool finalizing() const;
			//! Metoda anuluje ��danie zako�czenia przetwarzania w danym w�tku aby mo�na by�o je wznowi�
			void reset();
			//! Logika przetwarzania job�w w ramach jednego w�tku
			virtual void run();

		private:
			//! Info czy ko�czymy przetwarzanie w ramach danego w�tku
			volatile bool finish_;
			//! JobManager
			JobManagerImpl * jobManager_;
		};

		friend class ThreadWorker;

		DEFINE_SMART_POINTERS(ThreadWorker)

		//! Wszystkie dane niezb�dne do obs�ugi przetwarzania zada�
		struct WorkerDescription
		{
			//! W�tek przetwarzaj�cy
			IThreadPtr thread;
			//! Obiekt obs�uguj�cy logik� przetwarzania
			ThreadWorkerPtr threadWorker;
		};

	private:
		//! Agregat trzymaj�cy w�tki realizuj�ce zadania
		typedef std::list<WorkerDescription> Threads;	
		//! Typ agreguj�cy zadania
		typedef std::list<IRunnablePtr> Jobs;

	public:
		//! \param workerThread W�tek realizuj�cy zadania
		JobManagerImpl(IThreadPtr workerThread);
		//! Destruktor
		~JobManagerImpl();

		//! \param job Zadanie dodawane do kolejki
		void addJob(IRunnablePtr job);
		//! \param thread Dodawany w�tek realizuj�cy zadania
		void addWorkerThread(IThreadPtr thread);
		//! \param thread Usuwany w�tek realizuj�cy zadania
		void removeWorkerThread(IThreadPtr thread);
		//! \return Ilo�� w�tk�w realizuj�cych zadania
		const unsigned int workerThreadsCount() const;
		//! \param idx Indeks w�tku realizuj�cego zadania
		//! \return W�tek realizuj�cy zadania
		IThreadConstPtr workerThread(const unsigned int idx) const;
		//! \param idx Indeks w�tku realizuj�cego zadania
		//! \return W�tek realizuj�cy zadania
		IThreadPtr workerThread(const unsigned int idx);

	private:
		//! Metoda realizuj�ca nadzorowanie w�tk�w przetwarzj�cych
		void maintainerRunner();
		//! \return Job do przetworzenia lub nullptr je�li nie ma �adnych job�w w kolejce
		IRunnablePtr getJob(ThreadWorker * threadWorker);
		//! Sprawdza koniec przetwarzania dla w�tk�w
		const bool workerFinish() const;

	private:
		//! W�tki do usuni�cia
		Threads workersToRemove_;
		//! W�tki realizujace przetwarzanie zada�
		Threads workerThreads_;
		//! W�tkek nadzoruj�cy prac� pozosta�ych w�tk�w
		IThreadPtr maintainerThread_;
		//! Zadania
		Jobs jobs_;
		//! Synchronizacja stanu managera
		mutable StrictSyncPolicy synch_;
		//! Czy ko�czymy dzia�anie managera job�w
		volatile bool finish_;
		//! Warunek oczekiwania na joby w kolejce
		QWaitCondition jobWait_;
		//! Obiekt synchronizuj�cy kolejk� job�w
		QMutex jobsSynch_;
	};

}

#endif	//	HEADER_GUARD___JOBMANAGERIMPL_H__
