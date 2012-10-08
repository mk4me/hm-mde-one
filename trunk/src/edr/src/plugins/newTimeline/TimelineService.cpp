#include "TimelinePCH.h"


#include <core/PluginCommon.h>
#include <plugins/newTimeline/TimelineService.h>
#include "TimelineWidget.h"

TimelineService::TimelineService() : name("newTimeline"),
    controller(new timeline::Controller())
{
    widget = new TimelineWidget(controller);
}


TimelineService::~TimelineService()
{

}

 void TimelineService::finalize()
 {
     setPlaying(false);
 }

void TimelineService::setChannelTooltip(const std::string & path, const std::string & tooltip)
{
    widget->setChannelTooltip(path, tooltip);
}

std::string TimelineService::getChannelTooltip(const std::string & path) const
{
    return widget->getChannelTooltip(path);
}

void TimelineService::setOnChannelClick(const UIChannelAction & action)
{
    widget->setOnChannelClick(action);
}

const TimelineService::UIChannelAction & TimelineService::getOnChannelClick() const
{
    return widget->getOnChannelClick();
}

void TimelineService::setOnChannelDblClick(const UIChannelAction & action)
{
    widget->setOnChannelDblClick(action);
}

const TimelineService::UIChannelAction & TimelineService::getOnChannelDblClick() const
{
    return widget->getOnChannelDblClick();
}

void TimelineService::setOnChannelRemove(const TimelineService::UIChannelAction & action)
{
    widget->setOnChannelRemove(action);
}

const TimelineService::UIChannelAction & TimelineService::getOnChannelRemove() const
{
    return widget->getOnChannelRemove();
}
  
 QWidget* TimelineService::getWidget(core::IActionsGroupManager * actionsManager)
 {
     return nullptr;
 }

QWidget* TimelineService::getControlWidget(core::IActionsGroupManager * actionsManager)
{
    widget->loadToolbarElements(actionsManager);
    return widget;
}

const std::string& TimelineService::getName() const
{
    return name;
}

void TimelineService::addChannel(const std::string & path, const timeline::IChannelPtr & channel)
{
    controller->addChannel(path, channel);
}

void TimelineService::removeChannel(const std::string & path)
{
    controller->removeChannel(path);
}

bool TimelineService::isPlaying() const
{
    return controller->isPlaying();
}
 
void TimelineService::setPlaying(bool playing)
{
    if(playing == true){
        controller->play();
    }else{
        controller->pause();
    } 
}

double TimelineService::getTime() const
{
    return controller->getTime();
}
 
void TimelineService::setTime(double time)
{
    controller->setTime(time);
}
    
double TimelineService::getLength() const
{
    return controller->getLength();
}

double TimelineService::getNormalizedTime() const
{
    return controller->getNormalizedTime();
}

void TimelineService::setNormalizedTime(double time)
{
    controller->setNormalizedTime(time);
}
