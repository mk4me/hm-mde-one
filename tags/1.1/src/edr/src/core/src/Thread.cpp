#include "Thread.h"

using namespace core;

Thread::Thread(const std::string & who, const std::string & destination,
	utils::IThreadPtr thread) : who_(who), destination_(destination),
	thread_(thread)
{
	if(thread_ == nullptr){
		throw std::invalid_argument("Uninitialized thread");
	}
}

Thread::Thread(const std::string & who, utils::IThreadPtr thread)
	: who_(who), thread_(thread), destination_("Unspecified")
{
	if(thread_ == nullptr){
		throw std::invalid_argument("Uninitialized thread");
	}
}

Thread::~Thread()
{

}

const std::string & Thread::who() const
{
	return who_;
}

const std::string & Thread::destination() const
{
	utils::ScopedLock<utils::StrictSyncPolicy> lock(synch_);
	return destination_;
}

void Thread::setDestination(const std::string & destination)
{
	utils::ScopedLock<utils::StrictSyncPolicy> lock(synch_);
	destination_ = destination;
}

void Thread::start(const utils::IRunnablePtr & runnable, const Priority priority)
{
	utils::ScopedLock<utils::StrictSyncPolicy> lock(synch_);
	thread_->start(runnable, priority);
}

void Thread::cancel()
{
	utils::ScopedLock<utils::StrictSyncPolicy> lock(synch_);
	thread_->cancel();
}

void Thread::join()
{
	thread_->join();
}

const bool Thread::running() const
{
	utils::ScopedLock<utils::StrictSyncPolicy> lock(synch_);
	return thread_->running();
}

const Thread::Priority Thread::priority() const
{
	utils::ScopedLock<utils::StrictSyncPolicy> lock(synch_);
	return thread_->priority();
}

const Thread::size_type Thread::stackSize() const
{
	utils::ScopedLock<utils::StrictSyncPolicy> lock(synch_);
	return thread_->stackSize();	
}

void Thread::setStackSize(const size_type stackSize)
{
	utils::ScopedLock<utils::StrictSyncPolicy> lock(synch_);
	thread_->setStackSize(stackSize);
}

void Thread::setPriority(const Priority priority)
{
	utils::ScopedLock<utils::StrictSyncPolicy> lock(synch_);
	thread_->setPriority(priority);
}

const utils::IRunnableConstPtr Thread::runnable() const
{
	utils::ScopedLock<utils::StrictSyncPolicy> lock(synch_);
	return thread_->runnable();
}

utils::IRunnablePtr Thread::runnable()
{
	utils::ScopedLock<utils::StrictSyncPolicy> lock(synch_);
	return thread_->runnable();
}