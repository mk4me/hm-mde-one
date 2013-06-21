#include <utils/Thread.h>
#include <QtCore/QThread>
#include <QtCore/QWaitCondition>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <ctime>

using namespace utils;

//! Wewn�trzna implementacja w�tku realizuj�ca ca�� jego obs�uge
class Thread::ThreadImpl
{
private:
	//! Obiekt w�tku realizuj�cy kolejne przetwarzania
	IThreadPtr thread_;
	//! Czy wystartowano
	bool started_;	
	//! Priorytet wątku
	IThread::Priority priority_;
private:

	//! \param priority Priorytet wg interfejsu IThread
	//! \return Priorytet dla w�tku w Qt kt�ry faktyczie realizuje nasz w�tek w systemei operacyjnym
	static const QThread::Priority translateThreadPriority(const IThread::Priority priority)
	{
		QThread::Priority ret = QThread::InheritPriority;

		switch(priority)
		{
		case IThread::Idle:
			ret = QThread::IdlePriority;
			break;

		case IThread::Lowest:
			ret = QThread::LowestPriority;
			break;

		case IThread::Low:
			ret = QThread::LowPriority;
			break;

		case IThread::Normal:
			ret = QThread::NormalPriority;
			break;		

		case IThread::High:
			ret = QThread::HighPriority;
			break;

		case IThread::Highest:
			ret = QThread::HighestPriority;
			break;

		case IThread::TimeCritical:
			ret = QThread::TimeCriticalPriority;
			break;
		}

		return ret;
	}

public:
	//! Publiczny konstruktor
	ThreadImpl(IThreadPtr thread) : thread_(thread), started_(false), priority_(IThread::Inheritate)
	{
		if(thread_ == nullptr){
			throw std::invalid_argument("Uninitialized thread to manage");
		}
	}

	//! Publiczny destruktor
	~ThreadImpl()
	{		
		if(thread_->running()){
			thread_->cancel();
			thread_->join();
		}
	}

	void cancel()
	{
		thread_->cancel();
	}

	void join()
	{
		thread_->join();	
	}

	void start(const RunnablePtr & runnable, const IThread::Priority priority)
	{		
		if(started_ == true){
			throw std::runtime_error("Thread already started once");
		}

		started_ = true;

		if(thread_->running() == true){
			throw std::runtime_error("Thread started outside");
		}

		thread_->start(runnable, priority);
	}

	IThread::Priority priority() const
	{
		return priority_;
	}

	IThread::size_type stackSize() const
	{
		return thread_->stackSize();
	}

	RunnableConstPtr runnable() const
	{
		return thread_->runnable();
	}

	RunnablePtr runnable()
	{
		return thread_->runnable();
	}

	const bool running() const
	{
		return thread_->running();
	}

	void setStackSize(IThread::size_type stackSize)
	{
		thread_->setStackSize(stackSize);
	}

	void setPriority(const IThread::Priority priority)
	{
		priority_ = priority;
		if(thread_->running() == true){
			thread_->setPriority(priority_);
		}
	}
};

Thread::Thread(IThreadPtr thread) : impl_(new ThreadImpl(thread))
{
	
}

Thread::~Thread()
{

}

void Thread::cancel()
{
	impl_->cancel();
}

void Thread::join()
{
	impl_->join();
}

void Thread::start(const RunnablePtr & runnable, const Priority priority)
{
	impl_->start(runnable, priority);
}

const bool Thread::running() const
{
	return impl_->running();
}

const IThread::Priority Thread::priority() const
{
	return impl_->priority();
}

const IThread::size_type Thread::stackSize() const
{
	return impl_->stackSize();
}

void Thread::setStackSize(IThread::size_type stackSize)
{
	impl_->setStackSize(stackSize);
}

void Thread::setPriority(IThread::Priority priority)
{
	impl_->setPriority(priority);
}

const RunnableConstPtr Thread::runnable() const
{
	return impl_->runnable();
}

RunnablePtr Thread::runnable()
{
	return impl_->runnable();
}
