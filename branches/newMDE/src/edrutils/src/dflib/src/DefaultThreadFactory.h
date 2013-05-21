/********************************************************************
    created:  2012/12/11
    created:  11:12:2012   15:15
    filename: DefaultThreadFactory.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___DEFAULTTHREADFACTORY_H__
#define HEADER_GUARD___DEFAULTTHREADFACTORY_H__

#include <utils/IThreadFactory.h>

class DefaultThreadFactory : public utils::IThreadFactory
{
public:

	virtual utils::IThread * createThread(const Functor & func);	
};

#endif	//	HEADER_GUARD___DEFAULTTHREADFACTORY_H__
