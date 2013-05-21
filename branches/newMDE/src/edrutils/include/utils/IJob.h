/********************************************************************
    created:  2012/10/31
    created:  31:10:2012   12:55
    filename: ThreadingPolicies.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_UTILS__IJOB_H__
#define HEADER_GUARD_UTILS__IJOB_H__

namespace utils{


	class IJob
	{
	public:

		enum Status
		{
			JOB_WAITING,	//! Czeka w kolejce
			JOB_STARTED,	//! Rozpocz�ta
			JOB_FINISHED,	//! Zako�czona powodzeniem
			JOB_CANCELLED,	//! Anulowana
			JOB_ERROR		//! Zako�czona niepowodzeniem
		};

	public:

		Status status();

		virtual void execute() = 0;
	};

	class IJoinableJob
	{
	public:
		virtual void join();
	};

	class ICancelableJob
	{
	public:
		virtual void cancel();
	};

}

#endif  //  HEADER_GUARD_UTILS__THREADINGPOLICIES_H__
