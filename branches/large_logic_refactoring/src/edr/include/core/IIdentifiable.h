/********************************************************************
	created:  2010/09/28
	created:  28:9:2010   1:22
	filename: IIdentifiable.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__IIDENTIFIABLE_H__
#define __HEADER_GUARD__IIDENTIFIABLE_H__

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <core/BaseDataTypes.h>

namespace core {

//! Interfejs reprezentujący obiekt możliwy do identyfikacji.
class IIdentifiable
{
public:
    virtual ~IIdentifiable() 
    { 
    }

    //! \return Unikalny identyfikator obiektu
    virtual UniqueID getID() const = 0;
    //! \return Opis obiektu
    virtual const std::string & getDescription() const = 0;
};

}

//! Makro ułatwiające "wstrzykiwanie" metod interfejsu IIdentifable do klas implementujących go
#define UNIQUE_ID(uuidStr, shortDescription)                            \
 public:                                                                \
    static boost::uuids::uuid getClassID()                              \
    {                                                                   \
        static std::string s(uuidStr);                                  \
        static boost::uuids::string_generator gen;                      \
        return gen(s);                                                  \
    }                                                                   \
                                                                        \
    virtual boost::uuids::uuid getID() const                            \
    {                                                                   \
        return getClassID();                                            \
    }                                                                   \
                                                                        \
    virtual const std::string & getDescription() const                  \
    {                                                                   \
        static const std::string description(shortDescription);         \
        return description;                                             \
    }                                                                   \
private:

#endif  // __HEADER_GUARD__IIDENTIFIABLE_H__
