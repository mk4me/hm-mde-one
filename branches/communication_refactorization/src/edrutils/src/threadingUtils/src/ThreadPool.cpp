#include <threadingUtils/ThreadPool.h>
#include <threadingUtils/SynchronizationPolicies.h>
#include <utils/Debug.h>

//TODO
//synchronizacja niszczenia Poola i wątków

using namespace threadingUtils;

class ThreadPool::ThreadPoolImpl
{
private:

	//! Klasa pomocnicza, wrapuje faktyczny wątek, zwraca go pod kontrolę poola
	//! w momencie niszczenia
	class TPThread : public IThread
	{
		friend class ThreadPoolImpl;

	public:

		TPThread(IThreadPtr thread)
			: thread_(thread), threadPool_(nullptr)
		{
			if (thread_ == nullptr){
				throw std::invalid_argument("Uninitialized pool thread");
			}
		}

		//! Wirtualny destruktor - zwraca wątek do puli
		virtual ~TPThread()
		{
			if (threadPool_ != nullptr){
				threadPool_->returnThreadToPool();
			}
		}

		virtual void start(const IRunnablePtr & runnable, const Priority priority)
		{
			thread_->start(runnable, priority);
		}

		//! Metoda zabija w�tek/grupe, nie nadaje si� on/ona ponownie do u�ycia
		virtual void cancel()
		{
			thread_->cancel();
		}

		//! Metoda blokuj�ca a� w�tek/grupa nie zako�czy przetwarzania lub nie zostanie zniszczony/a
		virtual void join()
		{
			thread_->join();
		}

		//! \return Czy wątek wciąż działa
		virtual const bool running() const
		{
			return thread_->running();
		}

		//! \return aktualny priorytet w�tku
		virtual const Priority priority() const
		{
			return thread_->priority();
		}

		//! \return Aktualny rozmiar stosu w�tku
		virtual const size_type stackSize() const
		{
			return thread_->stackSize();
		}

		//! \param stackSize Rozmiar stosu w�tku
		//! Metoda powinna by� wywo�ywana przed pierwszym uruchomieniem w�tku,
		//! potem nie ma mo�liwo�ci zmiany tego parametru. W przypadku braku
		//! wywo�ania tej metody b�dzie u�yty domy�lny stos
		//! W przypadku wywo�ania tej metody podczas dzia�ania watku powinien być rzucany wyj�tek
		virtual void setStackSize(const size_type stackSize)
		{
			thread_->setStackSize(stackSize);
		}

		//! \param priority Priorytet wątku
		virtual void setPriority(const Priority priority)
		{
			thread_->setPriority(priority);
		}

		//! \return Aktualnie wykonywana operacja w w�tku, mo�e by� null jesli nic si� nie dzieje
		virtual const IRunnableConstPtr runnable() const
		{
			return thread_->runnable();
		}

		//! \return Aktualnie wykonywana operacja w w�tku, mo�e by� null jesli nic si� nie dzieje
		virtual IRunnablePtr runnable()
		{
			return thread_->runnable();
		}

	private:
		//! \param threadPool Aktualna thread pool obsługująca ten wątek
		void setThreadPool(ThreadPoolImpl * threadPool)
		{
			threadPool_ = threadPool;
		}

	private:
		ThreadPoolImpl * threadPool_;
		IThreadPtr thread_;
	};

	friend class TPThread;

private:
	//! Fabryka logicznych wątków
	IThreadFactoryPtr threadFactory_;
	//! Maksymalna ilo�� w�tk�w do utworzenia
	ThreadPool::size_type maxThreads_;
	//! Minimalna ilo�� w�tk�w do utworzenia
	ThreadPool::size_type minThreads_;
	//! W�tki do ponownego u�ycia
	IThreadPool::Threads freeThreads_;
	//! Aktualna ilość wątków
	ThreadPool::size_type threadsCount_;
	//! Obiekt synchronizuj�cy stan poola
	StrictSyncPolicy sync_;

public:

	ThreadPoolImpl(IThreadFactoryPtr threadFactory, ThreadPool::size_type minThreads,
		ThreadPool::size_type maxThreads)
		: threadFactory_(threadFactory),
		minThreads_(minThreads), maxThreads_(maxThreads),
		threadsCount_(0)
	{
		if (threadFactory_ == nullptr){
			throw std::invalid_argument("Uninitialized thread factory for thread pool");
		}

		if (maxThreads_ < minThreads_) {
			throw std::invalid_argument("Max threads value less then min threads value");
		}
	}

	~ThreadPoolImpl()
	{
		UTILS_ASSERT(threadsCount_ == 0);
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
		return threadsCount_;
	}

	IThreadPtr getThread()
	{
		ScopedLock<StrictSyncPolicy> lock(sync_);
		//czy osiagnelismy limit? wszystkie watki poola zajete
		if (maxThreads_ <= threadsCount_){
			throw NoFreeThreadAvaiable("All threads in use");
		}

		//probujemy pobrac wolny executor
		IThreadPtr executorThread;

		if (freeThreads_.empty() == false){
			//pobieramy wolny executor
			executorThread = freeThreads_.front();
			freeThreads_.pop_front();
		}
		else{
			//tworzymy nowy executor
			executorThread = threadFactory_->createThread();
		}

		//tworzymy watek
		auto tpt = new TPThread(executorThread);
		tpt->setThreadPool(this);

		//zwiększam sumaryczną ilość wątków
		++threadsCount_;

		return IThreadPtr(tpt);
	}

	void getThreads(const IThreadPool::size_type groupSize,
		IThreadPool::Threads & threads, const bool exact)
	{
		ScopedLock<StrictSyncPolicy> lock(sync_);

		//czy osiagnelismy limit? grupa za duza na mozliwosci poola?
		if (maxThreads_ <= threadsCount_ + groupSize && exact == true){
			throw NoFreeThreadAvaiable("Not enough free threads");
		}

		//ile moge wziac z puli
		unsigned int useFree = std::min(groupSize, freeThreads_.size());
		//ile maksymalnie mogę wziąć
		const unsigned int toTake = std::min(maxThreads_ - threadsCount_, groupSize);

		//pobieram iterator ostatniego mozliwego
		auto last = freeThreads_.begin();
		std::advance(last, useFree);

		//inicjujemy liste executorow
		std::list<IThreadPtr> executorsThreads(freeThreads_.begin(), last);
		// usuwamy z listy dostepnych
		freeThreads_.erase(freeThreads_.begin(), last);

		//Czy mamy juz tyle ile potrzeba?
		while (useFree++ < toTake){
			//musimy utworzyc nowych executorow
			executorsThreads.push_back(threadFactory_->createThread());
		}

		//watki dla uzytkownika
		std::list<IThreadPtr> toInsert;

		//tworzymy watki dla uzytkownika
		for (auto it = executorsThreads.begin(); it != executorsThreads.end(); ++it) {
			//tworzymy watek
			auto tpt = new TPThread(*it);
			tpt->setThreadPool(this);
			toInsert.push_back(IThreadPtr(tpt));
		}

		threadsCount_ += toTake;

		// przepisujemy
		threads.insert(threads.end(), toInsert.begin(), toInsert.end());
	}

	void setMaxThreads(ThreadPool::size_type maxThreads)
	{
		ScopedLock<StrictSyncPolicy> lock(sync_);
		if (maxThreads < minThreads_) {
			throw std::invalid_argument("Max threads value less then min threads value");
		}

		maxThreads_ = maxThreads;
	}

	void setMinThreads(ThreadPool::size_type minThreads)
	{
		ScopedLock<StrictSyncPolicy> lock(sync_);
		if (maxThreads_ < minThreads) {
			throw std::invalid_argument("Min threads value greater then max threads value");
		}

		minThreads_ = minThreads;
	}

	void returnThreadToPool()
	{
		ScopedLock<StrictSyncPolicy> lock(sync_);
		if (freeThreads_.size() < minThreads_){
			freeThreads_.push_back(threadFactory_->createThread());
		}

		--threadsCount_;
	}
};

ThreadPool::ThreadPool(IThreadFactoryPtr threadFactory, size_type minThreads, size_type maxThreads)
: impl_(new ThreadPoolImpl(threadFactory, minThreads, maxThreads))
{
}

ThreadPool::~ThreadPool()
{
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

IThreadPtr ThreadPool::getThread()
{
	return impl_->getThread();
}

void ThreadPool::getThreads(const size_type groupSize, Threads & threads,
	const bool exact)
{
	return impl_->getThreads(groupSize, threads, exact);
}

void ThreadPool::setMaxThreads(ThreadPool::size_type maxThreads)
{
	impl_->setMaxThreads(maxThreads);
}

void ThreadPool::setMinThreads(ThreadPool::size_type minThreads)
{
	impl_->setMinThreads(minThreads);
}