#include "JobManagerImpl.h"
#include <threadingUtils/SynchronizationPolicies.h>
#include <QtCore/QMutexLocker>
#include <boost/thread.hpp>

using namespace threadingUtils;

JobManager::JobManagerImpl::ThreadWorker::ThreadWorker(JobManagerImpl * jobManager)
: jobManager_(jobManager), finish_(false)
{
}

JobManager::JobManagerImpl::ThreadWorker::~ThreadWorker()
{
}

void JobManager::JobManagerImpl::ThreadWorker::finish()
{
	finish_ = true;
}

const bool JobManager::JobManagerImpl::ThreadWorker::finalizing() const
{
	return finish_;
}

void JobManager::JobManagerImpl::ThreadWorker::run()
{
	while (finish_ == false && jobManager_->workerFinish() == false)
	{
		IRunnablePtr job = jobManager_->getJob(this);
		if (job != nullptr)
		{
			try{
				job->run();
			}
			catch (...){
			}
		}
	}
}

JobManager::JobManagerImpl::JobManagerImpl(IThreadPtr maintainerThread)
: maintainerThread_(maintainerThread), finish_(false)
{
	if (maintainerThread_ == nullptr){
		throw std::invalid_argument("Uninitialized maintainer`s thread");
	}
}

JobManager::JobManagerImpl::~JobManagerImpl()
{
	//zaznaczam koniec pracy w¹tku nadzorujacego
	finish_ = true;
	//czekam a¿ przestanie dzia³aæ aby nie wzbudza³ pozosta³ych
	maintainerThread_->join();

	//sygnalizujê pozosta³ym koniecznoœæ zakoñczenia pracy
	for (auto it = workerThreads_.begin(); it != workerThreads_.end(); ++it){
		(*it).threadWorker->finish();
	}

	//profilaktycznie budzê wszystkich
	jobWait_.wakeAll();

	//czekam a¿ zakoñcz¹ ¿ywot
	for (auto it = workerThreads_.begin(); it != workerThreads_.end(); ++it){
		(*it).thread->join();
	}
}

void JobManager::JobManagerImpl::addJob(IRunnablePtr job)
{
	//TODO
	//sprawdzac czy jeden job nie scheduluje innych - potenjalnie niebezpieczne, deadlock

	QMutexLocker lock(&jobsSynch_);
	jobs_.push_back(job);
	jobWait_.wakeOne();
}

void JobManager::JobManagerImpl::addWorkerThread(IThreadPtr workerThread)
{
	ScopedLock<StrictSyncPolicy> lock(synch_);

	auto it = workerThreads_.begin();

	while (it != workerThreads_.end()){
		if ((*it).thread == workerThread){
			break;
		}

		++it;
	}

	if (it != workerThreads_.end()){
		throw std::invalid_argument("Thread already registered in job manager as worker");
	}

	if (maintainerThread_ == workerThread){
		throw std::invalid_argument("Thread already registered in job manager as maintainer thread");
	}

	if (workerThread->running() == true || workerThread->runnable() != nullptr){
		throw std::invalid_argument("Thread already used outside job manager");
	}

	WorkerDescription wd;
	wd.thread = workerThread;
	wd.threadWorker.reset(new ThreadWorker(this));

	//startujemy
	wd.thread->start(wd.threadWorker);

	//zapamiêtujemy
	workerThreads_.push_back(wd);
}

void JobManager::JobManagerImpl::removeWorkerThread(IThreadPtr thread)
{
	ScopedLock<StrictSyncPolicy> lock(synch_);

	//auto it = std::find(workerThreads_.begin(), workerThreads_.end(), thread);
	auto it = workerThreads_.begin();
	for ( ; it != workerThreads_.end(); ++it){
		if ((*it).thread == thread){
			break;
		}
	}

	if (it == workerThreads_.end()){
		throw std::invalid_argument("Thread not registered in job manager as worker");
	}

	(*it).threadWorker->finish();

	workersToRemove_.push_back(*it);
	workerThreads_.erase(it);
}

const unsigned int JobManager::JobManagerImpl::workerThreadsCount() const
{
	ScopedLock<StrictSyncPolicy> lock(synch_);
	return workerThreads_.size();
}

IThreadConstPtr JobManager::JobManagerImpl::workerThread(const unsigned int idx) const
{
	ScopedLock<StrictSyncPolicy> lock(synch_);
	if (idx >= workerThreads_.size()){
		throw std::runtime_error("Index beyond workers count");
	}

	auto it = workerThreads_.begin();
	std::advance(it, idx);
	return (*it).thread;
}

IThreadPtr JobManager::JobManagerImpl::workerThread(const unsigned int idx)
{
	ScopedLock<StrictSyncPolicy> lock(synch_);
	if (idx >= workerThreads_.size()){
		throw std::runtime_error("Index beyond workers count");
	}

	auto it = workerThreads_.begin();
	std::advance(it, idx);
	return (*it).thread;
}

const bool JobManager::JobManagerImpl::workerFinish() const
{
	return finish_;
}

IRunnablePtr JobManager::JobManagerImpl::getJob(ThreadWorker * threadWorker)
{
	QMutexLocker lock(&jobsSynch_);

	while ((jobs_.empty() == true) && (finish_ == false) && threadWorker->finalizing() == false)
	{
		jobWait_.wait(&jobsSynch_);
	}

	IRunnablePtr job;

	if (finish_ == false && threadWorker->finalizing() == false)
	{
		job = jobs_.front();
		jobs_.pop_front();
	}

	return job;
}

void JobManager::JobManagerImpl::maintainerRunner()
{
	while (finish_ == false){
		//usuwamy nieu¿ywane w¹tki
		ScopedLock<StrictSyncPolicy> lock(synch_);
		{
			auto it = workersToRemove_.begin();
			while (it != workersToRemove_.end()){
				if ((*it).thread->running() == false){
					it = workersToRemove_.erase(it);
				}
				else{
					++it;
				}
			}
		}

		{
			//to nie powinno mieæ nigdy miejsca, ale kto wie?
			// jak bêdzie krytyczny fail joba to w¹tek mo¿e wylecieæ
			//wznawiamy nasze w¹tki jeœli jakiœ pad³
			for (auto it = workerThreads_.begin(); it != workerThreads_.end(); ++it){
				if ((*it).thread->running() == false){
					//ponowne uruchomienie danego w¹tku
					try{
						(*it).thread->start((*it).threadWorker);
					}
					catch (...){
					}
				}
			}
		}

		//TODO
		//dodac conditional variable na ktorej bedzie sie usypial
		//a w WorkerThread RAII sprawdzajace czy faktycznie konczymy dzialanie czy wylatujemy,
		//jezeli wylatujemy to wakeOne na tej condition variable

		//chwila przerwy
		boost::this_thread::sleep(boost::posix_time::microseconds(100000));
	}
}