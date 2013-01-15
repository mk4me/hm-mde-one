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

namespace plugin {

//! Interfejs reprezentujący obiekt możliwy do identyfikacji.
class IIdentifiable
{
public:
    //! \return Unikalny identyfikator obiektu
    virtual UniqueID getID() const = 0;
};

class IDescription
{
public:
	virtual const std::string & getName() const = 0;
	virtual const std::string  getDescription() const = 0;
};

class ICoreElement : public IIdentifiable, public IDescription
{
public:

	virtual ICoreElement * create() const = 0;
};

}

//! Makro ułatwiające "wstrzykiwanie" metod interfejsu IIdentifable do klas implementujących go
#define UNIQUE_ID(uuidStr)                 \
 public:                                                                \
    inline static const boost::uuids::uuid getClassID()                 \
    {                                                                   \
        static const std::string s(uuidStr);                            \
        static boost::uuids::string_generator gen;                      \
		static const boost::uuids::uuid ret(gen(s));					\
        return ret;														\
    }                                                                   \
                                                                        \
    virtual boost::uuids::uuid getID() const                            \
    {                                                                   \
        return getClassID();                                            \
    }                                                                   \
private:

#endif  // __HEADER_GUARD__IIDENTIFIABLE_H__
