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
#include <corelib/BaseDataTypes.h>

namespace plugin {

//! Interfejs reprezentujący obiekt możliwy do identyfikacji.
class IIdentifiable
{
public:
    //! \return Unikalny identyfikator obiektu
    virtual core::UniqueID getID() const = 0;

	virtual ~IIdentifiable() {}
};

class IDescription
{
public:
	virtual const std::string getName() const = 0;
	virtual const std::string  getDescription() const = 0;

	virtual ~IDescription() {}
};

class ICoreElement : public IIdentifiable, public IDescription
{
public:

	virtual ICoreElement * create() const = 0;

	virtual ~ICoreElement() {}
};

}

//! Makro ułatwiające "wstrzykiwanie" metod interfejsu IIdentifable do klas implementujących go
#define UNIQUE_ID(uuidStr)                 \
 public:                                                                \
    inline static const core::UniqueID getClassID()                 \
    {                                                                   \
        static const std::string s(uuidStr);                            \
        static boost::uuids::string_generator gen;                      \
		static const core::UniqueID ret(gen(s));					\
        return ret;														\
    }                                                                   \
                                                                        \
    virtual core::UniqueID getID() const                            \
    {                                                                   \
        return getClassID();                                            \
    }                                                                   \
private:

//! Makro ułatwiające "wstrzykiwanie" metod interfejsu IIdentifable do klas implementujących go
#define CLASS_DESCRIPTION(shortName, description)                  \
 public:                                                            \
	inline static const std::string getClassName()                  \
	{                                                               \
		return std::string(shortName);								\
	}                                                               \
																	\
	inline static const std::string getClassDescription()           \
	{                                                               \
		return std::string(description);							\
	}																\
																	\
	virtual const std::string getName() const                       \
	{                                                               \
		return getClassName();                                      \
	}                                                               \
																	\
	virtual const std::string  getDescription() const				\
	{                                                               \
		return getClassDescription();                               \
	}                                                               \
private:

#endif  // __HEADER_GUARD__IIDENTIFIABLE_H__
