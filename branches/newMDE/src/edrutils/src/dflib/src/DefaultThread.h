/********************************************************************
    created:  2012/12/11
    created:  11:12:2012   15:04
    filename: DefaultThread.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___DEFAULTTHREAD_H__
#define HEADER_GUARD___DEFAULTTHREAD_H__

#include <QtCore/QThread>
#include <utils/IThread.h>
#include <utils/IThreadFactory.h>


class DefaultThread : public utils::IThread, private QThread
{
public:
	DefaultThread(const utils::IThreadFactory::Functor & func);

	virtual void start();
	virtual void cancel();

	virtual void join();

protected:

	virtual void run();

private:
	utils::IThreadFactory::Functor func_;
};

#endif	//	HEADER_GUARD___DEFAULTTHREAD_H__
