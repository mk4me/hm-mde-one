#include "TimelinePCH.h"

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
  
 QWidget* TimelineService::getWidget(std::vector<QObject*>& actions)
 {
     return nullptr;
 }

QWidget* TimelineService::getControlWidget(std::vector<QObject*>& actions)
{
    widget->loadToolbarElements(actions);
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