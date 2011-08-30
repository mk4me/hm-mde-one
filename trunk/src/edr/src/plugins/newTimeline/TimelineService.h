/********************************************************************
    created:  2011/08/08
    created:  8:8:2011   8:05
    filename: TimelineService.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_TIMELINE__TIMELINESERVICE_H__
#define HEADER_GUARD_TIMELINE__TIMELINESERVICE_H__

#include <plugins/newTimeline/ITimelineService.h>

class TimelineWidget;

class TimelineService : public ITimelineService
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
    //! \param stream Strumie� do dodania.
    virtual void addChannel(const std::string & path, const timeline::IChannelPtr & channel);

    virtual void removeChannel(const std::string & path);
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

    //!
    virtual void setChannelTooltip(const std::string & path, const std::string & tooltip);
    //!
    virtual std::string getChannelTooltip(const std::string & path) const;
    //!
    virtual void setOnChannelClick(const UIChannelAction & action);
    //!
    virtual const UIChannelAction & getOnChannelClick() const;
    //!
    virtual void setOnChannelDblClick(const UIChannelAction & action);
    //!
    virtual const UIChannelAction & getOnChannelDblClick() const;

    virtual void setOnChannelRemove(const UIChannelAction & action);

    virtual const UIChannelAction & getOnChannelRemove() const;

public:

    //! \return Kontroler.
    virtual timeline::IController* getController()
    {
        return controller.get();
    }
    //! \return Kontroler.
    virtual const timeline::IController* getController() const
    {
        return controller.get();
    }
};

#endif  // __HEADER_GUARD__TIMELINESERVICE_H__