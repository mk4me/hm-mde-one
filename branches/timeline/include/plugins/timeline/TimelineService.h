/********************************************************************
	created:  2010/10/07
	created:  7:10:2010   12:06
	filename: TimelineService.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__TIMELINESERVICE_H__
#define __HEADER_GUARD__TIMELINESERVICE_H__

#include <core/IBaseService.h>
#include <plugins/timeline/Stream.h>
#include <plugins/timeline/Controller.h>
#include <plugins/timeline/ITimelineClient.h>

class TimelineWidget;

class TimelineService : public IBaseService
{
    UNIQUE_ID('TIML','SRVC');
private:
    //! Widget.
    TimelineWidget* widget;
    //! Model timeline'a.
    timeline::Controller* controller;
    //! Czy wykonuje si� seeka?
    bool seekRequested;

public:
    TimelineService();
    virtual ~TimelineService();

    virtual AsyncResult OnTick(double delta);
    virtual AsyncResult OnAdded(IServiceManager* serviceManager);
    virtual AsyncResult OnServicesAdded(IServiceManager* serviceManager);

    virtual IWidget* getWidget();
    virtual void SetModel(IDataManager* dataManager);

public:
    //!
    virtual AsyncResult compute(IServiceManager* serviceManager);

public:

    //! \return Kontroler.
    inline timeline::Controller* getController()
    {
        return controller;
    }
    //! \return Kontroler.
    inline const timeline::Controller* getController() const
    {
        return controller;
    }

    //! \return true je�eli za�yczono sobie seeka, ale jeszcze nie uda�o si� go wykona�.
    inline bool isSeekRequested() const
    { 
        return seekRequested;
    }
    //! Metod� t� nale�y wywo�a� z parametrem true je�eli nadaje si� nowy czas timeline'a, a nie chce
    //! si�, aby nast�powa�a automatyczna inkremetnacja czasu zanim dekodery osi�gn� zadany punkt czasowy.
    //! \param seekRequested true je�eli za�yczono sobie seeka, ale jeszcze nie uda�o si� go wykona�.
    inline void setSeekRequested(bool seekRequested) 
    { 
        this->seekRequested = seekRequested; 
    }


};


#endif  // __HEADER_GUARD__TIMELINESERVICE_H__