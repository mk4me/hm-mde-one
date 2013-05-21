/********************************************************************
    created:  2012/12/11
    created:  11:12:2012   14:54
    filename: IThread.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___ITHREAD_H__
#define HEADER_GUARD___ITHREAD_H__

namespace utils {

class IThread
{
public:

	virtual void start() = 0;
	virtual void cancel() = 0;

	virtual void join() = 0;
};

}

#endif	//	HEADER_GUARD___ITHREAD_H__
