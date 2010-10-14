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
#include <plugins/timeline/ITimelineService.h>

class TimelineWidget;

class TimelineService : public ITimeline, public IBaseService
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

    virtual AsyncResult OnAdded(IServiceManager* serviceManager);
    

    virtual IWidget* getWidget();
    virtual void SetModel(IDataManager* dataManager);

// IBaseService
public:
    //!
    virtual AsyncResult init(IServiceManager* serviceManager);
    //!
    virtual AsyncResult compute();
    //!
    virtual AsyncResult lateUpdate(IServiceManager* serviceManager);
    //!
    virtual AsyncResult update(IServiceManager* serviceManager);

// ITimeline
public:
    //! \param stream Strumie� do dodania.
    virtual void addStream(timeline::StreamPtr stream);
    //! \param stream Strumie� do usuni�cia.
    virtual void removeStream(timeline::StreamPtr stream);
    //! \return true je�eli timeline sam si� odtwarza.
    virtual bool isPlaying() const;
    //!
    virtual void setPlaying(bool playing);
    //!
    virtual double getTime() const;
    //!
    virtual void setTime(double time);
    //!
    virtual double getLength() const;
    virtual double getNormalizedTime() const;
    virtual void setNormalizedTime(double time);

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
    void setSeekRequested(bool seekRequested);


};


#endif  // __HEADER_GUARD__TIMELINESERVICE_H__