/********************************************************************
	created:  2010/09/28
	created:  28:9:2010   1:22
	filename: IIdentifiable.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__IIDENTIFIABLE_H__
#define __HEADER_GUARD__IIDENTIFIABLE_H__

#include <corelib/BaseDataTypes.h>

namespace plugin {

//! Interfejs reprezentujący obiekt możliwy do identyfikacji.
class IIdentifiable
{
public:
	//! Destruktor wirtualny
	virtual ~IIdentifiable() {}
    //! \return Unikalny identyfikator obiektu
    virtual const core::UniqueID ID() const = 0;
};

//! Interfejs dostarczający opisu
class IDescription
{
public:
	//! Destruktor wirtualny
	virtual ~IDescription() {}
	//! \return Nazwa
	virtual const std::string name() const = 0;
	//! \return Nazwa skrócona
	virtual const std::string shortName() const = 0;
	//! \return Opis
	virtual const std::string  description() const = 0;
};

class Description : public IDescription
{
public:
	//! Konstruktor
	Description(const std::string & name = std::string(),
		const std::string & shortName = std::string(),
		const std::string & description = std::string())
		: name_(name), shortName_(shortName),
		description_(description)
	{

	}

	void setName(const std::string & name) { name_ = name; }
	void setShortName(const std::string & shortName) { shortName_ = shortName; }
	void setDescription(const std::string & description) { description_ = description; }

	//! Destruktor wirtualny
	virtual ~Description() {}
	//! \return Nazwa
	virtual const std::string name() const { return name_; }
	//! \return Nazwa skrócona
	virtual const std::string shortName() const { return shortName_; }
	//! \return Opis
	virtual const std::string  description() const { return description_; }

private:
	std::string name_;
	std::string shortName_;
	std::string description_;
};

//! Interfejs dostarczający opisu dostawcy produktu
class IVendorDescription : public IDescription
{
public:
	//! Destruktor wirtualny
	virtual ~IVendorDescription() {}
	//! \return Kontakt
	virtual const std::string contact() const = 0;	
};

class VendorDescription : public IVendorDescription
{
public:
	//! Konstruktor
	VendorDescription(const std::string & name = std::string(),
		const std::string & shortName = std::string(),
		const std::string & description = std::string(),
		const std::string & contact = std::string())
		: desc(name, shortName, description),
		contact_(contact)
	{

	}

	//! Destruktor wirtualny
	virtual ~VendorDescription() {}
	//! \return Nazwa
	virtual const std::string name() const { return desc.name(); }
	//! \return Nazwa skrócona
	virtual const std::string shortName() const { return desc.shortName(); }
	//! \return Opis
	virtual const std::string  description() const { return desc.description(); }
	//! \return Kontakt
	virtual const std::string contact() const { return contact_; }

	void setName(const std::string & name) { desc.setName(name); }
	void setShortName(const std::string & shortName) { desc.setShortName(shortName); }
	void setDescription(const std::string & description) { desc.setDescription(description); }
	void setContact(const std::string & contact) { contact_ = contact; }

private:
	Description desc;
	std::string contact_;
};

//! Interfejs podstawowych elemntów aplikacji
class ICoreElement : public IIdentifiable, public IDescription
{
public:
	//! Destruktor wirtualny
	virtual ~ICoreElement() {}
	//! \return Instancja danego elementu
	virtual ICoreElement * create() const = 0;
};

}

//! Makro ułatwiające "wstrzykiwanie" metod interfejsu IIdentifable do klas implementujących go
#define UNIQUE_ID(uuidStr)												\
 public:                                                                \
    inline static const core::UniqueID getClassID()						\
    {                                                                   \
		static const auto cid = core::UID::GenerateUniqueID(uuidStr);	\
		return cid;														\
    }                                                                   \
                                                                        \
    virtual const core::UniqueID ID() const									\
    {                                                                   \
        return getClassID();                                            \
    }                                                                   \
private:

//! Makro ułatwiające "wstrzykiwanie" metod interfejsu IIdentifable do klas implementujących go
#define CLASS_DESCRIPTION(name_, description_)						\
 public:                                                            \
	inline static const std::string getClassName()                  \
	{                                                               \
		return std::string(name_);									\
	}                                                               \
																	\
	inline static const std::string getClassDescription()           \
	{                                                               \
		return std::string(description_);							\
	}																\
																	\
	virtual const std::string name() const							\
	{                                                               \
		return getClassName();                                      \
	}                                                               \
																	\
	virtual const std::string shortName() const						\
	{                                                               \
		return getClassName();                                      \
	}                                                               \
																	\
	virtual const std::string description() const					\
	{                                                               \
		return getClassDescription();                               \
	}                                                               \
private:

//! Makro ułatwiające "wstrzykiwanie" metod interfejsu IIdentifable do klas implementujących go
#define CLASS_DESCRIPTION_EXT(name, shortName, description)			\
	public:															\
	inline static const std::string getClassName()					\
	{																\
		return std::string(name);									\
	}																\
																	\
	inline static const std::string getClassDescription()           \
	{																\
		return std::string(description);							\
	}																\
																	\
	virtual const std::string name() const							\
	{																\
		return getClassName();										\
	}																\
																	\
	virtual const std::string shortName() const						\
	{																\
		return std::string(shortName);								\
	}																\
																	\
	virtual const std::string  description() const					\
	{																\
		return getClassDescription();								\
	}																\
	private:

#endif  // __HEADER_GUARD__IIDENTIFIABLE_H__
