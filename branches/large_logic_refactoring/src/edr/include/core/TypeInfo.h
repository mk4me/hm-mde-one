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
#include <stdlib.h>
//#include <boost/any.hpp>
#include <list>
#include <set>

namespace core {

    //! Struktura reprezentująca typ obiektu.
    class TypeInfo
    {
    private:
        //! Wewnętrzna informacja o typie.
         const std::type_info* typeinfo;

    public:
        //! \param typeinfo
        TypeInfo() : typeinfo(&typeid(TypeInfo))
        {

        }

        //! \param typeinfo
        TypeInfo(const std::type_info& typeinfo) : typeinfo(&typeinfo)
        {

        }

        //! \param type
        TypeInfo(const TypeInfo& type) : typeinfo(type.typeinfo)
        {

        }

        //! Jawny operator rzutowania na type_info.
        inline TypeInfo& operator =(const TypeInfo & other)
        {
            typeinfo = other.typeinfo;
            return *this;
        }

        //! Jawny operator rzutowania na type_info.
        inline operator const std::type_info&() const
        {
            return *typeinfo;
        }

        //! \param obj
        inline bool operator<(const TypeInfo& obj) const
        {
            return (*typeinfo).before(*(obj.typeinfo)) != 0;
        }

        //! \param obj
        inline bool operator==(const TypeInfo& obj) const
        {
            return areTypesEqual(*typeinfo, *(obj.typeinfo));
        }

        //! \param rhs
        inline bool operator==(const std::type_info& rhs) const
        {
            return areTypesEqual(*typeinfo, rhs);
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
            return (*typeinfo).before(rhs) > 0;
        }

        //! \param obj
        inline bool before(const TypeInfo& obj) const
        {
            return before(*(obj.typeinfo));
        }

        //! \return
        inline const char* name() const
        {
            return (*typeinfo).name();
        }

        //! \return
        inline size_t hash_code() const
        {
            return (*typeinfo).hash_code();
        }

        //rev - niezgodne ze standardem
        //! \return
        //inline const char * raw_name() const
        //{
        //    return (*typeinfo).raw_name();
        //}

    protected:
        static inline bool areTypesEqual(const std::type_info& t1, const std::type_info& t2)
        {
            // tutaj używamy stałej z boost/any - pozwala określić, czy lepiej porównywać po nazwie, czy po adresie
            #ifdef BOOST_AUX_ANY_TYPE_ID_NAME
                        return std::strcmp(t1.name(), t2.name()) == 0;
            #else
                        return t1 == t2;
            #endif
        }

    };

    typedef std::list<TypeInfo> TypeInfoList;
    typedef std::set<TypeInfo> TypeInfoSet;
}

#endif  // HEADER_GUARD_CORE__TYPEINFO_H__
