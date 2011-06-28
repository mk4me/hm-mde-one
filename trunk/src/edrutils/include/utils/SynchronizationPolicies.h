/********************************************************************
    created:  2011/06/14
    created:  14:6:2011   8:48
    filename: SynchronizationPolicies.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_UTILS__SYNCHRONIZATIONPOLICIES_H__
#define HEADER_GUARD_UTILS__SYNCHRONIZATIONPOLICIES_H__

#include <boost/thread/mutex.hpp>
#include <boost/thread/recursive_mutex.hpp>

class NoSynchPolicy
{
public:
    void lock() {}
    void unlock() {}
    bool tryLock() { return true; }
};

class RecursiveSynchPolicy
{
public:
    void lock() { recMutex.lock(); }
    void unlock() { recMutex.unlock(); }
    bool tryLock() { return recMutex.try_lock(); }

private:
    boost::recursive_mutex recMutex;
};

class StrictSynchPolicy
{
public:
    void lock() { mut.lock(); }
    void unlock() { mut.unlock(); }
    bool tryLock() { return mut.try_lock(); }

private:
    boost::mutex mut;
};

#endif  //  HEADER_GUARD_UTILS__SYNCHRONIZATIONPOLICIES_H__