/********************************************************************
    created:  2012/10/31
    created:  31:10:2012   12:55
    filename: ThreadingPolicies.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_UTILS__THREADINGPOLICIES_H__
#define HEADER_GUARD_UTILS__THREADINGPOLICIES_H__

#include <OpenThreads/Thread>
#include <boost/function.hpp>
#include <QtCore/QThread>

namespace utils{

typedef boost::function<void()> Functor;

class NoThreadingPolicy
{
public:

	class Thread
	{
	public:

		Thread(const Functor & functor) : functor_(functor) {}

		void start() { try{ functor_(); } catch(std::exception &){ } catch(...) {} }

		void cancel() {}

		void join(){}

		const bool running() { return false; }

	private:
		Functor functor_;
	};

	typedef Thread thread_type;
};

class OpenThreadsThreadingPolicy
{
public:

	class Thread
	{
	private:

		class InnerThreadImp : public OpenThreads::Thread
		{
		public:
			InnerThreadImp(const Functor & functor) : functor_(functor) {}

			virtual void run()
			{
				if(functor_.empty() == false)
				{
					try{
						functor_();
					}catch(std::exception &)
					{

					}catch(...)
					{

					}
				}
			}

		private:
			Functor functor_;
		};

	public:

		Thread(const Functor & functor) : innerThread_(functor) {}

		void start() { innerThread_.run(); }

		void cancel() { innerThread_.cancel(); }

		void join(){ innerThread_.join(); }

		const bool running() { return false; }

	private:
		InnerThreadImp innerThread_;
	};

	typedef Thread thread_type;
};

class QTThreadingPolicy
{
public:

	class Thread
	{
	private:

		class InnerThreadImp : public QThread
		{
		public:
			InnerThreadImp(const Functor & functor) : functor_(functor) {}

		protected:
			virtual void run()
			{
				if(functor_.empty() == false)
				{
					try{
						functor_();
					}catch(std::exception &)
					{

					}catch(...)
					{

					}
				}
			}

		private:
			Functor functor_;
		};

	public:

		Thread(const Functor & functor) : innerThread_(functor) {}

		void start() { innerThread_.start(); }

		void cancel() { innerThread_.terminate(); }

		void join(){ innerThread_.wait(); }

		const bool running() { return false; }

	private:
		InnerThreadImp innerThread_;
	};

	typedef Thread thread_type;
};

}

#endif  //  HEADER_GUARD_UTILS__THREADINGPOLICIES_H__
