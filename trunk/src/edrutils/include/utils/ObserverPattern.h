/********************************************************************
	created:  2010/05/14
	created:  14:5:2010   10:05
	filename: ObserverPattern.h
	author:	  Piotr Gwiazdowski

	purpose:
*********************************************************************/
#ifndef HEADER_GUARD__OBSERVERPATTERN_H__
#define HEADER_GUARD__OBSERVERPATTERN_H__

#include <utils/Config.h>
#include <stdexcept>
#include <set>
#include <algorithm>
#include <utils/PtrPolicyRaw.h>
#include <utils/Debug.h>

////////////////////////////////////////////////////////////////////////////////
namespace utils {
////////////////////////////////////////////////////////////////////////////////

template <class T>
class Observable;

//------------------------------------------------------------------------------

/**
 *	Interfejs obserwatora.
 *  \tparam T Typ do obserwowania.
 */
template <class T>
class Observer
{
    friend class Observable<T>;

public:
    //!
    typedef T ObservedType;
    //!
    typedef Observer<T> ObserverType;

    typedef Observable<T> ObservableType;

    typedef ObservableType* ObservablePtr;

    typedef std::set<ObservablePtr> ObservedObjects;

public:
    //! Polimorficzny destruktor.
    virtual ~Observer()
    {
        for(auto it = observedObjects.begin(); it != observedObjects.end(); ++it){
            (*it)->observers.erase(this);
        }
    }
    //! Aktualizacja obserwatora.
    //! \param subject Obiekt podany obserwacji.
    virtual void update(const T * subject) = 0;

    //! \return Lista obserwowanych obiektów
    const ObservedObjects & getObservedObjects() const
    {
        return observedObjects;
    }

private:
    ObservedObjects observedObjects;
};

//------------------------------------------------------------------------------

/**
 *  Klasa bazowa dla obiektów obserwowalnych.
 *  \tparam T Typ dziedzicz¹cy po tej klasie.
 */
template <class T>
class Observable
{
    friend class Observer<T>;

public:
    //! Bie¿¹cy typ.
    typedef Observer<T> ObserverType;
    //! Typ wskaŸnika.
    typedef ObserverType * ObserverPtr;

private:
    //! Kolekcja elementów obserwuj¹cych
    typedef std::set<ObserverPtr> Observers;

private:
    //! Obserwatorzy.
    Observers observers;
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
    virtual ~Observable()
    {
        detachAll();
    }

    //! \param observer Obserwator do dodania.
    void attach(ObserverPtr observer)
    {
        auto it = observers.find( observer);
        if ( it == observers.end() ) {
            observer->observedObjects.insert(this);
            observers.insert(observer);
        } else {
            throw std::invalid_argument("Observer already attached.");
        }
    }

    //! \param observer Obserwator do usuniêcia.
    void detach(ObserverPtr observer)
    {
        auto it = observers.find(observer);
        if ( it != observers.end() ) {
            observer->observedObjects.erase(this);
            observers.erase(it);
        } else {
            throw std::invalid_argument("Observer not attached.");
        }
    }

    //! Czy obserwator jest podpiêty?
    //! \param observer Obserwator.
    //! \return
    bool isAttached(ObserverPtr observer)
    {
        return ( observers.find(observer) != observers.end() );
    }

    //! Usuwa wszystkich podpiêtych obserwatorów.
    void detachAll()
    {
        for(auto it = observers.begin(); it != observers.end(); ++it){
            (*it)->observedObjects.erase(this);
        }

        observers.swap(Observers());
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
        for ( auto it = observers.begin(); it != observers.end(); ++it) {
            (*it)->update(self);
        }
        isUpdating = false;
    }
};

//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
} // namespace utils
////////////////////////////////////////////////////////////////////////////////

#endif    // HEADER_GUARD__OBSERVERPATTERN_H__
