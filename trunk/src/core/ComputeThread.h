/********************************************************************
	created:  2010/10/25
	created:  25:10:2010   12:26
	filename: ComputeThread.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__COMPUTETHREAD_H__
#define __HEADER_GUARD__COMPUTETHREAD_H__

#include <OpenThreads/Thread>

class ServiceManager;

class ComputeThread : public OpenThreads::Thread
{
private:
    ServiceManager* serviceManager;
    double refreshRate;
    volatile bool done;

public:
    //! 
    //! \param serviceManager
    //! \param refreshRate
    ComputeThread(ServiceManager* serviceManager, double refreshRate);
    //! Autosynchronizuj�cy si� destruktor.
    virtual ~ComputeThread();

    //! \param done Czy ma si� zako�czy�?
    void setDone(bool done);
    //! Cia�o w�tku.
    virtual void run();
};



#endif  // __HEADER_GUARD__COMPUTETHREAD_H__