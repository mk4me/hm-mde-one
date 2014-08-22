#include "Thread.h"

using namespace core;

Thread::Thread(const std::string & who, const std::string & destination,
	threadingUtils::IThreadPtr thread) : who_(who), destination_(destination),
	thread_(thread)
{
	if (thread_ == nullptr){
		throw std::invalid_argument("Uninitialized thread");
	}
}

Thread::Thread(const std::string & who, threadingUtils::IThreadPtr thread)
: who_(who), thread_(thread), destination_("Unspecified")
{
	if (thread_ == nullptr){
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
	threadingUtils::ScopedLock<threadingUtils::StrictSyncPolicy> lock(synch_);
	return destination_;
}

void Thread::setDestination(const std::string & destination)
{
	threadingUtils::ScopedLock<threadingUtils::StrictSyncPolicy> lock(synch_);
	destination_ = destination;
}

void Thread::start(const threadingUtils::IRunnablePtr & runnable, const Priority priority)
{
	threadingUtils::ScopedLock<threadingUtils::StrictSyncPolicy> lock(synch_);
	thread_->start(runnable, priority);
}

void Thread::cancel()
{
	threadingUtils::ScopedLock<threadingUtils::StrictSyncPolicy> lock(synch_);
	thread_->cancel();
}

void Thread::join()
{
	thread_->join();
}

const bool Thread::running() const
{
	threadingUtils::ScopedLock<threadingUtils::StrictSyncPolicy> lock(synch_);
	return thread_->running();
}

const Thread::Priority Thread::priority() const
{
	threadingUtils::ScopedLock<threadingUtils::StrictSyncPolicy> lock(synch_);
	return thread_->priority();
}

const Thread::size_type Thread::stackSize() const
{
	threadingUtils::ScopedLock<threadingUtils::StrictSyncPolicy> lock(synch_);
	return thread_->stackSize();
}

void Thread::setStackSize(const size_type stackSize)
{
	threadingUtils::ScopedLock<threadingUtils::StrictSyncPolicy> lock(synch_);
	thread_->setStackSize(stackSize);
}

void Thread::setPriority(const Priority priority)
{
	threadingUtils::ScopedLock<threadingUtils::StrictSyncPolicy> lock(synch_);
	thread_->setPriority(priority);
}

const threadingUtils::IRunnableConstPtr Thread::runnable() const
{
	threadingUtils::ScopedLock<threadingUtils::StrictSyncPolicy> lock(synch_);
	return thread_->runnable();
}

threadingUtils::IRunnablePtr Thread::runnable()
{
	threadingUtils::ScopedLock<threadingUtils::StrictSyncPolicy> lock(synch_);
	return thread_->runnable();
}