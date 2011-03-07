/********************************************************************
	created:  2010/05/14
	created:  14:5:2010   10:05
	filename: ObserverPattern.h
	author:	  Piotr Gwiazdowski

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD__OBSERVERPATTERN_H__
#define __HEADER_GUARD__OBSERVERPATTERN_H__

#include <utils/Config.h>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <utils/PtrPolicyRaw.h>
#include <utils/Debug.h>

////////////////////////////////////////////////////////////////////////////////
namespace utils {
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

/**
 *	Interfejs obserwatora.
 *  \tparam T Typ do obserwowania.
 */
template <class T>
class Observer
{
public:
    //!
    typedef T ObservedType;
    //!
    typedef Observer<T> Type;

public:
    //! Polimorficzny destruktor.
    virtual ~Observer() {}
    //! Aktualizacja obserwatora.
    //! \param subject Obiekt podany obserwacji.
    virtual void update(const T * subject) = 0;
};

//------------------------------------------------------------------------------

/**
 *  Klasa bazowa dla obiektów obserwowalnych.
 *  \tparam T Typ dziedzicz¹cy po tej klasie.
 */
template <class T>
class Observable
{
public:
    //! Bie¿¹cy typ.
    typedef Observer<T> ObserverType;
    //! Typ wskaŸnika.
    typedef ObserverType * ObserverPtr;
    //typedef typename PtrPolicy::template Ptr<ObserverType>::Type ObserverPtr;

private:
    //! Obserwatorzy.
    std::vector<ObserverPtr> observers;
    //! Obserwowany obiekt.
    const T * self;
    //!
    volatile bool isUpdating;

protected:
    //! Tylko klasa pochodna mo¿e wywo³aæ ten konstruktor.
    Observable() : self(NULL), isUpdating(false) {}
    //! Tylko klasa pochodna mo¿e wywo³aæ ten konstruktor.
    Observable(const T* self) : self(self), isUpdating(false) {}

public:
    //!
    virtual ~Observable() {}

    //! \param observer Obserwator do dodania.
    void attach(ObserverPtr observer)
    {
        if ( std::find(observers.begin(), observers.end(), observer) == observers.end() ) {
            observers.push_back(observer);
        } else {
            throw std::invalid_argument("Visitor already attached.");
        }
    }

    //! \param observer Obserwator do usuniêcia.
    void detach(ObserverPtr observer)
    {
        typename std::vector<ObserverPtr>::iterator last = std::remove( observers.begin(), observers.end(), observer );
        if ( last != observers.end() ) {
            observers.resize( std::distance(observers.begin(), last) );
        } else {
            throw std::invalid_argument("Visitor not attached.");
        }
    }

    //! Czy obserwator jest podpiêty?
    //! \param observer Obserwator.
    //! \return
    bool isAttached(ObserverPtr observer)
    {
        return ( std::find(observers.begin(), observers.end(), observer) != observers.end() );
    }

    //! Usuwa wszystkich podpiêtych obserwatorów.
    void detachAll()
    {
        observers.clear();
    }

    //! Aktualizuje wszystkie wyniki.
    virtual void notify()
    {
        if ( !self ) {
            self = dynamic_cast<const T*>(this);
            UTILS_ASSERT(self, "Error casting to desired type.");
        }
        if ( !self ) {
            throw std::runtime_error("Could not deduce self-pointer");
        }

        if (isUpdating) {
            UTILS_ASSERT(false, "Can't update during notify.");
        }

        isUpdating = true;
        for ( typename std::vector<ObserverPtr>::iterator it = observers.begin(); it != observers.end(); ++ it) {
            (*it)->update(self);
        }
        isUpdating = false;
    }
};

//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
} // namespace utils
////////////////////////////////////////////////////////////////////////////////

#endif    // __HEADER_GUARD__OBSERVERPATTERN_H__
