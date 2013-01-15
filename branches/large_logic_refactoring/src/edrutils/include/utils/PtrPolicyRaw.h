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
    template <class T> struct Ptr 
    {
        typedef T* Type;
    };
    //! Tylko deklaracja, specjalizacja wyciąga wskaźnik!
    template <class T> struct Pointed;
    //! Specjalizacja wyciągająca wskaźnik.
    template <class T> struct Pointed<T*>
    {
        typedef T Type;
    };

    //! Klasa bazowa wymagana dla typów poddanych działaniu wskaźnika.
    typedef NullType Base;

    //! Zerowanie wartości wskaźnika.
    template <class T>
    void initPtr( T *& ptr )
    {
        ptr = nullptr;
    }

    //! Ustawienie nowej wartości.
    template <class T>
    void setPtr( T *& ptr, T * data)
    {
        if ( ptr != data ) {
            deletePtr(ptr);
            ptr = data;
        }
    }

    //! Zwraca surowy wskaźnik
    //! \param ptr
    template<class T>
    void* getRawPtr(T* ptr)
    {
        return ptr;
    }

    //! Zwraca surowy wskaźnik
    //! \param ptr
    template<class T>
    const void* getRawPtr(const T * ptr) const
    {
        return ptr;
    }

    //! Czy wskaźnik jest unikatowy?
    template <class T>
    bool isUnique( const T * ptr) const
    {
        return false;
    }
};

template <> struct is_ptr_policy<PtrPolicyRaw> : public __traits::true_type {};

//! Klasa bazowa dla adaptera umożliwiająca wykorzystanie zwykłych wskaźników.
//! Dodając rozszerzenie dla innych typów wskaźników należy powielić
//! cały interfejs publiczny.
struct PtrPolicyRawWeak : public PtrPolicyRaw
{
    //! Ustawienie nowej wartości.
    template <class T>
    void setPtr( T *& ptr, T * data)
    {
        ptr = data;
    }
};

template <> struct is_ptr_policy<PtrPolicyRawWeak> : public __traits::true_type {};

////////////////////////////////////////////////////////////////////////////////
} // namespace utils
////////////////////////////////////////////////////////////////////////////////


#endif    // __AVINOUT_POLICIES_H__
