/********************************************************************
    created:  2011/04/14
    created:  14:4:2011   12:38
    filename: TypeInfo.h
    author:   Mateusz Janiak

    purpose:
*********************************************************************/
#ifndef HEADER_GUARD_UTILS__TYPEINFO_H__
#define HEADER_GUARD_UTILS__TYPEINFO_H__

#include <typeinfo>
#include <stdlib.h>
#include <list>
#include <set>

namespace utils {

    //! Struktura reprezentująca typ obiektu.
    class TypeInfo
    {
    private:
        //! Wewnętrzna informacja o typie.
         const std::type_info* typeinfo;

    public:
       //! \param typeinfo
       TypeInfo();

        //! \param typeinfo
        TypeInfo(const std::type_info& typeinfo);

        //! \param type
        TypeInfo(const TypeInfo& type);

        //! Jawny operator rzutowania na type_info.
        TypeInfo& operator =(const TypeInfo & other);

        //! Jawny operator rzutowania na type_info.
        operator const std::type_info&() const;

        //! \param obj
        const bool operator<(const TypeInfo& obj) const;

        //! \param obj
        const bool operator==(const TypeInfo& obj) const;

        //! \param rhs
        const bool operator==(const std::type_info& rhs) const;

        //! \param rhs
        const bool operator!=(const std::type_info& rhs) const;

        //! \param obj
        const bool operator!=(const TypeInfo& obj) const;

        //! \param rhs
        const bool before(const std::type_info& rhs) const;

        //! \param obj
        const bool before(const TypeInfo& obj) const;

        //! \return
        const char* name() const;

        //! \return
        const size_t hash_code() const;

    protected:
        static const bool areTypesEqual(const std::type_info& t1, const std::type_info& t2);

    };

    typedef std::list<TypeInfo> TypeInfoList;
    typedef std::set<TypeInfo> TypeInfoSet;
}

#endif  // HEADER_GUARD_UTILS__TYPEINFO_H__
