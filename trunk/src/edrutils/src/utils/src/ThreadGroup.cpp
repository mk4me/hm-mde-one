#include <utils/ThreadGroup.h>
#include <utils/Thread.h>
#include <QtCore/QMutex>
#include <QtCore/QWaitCondition>
#include <QtCore/QMutexLocker>
#include <map>
#include <list>

using namespace utils;

class ThreadGroup::ThreadGroupImpl
{
private:	
	//! W�tki
	ThreadGroup::Threads threads_;
	//! Polityka kończenia wątków w grupie
	IThreadGroup::StartPolicy startPolicy_;
	//! Ilość zakończonych niepowodzeniem
	unsigned int failedStarts_;


public:
	ThreadGroupImpl() : startPolicy_(IThreadGroup::ALL_OR_NONE), failedStarts_(0)
	{

	}

	~ThreadGroupImpl()
	{
		//anuluje
		for(auto it = threads_.begin(); it != threads_.end(); ++it){
			(*it).thread->cancel();
		}

		//czekam na faktyczne zakonczenie
		for(auto it = threads_.begin(); it != threads_.end(); ++it){
			(*it).thread->join();
		}
	}

	void cancel()
	{
		for(auto it = threads_.begin(); it != threads_.end(); ++it){
			(*it).thread->cancel();
		}
	}

	void join()
	{
		//kopia dla pewności
		IThreadGroup::Threads locThreads(threads_);

		for(auto it = locThreads.begin(); it != locThreads.end(); ++it){
			(*it).thread->join();
		}
	}

	void start(const ThreadGroup::Threads & threads, const IThreadGroup::StartPolicy startPolicy)
	{
		for(auto it = threads_.begin(); it != threads_.end(); ++it){
			if( (*it).thread->running() == true){
				throw std::runtime_error("Group already running");
			}
		}

		if(threads.empty() == true){
			throw std::invalid_argument("No threads in group");
		}

		unsigned int failed = 0;

		for(auto it = threads.begin(); it != threads.end(); ++it){
			
			if((*it).thread == nullptr){
				throw std::invalid_argument("Uninitialized thread in group");
			}

			try{
				(*it).thread->start((*it).runnable, (*it).priority);
			}catch(...){
				if(startPolicy == IThreadGroup::ALL_OR_NONE){
					for(auto IT = threads.begin(); IT != it; ++IT){
						(*it).thread->cancel();
					}

					throw std::runtime_error("FinishPolicy violated");
				}else{
					++failed;
				}
			}
		}

		if(failed == threads.size()){
			throw std::runtime_error("Failed to run group");
		}

		failedStarts_ = failed;
		startPolicy_ = startPolicy;
		threads_ = threads;
	}

	const IThreadGroup::StartPolicy startPolicy() const
	{
		return startPolicy_;
	}

	const bool running() const
	{
		for(auto it = threads_.begin(); it != threads_.end(); ++it){
			if((*it).thread->running() == true){
				return true;
			}
		}

		return false;
	}

	const ThreadGroup::size_type size() const
	{
		return threads_.size();
	}

	IThreadConstPtr thread(ThreadGroup::size_type idx) const
	{
		return threads_[idx].thread;
	}
};

ThreadGroup::ThreadGroup() : impl(new ThreadGroupImpl())
{

}

ThreadGroup::~ThreadGroup()
{

}

void ThreadGroup::cancel()
{
	impl->cancel();
}

void ThreadGroup::join()
{
	impl->join();
}


void ThreadGroup::start(const Threads & threads, const IThreadGroup::StartPolicy startPolicy)
{
	impl->start(threads, startPolicy);
}

const IThreadGroup::StartPolicy ThreadGroup::startPolicy() const
{
	return impl->startPolicy();
}

const bool ThreadGroup::running() const
{
	return impl->running();
}

const IThreadGroup::size_type ThreadGroup::size() const
{
	return impl->size();
}

IThreadConstPtr ThreadGroup::thread(size_type idx) const
{
	return impl->thread(idx);
}
