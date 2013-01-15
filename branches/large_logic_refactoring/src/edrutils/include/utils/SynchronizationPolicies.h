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

class NoSyncPolicy
{
public:
    void lock() {}
    void unlock() {}
    const bool tryLock() { return true; }
};

class RecursiveSyncPolicy
{
public:

	RecursiveSyncPolicy() : recMutex(QMutex::Recursive) {}

    void lock() { recMutex.lock(); }
    void unlock() { recMutex.unlock(); }
    const bool tryLock() { return recMutex.tryLock(); }

private:
    //OpenThreads::ReentrantMutex recMutex;
	QMutex recMutex;
};

class StrictSyncPolicy
{
public:

	StrictSyncPolicy() : mut(QMutex::NonRecursive) {}

    void lock() { mut.lock(); }
    void unlock() { mut.unlock(); }
    const bool tryLock() { return mut.tryLock(); }

private:
    //OpenThreads::Mutex mut;
	QMutex mut;
};

template<class SyncPolicy>
class EmptyScopedLock
{
public:

	EmptyScopedLock(SyncPolicy & mux) {}

	~EmptyScopedLock() {}
};


template<class SyncPolicy>
class ScopedLock
{
public:

	ScopedLock(SyncPolicy & mux) : mux(mux)
	{
		mux.lock();
	}

	~ScopedLock()
	{
		mux.unlock();
	}

private:
	SyncPolicy & mux;
};

template<bool threading>
class SynchronizedT : public boost::mpl::if_c<threading, RecursiveSyncPolicy, NoSyncPolicy>::type
{
public:

	typedef typename boost::mpl::if_c<threading, RecursiveSyncPolicy, NoSyncPolicy>::type RecursiveSyncPolicy;
	typedef typename boost::mpl::if_c<threading, ScopedLock<RecursiveSyncPolicy>, EmptyScopedLock<RecursiveSyncPolicy>>::type ScopedLock;

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
