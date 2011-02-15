/********************************************************************
	created:  2010/11/22
	created:  22:11:2010   19:19
	filename: Placeable.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__PLACEABLE_H__
#define __HEADER_GUARD__PLACEABLE_H__

#include <string>
#include <stdexcept>
#include <osgWidget/Widget>
#include <utils/Debug.h>
#include <boost/type_traits.hpp>


////////////////////////////////////////////////////////////////////////////////
namespace osgUI {
////////////////////////////////////////////////////////////////////////////////

//! Wyj¹tek do rzucania przez je¿eli weakptr wygas³.
// TODO: przenieœæ do kodu bibliotecznego
class WeakPtrExpiredException : public std::runtime_error
{
public:
    WeakPtrExpiredException(const std::string& msg = "") : std::runtime_error(msg)
    {}
};

//! Funkcja do konwersji osg::observer_ptr -> osg::ref_ptr
// TODO: przenieœæ do kodu bibliotecznego
template<class T> osg::ref_ptr<T> observer_ptr_to_ref_ptr( osg::observer_ptr<T> & wpt )
{
    osg::ref_ptr<T> result;
    if ( wpt.lock(result) ) {
        return result;
    }  else {
        throw WeakPtrExpiredException();
    }
} 

//! Funkcja do konwersji boost::weak_ptr -> boost::shared_ptr
// TODO: przenieœæ do kodu bibliotecznego
template<class T> boost::shared_ptr<T> weak_ptr_to_shared_ptr( boost::weak_ptr<T> & wpt )
{
    if ( boost::shared_ptr<T> result = wpt.lock() ) {
        return result;
    }  else {
        throw WeakPtrExpiredException();
    }
} 

//! Podstawa callbacka.
// TODO: przenieœæ do kodu bibliotecznego
struct OsgCallbackBase
{
protected:
    //! WskaŸnik obserwator wskazujacy wa¿noœæ callbacka.
    osg::observer_ptr<osg::Referenced> host;

public:
    //! \param host Obiekt do którego kierowany jest callback.
    OsgCallbackBase(osg::Referenced* host) : host(host) 
    {}
    //! \param callback Wzór.
    OsgCallbackBase(const OsgCallbackBase& callback) : host(callback.host) 
    {}
    //! Destruktor polimorficzny (coœ musi zwalniaæ wskaŸnik host)
    virtual ~OsgCallbackBase() 
    {}

private:
    // Idiom "safe bool", bezpieczniejszy od tradycyjnej konwersji
    //   http://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Safe_bool
    //   http://lists.boost.org/Archives/boost/2003/09/52856.php
    void dummy_bool() const {}

public:
    typedef void (OsgCallbackBase::*unspecified_bool_type)() const;
    //! \return Safe bool
    operator unspecified_bool_type() const 
    { 
        return host.valid()? &OsgCallbackBase::dummy_bool : 0; 
    }
};


/**
 *	Rozszerzenie widgetu o wywo³ywanie callbacka gdy nada siê mu pozycjê.
 */
template <class Base, class PlacementCallback>
class Placeable : public Base
{
    UTILS_STATIC_ASSERT((boost::is_base_of<osgWidget::Widget, Base>::value), "Base class should inherit from osgWidget::Widget");
private:
    //! Obiekt do umieszczenia.
    PlacementCallback callback;

public:
    //! Konstruktor domyœlny (dla osgWidget).
    //! \param name
    Placeable(const std::string& name = "") : Base(name), callback()
    {}
    //! Konstruktor kopiuj¹cy.
    //! \param placeable
    //! \param copyop
    Placeable(const Placeable& placeable, const osg::CopyOp& copyop) :
        Base(placeable, copyop), callback(placeable.callback)
    {}

    //! \return Callback.
    PlacementCallback& getPlacementCallback()
    { 
        return callback;
    }
    //! \return Callback.
    const PlacementCallback& getPlacementCallback() const
    { 
        return callback;
    }
    //! \param callback Callback.
    void setPlacementCallback(PlacementCallback callback) 
    { 
        this->callback = callback; 
    }

    //! Pozycjonuje wierzcho³ki.
    virtual void positioned()
    {
        Base::positioned();
        if ( callback ) {
            try {
                // wywo³anie callbacka
                callback( this, getX(), getY(), getWidth(), getHeight() );
            } catch ( WeakPtrExpiredException& ) {
                // wyzerowanie callbacka
                callback = PlacementCallback();
            }
        }
    }
};

////////////////////////////////////////////////////////////////////////////////
} // namespace osgUI
////////////////////////////////////////////////////////////////////////////////


#endif  // __HEADER_GUARD__PLACEABLE_H__