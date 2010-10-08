/********************************************************************
	created:  2010/07/14
	created:  14:7:2010   17:39
	filename: Callback.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__CALLBACK_H__
#define __HEADER_GUARD__CALLBACK_H__

#include <vector>
#include <algorithm>
#include <osgWidget/EventInterface>
#include <utils/Debug.h>
#include <utils/Pool.h>

////////////////////////////////////////////////////////////////////////////////
namespace osgUI {
////////////////////////////////////////////////////////////////////////////////

/**
 *	Typ przechowywuj�cy informacje o zdarzeniu. Bazuje na puli (utils::Pool),
 *  dzi�ki czemu mo�e przechowywa� obiekty dowolnego typu (obiekty i prymitywy),
 *  ale tylko do okre�lonego rozmiaru. Poza to wa�na jest kolejno��, kt�ra
 *  jest u�ywana do ich odczytu!
 */
class Event : public utils::Pool
{
private:
  //! Obiekt zg�aszaj�cy event.
  osg::Object * sender;

public:
  //! Konstruktor ustawiaj�cy parametry.
  //! \param sender
  Event(osg::Object * sender) :
    sender(sender)
  {}

  //! \param sender
  //! \param data
  template <class T1>
  Event(osg::Object * sender, const T1 & data1) :
    utils::Pool(data1), sender(sender)
  {}

  //! \param sender
  //! \param data
  template <class T1, class T2>
  Event(osg::Object * sender, const T1 & data1, const T2 & data2) :
    utils::Pool(data1, data2), sender(sender)
  {}

  //! \param sender
  //! \param data
  template <class T1, class T2, class T3>
  Event(osg::Object * sender, const T1 & data1, const T2 & data2, const T3 & data3) :
    utils::Pool(data1, data2, data3), sender(sender)
  {}

  //! 
  //! \param sender
  //! \param data
  template <class T, int Size>
  Event(osg::Object * sender, T (&data)[Size]) :
    utils::Pool(data), sender(sender)
  {}

  //!
  osg::Object * getSender()
  {
    return sender;
  }
  //!
  template <class T>
  T * getSender(T * dummy = NULL)
  {
    return dynamic_cast<T*>(sender);
  }
};

/**
 *  Interfejs wszystkich callback�w.
 */
class CallbackInterface: public osg::Referenced
{
public:
  //! Pusty polimorficzny destruktor.
  virtual ~CallbackInterface() {}
  //! Operator nazwiasowy z zadanym eventem.
  virtual bool operator()(Event& event) = 0;
  //! Czy jest prawid�owy? Je�li nie jest usuwany!
  virtual bool isValid() = 0;
};

/**
 *	Callback u�ywaj�cy metody obiektu.
 */
template<typename T>
class ObjectCallback: public CallbackInterface
{
public:
  //! Metoda u�ywaj�ca eventu.
  typedef bool (T::*ObjectCallbackType)(Event&);

private:
  //! Obiekt �r�d�owy.
  osg::observer_ptr<T> object;
  //! Metoda.
  ObjectCallbackType callback;

public:
  //! \param callback
  //! \param obj
  ObjectCallback(ObjectCallbackType callback, T* obj):
    object   (obj),
    callback (callback) 
  {}
  //! \param ev
  virtual bool operator()(Event& ev) 
  {
    if ( object.valid() ) {
      return (object.get()->*callback)(ev);
    } else {
      return false;
    }
  }
  //! \return
  virtual bool isValid()
  {
    return object.valid();
  }

};

/**
 *	Callback u�ywaj�cy wska�nika na funkcj�.
 */
template<typename T>
class FunctionCallback: public CallbackInterface
{
private:
  //! Wska�nik na funckj�/funktor.
  T* callback;

public:
  //! 
  //! \param callback
  FunctionCallback(T* callback):
    callback(callback)
  {}
  //! 
  //! \param ev
  virtual bool operator()(Event& ev) 
  {
    return (*callback)(ev);
  }
  //! \return
  virtual bool isValid()
  {
    return true;
  }
};

/**
 *	
 */
class Callback : public osg::Referenced
{
protected:
  //! Faktyczny callback.
  osg::ref_ptr<CallbackInterface> callback;
  //! ID zdarzenia.
  int id;

public:
  //! Konstruktor zeruj�cy.
  Callback(): id(0), callback(NULL) {}

  //! Konstruktor kopiuj�cy.
  Callback(const Callback& rhs) : 
  osg::Referenced(rhs), id(rhs.id), callback(rhs.callback) 
  {}

  //! Konstruktor na podstawie wska�nika do metody.
  template<typename T>
  Callback(int id, bool (T::*function)(Event&), T* obj):
  id(id), callback(new ObjectCallback<T>(function, obj)) 
  {}

  //! Konstruktor na podstawie wska�nika do funkcji.
  template<typename T>
  Callback(int id, T* functor):
  id(id), callback(new FunctionCallback<T>(functor)) 
  {}

  //! Pusty wirtualny destruktor. 
  virtual ~Callback() {}

  //! 
  //! \param ev
  virtual bool operator()(Event& ev) 
  {
    if(!callback) return false;
    return (*callback)(ev);
  }

  //! \return false je�li Callbacka nale�y usun��.
  virtual bool isValid()
  {
    return callback ? callback->isValid() : false;
  }

  //! \return
  inline int getId() const
  { 
    return id;
  }
};


class EventSource
{
protected:
  //! Lista callback�w.
  typedef std::vector< osg::ref_ptr<Callback> > List;

protected:
  //! Lista callback�w.
  List callbacks;
 
protected:
  //! Tylko klasa pochodna mo�e go zniszczy�.
  virtual ~EventSource() {}

  //! Pomocnicza metoda do odpalania event�w.
  //! \param id
  //! \param sender
  bool fireEvent(int id, osg::Object* sender)
  {
    Event event(sender);
    return fireEvent(id, event);
  }

  //! 
  //! \param id
  //! \param sender
  //! \param data1
  template <class T1>
  bool fireEvent(int id, osg::Object* sender, const T1& data1)
  {
    Event event(sender, data1);
    return fireEvent(id, event);
  }

  //! 
  //! \param id
  //! \param sender
  //! \param data1
  //! \param data2
  template <class T1, class T2>
  bool fireEvent(int id, osg::Object* sender, const T1& data1, const T2& data2)
  {
    Event event(sender, data1, data2);
    return fireEvent(id, event);
  }

  //! 
  //! \param id
  //! \param sender
  //! \param data1
  //! \param data2
  //! \param data3
  template <class T1, class T2, class T3>
  bool fireEvent(int id, osg::Object* sender, const T1& data1, const T2& data2, const T3& data3)
  {
    Event event(sender, data1, data2, data3);
    return fireEvent(id, event);
  }

  //! Pomocnicza metoda do odpalania event�w.
  //! \param id
  //! \param sender
  template <class T, int Size>
  bool fireEvent(int id, osg::Object* sender, const T (&arr)[Size])
  {
    Event event(sender, arr);
    return fireEvent(id, event);
  }


  //! Powiadamia callbacki o zadanym id o zaj�ciu zdarzenia.
  /**
   *	Poniewa� parametr event jest referencj� (a nie sta�� referencj�) nie mo�na napisa�
   *  fireEvent(<jakie�_id>, Event(<sender>, <parametry>)) poniewa� spowoduje to b��d niezgodno�ci
   *  sygnatur (kompilator b�dzie szuka� odmiany z const Event&). Do zaradzenia temu s�u�� odmiany
   *  z jawnie podanymi parametrami.
   *  \param id
   *  \param event
   */
  bool fireEvent(int id, Event & event)
  {
    for (List::size_type i = 0; i < callbacks.size(); ++i) {
      // usuwamy "wymar�e" eventy
      if ( !callbacks[i]->isValid() ) {
        callbacks.erase( callbacks.begin() + i );
        --i;
      } else {
        // je�li kt�ry� callback zwr�ci� true - przerywamy
        if ( callbacks[i]->getId() == id && (*callbacks[i])(event) ) {
          return true;
        }
      }
    }
    return false;
  }

public:
  //! Dodaje callback do obiektu.
  //! \param callback
  void addCustomCallback( Callback * callback )
  {
    callbacks.push_back(callback);
  }

  //! Usuwa callback z obiektu.
  //! \param callback
  void removeCustomCallback( Callback * callback )
  {
    List::iterator removed = std::remove(callbacks.begin(), callbacks.end(), callback);
    if ( removed != callbacks.end() ) {
      callbacks.resize( std::distance(callbacks.begin(), removed) );
    }
  }

};


////////////////////////////////////////////////////////////////////////////////
} // namespace osgUI
////////////////////////////////////////////////////////////////////////////////


#endif  // __HEADER_GUARD__CALLBACK_H__