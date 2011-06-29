/********************************************************************
    created:  2011/05/12
    created:  12:5:2011   15:21
    filename: Synchronized.h
    author:   Mateusz Janiak
    
    purpose:  Klasa do zapewniania synchronizacji na poziomie obiekt�w.
                By wykona� operacje na obiekcie nale�y wykona� na nim LOCK(x) { blok instrukcji }
                lub pos�u�yc si� typem lokuj�cym dany obiekt na czas swojego �ycia - _Locker

                Koncepcja pochodzi ze storny: http://tlzprgmr.wordpress.com/2008/08/10/adding-a-lock-statement-to-c/
*********************************************************************/
#ifndef HEADER_GUARD_UTILS__SYNCHRONIZED_H__
#define HEADER_GUARD_UTILS__SYNCHRONIZED_H__

#include <boost/type_traits/is_scalar.hpp>
#include <boost/thread/mutex.hpp>

namespace utils 
{
    template<class T, bool>
    class SynchronizedImpl : public T
    {
    public:
        SynchronizedImpl() : T() { }
        SynchronizedImpl(const T &x) : T(x) { }
    };

    template<typename T>
    class SynchronizedImpl<T, true>
    {
    public:
        SynchronizedImpl() { }
        SynchronizedImpl(const T &x) : x_(x) { }
        operator T &() { return x_; }
        operator const T &() const { return x_; }
        SynchronizedImpl &operator= (T x) { x_ = x; return *this; }

    private:
        T x_;
    };

    template<typename T>
    class Synchronized :
        public SynchronizedImpl<T, boost::is_scalar<T>::value>, public boost::mutex
    {
    private:
        typedef SynchronizedImpl<T, boost::is_scalar<T>::value> base;

    public:
        Synchronized() : base(), boost::mutex() { }
        explicit Synchronized(const T &x) : base(x), boost::mutex() { }

        Synchronized &operator= (const T &rhs)
        {
            base &b = *this;
            b = rhs;
            return *this;
        }
    };

    class _Locker {
    public:
        _Locker(boost::mutex& m) : m_(m) { m_.lock(); run_ = true; }
        _Locker(boost::mutex& m, int) : m_(m) { run_ = m_.try_lock(); }
        ~_Locker() { m_.unlock(); }
        bool _run() { return run_; }
        void _stop() { run_ = false; }
    private:
        boost::mutex&   m_;
        bool            run_;
    };

    #define LOCK(x) for (_Locker _lock(x); _lock._run(); _lock._stop())
    #define TRY_LOCK(x) for (_Locker _lock(x, 0); _lock._run(); _lock._stop())
}

#endif  //  HEADER_GUARD_UTILS__SYNCHRONIZED_H__