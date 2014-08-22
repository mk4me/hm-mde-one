#include <threadingUtils/JobManager.h>
#include "JobManagerImpl.h"

using namespace threadingUtils;

JobManager::JobManager(IThreadPtr workerThread)
: impl_(new JobManagerImpl(workerThread))
{
}

JobManager::~JobManager()
{
}

void JobManager::addJob(IRunnablePtr job)
{
	impl_->addJob(job);
}

void JobManager::addWorkerThread(IThreadPtr thread)
{
	impl_->addWorkerThread(thread);
}

void JobManager::removeWorkerThread(IThreadPtr thread)
{
	impl_->removeWorkerThread(thread);
}

const unsigned int JobManager::workerThreadsCount() const
{
	return impl_->workerThreadsCount();
}

IThreadConstPtr JobManager::workerThread(const unsigned int idx) const
{
	return impl_->workerThread(idx);
}