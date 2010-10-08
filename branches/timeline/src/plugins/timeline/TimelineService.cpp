#include "TimelinePCH.h"
#include <core/IServiceManager.h>
#include <plugins/timeline/TimelineService.h>
#include "TimelineWidget.h"


TimelineService::TimelineService()
{
    widget = new TimelineWidget(this);
    controller = new timeline::Controller();
    controller->getModel()->attach(widget);
}

IWidget* TimelineService::getWidget()
{
    return reinterpret_cast<IWidget*>(widget);
}

AsyncResult TimelineService::OnTick( double delta )
{
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