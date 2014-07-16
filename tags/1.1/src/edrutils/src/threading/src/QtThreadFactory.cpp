#include <threading/QtThreadFactory.h>
#include <QtCore/QThread>

using namespace utils;

class QtThreadWrapper : public IThread
{
private:

	class LocalThread : public QThread
	{
	public:

		LocalThread()
		{
			setTerminationEnabled(true);
		}

		virtual ~LocalThread()
		{
			if(isRunning() == true){
				terminate();
				wait();
			}
		}

		void setRunnable(IRunnablePtr runnable)
		{
			runnable_ = runnable;
		}

		IRunnablePtr runnable()
		{
			return runnable_;
		}

		IRunnableConstPtr runnable() const
		{
			return runnable_;
		}

	protected:

		virtual void run()
		{
			IRunnablePtr r = runnable_;
			if(r != nullptr){
				try{
					r->run();
				}catch(...){

				}
			}
		}

	private:

		IRunnablePtr runnable_;
	};

public:

	QtThreadWrapper() : priority_(IThread::Inheritate), thread_(new LocalThread)
	{
		
	}

	virtual ~QtThreadWrapper()
	{
		thread_->setRunnable(IRunnablePtr());
	}

	virtual void start(const IRunnablePtr & runnable, const IThread::Priority priority)
	{		
		thread_->setRunnable(runnable);
		thread_->start(translateThreadPriority(priority));
		priority_ = priority;
	}
	
	virtual void cancel()
	{
		thread_->terminate();
	}

	virtual void join()
	{
		thread_->wait();
	}
	
	virtual const bool running() const
	{
		return thread_->isRunning();
	}
	
	virtual const IThread::Priority priority() const
	{
		return priority_;
	}
	
	virtual const size_type stackSize() const
	{
		return thread_->stackSize();
	}
	
	virtual void setStackSize(const size_type stackSize)
	{
		thread_->setStackSize(stackSize);
	}
	
	virtual void setPriority(const IThread::Priority priority)
	{		
		thread_->setPriority(translateThreadPriority(priority));
		priority_ = priority;
	}
	
	virtual const IRunnableConstPtr runnable() const
	{
		return thread_->runnable();
	}
	
	virtual IRunnablePtr runnable()
	{
		return thread_->runnable();
	}

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

private:
	IThread::Priority priority_;
	shared_ptr<LocalThread> thread_;
};

QtThreadFactory::QtThreadFactory()
{

}

QtThreadFactory::~QtThreadFactory()
{

}

IThreadPtr QtThreadFactory::createThread()
{
	return IThreadPtr(new QtThreadWrapper);
}