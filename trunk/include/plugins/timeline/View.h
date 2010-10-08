/********************************************************************
	created:  2010/05/20
	created:  20:5:2010   16:59
	filename: View.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__TIMELINEVIEW_H__
#define __HEADER_GUARD__TIMELINEVIEW_H__

#include <boost/shared_ptr.hpp>
#include <utils/ObserverPattern.h>
#include "Controller.h"

////////////////////////////////////////////////////////////////////////////////
namespace timeline {
////////////////////////////////////////////////////////////////////////////////

/**
 * Klasa bazowa dla widoku timeline.
 */
class View : public utils::Observer<Model>
{
public:
    //! Typ wskaünika na kontroler.
    typedef boost::shared_ptr<Controller> ControllerPtr;

protected:
    //! Kontroler timeline.
    ControllerPtr controller;

public:
    //! Konstruktor zerujπcy.
    View()
    {}
    //! Destruktor.
    virtual ~View()
    {}

    //! \return Uøywany kontroler.
    inline ControllerPtr getController()
    { 
        return controller;
    }
    //! \return Uøywany kontroler.
    inline const ControllerPtr getController() const
    { 
        return controller;
    }
    //! \param controller Uøywany kontroler.
    inline void setController(ControllerPtr controller) 
    { 
        this->controller = ControllerPtr(controller);
    }
};

////////////////////////////////////////////////////////////////////////////////
} // namespace timeline
////////////////////////////////////////////////////////////////////////////////

#endif  // __HEADER_GUARD__TIMELINEVIEW_H__