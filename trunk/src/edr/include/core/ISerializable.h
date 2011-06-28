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

//! Interfejs s�u��cy do serializacji i deserializacji.
class ISerializable
{
public:
    virtual ~ISerializable() 
    {
    }
    //! \param output Strumie� wyj�ciowy do kt�rego robimy zrzut zserializowanego obiektu
    virtual void serialize(std::ostream& output) = 0;
    //! \param input Strumie� z kt�rego dokonujemy deserializacji obiektu
    virtual void deserialize(std::istream& input) = 0;
};

}
#endif