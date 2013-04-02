#include <utils/ThreadPool.h>
#include <utils/Thread.h>
#include <utils/ThreadGroup.h>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <list>
#include <map>

using namespace utils;

class ThreadPool::ThreadPoolImpl
{
private:

	//! Implementacja w¹tku logicznego dla poola
	//! W¹tek ten pozwala wykonywaæ zadania wielokrotnie jedno po drugim
	//! za pomoca metody start
	//! Wewnêtrznie obs³uguje ju¿ faktyczny w¹tek w systemie reprezentowany przez klasê QThread
	class TPThread : public IThread
	{
	private:
		//! Pool z którego pochodzi w¹tek
		ThreadPoolImpl * tp;
		//! W¹tek obs³uguj¹cy zadania, bêdzie wykozystywany w pool wiele razy jeœli nada¿y siê taka okazja
		//! ma to zminimalizowaæ iloœæ tworzenia w¹tków
		ThreadPtr thread;

	public:
		//! \param tp ThreadPool dla któego utworzono w¹tek
		//! \param thread W¹tek faktycznie obs³uguj¹cy zadania
		TPThread(ThreadPoolImpl * tp, const ThreadPtr & thread) : tp(tp), thread(thread)
		{

		}

		virtual ~TPThread()
		{

		}

		virtual void cancel()
		{
			thread->cancel();
		}

		virtual void join()
		{
			thread->join();
		}

		virtual const IThreadingBase::Status status() const
		{
			return thread->status();
		}

		virtual const IThreadingBase::Result result() const
		{
			return thread->result();
		}

		virtual void start(const RunnablePtr & runnable, const Priority priority)
		{
			thread->start(runnable, priority);
		}

		virtual const float idleTime() const
		{
			return thread->idleTime();
		}

		virtual const IThread::Priority priority() const
		{
			return thread->priority();
		}

		virtual void setStackSize(size_type stackSize)
		{
			thread->setStackSize(stackSize);
		}

		virtual const IThread::size_type stackSize() const
		{
			return thread->stackSize();
		}

		virtual RunnableConstPtr runnable() const
		{
			return thread->runnable();
		}

		virtual RunnablePtr runnable()
		{
			return thread->runnable();
		}
	};

private:
	//! Maksymalna iloœæ w¹tków do utworzenia
	ThreadPool::size_type maxThreads_;
	//! Minimalna iloœæ w¹tków do utworzenia
	ThreadPool::size_type minThreads_;
	//! Dzia³aj¹ce w¹tki
	std::map<IThread*, ThreadPtr> logicalThreads_;
	//! W¹tki do ponownego u¿ycia
	std::list<ThreadPtr> freeThreads_;
	//! W¹tki w u¿yciu
	std::list<ThreadPtr> usedThreads_;
	//! Obiekt synchronizuj¹cy stan poola
	QMutex synch;

public:

	ThreadPoolImpl(ThreadPool::size_type minThreads, ThreadPool::size_type maxThreads) : minThreads_(minThreads), maxThreads_(maxThreads), synch(QMutex::NonRecursive)
	{
		if(maxThreads_ < minThreads_) {
			throw std::invalid_argument("Max threads value less then min threads value");
		}
	}

	~ThreadPoolImpl()
	{

	}

	const ThreadPool::size_type maxThreads() const
	{
		return maxThreads_;
	}

	const ThreadPool::size_type minThreads() const
	{
		return minThreads_;
	}

	const ThreadPool::size_type threadsCount() const
	{
		return usedThreads_.size();
	}

	ThreadPtr getOrCreateThread()
	{
		QMutexLocker lock(&synch);
		//czy osiagnelismy limit? wszystkie watki poola zajete
		if(maxThreads_ <= usedThreads_.size()){
			throw NoFreeThreadAvaiable();
		}

		//probujemy pobrac wolny executor
		ThreadPtr executorThread;

		if(freeThreads_.empty() == false){
			//pobieramy wolny executor
			executorThread = freeThreads_.front();
			freeThreads_.pop_front();				
		}else{
			//tworzymy nowy executor
			executorThread.reset(new Thread());
		}

		//zapamietuje watek
		usedThreads_.push_back(executorThread);

		//tworzymy watek
		auto tpt = new TPThread(this, executorThread);
		//zapamietujemy watek i jego executor
		logicalThreads_.insert(std::map<IThread*, ThreadPtr>::value_type(tpt,executorThread));
		return ThreadPtr(tpt);
	}

	void getOrCreateThreadsGroup(const IThreadGroup::size_type groupSize, std::vector<ThreadPtr> & threads)
	{
		QMutexLocker lock(&synch);

		//czy osiagnelismy limit? grupa za duza na mozliwosci poola?
		if(maxThreads_ <= logicalThreads_.size() + groupSize){
			throw NoFreeThreadAvaiable();
		}

		//ile moge wziac z puli
		unsigned int useFree = std::min(groupSize, freeThreads_.size());

		//pobieram iterator ostatniego mozliwego
		auto last = freeThreads_.begin();
		std::advance(last, useFree);

		//inicjujemy liste executorow
		std::list<ThreadPtr> executorsThreads(freeThreads_.begin(), last);
		// usuwamy z listy dostepnych
		freeThreads_.erase(freeThreads_.begin(), last);

		//Czy mamy juz tyle ile potrzeba?
		if(useFree < groupSize){
			//musimy utworzyc nowych executorow
			for(unsigned int i = useFree; i < useFree; ++i){
				executorsThreads.push_back(ThreadPtr(new Thread()));
			}
		}

		//zapamietuje watki
		usedThreads_.insert(usedThreads_.end(), executorsThreads.begin(), executorsThreads.end());
		
		//watki dla uzytkownika
		std::list<ThreadPtr> toInsert;

		//tworzymy watki dla uzytkownika
		for(auto it = executorsThreads.begin(); it != executorsThreads.end(); ++it) {
			//tworzymy watek
			auto tpt = new TPThread(this, *it);
			//zapamietujemy watek i jego executor
			logicalThreads_.insert(std::map<IThread*, ThreadPtr>::value_type(tpt,*it));

			toInsert.push_back(ThreadPtr(tpt));
		}

		// przepisujemy
		threads.insert(threads.end(), toInsert.begin(), toInsert.end());
	}
	
	void setMaxThreads(ThreadPool::size_type maxThreads)
	{
		QMutexLocker lock(&synch);
		if(maxThreads < minThreads_) {
			throw std::invalid_argument("Max threads value less then min threads value");
		}

		maxThreads_ = maxThreads;
	}
	
	void setMinThreads(ThreadPool::size_type minThreads)
	{
		QMutexLocker lock(&synch);
		if(maxThreads_ < minThreads) {
			throw std::invalid_argument("Min threads value greater then max threads value");
		}

		minThreads_ = minThreads;
	}

	void reuseThreads(IThread * thread)
	{
		QMutexLocker lock(&synch);
		auto it = logicalThreads_.find(thread);
		usedThreads_.remove(it->second);

		if(usedThreads_.size() < minThreads_){
			freeThreads_.push_back(it->second);
		}

		logicalThreads_.erase(it);
	}
};

ThreadPool::ThreadPool(size_type minThreads, size_type maxThreads)
{
	impl_.reset(new ThreadPoolImpl(minThreads, maxThreads));
}

const ThreadPool::size_type ThreadPool::maxThreads() const
{
	return impl_->maxThreads();
}

const ThreadPool::size_type ThreadPool::minThreads() const
{
	return impl_->minThreads();
}

const ThreadPool::size_type ThreadPool::threadsCount() const
{
	return impl_->threadsCount();
}

ThreadPtr ThreadPool::getOrCreateThread()
{
	return impl_->getOrCreateThread();
}

void ThreadPool::getOrCreateThreadsGroup(const size_type groupSize, std::vector<ThreadPtr> & threads)
{
	return impl_->getOrCreateThreadsGroup(groupSize, threads);
}

void ThreadPool::setMaxThreads(ThreadPool::size_type maxThreads)
{
	impl_->setMaxThreads(maxThreads);
}

void ThreadPool::setMinThreads(ThreadPool::size_type minThreads)
{
	impl_->setMinThreads(minThreads);
}