#include "TimelinePCH.h"
#include <core/IServiceManager.h>
#include "TimelinePlugin.h"
#include <core/DllExports.h>


PLUGIN_REGISTER(TimelinePlugin)

TimelinePlugin::TimelinePlugin()
: name("Timeline Plugin")
{
    service = new TimelineService();
}

TimelinePlugin::~TimelinePlugin()
{
    UnloadPlugin();
}

void TimelinePlugin::UnloadPlugin()
{
    delete service;
}

int TimelinePlugin::GetWidgetsCount()
{
    return 1;
}

IWidget* TimelinePlugin::GetDockWidget( int i /*= 0*/ )
{
    return service->getWidget();
}

POSITION TimelinePlugin::GetWidgetPos( int i /*= 0*/ )
{
    return UP;
}

const std::string& TimelinePlugin::GetPluginName()
{
    return name;
}

void TimelinePlugin::RegisterServices( IServiceManager *serviceManager )
{
    serviceManager->registerService(service);
}

void TimelinePlugin::SetScene( osgViewer::Scene *scene )
{

}

void TimelinePlugin::SetScene( osgViewer::Scene *scene, IServiceManager *serviceManager )
{

}

std::string TimelinePlugin::GetWidgetName( int i /*= 0*/ )
{
    std::ostringstream out;
    out << i;
    return out.str();
}