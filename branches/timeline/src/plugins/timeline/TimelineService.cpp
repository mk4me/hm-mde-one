#include "TimelinePCH.h"
#include <core/IServiceManager.h>
#include <plugins/timeline/TimelineService.h>
#include "TimelineWidget.h"


TimelineService::TimelineService()
:   seekRequested(false)
{
    widget = new TimelineWidget(this);
    controller = new timeline::Controller();
    controller->attach(widget);
}

IWidget* TimelineService::getWidget()
{
    return reinterpret_cast<IWidget*>(widget);
}

AsyncResult TimelineService::OnTick( double delta )
{
    // state = controller->captureState();
    if ( controller->isPlaying() ) {
        if ( isSeekRequested() ) {
            // czekamy do momentu gdy uda siê uzyskaæ zadany punkt czasowy
            setSeekRequested( controller->isTimeRequested() );
        } else {
            // inkrementacja
            controller->setTime( controller->getTime() + delta * controller->getTimeScale() );
        }
    }
    return AsyncResult_Complete;
}

AsyncResult TimelineService::OnAdded( IServiceManager* serviceManager )
{
    return AsyncResult_Complete;
}

void TimelineService::SetModel( IDataManager* dataManager )
{

}

AsyncResult TimelineService::OnServicesAdded( IServiceManager* serviceManager )
{
    for (int i = 0; i < serviceManager->getNumServices(); ++i) {
        ITimelineClient* client = dynamic_cast<ITimelineClient*>(serviceManager->getService(i));
        if ( client ) {
            timeline::Model::Streams streams = client->getStreams();
            for ( size_t j = 0; j < streams.size(); ++j ) {
                controller->getModel()->addStream( streams[j] );
            }
        }
    }
    //controller->play();
    return AsyncResult_Complete;
}

TimelineService::~TimelineService()
{
    controller->pause();
    delete controller;
}

AsyncResult TimelineService::compute( IServiceManager* serviceManager )
{
    return AsyncResult_Complete;
}