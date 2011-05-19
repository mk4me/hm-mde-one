/********************************************************************
	created:	2011/05/12
	created:	12:5:2011   9:09
	filename: 	ISerializable.h
	
	purpose:	
*********************************************************************/


#ifndef HEADER_GUARD_CORE__ISERIALIZABLE_H__
#define HEADER_GUARD_CORE__ISERIALIZABLE_H__

#include <iostream>

namespace core {

class ISerializable
{
public:
    virtual ~ISerializable() 
    {
    }
    virtual void serialize(std::ostream& output) = 0;
    virtual void deserialize(std::istream& istream) = 0;
};

}
#endif