#include "CorePCH.h"
#include "TypeInfo.h"
#include <boost/any.hpp>

namespace core{

    //! \param typeinfo
    TypeInfo::TypeInfo(const std::type_info& typeinfo) : typeinfo(typeinfo)
    {

    }

    //! \param type
    TypeInfo::TypeInfo(const TypeInfo& type) : typeinfo(type.typeinfo)
    {

    }

    //! Jawny operator rzutowania na type_info.
    inline TypeInfo::operator const std::type_info&() const
    {
        return typeinfo;
    }

    //! \param obj
    inline bool TypeInfo::operator<(const TypeInfo& obj) const
    {
        return typeinfo.before(obj.typeinfo) != 0;
    } 

    //! \param obj
    inline bool TypeInfo::operator==(const TypeInfo& obj) const
    {
        return areTypesEqual(typeinfo, obj.typeinfo);
    }

    //! \param rhs
    inline bool TypeInfo::operator==(const type_info& rhs) const
    {
        return areTypesEqual(typeinfo, rhs);
    }

    //! \param rhs
    inline bool TypeInfo::operator!=(const type_info& rhs) const
    {
        return !(*this == rhs);
    }

    //! \param obj
    inline bool TypeInfo::operator!=(const TypeInfo& obj) const
    {
        return !(*this == obj);
    }

    //! \param rhs
    inline bool TypeInfo::before(const type_info& rhs) const
    {
        return typeinfo.before(rhs);
    }

    //! \param obj
    inline bool TypeInfo::before(const TypeInfo& obj) const
    {
        return before(obj.typeinfo);
    }

    //! \return
    inline const char* TypeInfo::name() const
    {
        return typeinfo.name();
    }

    //! \return
    inline size_t TypeInfo::hash_code() const
    {
        return typeinfo.hash_code();
    }

    //! \return
    inline const char * TypeInfo::raw_name() const
    {
        return typeinfo.raw_name();
    }

    inline bool TypeInfo::areTypesEqual(const std::type_info& t1, const std::type_info& t2)
    {
        // tutaj u¿ywamy sta³ej z boost/any - pozwala okreœliæ, czy lepiej porównywaæ po nazwie, czy po adresie
        #ifdef BOOST_AUX_ANY_TYPE_ID_NAME
                return std::strcmp(t1.name(), t2.name()) == 0;
        #else
                return t1 == t2;
        #endif
    }
}