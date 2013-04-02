#include "ThreadTest.h"
#include <utils/Thread.h>
#include <QtCore/QMutex>
#include <exception>
#include <algorithm>
#include <vector>

using namespace utils;

CPPUNIT_TEST_SUITE_REGISTRATION( ThreadTest );

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

class TestThreadObserver : public Observer<IThreadingBase>
{
public:

	struct ThreadState
	{
		IThreadingBase::Status status;
		IThreadingBase::Result result;

		bool operator==(const ThreadState & s2)
		{
			if(result == s2.result && status == s2.status){
				return true;
			}

			return false;
		}
	};

	typedef std::vector<ThreadState> States;

public:

	virtual void update(const IThreadingBase * thread)
	{
		ThreadState state;
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

void ThreadTest::testJoin()
{
	Thread t;
	int val = 2;
	boost::shared_ptr<TestRunnable> runnable(new TestRunnable(val));
	t.start(runnable, IThread::Inheritate);
	t.join();
	CPPUNIT_ASSERT_EQUAL(val + 1, runnable->value());
}

void ThreadTest::testCancel()
{
	Thread t;
	int val = 2;
	boost::shared_ptr<TestRunnable> runnable(new TestRunnable(val));
	runnable->m2().lock();
	t.start(runnable, IThread::Inheritate);
	runnable->m1().lock();
	t.cancel();
	t.join();
	CPPUNIT_ASSERT_EQUAL(val, runnable->value());
}

void ThreadTest::testNotify()
{
	TestThreadObserver::States states;
	TestThreadObserver::ThreadState ts;

	TestThreadObserver observer;
	{
		Thread t;

		t.attach(&observer);

		CPPUNIT_ASSERT_EQUAL(t.status(), IThreadingBase::Idle);
		CPPUNIT_ASSERT_EQUAL(t.result(), IThreadingBase::FirstProcessing);

		int val = 2;
		boost::shared_ptr<TestRunnable> runnable(new TestRunnable(val));
		t.start(runnable, IThread::Inheritate);
		ts.result = IThreadingBase::FirstProcessing;
		ts.status = IThreadingBase::Running;
		states.push_back(ts);
		t.join();
		ts.result = IThreadingBase::Finished;
		ts.status = IThreadingBase::Idle;
		states.push_back(ts);
	}

	ts.result = IThreadingBase::Finished;
	ts.status = IThreadingBase::Killed;
	states.push_back(ts);

	{
		Thread t;

		t.attach(&observer);

		int val = 2;
		boost::shared_ptr<TestRunnable> runnable(new TestRunnable(val));
		runnable->m2().lock();
		t.start(runnable, IThread::Inheritate);
		ts.result = IThreadingBase::FirstProcessing;
		ts.status = IThreadingBase::Running;
		states.push_back(ts);
		runnable->m1().lock();
		t.cancel();
		t.join();
		ts.result = IThreadingBase::Canceled;
		ts.status = IThreadingBase::Idle;
		states.push_back(ts);
	}

	ts.result = IThreadingBase::Canceled;
	ts.status = IThreadingBase::Killed;
	states.push_back(ts);

	{
		Thread t;

		t.attach(&observer);

		RunnablePtr runnable(new TestExceptionRunnable());
		t.start(runnable, IThread::Inheritate);
		ts.result = IThreadingBase::FirstProcessing;
		ts.status = IThreadingBase::Running;
		states.push_back(ts);
		t.join();
		ts.result = IThreadingBase::Error;
		ts.status = IThreadingBase::Idle;
		states.push_back(ts);
	}

	ts.result = IThreadingBase::Error;
	ts.status = IThreadingBase::Killed;
	states.push_back(ts);

	CPPUNIT_ASSERT( ((states.size() == observer.states().size()) && (std::equal(states.begin(), states.end(), observer.states().begin()) == true)));
}

void ThreadTest::testExceptionPersistence()
{
	Thread t;

	RunnablePtr runnable1(new TestExceptionRunnable());
	t.start(runnable1, IThread::Inheritate);
	t.join();

	int val = 2;
	boost::shared_ptr<TestRunnable> runnable2(new TestRunnable(val));
	t.start(runnable2, IThread::Inheritate);
	t.join();
	CPPUNIT_ASSERT_EQUAL(val + 1, runnable2->value());
}

void ThreadTest::testCancelPersistence()
{
	Thread t;

	int val = 2;
	boost::shared_ptr<TestRunnable> runnable1(new TestRunnable(val));
	runnable1->m2().lock();
	t.start(runnable1, IThread::Inheritate);
	runnable1->m1().lock();
	t.cancel();
	t.join();

	boost::shared_ptr<TestRunnable> runnable2(new TestRunnable(val));
	t.start(runnable2, IThread::Inheritate);
	t.join();
	CPPUNIT_ASSERT_EQUAL(val + 1, runnable2->value());
}