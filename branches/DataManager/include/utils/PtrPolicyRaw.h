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

//! Klasa bazowa dla adaptera umo�liwiaj�ca wykorzystanie zwyk�ych wska�nik�w.
//! Dodaj�c rozszerzenie dla innych typ�w wska�nik�w nale�y powieli�
//! ca�y interfejs publiczny.
struct PtrPolicyRaw
{
    //! Typ wska�nika.
    template <class T> struct Ptr 
    {
        typedef T* Type;
    };
    //! Tylko deklaracja, specjalizacja wyci�ga wska�nik!
    template <class T> struct Pointed;
    //! Specjalizacja wyci�gaj�ca wska�nik.
    template <class T> struct Pointed<T*>
    {
        typedef T Type;
    };

    //! Klasa bazowa wymagana dla typ�w poddanych dzia�aniu wska�nika.
    typedef NullType Base;

    //! Zerowanie warto�ci wska�nika.
    template <class T>
    void initPtr( T *& ptr )
    {
        ptr = NULL;
    }

    //! Ustawienie nowej warto�ci.
    template <class T>
    void setPtr( T *& ptr, T * data)
    {
        if ( ptr != data ) {
            deletePtr(ptr);
            ptr = data;
        }
    }

    //! Czy wska�nik jest unikatowy?
    template <class T>
    bool isUnique( const T * ptr) const
    {
        return false;
    }
};

//! Klasa bazowa dla adaptera umo�liwiaj�ca wykorzystanie zwyk�ych wska�nik�w.
//! Dodaj�c rozszerzenie dla innych typ�w wska�nik�w nale�y powieli�
//! ca�y interfejs publiczny.
struct PtrPolicyRawWeak : public PtrPolicyRaw
{
    //! Ustawienie nowej warto�ci.
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
