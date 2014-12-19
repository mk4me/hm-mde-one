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

public:
    //! Polimorficzny destruktor.
    virtual ~Observer()
    {
		//TODO tak nie moze być! Zawsze musi być zagwarantowane że obserwowany obiekt zyje dłużej!!
        /*for(auto it = observedObjects.begin(); it != observedObjects.end(); ++it){
            (*it)->observers.erase(this);
        }*/
    }
    //! Aktualizacja obserwatora.
    //! \param subject Obiekt podany obserwacji.
    virtual void update(const T * subject) = 0;
};

//------------------------------------------------------------------------------

/**
 *  Klasa bazowa dla obiektów obserwowalnych.
 *  \tparam T Typ dziedziczący po tej klasie.
 */
template <class T>
class Observable
{
    friend class Observer<T>;

public:
    //! Bieżący typ.
    typedef Observer<T> ObserverType;
    //! Typ wskaźnika.
    typedef ObserverType * ObserverPtr;

private:
    //! Kolekcja elementów obserwujących
    typedef std::set<ObserverPtr> Observers;

private:
    //! Obserwatorzy.
    Observers observers;
    //! Obserwowany obiekt.
    const T * self;
    //!
    volatile bool isUpdating;

protected:
    //! Tylko klasa pochodna może wywołać ten konstruktor.
    Observable() : self(NULL), isUpdating(false) {}
    //! Tylko klasa pochodna może wywołać ten konstruktor.
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
            observers.insert(observer);
        } else {
            throw std::invalid_argument("Observer already attached.");
        }
    }

    //! \param observer Obserwator do usunięcia.
    void detach(ObserverPtr observer)
    {
        auto it = observers.find(observer);
        if ( it != observers.end() ) {
            observers.erase(it);
        } else {
            throw std::invalid_argument("Observer not attached.");
        }
    }

    //! Czy obserwator jest podpięty?
    //! \param observer Obserwator.
    //! \return
    const bool isAttached(ObserverPtr observer)
    {
        return ( observers.find(observer) != observers.end() );
    }

    //! Aktualizuje wszystkie wyniki.
    void notify()
    {
        if ( !self ) {
            self = dynamic_cast<const T*>(this);
            UTILS_ASSERT(self, "Error casting to desired type.");
        }
        if ( !self ) {
            throw std::runtime_error("Could not deduce self-pointer");
        }

        if (isUpdating) {
            UTILS_ASSERT(false, "Can't update during notify. Maybe should use synchronized version?");
        }

        isUpdating = true;
        for ( auto it = observers.begin(); it != observers.end(); ++it) {
            (*it)->update(self);
        }
        isUpdating = false;
    }

protected:

	//! Usuwa wszystkich podpiętych obserwatorów.
	void detachAll()
	{
		Observers().swap(observers);
	}
};


class GeneralObservable : public Observable<GeneralObservable>
{
public:
	GeneralObservable() {}

	virtual ~GeneralObservable() {}
};

//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
} // namespace utils
////////////////////////////////////////////////////////////////////////////////

#endif    // HEADER_GUARD__OBSERVERPATTERN_H__
