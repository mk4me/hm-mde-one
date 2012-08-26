/********************************************************************
    created:  2011/06/14
    created:  14:6:2011   8:48
    filename: SynchronizationPolicies.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_UTILS__SYNCHRONIZATIONPOLICIES_H__
#define HEADER_GUARD_UTILS__SYNCHRONIZATIONPOLICIES_H__

#include <OpenThreads/Mutex>
#include <OpenThreads/ReentrantMutex>

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
    bool tryLock() { return recMutex.trylock() == 0; }

private:
    OpenThreads::ReentrantMutex recMutex;
};

class StrictSynchPolicy
{
public:
    void lock() { mut.lock(); }
    void unlock() { mut.unlock(); }
    bool tryLock() { return mut.trylock() == 0; }

private:
    OpenThreads::Mutex mut;
};

#endif  //  HEADER_GUARD_UTILS__SYNCHRONIZATIONPOLICIES_H__
