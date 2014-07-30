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
#include <vector>
#include <boost/type_traits.hpp>

namespace utils {

    //! Struktura reprezentująca typ obiektu.
    class TypeInfo
    {
    private:
        //! Wewnętrzna informacja o typie.
         const std::type_info* typeinfo;

    public:

        //! Konstruktor
		//! \param ti Informacja o typie
		TypeInfo(const std::type_info & ti = typeid(void));

        //! \param type Kopiowany obiekt
        TypeInfo(const TypeInfo& type);

		//! Destruktor niewirtualny
		virtual ~TypeInfo();

        //! Jawny operator rzutowania na type_info.
        TypeInfo& operator =(const TypeInfo & other);

        //! Jawny operator rzutowania na type_info.
        operator const std::type_info&() const;

		//! \param obj Obiekt porównywany
		//! \return Czy nasz typ jest przed obj
        const bool operator<(const TypeInfo& obj) const;

		//! \param obj Obiekt porównywany
		//! \return Czy nasz typ równa się obj
        const bool operator==(const TypeInfo& obj) const;

        //! \param obj Obiekt porównywany
		//! \return Czy nasz typ różni się od obj
        const bool operator!=(const TypeInfo& obj) const;

        //! \param obj Obiekt porównywany
		//! \return Czy nasz obiekt jest przed obj
        virtual const bool before(const TypeInfo& obj) const;

		//! \param obj Obiekt porównywany
		//! \return Czy typy są identyczne
		virtual const bool compare(const TypeInfo& obj) const;

        //! \return Nazwa typu
        const char* name() const;

        //! \return Skrót informacji o typie
        const size_t hash_code() const;
    };

	class ExtendedTypeInfo : public TypeInfo
	{
	private:
		//! Rozmiar typu [bytes]
		std::size_t size_;

	private:

		//! \tpara T Typ dla którego chcemy pobrać rozmiar
		//! Wersja kiedy T = void
		template<typename T>
		static const unsigned long long typeSize(boost::true_type)
		{
			return 0;
		}

		//! \tpara T Typ dla którego chcemy pobrać rozmiar
		//! Wersja kiedy T != void
		template<typename T>
		static const unsigned long long typeSize(boost::false_type)
		{
			return sizeof(T);
		}

		//! Konstruktor prywatny
		//! \param ti Informacje o typie
		//! \param size Rozmiar typu
		ExtendedTypeInfo(const std::type_info & ti, const std::size_t size);

	public:

		//! Domyślny konstruktor
		ExtendedTypeInfo();

		//! Konstruktor
		//! \param eit Rozszerzona informacja o typie
		explicit ExtendedTypeInfo(const ExtendedTypeInfo & eti);

		//! Konstruktor
		//! \tparam T Typ obiektu
		template<typename T>
		const ExtendedTypeInfo info(const T * dummy = nullptr)
		{
			return ExtendedTypeInfo(typeid(T), typeSize<T>(boost::is_void<T>::type));
		}

		//! Destruktor
		virtual ~ExtendedTypeInfo();

		//! Jawny operator rzutowania na type_info.
		ExtendedTypeInfo& operator =(const ExtendedTypeInfo & other);

		//! \param obj Obiekt porównywany
		//! \return Czy typy są identyczne
		virtual const bool compare(const ExtendedTypeInfo& obj) const;

		//! \return Rozmiar zajmowany przez ten typ
		const std::size_t size() const;
	};

    typedef std::list<TypeInfo> TypeInfoList;
    typedef std::set<TypeInfo> TypeInfoSet;
	typedef std::vector<TypeInfo> TypeInfoVector;
}

#endif  // HEADER_GUARD_UTILS__TYPEINFO_H__
