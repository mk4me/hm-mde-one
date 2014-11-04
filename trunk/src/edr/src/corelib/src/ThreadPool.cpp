#include "CoreLibPCH.h"
#include <corelib/ThreadPool.h>
#include <corelib/PluginCommon.h>
#include <corelib/IApplication.h>
#include <corelib/ILog.h>

using namespace core;

LogPtr ThreadPool::log_;

ThreadPool::Thread::DescriptionLocalGuard::DescriptionLocalGuard(utils::shared_ptr<ThreadPool::Thread::Description> td)
{
	if (ThreadPool::Thread::tlsTD.get() == nullptr){
		ThreadPool::Thread::tlsTD.reset(new utils::shared_ptr<ThreadPool::Thread::Description>(td));
	}
	else{
		(*ThreadPool::Thread::tlsTD) = td;
	}
}

ThreadPool::Thread::DescriptionLocalGuard::~DescriptionLocalGuard()
{
	auto td = ThreadPool::Thread::tlsTD.get();
	if (td != nullptr){
		auto unpackTD = *td;
		if (unpackTD != nullptr){
			unpackTD->owner = "No explicit owner";
			unpackTD->destination = "No explicit destination";
		}
	}	
}

ThreadPool::Thread::Thread()
{

}

ThreadPool::Thread::Thread(InnerThreadPool::Thread && innerThread, const std::string & owner,
	const std::string & destination) : innerThread(std::move(innerThread))
{
	description = std::make_shared<Description>(Description{ owner, destination });
}

ThreadPool::Thread::Thread(Thread && Other) : innerThread(std::move(Other.innerThread)),
description(std::move(Other.description))
{

}

ThreadPool::Thread::~Thread()
{

}

ThreadPool::Thread& ThreadPool::Thread::operator=(ThreadPool::Thread&& Other) _NOEXCEPT
{
	innerThread = std::move(Other.innerThread);
	description = std::move(Other.description);
	return *this;
}

void ThreadPool::Thread::swap(ThreadPool::Thread& Other) _NOEXCEPT
{
	std::swap(innerThread, Other.innerThread);
	std::swap(description, Other.description);
}

const bool ThreadPool::Thread::joinable() const _NOEXCEPT
{
	return innerThread.joinable();
}

void ThreadPool::Thread::join()
{
	innerThread.join();
}

void ThreadPool::Thread::detach()
{
	innerThread.detach();
}

std::thread::id ThreadPool::Thread::get_id() const _NOEXCEPT
{
	return innerThread.get_id();
}

std::thread::native_handle_type ThreadPool::Thread::native_handle()
{
	return innerThread.native_handle();
}

void ThreadPool::Thread::interrupt()
{
	innerThread.interrupt();
}

const bool ThreadPool::Thread::interruptible() const
{
	return innerThread.interruptible();
}

void ThreadPool::Thread::interruptionPoint()
{
	InnerThreadPool::Thread::interruptionPoint();
}

void ThreadPool::Thread::resetInterruption()
{
	InnerThreadPool::Thread::resetInterruption();
}

const std::string ThreadPool::Thread::owner() const
{
	return description != nullptr ? description->owner : "No explicit owner";
}

const std::string ThreadPool::Thread::destination() const
{
	return description != nullptr ? description->destination : "No explicit destination";
}

const std::string ThreadPool::Thread::currentOwner()
{
	return (*tlsTD)->owner;
}

const std::string ThreadPool::Thread::currentDestination()
{
	return (*tlsTD)->destination;
}

ThreadPool::Thread::TLS ThreadPool::Thread::tlsTD;
		
ThreadPool::ThreadPool(InnerThreadPool * tp) : tp(tp)
{
	if (tp == nullptr){
		throw std::runtime_error("Uninitialized inner thread");
	}
}

const ThreadPool::size_type ThreadPool::maxThreads() const
{
	return tp->maxThreads();
}

const ThreadPool::size_type ThreadPool::minThreads() const
{
	return tp->minThreads();
}

const ThreadPool::size_type ThreadPool::threadsCount() const
{
	return tp->threadsCount();
}

ThreadPool::Thread ThreadPool::get(const std::string & who, const std::string & destination)
{
	return std::move(Thread(std::move(tp->get()), who, destination));
}

const ThreadPool::size_type ThreadPool::get(const size_type groupSize,
	Threads & threads, const bool exact, const std::string & who,
	const std::string & destination)
{

	InnerThreadPool::Threads innerThreads;
	auto ret = tp->get(groupSize, innerThreads, exact);

	auto dest(destination);

	if (dest.empty() == true){
		dest = std::string("No explicit destination");
	}

	for (auto & t : innerThreads)
	{
		threads.push_back(Thread(std::move(t), who, dest));
	}

	return ret;
}

void ThreadPool::setLog(LogPtr log)
{
	if (log_ == nullptr){
		log_ = log;
	}	
}

void ThreadPool::logError(const std::string & message)
{
	LOG_ERROR(log_, "Thread ID " << std::this_thread::get_id() << " owned by " << ThreadPool::Thread::currentOwner() << " for " << ThreadPool::Thread::currentDestination() << " failed while calling work : " << message << " Finalizing thread.");
}