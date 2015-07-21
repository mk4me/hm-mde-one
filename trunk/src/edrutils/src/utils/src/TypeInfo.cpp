#include <utils/TypeInfo.h>
#include <typeindex>

static inline const bool areTypesEqual(const std::type_info& t1, const std::type_info& t2)
{
	//TODO - czy to konieczne, czy da siê to robiæ bezpiecznie bez typeindex?
	return std::type_index(t1) == std::type_index(t2);
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
	return (size_ == obj.size_) && (TypeInfo::compare(obj) == true);
}