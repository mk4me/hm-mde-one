#include <utils/TypeInfo.h>
#include <boost/any.hpp>

const bool areTypesEqual(const std::type_info& t1, const std::type_info& t2)
{
	// tutaj u¿ywamy sta³ej z boost/any - pozwala okreœliæ, czy lepiej porównywaæ po nazwie, czy po adresie
#ifdef BOOST_AUX_ANY_TYPE_ID_NAME
	return std::strcmp(t1.name(), t2.name()) == 0;
#else
	return t1 == t2;
#endif
}

using namespace utils;

TypeInfo::TypeInfo(const std::type_info & ti)
	: typeinfo(&ti)
{

}

TypeInfo::TypeInfo(const TypeInfo& type)
	: typeinfo(type.typeinfo)
{

}

TypeInfo::~TypeInfo()
{

}

TypeInfo& TypeInfo::operator=(const TypeInfo & other)
{
	typeinfo = other.typeinfo;
	return *this;
}

TypeInfo::operator const std::type_info&() const
{
	return *typeinfo;
}

const bool TypeInfo::operator<(const TypeInfo& obj) const
{
	return before(obj);
}

const bool TypeInfo::operator==(const TypeInfo& obj) const
{
	return compare(obj);
}

const bool TypeInfo::operator!=(const TypeInfo& obj) const
{
	return !(*this == obj);
}

const bool TypeInfo::before(const TypeInfo& obj) const
{
	return (*typeinfo).before(obj);
}

const bool TypeInfo::compare(const TypeInfo& obj) const
{
	return areTypesEqual(*typeinfo, *(obj.typeinfo));
}

const char* TypeInfo::name() const
{
	return (*typeinfo).name();
}

const size_t TypeInfo::hash_code() const
{
	return (*typeinfo).hash_code();
}


ExtendedTypeInfo::ExtendedTypeInfo()
	: TypeInfo(), size_(0)
{

}

ExtendedTypeInfo::ExtendedTypeInfo(const ExtendedTypeInfo & eti)
	: TypeInfo(eti), size_(eti.size_)
{

}

ExtendedTypeInfo::ExtendedTypeInfo(const std::type_info & ti, const std::size_t size)
	: TypeInfo(ti), size_(size)
{

}

ExtendedTypeInfo::~ExtendedTypeInfo()
{

}

const std::size_t ExtendedTypeInfo::size() const
{
	return size_;
}

ExtendedTypeInfo& ExtendedTypeInfo::operator =(const ExtendedTypeInfo & other)
{
	TypeInfo::operator=(other);
	size_ = other.size_;
	return *this;
}

const bool ExtendedTypeInfo::compare(const ExtendedTypeInfo& obj) const
{
	return size_ == obj.size_ && TypeInfo::compare(obj);
}