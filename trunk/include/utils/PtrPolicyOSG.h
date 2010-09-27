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

////////////////////////////////////////////////////////////////////////////////
namespace utils {
////////////////////////////////////////////////////////////////////////////////

/**
*	Domy�lna implementacja dla wska�nik�w osg::ref_ptr.
*/
struct PtrPolicyOSG
{
    //! Typ wska�nika.
    template <class T> struct Ptr 
    {
        typedef osg::ref_ptr<T> Type;
    };
    //! Tylko deklaracja, specjalizacja wyci�ga wska�nik!
    template <class T> struct Pointed;
    //! Specjalizacja wyci�gaj�ca wska�nik.
    template <class T> struct Pointed<osg::ref_ptr<T> >
    {
        typedef T Type;
    };

    //! Klasa bazowa.
    typedef osg::Referenced Base;

    //! Inicjalizacja warto�ci.
    //! \param ptr
    template <class T>
    void initPtr( T & ptr )
    {}

    //! Implemetnacja resetowania wska�nika dla osg::ref_ptr.
    //! \param ptr
    //! \param data
    template<class T>
    void setPtr(osg::ref_ptr<T> & ptr, T * data)
    {
        ptr = data;
    }

    //! Implemetnacja resetowania wska�nika dla osg::ref_ptr.
    //! \param ptr
    //! \param data
    template<class T>
    void setPtr(osg::ref_ptr<T> & ptr, osg::ref_ptr<T> & data)
    {
        ptr = data;
    }

    //! Implementacja unikalno�ci wska�nika dla osg::ref_ptr.
    //! \param ptr
    template<class T>
    bool isUnique(osg::ref_ptr<T> & ptr) const
    {
        return ptr->referenceCount() == 0;
    }
};

////////////////////////////////////////////////////////////////////////////////
} // namespace utils
////////////////////////////////////////////////////////////////////////////////

#endif    // __AVINOUT_POLICIESOSG_H__