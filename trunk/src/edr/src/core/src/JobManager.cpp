#include "JobManager.h"
#include "Job.h"

core::JobManager::JobManager(utils::IThreadPtr managmentThread)
	: jobManager_(new utils::JobManager(managmentThread))
{

}

core::JobManager::~JobManager()
{

}

core::IJobPtr core::JobManager::addJob(const std::string & who, const std::string & name,
	utils::IRunnablePtr runnable)
{
	if(who.empty() == true || name.empty() == true){
		throw std::runtime_error("Job description missing");
	}

	if(runnable == nullptr){
		throw std::invalid_argument("Uninitialized job");
	}

	utils::shared_ptr<core::Job> ret(new core::Job(who, name, runnable));	

	jobManager_->addJob(ret);

	return ret;
}

void core::JobManager::addWorkerThread(utils::IThreadPtr workerThread)
{
	utils::ScopedLock<utils::StrictSyncPolicy> lock(synch_);
	jobManager_->addWorkerThread(workerThread);
}

void core::JobManager::removeWorkerThread(utils::IThreadPtr workerThread)
{
	utils::ScopedLock<utils::StrictSyncPolicy> lock(synch_);
	jobManager_->removeWorkerThread(workerThread);
}