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
*	Domyúlna implementacja dla wskaünikÛw boost::shared_ptr.
*/
struct PtrPolicyBoost
{
    //! Typ wskaünika.
    template <class T> struct Ptr 
    {
        typedef boost::shared_ptr<T> Type;
    };
    //! Tylko deklaracja, specjalizacja wyciπga wskaünik!
    template <class T> struct Pointed;
    //! Specjalizacja wyciπgajπca wskaünik.
    template <class T> struct Pointed< boost::shared_ptr<T> >
    {
        typedef typename boost::shared_ptr<T>::element_type Type;
    };

    //! Klasa bazowa.
    typedef NullType Base;

    //! Zerowanie wskaünika.
    //! \param ptr
    template <class T>
    void initPtr( T & ptr )
    {}

    //! Ustawienie wartoúci wskaünika surowymi danymi.
    //! \param ptr
    //! \param data
    template<class T>
    void setPtr(boost::shared_ptr<T> & ptr, T * data)
    {
        ptr.reset(data);
    }

    //! Ustawienie wartoúci wskaünika poprzes kopiowanie z innego mπdrego wskaünika.
    //! \param ptr
    //! \param data
    template<class T>
    void setPtr(boost::shared_ptr<T> & ptr, const boost::shared_ptr<T> & data)
    {
        ptr = data;
    }

    //! Czy wskaünik jest unikatowy?
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
