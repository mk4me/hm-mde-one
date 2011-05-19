/********************************************************************
	created:	2011/05/12
	created:	12:5:2011   9:12
	filename: 	IClonable.h
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORE__ICLONABLE_H__
#define HEADER_GUARD_CORE__ICLONABLE_H__

namespace core {

class IClonable
{
public:
    virtual void* createClone() = 0;
    virtual ~IClonable() 
    {
    }
};

}

#endif