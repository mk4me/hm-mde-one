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

////////////////////////////////////////////////////////////////////////////////
namespace utils {
////////////////////////////////////////////////////////////////////////////////

//! Klasa bazowa dla adaptera umoøliwiajπca wykorzystanie zwyk≥ych wskaünikÛw.
//! Dodajπc rozszerzenie dla innych typÛw wskaünikÛw naleøy powieliÊ
//! ca≥y interfejs publiczny.
struct PtrPolicyRaw
{
    //! Typ wskaünika.
    template <class T> struct Ptr 
    {
        typedef T* Type;
    };
    //! Tylko deklaracja, specjalizacja wyciπga wskaünik!
    template <class T> struct Pointed;
    //! Specjalizacja wyciπgajπca wskaünik.
    template <class T> struct Pointed<T*>
    {
        typedef T Type;
    };

    //! Klasa bazowa wymagana dla typÛw poddanych dzia≥aniu wskaünika.
    typedef NullType Base;

    //! Zerowanie wartoúci wskaünika.
    template <class T>
    void initPtr( T *& ptr )
    {
        ptr = NULL;
    }

    //! Ustawienie nowej wartoúci.
    template <class T>
    void setPtr( T *& ptr, T * data)
    {
        if ( ptr != data ) {
            deletePtr(ptr);
            ptr = data;
        }
    }

    //! Czy wskaünik jest unikatowy?
    template <class T>
    bool isUnique( const T * ptr) const
    {
        return false;
    }
};

//! Klasa bazowa dla adaptera umoøliwiajπca wykorzystanie zwyk≥ych wskaünikÛw.
//! Dodajπc rozszerzenie dla innych typÛw wskaünikÛw naleøy powieliÊ
//! ca≥y interfejs publiczny.
struct PtrPolicyRawWeak : public PtrPolicyRaw
{
    //! Ustawienie nowej wartoúci.
    template <class T>
    void setPtr( T *& ptr, T * data)
    {
        ptr = data;
    }
};

////////////////////////////////////////////////////////////////////////////////
} // namespace utils
////////////////////////////////////////////////////////////////////////////////


#endif    // __AVINOUT_POLICIES_H__
