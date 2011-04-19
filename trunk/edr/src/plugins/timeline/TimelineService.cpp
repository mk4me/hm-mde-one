#include "TimelinePCH.h"
#include <core/IServiceManager.h>
#include <plugins/timeline/TimelineService.h>
#include "TimelineWidget.h"


TimelineService::TimelineService()
:   seekRequested(false),
    name("Timeline")
{
    widget = new TimelineWidget(this);
    controller = new timeline::Controller();
    controller->setAsynchronous(true);
    controller->attach(widget);
}


TimelineService::~TimelineService()
{
    controller->pause();
    delete controller;
}

QWidget* TimelineService::getWidget()
{
    return nullptr;
}

QWidget* TimelineService::getControlWidget()
{
    return widget;
}

void TimelineService::update( double time, double timeDelta )
{
    widget->setBusy(controller->isBusy() || controller->isTimeDirty());
}

void TimelineService::lateUpdate( double time, double timeDelta)
{
    if ( controller->isPlaying() ) {
        if ( isSeekRequested() ) {
            setSeekRequested( controller->isBusy() || controller->isTimeDirty() );
        }  else {
            controller->setTime( controller->getTime() + timeDelta * controller->getTimeScale() );
        }
    }
}

void TimelineService::setSeekRequested( bool seekRequested )
{
    this->seekRequested = seekRequested;
}

void TimelineService::addStream( timeline::StreamPtr stream )
{
    controller->addStream(stream);
}

void TimelineService::removeStream( timeline::StreamPtr stream )
{
    controller->removeStream(stream);
}

bool TimelineService::isPlaying() const
{
    return controller->isPlaying();
}

void TimelineService::setPlaying( bool playing )
{
    if ( playing ) {
        controller->play();
    } else {
        controller->pause();
    }
}

double TimelineService::getTime() const
{
    return controller->getTime();
}

void TimelineService::setTime( double time )
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

void TimelineService::setNormalizedTime( double time )
{
    return controller->setNormalizedTime(time);
}

const std::string& TimelineService::getName() const
{
    return name;
}

