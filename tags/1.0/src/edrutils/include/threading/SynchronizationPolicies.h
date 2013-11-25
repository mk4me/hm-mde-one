/********************************************************************
    created:  2011/06/14
    created:  14:6:2011   8:48
    filename: SynchronizationPolicies.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_UTILS__SYNCHRONIZATIONPOLICIES_H__
#define HEADER_GUARD_UTILS__SYNCHRONIZATIONPOLICIES_H__

#include <QtCore/QMutex>
#include <boost/mpl/if.hpp>

namespace utils {

class LockingPolicy
{
public:
	virtual void lock() = 0;
	virtual void unlock() = 0;
	virtual const bool tryLock() = 0;
};

class NoSyncPolicy : public LockingPolicy
{
public:
    virtual void lock() {}
    virtual void unlock() {}
    virtual const bool tryLock() { return true; }
};

class RecursiveSyncPolicy : public LockingPolicy
{
public:

	RecursiveSyncPolicy() : recMutex(QMutex::Recursive) {}

    virtual void lock() { recMutex.lock(); }
    virtual void unlock() { recMutex.unlock(); }
    virtual const bool tryLock() { return recMutex.tryLock(); }

private:    
	QMutex recMutex;
};

class StrictSyncPolicy : public LockingPolicy
{
public:

	StrictSyncPolicy() : mut(QMutex::NonRecursive) {}

    virtual void lock() { mut.lock(); }
    virtual void unlock() { mut.unlock(); }
    virtual const bool tryLock() { return mut.tryLock(); }

private:    
	QMutex mut;
};

class EmptyScopedLock
{
public:
	template<class SyncPolicy>
	EmptyScopedLock(SyncPolicy & mux) {}

	virtual ~EmptyScopedLock() {}
};


template<class SyncPolicy>
class ScopedLock
{
public:
	ScopedLock(SyncPolicy & mux) : mux(mux)
	{
		mux.lock();
	}

	virtual ~ScopedLock()
	{
		mux.unlock();
	}

private:
	LockingPolicy & mux;
};

template<bool threading>
class SynchronizedT : public boost::mpl::if_c<threading, RecursiveSyncPolicy, NoSyncPolicy>::type
{
public:

	typedef typename boost::mpl::if_c<threading, RecursiveSyncPolicy, NoSyncPolicy>::type RecursiveSyncPolicy;
	typedef typename boost::mpl::if_c<threading, ScopedLock<RecursiveSyncPolicy>, EmptyScopedLock>::type ScopedLock;

protected:

	SynchronizedT()
	{
		mux = this;
	}

public:

	virtual ~SynchronizedT() {}

protected:

	mutable RecursiveSyncPolicy * mux;
};

}

#endif  //  HEADER_GUARD_UTILS__SYNCHRONIZATIONPOLICIES_H__
