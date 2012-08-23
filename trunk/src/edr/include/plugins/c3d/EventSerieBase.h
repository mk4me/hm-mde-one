/********************************************************************
	created:	2012/03/25
	created:	25:3:2012   20:51
	filename: 	EventSerieBase.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_C3DPLUGIN__EVENTSERIEBASE_H__
#define HEADER_GUARD_C3DPLUGIN__EVENTSERIEBASE_H__

#include <core/IVisualizer.h>
#include <plugins/c3d/C3DCollections.h>

//! klasa dostarcza eventow do podstawowej serii danych.
//! je�li seria danych ma je obs�ugiwac, powinna dziedziczyc po tej klasie
class EventSerieBase : public core::IVisualizer::TimeSerieBase
{
public:
    //! Abstraktycja metoda, seria danych po jej wywo�aniu powinna zainicjalizowac sobie obsluge eventow
    //! \param val 
    virtual void setEvents(EventsCollectionConstPtr val) = 0;
    //! \return kontekst, dla kt�rego aktualnie rozpatrywany s� jointy (typowo left, right)
    C3DEventsCollection::Context getContext() const { return context; }
    //! ustawia kontekst, dla kt�rego aktualnie rozpatrywany s� jointy 
    //! \param val kontekst typowo left, right
    void setContext(C3DEventsCollection::Context val) { context = val; }

private:
    //! kontekst typowo left, right
    C3DEventsCollection::Context context; 

};
typedef core::shared_ptr<EventSerieBase> EventSerieBasePtr;
typedef core::shared_ptr<const EventSerieBase> EventSerieBaseConstPtr;

#endif
