#include "ThreadGroupTest.h"
#include <utils/ThreadGroup.h>
#include <QtCore/QMutex>
#include <exception>
#include <algorithm>
#include <vector>
#include <utils/Thread.h>

using namespace utils;

CPPUNIT_TEST_SUITE_REGISTRATION( ThreadGroupTest );

class TestExceptionRunnable : public IRunnable
{
public:
	virtual void run()
	{
		throw std::runtime_error("Runnable exception");
	}
};

class TestRunnable : public IRunnable
{
public:

	TestRunnable(int val) : val(val), m1_(QMutex::NonRecursive), m2_(QMutex::NonRecursive)
	{
		m1_.lock();
	}

	virtual void run()
	{
		m1_.unlock();
		m2_.lock();
		++val;
	}

	const int value() const
	{
		return val;
	}

	QMutex & m1()
	{
		return m1_;
	}

	QMutex & m2()
	{
		return m2_;
	}

private:
	volatile int val;
	QMutex m1_;
	QMutex m2_;
};

class TestThreadGroupObserver : public Observer<IThreadingBase>
{
public:

	struct ThreadGroupState
	{
		IThreadingBase::Status status;
		IThreadingBase::Result result;

		bool operator==(const ThreadGroupState & s2)
		{
			if(result == s2.result && status == s2.status){
				return true;
			}

			return false;
		}
	};

	typedef std::vector<ThreadGroupState> States;

public:

	virtual void update(const IThreadingBase * thread)
	{
		ThreadGroupState state;
		state.result = thread->result();
		state.status = thread->status();
		states_.push_back(state);
	}

	const States & states() const
	{
		return states_;
	}

	States & states()
	{
		return states_;
	}

private:
	States states_;
};

void ThreadGroupTest::testJoin()
{
	ThreadGroup tg(2);

	CPPUNIT_ASSERT(tg.size() == 2);

	std::vector<boost::shared_ptr<TestRunnable>> runnables;

	for(int i = 0; i < 2; ++i){
		runnables.push_back(boost::shared_ptr<TestRunnable>(new TestRunnable(i)));
	}

	std::vector<RunnablePtr> tgRunnables(runnables.begin(), runnables.end());

	tg.start(tgRunnables, IThread::Inheritate);
	tg.join();

	bool equal = true;

	for(int i = 0; i < 2; ++i){
		if(boost::dynamic_pointer_cast<const TestRunnable>(tg.thread(i)->runnable())->value() != i+1){
			equal = false;
			break;
		}
	}

	CPPUNIT_ASSERT(equal);
}

void ThreadGroupTest::testCancel()
{
	ThreadGroup tg(2);

	CPPUNIT_ASSERT(tg.size() == 2);

	std::vector<boost::shared_ptr<TestRunnable>> runnables;

	for(int i = 0; i < 2; ++i){
		runnables.push_back(boost::shared_ptr<TestRunnable>(new TestRunnable(i)));
	}

	std::vector<RunnablePtr> tgRunnables(runnables.begin(), runnables.end());
	runnables[0]->m2().lock();
	tg.start(tgRunnables, IThread::Inheritate);
	runnables[0]->m1().lock();
	tg.cancel();
	tg.join();

	CPPUNIT_ASSERT(runnables[0]->value() == 0);
}

void ThreadGroupTest::testNotify()
{
	TestThreadGroupObserver::States states;
	TestThreadGroupObserver::ThreadGroupState ts;

	TestThreadGroupObserver observer;
	{
		ThreadGroup tg(2);

		tg.attach(&observer);

		CPPUNIT_ASSERT_EQUAL(tg.status(), IThreadingBase::Idle);
		CPPUNIT_ASSERT_EQUAL(tg.result(), IThreadingBase::FirstProcessing);

		std::vector<boost::shared_ptr<TestRunnable>> runnables;

		for(int i = 0; i < 2; ++i){
			runnables.push_back(boost::shared_ptr<TestRunnable>(new TestRunnable(i)));
		}

		std::vector<RunnablePtr> tgRunnables(runnables.begin(), runnables.end());

		tg.start(tgRunnables, IThread::Inheritate);
		ts.result = IThreadingBase::FirstProcessing;
		ts.status = IThreadingBase::Running;
		states.push_back(ts);
		tg.join();
		ts.result = IThreadingBase::Finished;
		ts.status = IThreadingBase::Idle;
		states.push_back(ts);
	}

	ts.result = IThreadingBase::Finished;
	ts.status = IThreadingBase::Killed;
	states.push_back(ts);

	{
		ThreadGroup tg(2);

		tg.attach(&observer);

		std::vector<boost::shared_ptr<TestRunnable>> runnables;

		for(int i = 0; i < 2; ++i){
			runnables.push_back(boost::shared_ptr<TestRunnable>(new TestRunnable(i)));
		}

		std::vector<RunnablePtr> tgRunnables(runnables.begin(), runnables.end());
		runnables[0]->m2().lock();
		tg.start(tgRunnables, IThread::Inheritate);
		ts.result = IThreadingBase::FirstProcessing;
		ts.status = IThreadingBase::Running;
		states.push_back(ts);
		runnables[0]->m1().lock();
		tg.cancel();
		tg.join();		
		ts.result = IThreadingBase::Canceled;
		ts.status = IThreadingBase::Idle;
		states.push_back(ts);
	}

	ts.result = IThreadingBase::Canceled;
	ts.status = IThreadingBase::Killed;
	states.push_back(ts);

	{
		ThreadGroup tg(2);

		tg.attach(&observer);

		std::vector<RunnablePtr> runnables;
		
		runnables.push_back(RunnablePtr(new TestRunnable(0)));
		runnables.push_back(RunnablePtr(new TestExceptionRunnable()));

		tg.start(runnables, IThread::Inheritate);
		ts.result = IThreadingBase::FirstProcessing;
		ts.status = IThreadingBase::Running;
		states.push_back(ts);
		tg.join();		
		ts.result = IThreadingBase::Error;
		ts.status = IThreadingBase::Idle;
		states.push_back(ts);
	}

	ts.result = IThreadingBase::Error;
	ts.status = IThreadingBase::Killed;
	states.push_back(ts);

	CPPUNIT_ASSERT( ((states.size() == observer.states().size()) && (std::equal(states.begin(), states.end(), observer.states().begin()) == true)));
}

void ThreadGroupTest::testExceptionPersistence()
{
	ThreadGroup tg(2);

	{
		std::vector<boost::shared_ptr<TestRunnable>> runnables;

		for(int i = 0; i < 2; ++i){
			runnables.push_back(boost::shared_ptr<TestRunnable>(new TestRunnable(i)));
		}

		std::vector<RunnablePtr> tgRunnables(runnables.begin(), runnables.end());

		tg.start(tgRunnables, IThread::Inheritate);
		tg.join();

		bool equal = true;

		for(int i = 0; i < 2; ++i){
			if(boost::dynamic_pointer_cast<const TestRunnable>(tg.thread(i)->runnable())->value() != i+1){
				equal = false;
				break;
			}
		}

		CPPUNIT_ASSERT(equal);
	}

	{
		std::vector<boost::shared_ptr<TestRunnable>> runnables;

		for(int i = 10; i < 12; ++i){
			runnables.push_back(boost::shared_ptr<TestRunnable>(new TestRunnable(i)));
		}

		std::vector<RunnablePtr> tgRunnables(runnables.begin(), runnables.end());

		tg.start(tgRunnables, IThread::Inheritate);
		tg.join();

		bool equal = true;

		for(int i = 10; i < 12; ++i){
			if(boost::dynamic_pointer_cast<const TestRunnable>(tg.thread(i-10)->runnable())->value() != i+1){
				equal = false;
				break;
			}
		}

		CPPUNIT_ASSERT(equal);
	}
}

void ThreadGroupTest::testCancelPersistence()
{
	ThreadGroup tg(2);

	{
		std::vector<boost::shared_ptr<TestRunnable>> runnables;

		for(int i = 0; i < 2; ++i){
			runnables.push_back(boost::shared_ptr<TestRunnable>(new TestRunnable(i)));
		}

		std::vector<RunnablePtr> tgRunnables(runnables.begin(), runnables.end());
		runnables[0]->m2().lock();
		tg.start(tgRunnables, IThread::Inheritate);
		runnables[0]->m1().lock();
		tg.cancel();
		tg.join();		
	}

	{
		std::vector<boost::shared_ptr<TestRunnable>> runnables;

		for(int i = 0; i < 2; ++i){
			runnables.push_back(boost::shared_ptr<TestRunnable>(new TestRunnable(i)));
		}

		std::vector<RunnablePtr> tgRunnables(runnables.begin(), runnables.end());

		tg.start(tgRunnables, IThread::Inheritate);
		tg.join();

		bool equal = true;

		for(int i = 0; i < 2; ++i){
			if(boost::dynamic_pointer_cast<const TestRunnable>(tg.thread(i)->runnable())->value() != i+1){
				equal = false;
				break;
			}
		}

		CPPUNIT_ASSERT(equal);
	}	
}