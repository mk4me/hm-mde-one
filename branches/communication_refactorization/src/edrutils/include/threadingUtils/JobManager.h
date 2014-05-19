/********************************************************************
	created:  2012/10/31
	created:  31:10:2012   12:55
	filename: JobManager.h
	author:   Mateusz Janiak

	purpose:  Klasa realizuj¹ca manager zadañ, pozwala manipulowac iloœci¹
	w¹tków realizuj¹cych zadania
	*********************************************************************/
#ifndef HEADER_GUARD_THREADINGUTILS__JOBMANAGER_H__
#define HEADER_GUARD_THREADINGUTILS__JOBMANAGER_H__

#include <threadingUtils/Export.h>
#include <threadingUtils/Runnable.h>
#include <threadingUtils/IThread.h>

namespace threadingUtils {
	class THREADINGUTILS_EXPORT IJobManager
	{
	public:
		//! Wirtualny destruktor
		virtual ~IJobManager() {}
		//! \param job Zadanie dodawane do kolejki
		virtual void addJob(IRunnablePtr job) = 0;
	};

	DEFINE_SMART_POINTERS(IJobManager)

	class THREADINGUTILS_EXPORT JobManager : public IJobManager
	{
	private:
		//! Klasa implementuj¹ca funkcjonalnoœæ
		class JobManagerImpl;

	private:
		//! Implementacja
		utils::shared_ptr<JobManagerImpl> impl_;

	public:

		//! \param workerThread W¹tek przetwarzajacy zadania
		JobManager(IThreadPtr workerThread);
		//! Destruktor wirtualny
		virtual ~JobManager();

		//! \param job Zadanie dodawane do kolejki
		virtual void addJob(IRunnablePtr job);
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
	};

	DEFINE_SMART_POINTERS(JobManager)
}

#endif  //  HEADER_GUARD_THREADINGUTILS__JOBMANAGER_H__
