/********************************************************************
	created:  2010/05/20
	created:  20:5:2010   16:59
	filename: View.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__TIMELINEVIEW_H__
#define __HEADER_GUARD__TIMELINEVIEW_H__

#include <utils/ObserverPattern.h>
#include "Controller.h"

////////////////////////////////////////////////////////////////////////////////
namespace timeline {
////////////////////////////////////////////////////////////////////////////////

/**
 * Klasa bazowa dla widoku timeline.
 */
template <class Model, class PtrPolicy = utils::PtrPolicyRaw>
class View : public PtrPolicy, public utils::Observer<Model>
{
public:
  //! Typ wska�nika na kontroler.
  typedef typename PtrPolicy::template Ptr<Controller>::Type ControllerPtr;
protected:
  //! Kontroler timeline.
  ControllerPtr controller;

public:
  //! Konstruktor zeruj�cy.
  View()
  {
    PtrPolicy::initPtr(controller);
  }
  //! Destruktor.
  virtual ~View()
  {
    PtrPolicy::setPtr(controller, (Controller*)NULL);
  }

  //! \return Prosty wska�nik na kontroler/
  inline const Model* getControllerPtr() const
  {
    return &(*controller);
  }
  //! \return U�ywany kontroler.
  inline ControllerPtr getController()
  { 
    return controller;
  }
  //! \return U�ywany kontroler.
  inline const ControllerPtr getController() const
  { 
    return controller;
  }
  //! \param controller U�ywany kontroler.
  inline void setController(ControllerPtr controller) 
  { 
    PtrPolicy::setPtr(this->controller, controller);
  }
};

////////////////////////////////////////////////////////////////////////////////
} // namespace timeline
////////////////////////////////////////////////////////////////////////////////

#endif  // __HEADER_GUARD__TIMELINEVIEW_H__