#include <utils/Thread.h>
#include <QtCore/QThread>
#include <QtCore/QWaitCondition>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <ctime>

using namespace utils;

//! Wewnêtrzna implementacja w¹tku realizuj¹ca ca³¹ jego obs³uge
class Thread::ThreadImpl
{
private:
	//! Faktyczny w¹tek systemu operacujnego realizuj¹cy przetwarzanie
	class InnerThread : public QThread
	{
	public:
		//! Typ funktora przekazywanego do w¹tku i wykonywanego potem w w¹tku
		typedef boost::function<void()> Functor;

	private:
		//! Obiekt funktora do wykonania
		Functor func;

	public:
		//! \param func Funktor do wykonania w w¹tku
		InnerThread(const Functor & func) : func(func)
		{

		}

		virtual ~InnerThread()
		{
			try{
				if(isRunning() == true){
					terminate();
				}

				wait();
			}catch(...){

			}
		}

	protected:
		//! Implementacja metody uruchamianej w nowym w¹tku
		virtual void run()
		{
			func();
		}
	};


private:
	//! Obiekt w¹tku realizuj¹cy kolejne przetwarzania
	boost::shared_ptr<InnerThread> thread_;
	//! Czy wystartowano
	volatile bool started_;
	//! Aktualny priorytet w¹tku
	Thread::Priority priority_;
	//! Aktualny rozmiar stosu dla w¹tku - UWAGA!! Mo¿e byæ przyczyn¹ problemów z uruchomieniem w¹tku!!
	Thread::size_type stackSize_;
	//! Status w¹tku
	volatile IThreadingBase::Status status_;
	//! Rezultat ostatniego przetwarzania
	volatile IThreadingBase::Result result_;
	//! Aktualnie wykonywany obiekt w w¹tku
	RunnablePtr runnable_;
	//! Obiekt przechowuj¹cy czas pocz¹tku stanu dla Idle
	clock_t idleStartTime_;
	//! Obiekt synchronizuj¹cy stan w¹tku
	mutable QMutex synch;
	//! Obiekt synchronizuj¹cy stan w¹tku
	QMutex cancelSynch;
	//! Obiekt synchronizuj¹cy pêtlê kolejnych wywo³añ w¹tku z nowymi zadaniami
	mutable QMutex runnableSynch;
	//! Obiekt na którym czekaj¹ obiekty na zakoñczenie aktualnej pracy w¹tku
	mutable QMutex waitSynch;
	//! Obiekt na którym czekaj¹ obiekty na zakoñczenie aktualnej pracy w¹tku
	mutable QMutex startSynch;
	//! Iloœæ wisz¹cych na joinie
	unsigned int waitingCounter;
	//! W¹tek logiczny do notyfikacji
	Thread * logicalThread;

private:
	//! Wewnêtrzna realizacja przetwarzania w¹tku
	//! Ma formê nieskoñczonej pêtli aby maksymalnie wyko¿ystaæ dany w¹tek bez potrzeby jego niszczenia i tworzenia
	void innerRun()
	{
		while(true){
			//czekamy na kolejne zadanie
			runnableSynch.lock();
			//czy zadanie jest puste?
			if(runnable_ != nullptr){
				
				//aktualizujemy status watku
				{
					QMutexLocker lock(&synch);
					status_ = IThreadingBase::Running;
					startSynch.unlock();

					//informujemy o zmianie stanu
					try{
						logicalThread->notify();
					}catch(...){

					}
				}

				//probujemy obsluzyc przetwarzanie
				try{
					runnable_->run();

					//aktualizujemy stan
					{				
						QMutexLocker lock(&synch);
						idleStartTime_ = clock();
						status_ = IThreadingBase::Idle;
						result_ = IThreadingBase::Finished;

						//notyfikujemy
						try{
							logicalThread->notify();
						}catch(...){

						}
					}

				}catch(...){

					//blad - ustawiamy stan
					{
						QMutexLocker lock(&synch);
						idleStartTime_ = clock();
						status_ = IThreadingBase::Idle;
						result_ = IThreadingBase::Error;						

						//notyfikujemy
						try{
							logicalThread->notify();
						}catch(...){

						}
					}
				}
				//zezwalamy na ponowny start
				started_ = false;
				//zwalniamy czekajacych
				waitSynch.unlock();
			}
		}
	}

	//! \param priority Priorytet wg interfejsu IThread
	//! \return Priorytet dla w¹tku w Qt który faktyczie realizuje nasz w¹tek w systemei operacyjnym
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
	ThreadImpl(Thread * thread) : cancelSynch(QMutex::NonRecursive), logicalThread(thread), status_(IThreadingBase::Idle),
		result_(IThreadingBase::FirstProcessing), idleStartTime_(clock()), priority_(IThread::Inheritate),
		stackSize_(0), waitingCounter(0), started_(false)
	{
		runnableSynch.lock();
	}

	//! Publiczny destruktor
	~ThreadImpl()
	{
		QMutexLocker lock(&synch);

		thread_.reset();

		if(status_ == IThreadingBase::Running){
			result_ = IThreadingBase::Canceled;
		}

		status_ = IThreadingBase::Killed;

		try{
			logicalThread->notify();
		}catch(...){

		}
	}

	void cancel()
	{
		QMutexLocker lock(&synch);

		if(started_ == true){
			thread_->terminate();
			status_ = IThreadingBase::Idle;
			result_ = IThreadingBase::Canceled;
			idleStartTime_ = clock();
			try{
				logicalThread->notify();
			}catch(...){

			}
			waitSynch.unlock();
			started_ = false;
			startSynch.unlock();
		}
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

	void start(const RunnablePtr & runnable, const IThread::Priority priority)
	{		
		{		
			QMutexLocker lock(&synch);

			// je¿eli ju¿ dzia³a to wyj¹tek
			if(started_ == true){
				throw RunningStartException();
			}

			started_ = true;

			//zezwol na join
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

		//blokujê  do momentu kiedy nie wystartuje
		startSynch.lock();

		auto iP = translateThreadPriority(priority);

		// czy trzeba tworzyæ nowy w¹tek - pierwszy raz lub po cancel
		if(result_ == IThreadingBase::FirstProcessing || result_ == IThreadingBase::Canceled){
			//czekaj na w¹tek je¿eli istnia³
			if(thread_ != nullptr){
				thread_->wait();
			}
			// utwórz nowy w¹tek
			thread_.reset(new InnerThread(boost::bind(&ThreadImpl::innerRun, this)));
			//inicjuj rozmiar stosu
			thread_->setStackSize(stackSize_);
			//startuj nowy w¹tek z zadanym priorytetem
			thread_->start(iP);
		}else if( priority != IThread::Inheritate){
			//mamy watek wiec zmieniamy priorytet
			thread_->setPriority(iP);
		}

		//ustaw parametry w¹tku
		priority_ = priority;		
		runnable_ = runnable;

		//uruchom przetwarzanie - mamy juz runnable
		runnableSynch.unlock();
		// czekaj na faktyczny start zadania
		QMutexLocker lock(&startSynch);
	}

	const float idleTime() const
	{
		QMutexLocker lock(&synch);
		//jezeli nie mamy running to znaczy ze nie ma idleTime
		return (status_ != IThreadingBase::Running) ? (float)(clock() - idleStartTime_)/(float)CLOCKS_PER_SEC : 0.0;
	}

	IThread::Priority priority() const
	{
		return priority_;
	}

	IThread::size_type stackSize() const
	{
		return stackSize_;
	}

	RunnableConstPtr runnable() const
	{
		return runnable_;
	}

	RunnablePtr runnable()
	{
		return runnable_;
	}

	void setStackSize(IThread::size_type stackSize)
	{
		QMutexLocker lock(&synch);
		if(result_ != IThreadingBase::FirstProcessing){
			throw std::runtime_error("Stack size can not be set while thread is running");
		}

		stackSize_ = stackSize;
	}
};

Thread::Thread()
{
	impl_.reset(new ThreadImpl(this));
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

const IThreadingBase::Status Thread::status() const
{
	return impl_->status();
}

const IThreadingBase::Result Thread::result() const
{
	return impl_->result();
}

void Thread::start(const RunnablePtr & runnable, const Priority priority)
{
	impl_->start(runnable, priority);
}

const float Thread::idleTime() const
{
	return impl_->idleTime();
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

RunnableConstPtr Thread::runnable() const
{
	return impl_->runnable();
}

RunnablePtr Thread::runnable()
{
	return impl_->runnable();
}