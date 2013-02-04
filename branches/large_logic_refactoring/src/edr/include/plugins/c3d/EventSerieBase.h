/********************************************************************
	created:	2012/03/25
	created:	25:3:2012   20:51
	filename: 	EventSerieBase.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_C3DPLUGIN__EVENTSERIEBASE_H__
#define HEADER_GUARD_C3DPLUGIN__EVENTSERIEBASE_H__

#include <corelib/IVisualizer.h>
#include <plugins/c3d/C3DCollections.h>

//! klasa dostarcza eventów do podstawowej serii danych.
//! jeśli seria danych ma je obsługiwać, powinna dziedziczyć po tej klasie
class EventSerieBase : public plugin::IVisualizer::ISerie, public plugin::IVisualizer::ITimeAvareSerieFeatures
{
public:
    //! Abstraktycja metoda, seria danych po jej wywołaniu powinna zainicjalizować sobie obsługę eventów
    //! \param val ustawiane eventy
    virtual void setEvents(EventsCollectionConstPtr val) = 0;
    //! \return kontekst, dla którego aktualnie rozpatrywane są jointy (typowo left, right)
    C3DEventsCollection::Context getContext() const { return context; }
    //! ustawia kontekst, dla którego aktualnie rozpatrywane są jointy 
    //! \param val kontekst, typowo left, right
    void setContext(C3DEventsCollection::Context val) { context = val; }

private:
    //! kontekst, typowo left, right
    C3DEventsCollection::Context context; 

};
typedef core::shared_ptr<EventSerieBase> EventSerieBasePtr;
typedef core::shared_ptr<const EventSerieBase> EventSerieBaseConstPtr;

#endif
