#include <utils/ThreadGroup.h>
#include <utils/Thread.h>
#include <QtCore/QMutex>
#include <QtCore/QWaitCondition>
#include <QtCore/QMutexLocker>
#include <map>
#include <list>

using namespace utils;

class ThreadGroup::ThreadGroupImpl : private Observer<IThreadingBase>
{
private:

	//! Struktura opisuj¹ca dane w¹tku
	struct ThreadData
	{
		//! W¹tek
		ThreadPtr thread;
		//! Status w¹tku
		IThreadingBase::Status status;
		//! Obiekty obserwuj¹ce w¹tek
		std::list<Observer<IThreadingBase>*> observers;
	};

private:	
	//! W¹tki
	std::vector<ThreadData> threads;
	//! Obiekt do synchronizacji stanu grupy
	mutable QMutex synch;
	//! Obiekt do synchronizacji stanu grupy podczas anylowania
	mutable QMutex cancelingSynch;
	//! Obiekt synchronizuj¹cy czekaj¹cych - join
	mutable QMutex waitSynch;
	//! Obiekt synchronizuj¹cy czekaj¹cych - join
	mutable QMutex startSynch;
	//! Iloœæ w¹tków melduj¹cych swój stan
	unsigned int waitingCounter;
	//! Poprzedni status grupy
	IThreadingBase::Status status_;
	//! Poprzedni rezultat grupy
	IThreadingBase::Result result_;
	//! Grupa której funkcjonalnoœæ realizujemy
	ThreadGroup * threadGroup;
	//! Info czy anulujemy pozosta³e watki
	volatile bool canceling;
	//! Info czy wystartowano
	volatile bool started_;

private:

	void init()
	{
		status_ = IThreadingBase::Idle;
		result_ = IThreadingBase::FirstProcessing;
		waitingCounter = 0;
		canceling = false;
		started_ = false;
	}

public:
	ThreadGroupImpl(ThreadGroup * threadGroup, ThreadGroup::size_type size) : threadGroup(threadGroup), synch(QMutex::Recursive),
		waitSynch(QMutex::NonRecursive), threads(size)
	{
		init();

		//Inicjujemy w¹tki
		for(unsigned int i = 0; i < size; ++i){
			ThreadData td;
			td.thread = ThreadPtr(new Thread());
			td.status = td.thread->status();
			td.thread->attach(this);
			threads[i] = td;
		}
	}

	ThreadGroupImpl(ThreadGroup * threadGroup, const std::vector<ThreadPtr> & threads) : threadGroup(threadGroup), synch(QMutex::Recursive),
		waitSynch(QMutex::NonRecursive), threads(threads.size())
	{
		init();

		//Inicjujemy w¹tki
		for(auto it = threads.begin(); it != threads.end(); ++it){
			if((*it)->result() != IThreadingBase::FirstProcessing || (*it)->status() == IThreadingBase::Running){
				throw std::runtime_error("Group initialized with already used thread");
			}
		}

		unsigned int idx = 0;
		for(auto it = threads.begin(); it != threads.end(); ++it, ++idx){
			ThreadData td;
			td.thread = *it;
			td.status = td.thread->status();
			td.thread->attach(this);
			this->threads[idx] = td;
		}
	}

	~ThreadGroupImpl()
	{
		{
			QMutexLocker lock(&synch);
			if(canceling == true){
				QMutexLocker lock(&cancelingSynch);
			}else{
				canceling = true;
			}
		}

		for(auto it = threads.begin(); it != threads.end(); ++it){
			(*it).thread->cancel();
		}

		for(auto it = threads.begin(); it != threads.end(); ++it){
			result_ = (*it).thread->result();
			if(result_ == IThreadingBase::Error || result_ == IThreadingBase::Canceled){
				break;
			}
		}

		std::vector<ThreadData>().swap(threads);

		status_ = IThreadingBase::Killed;

		try{
			threadGroup->notify();
		}catch(...){

		}
	}

	void cancel()
	{
		{
			QMutexLocker lock(&synch);
			if(canceling == true || status_ == IThreadingBase::Idle || started_ == false){
				return;
			}

			canceling = true;
		}		

		for(unsigned int i = 0; i < threads.size(); ++i){			
			threads[i].thread->cancel();
		}

		result_ = IThreadingBase::Canceled;
		status_ = IThreadingBase::Idle;
		started_ = false;

		for(unsigned int i = 0; i < threads.size(); ++i){
			for(auto it = threads[i].observers.begin(); it != threads[i].observers.end(); ++it){
				(*it)->update(threads[i].thread.get());
			}
		}

		threadGroup->notify();
		waitSynch.unlock();
	}

	void join()
	{
		bool update = false;
		{
			//jezeli nie mam running to join natychmiast wraca
			QMutexLocker lock(&synch);
			if(started_ == false){
				return;
			}else{
				update = true;
				++waitingCounter;
			}
		}

		//w przeciwnym wypadku wisze na mutexie az do skonczenia zadania
		QMutexLocker lock(&waitSynch);
		if(update == true){
			--waitingCounter;
		}
	}

	const IThreadingBase::Status status() const
	{
		return status_;
	}

	const IThreadingBase::Result result() const
	{
		return result_;
	}

	void start(const ThreadGroup::Runnables & funcs)
	{
		{
			QMutexLocker lock(&synch);

			if(started_ == true){
				throw RunningGroupStartException();
			}

			if(funcs.size() != threads.size()){
				throw std::invalid_argument("Runnables size mismatch group size");
			}

			started_ = true;

			waitSynch.lock();
		}

		//czekam az wszyscy ktozy joineli poprzednie zadanie zostana wzbudzeni
		while(waitingCounter > 0){
			QMutex mutex;
			mutex.lock();

			QWaitCondition waitCondition;
			waitCondition.wait(&mutex, 100);

			mutex.unlock();
		}

		canceling = false;

		startSynch.lock();

		for(unsigned int i = 0; i < threads.size(); ++i){
			threads[i].thread->start(funcs[i].runnable, funcs[i].priority);
		}

		QMutexLocker lock(&startSynch);		
	}

	const ThreadGroup::size_type size() const
	{
		return threads.size();
	}

	ThreadConstPtr thread(ThreadGroup::size_type idx) const
	{
		return threads[idx].thread;
	}

	virtual void update(const IThreadingBase * thread)
	{
		QMutexLocker lock(&synch);

		auto newResult = thread->result();
		auto newStatus = thread->status();
			
		if(canceling == true){
			return;
		}

		auto t = dynamic_cast<const IThread*>(thread);		
			
		if(newStatus == IThreadingBase::Idle && (newResult == IThreadingBase::Error || newResult == IThreadingBase::Canceled) ){
			canceling = true;
			cancelingSynch.lock();
			lock.unlock();

			for(unsigned int i = 0; i < threads.size(); ++i){
				if(threads[i].thread.get() != t){
					threads[i].thread->cancel();
				}
			}
		}

		bool updatePool = false;
		bool finished = true;

		for(unsigned int i = 0; i < threads.size(); ++i){
			if(threads[i].thread->result() != IThreadingBase::Finished){
				finished = false;
				break;
			}
		}

		if(finished == true){
			updatePool = true;
		}		

		bool updateAllThreads = false;
		unsigned int tIDX = 0;

		for(unsigned int i = 0; i < threads.size(); ++i){
			if(threads[i].thread.get() == t){
				tIDX = i;
				threads[i].status = newStatus;
				break;
			}
		}

		if(newStatus == IThreadingBase::Idle){
			for(unsigned int i = 0; i < threads.size(); ++i){
				if(threads[i].status == IThreadingBase::Running){
					newStatus = IThreadingBase::Running;
					break;
				}
			}
		}

		if(newStatus != status_){				
			status_ = newStatus;
			updatePool = true;

			if(status_ == IThreadingBase::Running){
				startSynch.unlock();
			}else if(status_ == IThreadingBase::Idle){
				started_ = false;
				waitSynch.unlock();
			}
		}

		if(newResult != result_){
			result_ = newResult;
			updatePool = true;
		}

		if(canceling == true){
			for(unsigned int i = 0; i < threads.size(); ++i){
				for(auto it = threads[i].observers.begin(); it != threads[i].observers.end(); ++it){
					(*it)->update(threads[i].thread.get());
				}
			}
		}else{
			for(auto it = threads[tIDX].observers.begin(); it != threads[tIDX].observers.end(); ++it){
				(*it)->update(thread);
			}
		}			

		if(updatePool == true || canceling == true){

			threadGroup->notify();
		}

		if(canceling == true){
			cancelingSynch.unlock();
		}
	}

	void attachToThread(IThreadGroup::size_type idx, Observer<IThreadingBase> * observer)
	{
		QMutexLocker lock(&synch);
		threads[idx].observers.push_back(observer);
	}

	void detachFromThread(IThreadGroup::size_type idx, Observer<IThreadingBase> * observer)
	{
		QMutexLocker lock(&synch);
		threads[idx].observers.remove(observer);
	}

	void start(const std::vector<RunnablePtr> & runnables, IThread::Priority priority)
	{
		ThreadGroup::Runnables r(runnables.size());

		for(unsigned int i = 0; i < runnables.size(); ++i){
			r[i].priority = priority;
			r[i].runnable = runnables[i];
		}

		start(r);
	}

	void setStacksSizes(const std::vector<IThread::size_type> & stacks)
	{
		if(stacks.size() != threads.size()){
			throw std::invalid_argument("Stacks size mismatch group size");
		}

		QMutexLocker lock(&synch);
		if(result_ == IThreadingBase::Finished){
			throw std::runtime_error("Can not set threads stacks while threads are running");
		}

		for(unsigned int i = 0; i < threads.size(); ++i){
			threads[i].thread->setStackSize(stacks[i]);
		}
	}
};

ThreadGroup::ThreadGroup(size_type size)
{
	impl.reset(new ThreadGroupImpl(this, size));
}

ThreadGroup::ThreadGroup(const std::vector<ThreadPtr> & threads)
{
	impl.reset(new ThreadGroupImpl(this, threads));
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

const IThreadingBase::Status ThreadGroup::status() const
{
	return impl->status();
}

const IThreadingBase::Result ThreadGroup::result() const
{
	return impl->result();
}

void ThreadGroup::start(const Runnables & funcs)
{
	impl->start(funcs);
}

void ThreadGroup::start(const std::vector<RunnablePtr> & runnables, IThread::Priority priority)
{
	impl->start(runnables, priority);
}

void ThreadGroup::setStacksSizes(const std::vector<IThread::size_type> & stacks)
{
	impl->setStacksSizes(stacks);
}

const IThreadGroup::size_type ThreadGroup::size() const
{
	return impl->size();
}

ThreadConstPtr ThreadGroup::thread(size_type idx) const
{
	return impl->thread(idx);
}

void ThreadGroup::attachToThread(size_type idx, Observer<IThreadingBase> * observer)
{
	impl->attachToThread(idx, observer);
}

void ThreadGroup::detachFromThread(size_type idx, Observer<IThreadingBase> * observer)
{
	impl->detachFromThread(idx, observer);
}