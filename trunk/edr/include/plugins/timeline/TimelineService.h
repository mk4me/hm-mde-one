/********************************************************************
	created:  2010/10/07
	created:  7:10:2010   12:06
	filename: TimelineService.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__TIMELINESERVICE_H__
#define __HEADER_GUARD__TIMELINESERVICE_H__

#include <core/IService.h>
#include <plugins/timeline/Stream.h>
#include <plugins/timeline/Controller.h>
#include <plugins/timeline/ITimelineClient.h>
#include <plugins/timeline/ITimeline.h>

class TimelineWidget;

class TimelineService : public ITimeline, public core::IService
{
    UNIQUE_ID("{0157346E-D0F3-4A4F-854F-37C87FA3E5F9}", "Timeline Service");
private:
    //! Widget.
    TimelineWidget* widget;
    //! Model timeline'a.
    timeline::Controller* controller;
    //! Czy wykonuje siê seeka?
    bool seekRequested;
    //!
    std::string name;

public:
    TimelineService();
    virtual ~TimelineService();

// IService
public:
    //!
    virtual QWidget* getWidget();
    //!
    virtual QWidget* getControlWidget();
    //!
    virtual void lateUpdate(double time, double timeDelta);
    //!
    virtual void update(double time, double timeDelta);
    //!
    virtual const std::string& getName() const;

// ITimeline
public:
    //! \param stream Strumieñ do dodania.
    virtual void addStream(timeline::StreamPtr stream);
    //! \param stream Strumieñ do usuniêcia.
    virtual void removeStream(timeline::StreamPtr stream);
    //! \return true je¿eli timeline sam siê odtwarza.
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

    //! \return true je¿eli za¿yczono sobie seeka, ale jeszcze nie uda³o siê go wykonaæ.
    inline bool isSeekRequested() const
    { 
        return seekRequested;
    }
    //! Metodê tê nale¿y wywo³aæ z parametrem true je¿eli nadaje siê nowy czas timeline'a, a nie chce
    //! siê, aby nastêpowa³a automatyczna inkremetnacja czasu zanim dekodery osi¹gn¹ zadany punkt czasowy.
    //! \param seekRequested true je¿eli za¿yczono sobie seeka, ale jeszcze nie uda³o siê go wykonaæ.
    void setSeekRequested(bool seekRequested);


};


#endif  // __HEADER_GUARD__TIMELINESERVICE_H__