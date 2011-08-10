/********************************************************************
    created:  2011/08/08
    created:  8:8:2011   8:05
    filename: TimelineService.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_TIMELINE__TIMELINESERVICE_H__
#define HEADER_GUARD_TIMELINE__TIMELINESERVICE_H__

#include <core/IService.h>
#include <timelinelib/IChannel.h>
#include <timelinelib/Controller.h>

class TimelineWidget;

class TimelineService : public core::IService
{
    UNIQUE_ID("{0157346E-D1F3-4A4F-854F-37C87FA3E5F9}", "newTimeline Service");
private:
    //! Widget.
    TimelineWidget* widget;
    //! Model timeline'a.
    timeline::ControllerPtr controller;
    //!
    std::string name;

public:
    TimelineService();
    virtual ~TimelineService();

// IService
public:

    virtual void finalize();
    //!
    virtual QWidget* getWidget(std::vector<QObject*>& actions);
    //!
    virtual QWidget* getControlWidget(std::vector<QObject*>& actions);
    //!
    virtual const std::string& getName() const;

public:
    //! \param stream Strumieñ do dodania.
    virtual void addChannel(const std::string & path, const timeline::IChannelPtr & channel);
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
    inline timeline::IController* getController()
    {
        return controller.get();
    }
    //! \return Kontroler.
    inline const timeline::IController* getController() const
    {
        return controller.get();
    }

    //! Metodê tê nale¿y wywo³aæ z parametrem true je¿eli nadaje siê nowy czas timeline'a, a nie chce
    //! siê, aby nastêpowa³a automatyczna inkremetnacja czasu zanim dekodery osi¹gn¹ zadany punkt czasowy.
    //! \param seekRequested true je¿eli za¿yczono sobie seeka, ale jeszcze nie uda³o siê go wykonaæ.
    //void setSeekRequested(bool seekRequested);

};

typedef core::shared_ptr<TimelineService> TimelinePtr;


#endif  // __HEADER_GUARD__TIMELINESERVICE_H__