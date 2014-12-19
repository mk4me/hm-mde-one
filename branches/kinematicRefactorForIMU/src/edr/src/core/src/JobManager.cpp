#include "JobManager.h"
#include "Job.h"

core::JobManager::JobManager(threadingUtils::IThreadPtr managmentThread)
: jobManager_(new threadingUtils::JobManager(managmentThread))
{
}

core::JobManager::~JobManager()
{
}

core::IJobPtr core::JobManager::addJob(const std::string & who, const std::string & name,
	threadingUtils::IRunnablePtr runnable)
{
	if (who.empty() == true || name.empty() == true){
		throw std::runtime_error("Job description missing");
	}

	if (runnable == nullptr){
		throw std::invalid_argument("Uninitialized job");
	}

	utils::shared_ptr<core::Job> ret(new core::Job(who, name, runnable));	
	jobManager_->addJob(ret);
	ret->lock();

	return ret;
}

const bool core::JobManager::execute(const std::string & who, const std::string & name,
	threadingUtils::FunctorRunnable::Functor f)
{
	threadingUtils::IRunnablePtr runnable(new threadingUtils::FunctorRunnable(f));
	auto job = addJob(who, name, runnable);
	job->wait();
	return job->status() == IJob::JOB_FINISHED;
}

void core::JobManager::addWorkerThread(threadingUtils::IThreadPtr workerThread)
{
	threadingUtils::ScopedLock<threadingUtils::StrictSyncPolicy> lock(synch_);
	jobManager_->addWorkerThread(workerThread);
}

void core::JobManager::removeWorkerThread(threadingUtils::IThreadPtr workerThread)
{
	threadingUtils::ScopedLock<threadingUtils::StrictSyncPolicy> lock(synch_);
	jobManager_->removeWorkerThread(workerThread);
}