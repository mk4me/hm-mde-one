/********************************************************************
	created:    2010/04/07
	created:    7:4:2010     18:06
	filename: PoliciesOSG.h
	author:	    Piotr Gwiazdowski
	
	purpose:    
*********************************************************************/
#ifndef __AVINOUT_POLICIESOSG_H__
#define __AVINOUT_POLICIESOSG_H__

#include <osg/ref_ptr>
#include <osg/Referenced>
#include <utils/PtrPolicyHelper.h>

////////////////////////////////////////////////////////////////////////////////
namespace utils {
////////////////////////////////////////////////////////////////////////////////

/**
*	Domyślna implementacja dla wskaźników osg::ref_ptr.
*/
struct PtrPolicyOSG
{
    //! Typ wskaźnika.
    template <class T> struct Ptr 
    {
        typedef osg::ref_ptr<T> Type;
    };
    //! Tylko deklaracja, specjalizacja wyciąga wskaźnik!
    template <class T> struct Pointed;
    //! Specjalizacja wyciągająca wskaźnik.
    template <class T> struct Pointed<osg::ref_ptr<T> >
    {
        typedef T Type;
    };

    //! Klasa bazowa.
    typedef osg::Referenced Base;

    //! Inicjalizacja wartości.
    //! \param ptr
    template <class T>
    static void initPtr( T & ptr )
    {}

    //! Implemetnacja resetowania wskaźnika dla osg::ref_ptr.
    //! \param ptr
    //! \param data
    template<class T>
    static void setPtr(osg::ref_ptr<T> & ptr, T * data)
    {
        ptr = data;
    }

    //! Implemetnacja resetowania wskaźnika dla osg::ref_ptr.
    //! \param ptr
    //! \param data
    template<class T>
    static void setPtr(osg::ref_ptr<T> & ptr, const osg::ref_ptr<T> & data)
    {
        ptr = data;
    }

	template<class T>
	static void swapPtr(osg::ref_ptr<T> & ptr, osg::ref_ptr<T> & data)
	{
		std::swap(ptr, data);
	}

    //! Zwraca surowy wskaźnik
    //! \param ptr
    template<class T>
    static void* getRawPtr(const osg::ref_ptr<T> & ptr)
    {
        return ptr.get();
    }

    //! Zwraca surowy wskaźnik
    //! \param ptr
    template<class T>
    static const void* getConstRawPtr(const osg::ref_ptr<T> & ptr)
    {
        return ptr.get();
    }

    //! Implementacja unikalności wskaźnika dla osg::ref_ptr.
    //! \param ptr
    template<class T>
    static const bool isUnique(const osg::ref_ptr<T> & ptr)
    {
        return ptr->referenceCount() == 0;
    }
};

template <> struct is_ptr_policy<PtrPolicyOSG> : public __traits::true_type {};

////////////////////////////////////////////////////////////////////////////////
} // namespace utils
////////////////////////////////////////////////////////////////////////////////

#endif    // __AVINOUT_POLICIESOSG_H__
