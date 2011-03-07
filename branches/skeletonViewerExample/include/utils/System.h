/********************************************************************
	created:    2010/08/11
	created:    11:8:2010     11:06
	filename: System.h
	author:	    Piotr Gwiazdowski

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD__SYSTEM_H__
#define __HEADER_GUARD__SYSTEM_H__

#include <utils/Config.h>
#if defined(__WIN32__)
#include <Windows.h>
#elif defined(__UNIX__)
#include <time.h>
#include <pthread.h>
#include <sys/types.h>
#endif

#include <utils/Debug.h>


////////////////////////////////////////////////////////////////////////////////
namespace utils {
namespace __system {
////////////////////////////////////////////////////////////////////////////////

#ifdef __WIN32__

//! Typ tykniêcia zegara.
typedef long long Tick;
//! ID w¹tku.
typedef unsigned ThreadID;

//! \return Tykniêcia zegara o wysokiej czêstotliwoœci.
inline Tick getTick()
{
    LARGE_INTEGER nCurrTime;
    QueryPerformanceCounter(&nCurrTime);
    return nCurrTime.QuadPart;
}

//! \return Tykniêcia zegara wyra¿one w sekundach.
inline double tickToSeconds(Tick value)
{
    static LARGE_INTEGER frequency;
    static BOOL __init = QueryPerformanceFrequency(&frequency);
    UTILS_ASSERT(__init);
    return static_cast<double>(value)/static_cast<double>(frequency.QuadPart);
}

//!
inline ThreadID getCurrentThreadID()
{
    return GetCurrentThreadId();
}

//!
struct Mutex : public CRITICAL_SECTION
{
    inline ~Mutex()
    {
        DeleteCriticalSection(this);
    }

    inline static Mutex* create()
    {
        Mutex* result = new Mutex();
        InitializeCriticalSection(result);
        return result;
    }

    inline void lock()
    {
        EnterCriticalSection(this);
    }

    inline void unlock()
    {
        LeaveCriticalSection(this);
    }

private:
    Mutex() {}
};


#else

//! Typ tykniêcia zegara.
typedef long long Tick;
//! ID w¹tku.
typedef pthread_t ThreadID;

//! \return Tykniêcia zegara o wysokiej czêstotliwoœci.
inline Tick getTick()
{
    struct timespec tp;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tp);
    // rezultat w mikrosekundach
    Tick result;
    result = 1000000 * tp.tv_sec + tp.tv_nsec / 1000;
    return result;
}

//! \return Tykniêcia zegara wyra¿one w sekundach.
inline double tickToSeconds(Tick value)
{
    return value / 1000000.0;
}

//!
inline ThreadID getCurrentThreadID()
{
    return pthread_self();
}

//!
struct Mutex
{
    pthread_mutex_t mutex;
    inline ~Mutex()
    {
        int err = pthread_mutex_destroy(&mutex);
        UTILS_ASSERT(err == 0, "Destroying mutex failed.");
    }

    inline static Mutex* create()
    {
        Mutex* result = new Mutex();
        int err = pthread_mutex_init(&result->mutex, NULL);
        UTILS_ASSERT(err == 0, "Creating mutex failed.");
        return result;
    }

    inline void lock()
    {
        pthread_mutex_unlock(&mutex);
    }

    inline void unlock()
    {
        pthread_mutex_lock(&mutex);
    }

private:
    Mutex() {}
};

#endif



////////////////////////////////////////////////////////////////////////////////
} // namespace __system
} // namespace utils
////////////////////////////////////////////////////////////////////////////////


#endif    // __HEADER_GUARD__SYSTEM_H__
