#include "ThreadPool.h"
#include "Thread.h"

core::ThreadPool::ThreadPool(utils::IThreadFactoryPtr threadFactory,
	const size_type minThreads, const size_type maxThreads)
	: threadPool_(new utils::ThreadPool(threadFactory,
		minThreads, maxThreads))
{

}

core::ThreadPool::~ThreadPool()
{

}

const core::ThreadPool::size_type core::ThreadPool::max() const
{
	utils::ScopedLock<utils::StrictSyncPolicy> lock(synch_);
	return threadPool_->maxThreads();
}

const core::ThreadPool::size_type core::ThreadPool::min() const
{
	utils::ScopedLock<utils::StrictSyncPolicy> lock(synch_);
	return threadPool_->minThreads();
}

const core::ThreadPool::size_type core::ThreadPool::free() const
{
	utils::ScopedLock<utils::StrictSyncPolicy> lock(synch_);
	return threadPool_->maxThreads() - threadPool_->threadsCount();
}

const core::ThreadPool::size_type core::ThreadPool::count() const
{
	utils::ScopedLock<utils::StrictSyncPolicy> lock(synch_);
	return threadPool_->threadsCount();
}

void core::ThreadPool::getThreads(const std::string & who,
	Threads & threads, const size_type count, const bool exact)
{
	utils::IThreadPool::Threads ts;
	threadPool_->getThreads(count, ts, exact);

	Threads ret;

	for(auto it = ts.begin(); it != ts.end(); ++it){
		ret.push_back(IThreadPtr(new Thread(who, *it)));
	}

	threads.insert(threads.end(), ret.begin(), ret.end());
}

void core::ThreadPool::setMaxThreads(size_type maxThreads)
{
	utils::ScopedLock<utils::StrictSyncPolicy> lock(synch_);
	threadPool_->setMaxThreads(maxThreads);
}

void core::ThreadPool::setMinThreads(size_type minThreads)
{
	utils::ScopedLock<utils::StrictSyncPolicy> lock(synch_);
	threadPool_->setMinThreads(minThreads);
}