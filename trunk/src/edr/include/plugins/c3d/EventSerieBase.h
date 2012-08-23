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

//! klasa dostarcza eventów do podstawowej serii danych.
//! jeœli seria danych ma je obs³ugiwaæ, powinna dziedziczyæ po tej klasie
class EventSerieBase : public core::IVisualizer::TimeSerieBase
{
public:
    //! Abstraktycja metoda, seria danych po jej wywo³aniu powinna zainicjalizowaæ sobie obs³ugê eventów
    //! \param val ustawiane eventy
    virtual void setEvents(EventsCollectionConstPtr val) = 0;
    //! \return kontekst, dla którego aktualnie rozpatrywane s¹ jointy (typowo left, right)
    C3DEventsCollection::Context getContext() const { return context; }
    //! ustawia kontekst, dla którego aktualnie rozpatrywane s¹ jointy 
    //! \param val kontekst, typowo left, right
    void setContext(C3DEventsCollection::Context val) { context = val; }

private:
    //! kontekst, typowo left, right
    C3DEventsCollection::Context context; 

};
typedef core::shared_ptr<EventSerieBase> EventSerieBasePtr;
typedef core::shared_ptr<const EventSerieBase> EventSerieBaseConstPtr;

#endif
