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

//! Interfejs reprezentuj¹cy obiekt mo¿liwy do identyfikacji.
class IIdentifiable
{
public:
    virtual ~IIdentifiable() {}
    virtual UniqueId getID() const = 0;
};

//! Makro s³u¿¹ce do nadania identyfikatora obiektowi implementuj¹cemu
//! interfejs IIdentifiable.
//! Przyk³ad u¿ycia: UNIQUE_ID('MAJR','T001')
#define UNIQUE_ID(major, minor)            \
private:                                \
    UniqueIdSpec<major, minor> __id;    \
public:                                 \
    virtual UniqueId getID() const      \
    {                                   \
        return __id;                    \
    }                                   \
    static UniqueId getClassID()        \
    {                                   \
        return UniqueId(major, minor);  \
    }        


#endif  // __HEADER_GUARD__IIDENTIFIABLE_H__