#include "Job.h"

using namespace core;

Job::Job(const std::string & who, const std::string & name,
	const utils::IRunnablePtr runnable)
	: who_(who), name_(name), runnable_(runnable),
	status_(IJob::JOB_PENDING)
{
	wait_.lock();
}

Job::~Job()
{
	unlock();
}

utils::IRunnablePtr Job::runnable()
{
	return runnable_;
}

utils::IRunnableConstPtr Job::runnable() const
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
	utils::ScopedLock<utils::StrictSyncPolicy> lock(synch_);
	return status_;
}

void Job::wait()
{
	utils::ScopedLock<utils::StrictSyncPolicy> lock(wait_);
}

void Job::unlock()
{
	wait_.unlock();
}

void Job::setStatus(const Status status)
{
	utils::ScopedLock<utils::StrictSyncPolicy> lock(synch_);
	status_ = status;
}

void Job::run()
{
	setStatus(IJob::JOB_WORKING);

	try{
		runnable_->run();
		setStatus(IJob::JOB_FINISHED);
	}catch(std::exception &){
	
	}catch(...){
	
	}

	if(status_ != IJob::JOB_FINISHED){	
		setStatus(IJob::JOB_ERROR);
	}

	unlock();
}