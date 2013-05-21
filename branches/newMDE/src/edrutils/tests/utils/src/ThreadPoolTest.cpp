#include "ThreadPoolTest.h"
#include <utils/ThreadPool.h>
#include <vector>

using namespace utils;

CPPUNIT_TEST_SUITE_REGISTRATION( ThreadPoolTest );

class TestExceptionRunnable : public IRunnable
{
public:
	virtual void run()
	{
		throw std::runtime_error("Runnable exception");
	}
};

class SimpleTestRunnable : public IRunnable
{
public:

	SimpleTestRunnable(int val) : val(val)
	{

	}

	virtual void run()
	{
		++val;
	}

	const int value() const
	{
		return val;
	}

private:
	volatile int val;
};

void ThreadPoolTest::testCreate()
{
	ThreadPool tp(5,10);

	CPPUNIT_ASSERT(tp.minThreads() == 5);
	CPPUNIT_ASSERT(tp.maxThreads() == 10);
	CPPUNIT_ASSERT(tp.threadsCount() == 0);	

	{
		auto t = tp.getOrCreateThread();
		CPPUNIT_ASSERT(t != nullptr);
		CPPUNIT_ASSERT(tp.threadsCount() == 1);
		{
			auto t = tp.getOrCreateThread();
			CPPUNIT_ASSERT(t != nullptr);
			CPPUNIT_ASSERT(tp.threadsCount() == 2);
		}

		CPPUNIT_ASSERT(tp.threadsCount() == 1);

		auto r = new SimpleTestRunnable(1);

		t->start(utils::RunnablePtr(r));
		t->join();

		CPPUNIT_ASSERT(r->value() == 2);
	}

	CPPUNIT_ASSERT(tp.threadsCount() == 0);	
}

void ThreadPoolTest::testCreateGroup()
{
	ThreadPool tp(16,100);

	CPPUNIT_ASSERT(tp.minThreads() == 16);
	CPPUNIT_ASSERT(tp.maxThreads() == 100);
	CPPUNIT_ASSERT(tp.threadsCount() == 0);	

	{
		std::vector<utils::ThreadPtr> ts;
		tp.getOrCreateThreadsGroup(4, ts);
		CPPUNIT_ASSERT(ts.size() == 4);
		CPPUNIT_ASSERT(tp.threadsCount() == 4);
		{
			std::vector<utils::ThreadPtr> ts;
			tp.getOrCreateThreadsGroup(10, ts);
			CPPUNIT_ASSERT(ts.size() == 10);
			CPPUNIT_ASSERT(tp.threadsCount() == 14);
		}

		CPPUNIT_ASSERT(tp.threadsCount() == 4);
	}

	CPPUNIT_ASSERT(tp.threadsCount() == 0);
}

void ThreadPoolTest::testToMuchException()
{
	ThreadPool tp(1,2);

	CPPUNIT_ASSERT(tp.minThreads() == 1);
	CPPUNIT_ASSERT(tp.maxThreads() == 2);
	CPPUNIT_ASSERT(tp.threadsCount() == 0);	

	std::vector<utils::ThreadPtr> ts;
	tp.getOrCreateThreadsGroup(4, ts);
}