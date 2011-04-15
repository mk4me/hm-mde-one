/********************************************************************
    created:  2011/04/14
    created:  14:4:2011   12:38
    filename: TypeInfo.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__TYPEINFO_H__
#define HEADER_GUARD_CORE__TYPEINFO_H__

#include <typeinfo>
#include <boost/any.hpp>

namespace core{

    //! Struktura reprezentuj¹ca typ obiektu.
    class TypeInfo
    {
        //! Wewnêtrzna informacja o typie.
        const std::type_info& typeinfo;

    public:
        //! \param typeinfo
        TypeInfo(const std::type_info& typeinfo) : typeinfo(typeinfo)
        {

        }

        //! \param type
        TypeInfo(const TypeInfo& type) : typeinfo(type.typeinfo)
        {

        }

        //! Jawny operator rzutowania na type_info.
        inline operator const std::type_info&() const
        {
            return typeinfo;
        }

        //! \param obj
        inline bool operator<(const TypeInfo& obj) const
        {
            return typeinfo.before(obj.typeinfo) != 0;
        } 

        //! \param obj
        inline bool operator==(const TypeInfo& obj) const
        {
            return areTypesEqual(typeinfo, obj.typeinfo);
        }

        //! \param rhs
        inline bool operator==(const std::type_info& rhs) const
        {
            return areTypesEqual(typeinfo, rhs);
        }

        //! \param rhs
        inline bool operator!=(const std::type_info& rhs) const
        {
            return !(*this == rhs);
        }

        //! \param obj
        inline bool operator!=(const TypeInfo& obj) const
        {
            return !(*this == obj);
        }

        //! \param rhs
        inline bool before(const std::type_info& rhs) const
        {
            return typeinfo.before(rhs);
        }

        //! \param obj
        inline bool before(const TypeInfo& obj) const
        {
            return before(obj.typeinfo);
        }

        //! \return
        inline const char* name() const
        {
            return typeinfo.name();
        }

        //! \return
        inline size_t hash_code() const
        {
            return typeinfo.hash_code();
        }

        //! \return
        inline const char * raw_name() const
        {
            return typeinfo.raw_name();
        }

    protected:
        static inline bool areTypesEqual(const std::type_info& t1, const std::type_info& t2)
        {
            // tutaj u¿ywamy sta³ej z boost/any - pozwala okreœliæ, czy lepiej porównywaæ po nazwie, czy po adresie
            #ifdef BOOST_AUX_ANY_TYPE_ID_NAME
                        return std::strcmp(t1.name(), t2.name()) == 0;
            #else
                        return t1 == t2;
            #endif
        }

    };
}

#endif  // HEADER_GUARD_CORE__TYPEINFO_H__