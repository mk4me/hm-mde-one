/********************************************************************
    created:  2012/10/31
    created:  31:10:2012   12:55
    filename: ThreadingPolicies.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_UTILS__IJOBMANAGER_H__
#define HEADER_GUARD_UTILS__IJOBMANAGER_H__

namespace utils{

class IJob;

class IJobPusher
{
public:
	virtual void pushJob(IJob * job) = 0;
};

class IJobManager : public IJobPusher
{
public:

	virtual void start() = 0;
	virtual void stop() = 0;
	virtual const bool running() = 0;

	virtual void pause() = 0;
	virtual const bool paused() = 0;
};

}

#endif  //  HEADER_GUARD_UTILS__THREADINGPOLICIES_H__
