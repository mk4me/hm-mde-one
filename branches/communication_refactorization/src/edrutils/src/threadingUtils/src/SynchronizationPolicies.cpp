#include <threadingUtils/SynchronizationPolicies.h>
#include <QtCore/QMutex>
#include <QtCore/QWaitCondition>

using namespace threadingUtils;

LockingPolicy::LockingPolicy()
{
}
LockingPolicy::~LockingPolicy()
{
}

void LockingPolicy::lock()
{
}

void LockingPolicy::unlock()
{
}

const bool LockingPolicy::tryLock()
{
	return true;
}

class RecursiveSyncPolicy::RecursiveSyncPolicyImpl
{
	friend class ConditionVariable;
public:
	RecursiveSyncPolicyImpl() : mux(QMutex::Recursive) {}
	~RecursiveSyncPolicyImpl() {}

	void lock() { mux.lock(); }
	void unlock() { mux.unlock(); }
	const bool tryLock() { return mux.tryLock(); }

private:
	//! Faktyczny mutex
	QMutex mux;
};

class StrictSyncPolicy::StrictSyncPolicyImpl
{
	friend class ConditionVariable;
public:
	StrictSyncPolicyImpl() : mux(QMutex::NonRecursive) {}
	~StrictSyncPolicyImpl() {}

	void lock() { mux.lock(); }
	void unlock() { mux.unlock(); }
	const bool tryLock() { return mux.tryLock(); }

private:
	//! Faktyczny mutex
	QMutex mux;
};

RecursiveSyncPolicy::RecursiveSyncPolicy() : impl(new RecursiveSyncPolicyImpl)
{
}

RecursiveSyncPolicy::~RecursiveSyncPolicy()
{
}

void RecursiveSyncPolicy::lock()
{
	impl->lock();
}

void RecursiveSyncPolicy::unlock()
{
	impl->unlock();
}

const bool RecursiveSyncPolicy::tryLock()
{
	return impl->tryLock();
}

StrictSyncPolicy::StrictSyncPolicy() : impl(new StrictSyncPolicyImpl)
{
}

StrictSyncPolicy::~StrictSyncPolicy()
{
}

void StrictSyncPolicy::lock()
{
	impl->lock();
}

void StrictSyncPolicy::unlock()
{
	impl->unlock();
}

const bool StrictSyncPolicy::tryLock()
{
	return impl->tryLock();
}

class ConditionVariable::ConditionVariableImpl
{
public:
	ConditionVariableImpl() {}
	~ConditionVariableImpl() {}

	void wait(QMutex * lock, const unsigned long time)
	{
		condition.wait(lock, time);
	}

	void wakeOne() { condition.wakeOne(); };
	void wakeAll() { condition.wakeAll(); }

private:

	QWaitCondition condition;
};

ConditionVariable::ConditionVariable()
: impl(new ConditionVariableImpl)
{
}

ConditionVariable::~ConditionVariable()
{
}

void ConditionVariable::wait(LockingPolicy * lock, const unsigned long time)
{
	StrictSyncPolicy * sp = dynamic_cast<StrictSyncPolicy*>(lock);
	QMutex * mux = nullptr;
	if (sp != nullptr){
		mux = &(sp->impl->mux);
	}
	else{
		RecursiveSyncPolicy * rp = dynamic_cast<RecursiveSyncPolicy*>(lock);
		if (rp != nullptr){
			mux = &(rp->impl->mux);
		}
	}

	if (mux != nullptr){
		impl->wait(mux, time);
	}
}

void ConditionVariable::wakeOne()
{
	impl->wakeOne();
}

void ConditionVariable::wakeAll()
{
	impl->wakeAll();
}