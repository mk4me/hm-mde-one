#include <utils/RerunnableThread.h>
#include <utils/Thread.h>
#include <utils/ReusableThread.h>

using namespace utils;

//! Implementacja
class RerunnableThread::RerunnableThreadImpl
{
private:
	//! Fabryka w¹tków zasilaj¹ca nas w nowe instancje
	IThreadFactoryPtr threadFactory_;
	//! Aktualny w¹tek
	IThreadPtr thread_;
	//! Rozmiar stosu
	IThread::size_type stackSize_;
	//! Czy ustawiano rozmiar stosu
	bool stackSizeSet;
	//! Runnable
	RunnablePtr runnable_;
	//! Priorytet
	IThread::Priority priority_;
	//! Czy anulowano
	bool canceled_;

public:
	RerunnableThreadImpl(IThreadFactoryPtr threadFactory) :
	  threadFactory_(threadFactory), stackSize_(0), priority_(IThread::Inheritate),
		canceled_(false), stackSizeSet(false)
	  {
		  if(threadFactory_ == nullptr){
			  throw std::invalid_argument("Uninitialized thread factory");
		  }
	  }

	  ~RerunnableThreadImpl()
	  {
		 
	  }

	  void start(const RunnablePtr & runnable, const IThread::Priority priority)
	  {		
		if(thread_ != nullptr && thread_->running() == true){
			throw std::runtime_error("Thread already running");
		}

		if(thread_ == nullptr || canceled_ == true){
			thread_.reset(new ReusableThread(threadFactory_->createThread()));
			if(stackSizeSet == true){
				thread_->setStackSize(stackSize_);
			}
			canceled_ = false;
		}

		runnable_ = runnable;	
		thread_->start(runnable, priority);		  
	  }

	  void cancel()
	  {
		  if(thread_ != nullptr){
			  thread_->cancel();
			  canceled_ = true;
		  }
	  }

	  void join()
	  {
		  if(thread_ != nullptr){
			  thread_->join();
		  }
	  }

	  const bool running() const
	  {
		  if(thread_ == nullptr){
			  return false;
		  }

		  return thread_->running();
	  }

	  const IThread::Priority priority() const
	  {
		  return priority_;
	  }

	  const IThread::size_type stackSize() const
	  {
		  return stackSize_;
	  }

	  void setStackSize(const IThread::size_type stackSize)
	  {
		  stackSizeSet = true;
		  stackSize_ = stackSize;
		  if(thread_ != nullptr){
			  thread_->setStackSize(stackSize);
		  }
	  }

	  void setPriority(const IThread::Priority priority)
	  {
		  priority_ = priority;
		  if(thread_ != nullptr){
			  thread_->setPriority(priority_);
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
};


RerunnableThread::RerunnableThread(IThreadFactoryPtr threadFactory)
	: impl_(new RerunnableThreadImpl(threadFactory))
{

}

RerunnableThread::~RerunnableThread()
{

}

void RerunnableThread::start(const RunnablePtr & runnable, const Priority priority)
{
	impl_->start(runnable, priority);
}

void RerunnableThread::cancel()
{
	impl_->cancel();
}

void RerunnableThread::join()
{
	impl_->join();
}

const bool RerunnableThread::running() const
{
	return impl_->running();
}

const IThread::Priority RerunnableThread::priority() const
{
	return impl_->priority();
}

const IThread::size_type RerunnableThread::stackSize() const
{
	return impl_->stackSize();
}

void RerunnableThread::setStackSize(const size_type stackSize)
{
	impl_->setStackSize(stackSize);
}

void RerunnableThread::setPriority(const IThread::Priority priority)
{
	impl_->setPriority(priority);
}

const RunnableConstPtr RerunnableThread::runnable() const
{
	return impl_->runnable();
}

RunnablePtr RerunnableThread::runnable()
{
	return impl_->runnable();
}