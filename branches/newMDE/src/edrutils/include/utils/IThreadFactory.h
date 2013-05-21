/********************************************************************
    created:  2012/12/11
    created:  11:12:2012   15:00
    filename: IThreadFactory.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___ITHREADFACTORY_H__
#define HEADER_GUARD___ITHREADFACTORY_H__

#include <boost/function.hpp>

namespace utils {

class IThread;

class IThreadFactory
{
public:

	typedef boost::function<void(void)> Functor;

public:

	virtual IThread * createThread(const Functor & func) = 0;	
};

}

#endif	//	HEADER_GUARD___ITHREADFACTORY_H__
