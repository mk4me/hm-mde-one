/********************************************************************
	created:  2010/09/28
	created:  28:9:2010   1:22
	filename: IIdentifiable.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__IIDENTIFIABLE_H__
#define __HEADER_GUARD__IIDENTIFIABLE_H__

#include <core/BaseDataTypes.h>

//! Interfejs reprezentujący obiekt możliwy do identyfikacji.
class IIdentifiable
{
public:
    virtual ~IIdentifiable() {}
    virtual UniqueID getID() const = 0;
};

//! Makro służące do nadania identyfikatora obiektowi implementującemu
//! interfejs IIdentifiable.
//! Przykład użycia: UNIQUE_ID('MAJR','T001')
#define UNIQUE_ID(major, minor)            \
private:                                \
    UniqueIdSpec<major, minor> __id;    \
public:                                 \
    virtual UniqueID getID() const      \
    {                                   \
        return __id;                    \
    }                                   \
    static UniqueID getClassID()        \
    {                                   \
        return UniqueID(major, minor);  \
    }        


#endif  // __HEADER_GUARD__IIDENTIFIABLE_H__