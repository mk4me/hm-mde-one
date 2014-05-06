/********************************************************************
	created:    2010/04/07
	created:    7:4:2010     17:59
	filename: VideoInputAdapterPolicies.h
	author:	    Piotr Gwiazdowski

	purpose:
*********************************************************************/
#ifndef __AVINOUT_POLICIES_H__
#define __AVINOUT_POLICIES_H__

#include <utils/Utils.h>
#include <utils/PtrPolicyHelper.h>

////////////////////////////////////////////////////////////////////////////////
namespace utils {
////////////////////////////////////////////////////////////////////////////////

//! Klasa bazowa dla adaptera umożliwiająca wykorzystanie zwykłych wskaźników.
//! Dodając rozszerzenie dla innych typów wskaźników należy powielić
//! cały interfejs publiczny.
struct PtrPolicyRaw
{
    //! Typ wskaźnika.
	template <typename T> struct Ptr
    {
        typedef T* Type;
    };
    //! Tylko deklaracja, specjalizacja wyciąga wskaźnik!
	template <typename T> struct Pointed;
    //! Specjalizacja wyciągająca wskaźnik.
	template <typename T> struct Pointed<T*>
    {
        typedef T Type;
    };

    //! Klasa bazowa wymagana dla typów poddanych działaniu wskaźnika.
    typedef NullType Base;

    //! Zerowanie wartości wskaźnika.
	template <typename T>
    static void initPtr( T *& ptr )
    {
        ptr = nullptr;
    }

    //! Ustawienie nowej wartości.
	template <typename T, typename Y>
    static void setPtr( T *& ptr, Y * data)
    {
        if ( ptr != data ) {
            deletePtr(ptr);
            ptr = data;
        }
    }

	//! Ustawienie nowej wartości.
	template <typename T>
	static void swapPtr( T *& ptr, T *& data)
	{
		std::swap(ptr, data);
	}

	template<typename T, typename Y>
	static const Y* dynamicCastPtr(const T* ptr)
	{
		return dynamic_cast<Y*>(ptr);
	}

	template<typename T, typename Y>
	static const Y* constCastPtr(const T* ptr)
	{
		return const_cast<Y*>(ptr);
	}

    //! Zwraca surowy wskaźnik
    //! \param ptr
	template<typename T>
    static void* getRawPtr(T* ptr)
    {
        return ptr;
    }

    //! Zwraca surowy wskaźnik
    //! \param ptr
	template<typename T>
    static const void* getConstRawPtr(const T * ptr)
    {
        return ptr;
    }

	//! Zwraca surowy wskaźnik
	//! \param ptr
	template<typename T>
	static T* getPtr(T* ptr)
	{
		return ptr;
	}

	//! Zwraca surowy wskaźnik
	//! \param ptr
	template<typename T>
	static const T* getConstPtr(const T * ptr)
	{
		return ptr;
	}

    //! Czy wskaźnik jest unikatowy?
	template <typename T>
    static const bool isUnique( const T * ptr)
    {
        return false;
    }

	//! \return Ilość referencji do tego wskaźnika
	template<typename T>
	static const long referenceCount(const T * ptr)
	{
		return -1;
	}
};

template <> struct is_ptr_policy<PtrPolicyRaw> : public __traits::true_type {};

//! Klasa bazowa dla adaptera umożliwiająca wykorzystanie zwykłych wskaźników.
//! Dodając rozszerzenie dla innych typów wskaźników należy powielić
//! cały interfejs publiczny.
struct PtrPolicyRawWeak : public PtrPolicyRaw
{
    //! Ustawienie nowej wartości.
	template <typename T>
    static void setPtr( T *& ptr, T * data)
    {
        ptr = data;
    }
};

template <> struct is_ptr_policy<PtrPolicyRawWeak> : public __traits::true_type {};

////////////////////////////////////////////////////////////////////////////////
} // namespace utils
////////////////////////////////////////////////////////////////////////////////


#endif    // __AVINOUT_POLICIES_H__
