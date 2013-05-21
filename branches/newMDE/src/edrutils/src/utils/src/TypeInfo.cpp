#include <utils/TypeInfo.h>
#include <boost/any.hpp>

using namespace utils;

//! \param typeinfo
TypeInfo::TypeInfo() : typeinfo(&typeid(void))
{

}

//! \param typeinfo
TypeInfo::TypeInfo(const std::type_info& typeinfo) : typeinfo(&typeinfo)
{

}

//! \param type
TypeInfo::TypeInfo(const TypeInfo& type) : typeinfo(type.typeinfo)
{

}

//! Jawny operator rzutowania na type_info.
TypeInfo& TypeInfo::operator=(const TypeInfo & other)
{
	typeinfo = other.typeinfo;
	return *this;
}

//! Jawny operator rzutowania na type_info.
TypeInfo::operator const std::type_info&() const
{
	return *typeinfo;
}

//! \param obj
const bool TypeInfo::operator<(const TypeInfo& obj) const
{
	return (*typeinfo).before(*(obj.typeinfo)) != 0;
}

//! \param obj
const bool TypeInfo::operator==(const TypeInfo& obj) const
{
	return areTypesEqual(*typeinfo, *(obj.typeinfo));
}

//! \param rhs
const bool TypeInfo::operator==(const std::type_info& rhs) const
{
	return areTypesEqual(*typeinfo, rhs);
}

//! \param rhs
const bool TypeInfo::operator!=(const std::type_info& rhs) const
{
	return !(*this == rhs);
}

//! \param obj
const bool TypeInfo::operator!=(const TypeInfo& obj) const
{
	return !(*this == obj);
}

//! \param rhs
const bool TypeInfo::before(const std::type_info& rhs) const
{
	return (*typeinfo).before(rhs) > 0;
}

//! \param obj
const bool TypeInfo::before(const TypeInfo& obj) const
{
	return before(*(obj.typeinfo));
}

//! \return
const char* TypeInfo::name() const
{
	return (*typeinfo).name();
}

//! \return
const size_t TypeInfo::hash_code() const
{
	return (*typeinfo).hash_code();
}

const bool TypeInfo::areTypesEqual(const std::type_info& t1, const std::type_info& t2)
{
	// tutaj u¿ywamy sta³ej z boost/any - pozwala okreœliæ, czy lepiej porównywaæ po nazwie, czy po adresie
#ifdef BOOST_AUX_ANY_TYPE_ID_NAME
	return std::strcmp(t1.name(), t2.name()) == 0;
#else
	return t1 == t2;
#endif
}