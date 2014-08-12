#include "Job.h"

using namespace core;

Job::Job(const std::string & who, const std::string & name,
	const threadingUtils::IRunnablePtr runnable)
	: who_(who), name_(name), runnable_(runnable),
	status_(IJob::JOB_PENDING)
{

}

Job::~Job()
{
	unlock();
}

threadingUtils::IRunnablePtr Job::runnable()
{
	return runnable_;
}

threadingUtils::IRunnableConstPtr Job::runnable() const
{
	return runnable_;
}

const std::string & Job::who() const
{
	return who_;
}

const std::string & Job::name() const
{
	return name_;
}

const IJob::Status Job::status() const
{	
	return status_;
}

void Job::wait()
{
	if (status_ == IJob::JOB_FINISHED || status_ == IJob::JOB_ERROR){
		return;
	}

	threadingUtils::StrictSyncPolicy sobj;
	sobj.lock();
	wait_.wait(&sobj);
}

void Job::unlock()
{
	wait_.wakeAll();
}

void Job::setStatus(const Status status)
{	
	status_ = status;
}

void Job::run()
{
	setStatus(IJob::JOB_WORKING);

	try{
		runnable_->run();
		setStatus(IJob::JOB_FINISHED);
	}
	catch (std::exception &){
	}
	catch (...){
	}

	if (status_ != IJob::JOB_FINISHED){
		setStatus(IJob::JOB_ERROR);
	}

	unlock();
}