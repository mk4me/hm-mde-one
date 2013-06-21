#include <threading/ReusableThread.h>
#include <threading/SynchronizationPolicies.h>
#include <boost/bind.hpp>

using namespace utils;

//! Implementacja
class ReusableThread::ReusableThreadImpl
{
private:
	//! Wątek w ramach którego realizujemy funkcjonalność pomownego użycia
	IThreadPtr thread_;
	//! Operacja do wykonania
	RunnablePtr runnable_;
	//! Synchronizacja zadań
	StrictSyncPolicy sync;
	//! Synchronizacja zadań
	StrictSyncPolicy wait;
	//! Czy wątek działa
	volatile bool running_;
	//! Czy kończymy działanie
	volatile bool finish;
	//! Priorytet
	IThread::Priority priority_;

public:
	ReusableThreadImpl(IThreadPtr thread) : thread_(thread), running_(false),
		priority_(IThread::Inheritate)
	{
		if(thread_ == nullptr){
			throw std::invalid_argument("Uninitialized reusable thread");
		}

		priority_ = thread_->priority();

		sync.lock();
	}

	~ReusableThreadImpl()
	{
		finish = true;
		sync.unlock();
		wait.unlock();

		if(thread_->running()){
			thread_->cancel();
			thread_->join();
		}
	}

	void start(const RunnablePtr & runnable, const IThread::Priority priority)
	{
		if(runnable == nullptr){
			throw std::invalid_argument("Uninitialized runnable");
		}

		if(running_ == true){
			std::runtime_error("Thread already running");
		}

		runnable_ = runnable;
		sync.unlock();
		wait.lock();
		running_ = true;
		if(thread_->running() == false){
			thread_->start(RunnablePtr(new FunctorRunnable(boost::bind(&ReusableThreadImpl::run, this))), priority);
		}
	}

	void join()
	{		
		ScopedLock<StrictSyncPolicy> lock(wait);
	}

	void cancel()
	{		
		thread_->cancel();
		running_ = false;
	}

	const IThread::Priority priority() const
	{
		return priority_;
	}

	const IThread::size_type stackSize() const
	{
		return thread_->stackSize();
	}

	void setStackSize(const IThread::size_type stackSize)
	{
		thread_->setStackSize(stackSize);
	}

	void setPriority(const IThread::Priority priority)
	{
		priority_ = priority;
		if(thread_->running() == true){
			thread_->setPriority(priority);
		}
	}

	const RunnableConstPtr runnable() const
	{
		return runnable_;
	}

	RunnablePtr runnable()
	{
		return runnable_;
	}

	const bool running() const
	{
		return running_;
	}

private:

	void run()
	{
		while(true)
		{
			sync.lock();
			if(finish == true){
				return;
			}

			try{
				runnable_->run();
				running_ = false;
			}catch(...){

			}
		}
	}
};


ReusableThread::ReusableThread(IThreadPtr thread)
	: impl_(new ReusableThreadImpl(thread))
{

}

ReusableThread::~ReusableThread()
{

}

void ReusableThread::start(const RunnablePtr & runnable, const Priority priority)
{
	impl_->start(runnable, priority);
}

void ReusableThread::cancel()
{
	impl_->cancel();
}

void ReusableThread::join()
{
	impl_->join();
}

const bool ReusableThread::running() const
{
	return impl_->running();
}

const IThread::Priority ReusableThread::priority() const
{
	return impl_->priority();
}

const IThread::size_type ReusableThread::stackSize() const
{
	return impl_->stackSize();
}

void ReusableThread::setStackSize(const size_type stackSize)
{
	impl_->setStackSize(stackSize);
}

void ReusableThread::setPriority(const IThread::Priority priority)
{
	impl_->setPriority(priority);
}

const RunnableConstPtr ReusableThread::runnable() const
{
	return impl_->runnable();
}

RunnablePtr ReusableThread::runnable()
{
	return impl_->runnable();
}