/********************************************************************
	created:    2010/04/07
	created:    7:4:2010     18:08
	filename: PolciesBoost.h
	author:	    Piotr Gwiazdowski
	
	purpose:    
*********************************************************************/
#ifndef __AVINOUT_POLCIESBOOST_H__
#define __AVINOUT_POLCIESBOOST_H__

#include <boost/shared_ptr.hpp>
#include <utils/Utils.h>
#include <utils/PtrPolicyHelper.h>

////////////////////////////////////////////////////////////////////////////////
namespace utils {
////////////////////////////////////////////////////////////////////////////////

/**
*	Domy�lna implementacja dla wska�nik�w boost::shared_ptr.
*/
struct PtrPolicyBoost
{
    //! Typ wska�nika.
    template <class T> struct Ptr 
    {
        typedef boost::shared_ptr<T> Type;
    };
    //! Tylko deklaracja, specjalizacja wyci�ga wska�nik!
    template <class T> struct Pointed;
    //! Specjalizacja wyci�gaj�ca wska�nik.
    template <class T> struct Pointed< boost::shared_ptr<T> >
    {
        typedef typename boost::shared_ptr<T>::element_type Type;
    };

    //! Klasa bazowa.
    typedef NullType Base;

    //! Zerowanie wska�nika.
    //! \param ptr
    template <class T>
    void initPtr( T & ptr )
    {}

    //! Ustawienie warto�ci wska�nika surowymi danymi.
    //! \param ptr
    //! \param data
    template<class T>
    void setPtr(boost::shared_ptr<T> & ptr, T * data)
    {
        ptr.reset(data);
    }

    //! Ustawienie warto�ci wska�nika poprzes kopiowanie z innego m�drego wska�nika.
    //! \param ptr
    //! \param data
    template<class T>
    void setPtr(boost::shared_ptr<T> & ptr, const boost::shared_ptr<T> & data)
    {
        ptr = data;
    }

    //! Czy wska�nik jest unikatowy?
    //! \param ptr
    template<class T>
    bool isUnique(boost::shared_ptr<T> & ptr) const
    {
        return ptr.unique();
    }
};

template <> struct is_ptr_policy<PtrPolicyBoost> : public __traits::true_type {};

////////////////////////////////////////////////////////////////////////////////
} // namespace utils
////////////////////////////////////////////////////////////////////////////////


#endif    // __AVINOUT_POLCIESBOOST_H__
