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
*	Domyślna implementacja dla wskaźników boost::shared_ptr.
*/
struct PtrPolicyBoost
{
    //! Typ wskaźnika.
    template <class T> struct Ptr 
    {
        typedef boost::shared_ptr<T> Type;
    };
    //! Tylko deklaracja, specjalizacja wyciąga wskaźnik!
    template <class T> struct Pointed;
    //! Specjalizacja wyciągająca wskaźnik.
    template <class T> struct Pointed< boost::shared_ptr<T> >
    {
        typedef typename boost::shared_ptr<T>::element_type Type;
    };

    //! Klasa bazowa.
    typedef NullType Base;

    //! Zerowanie wskaźnika.
    //! \param ptr
    template <class T>
    static void initPtr( T & ptr )
    {}

    //! Ustawienie wartości wskaźnika surowymi danymi.
    //! \param ptr
    //! \param data
    template<class T>
    static void setPtr(boost::shared_ptr<T> & ptr, T * data)
    {
        ptr.reset(data);
    }

    //! Ustawienie wartości wskaźnika poprzes kopiowanie z innego mądrego wskaźnika.
    //! \param ptr
    //! \param data
    template<class T>
    static void setPtr(boost::shared_ptr<T> & ptr, const boost::shared_ptr<T> & data)
    {
        ptr = data;
    }

	template<class T>
	static void swapPtr(boost::shared_ptr<T> & ptr, boost::shared_ptr<T> & data)
	{
		std::swap(ptr, data);
	}

    //! Zwraca surowy wskaźnik
    //! \param ptr
    template<class T>
    static void* getRawPtr(const boost::shared_ptr<T> & ptr)
    {
        return ptr.get();
    }

    //! Zwraca surowy wskaźnik
    //! \param ptr
    template<class T>
    static const void* getConstRawPtr(const boost::shared_ptr<T> & ptr)
    {
        return ptr.get();
    }

    //! Czy wskaźnik jest unikatowy?
    //! \param ptr
    template<class T>
    static const bool isUnique(boost::shared_ptr<T> & ptr)
    {
        return ptr.unique();
    }
};

template <> struct is_ptr_policy<PtrPolicyBoost> : public __traits::true_type {};

////////////////////////////////////////////////////////////////////////////////
} // namespace utils
////////////////////////////////////////////////////////////////////////////////


#endif    // __AVINOUT_POLCIESBOOST_H__