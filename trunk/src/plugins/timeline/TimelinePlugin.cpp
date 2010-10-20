#include "TimelinePCH.h"
#include <plugins/timeline/TimelineService.h>
#include <core/Plugin.h>

CORE_PLUGIN_BEGIN("Timeline", UniqueID('TIME', 'PLUG'))
CORE_PLUGIN_ADD_SERVICE(TimelineService)
CORE_PLUGIN_END

#if 0


#include <boost/lexical_cast.hpp>
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

std::string TimelinePlugin::GetWidgetName( int i /*= 0*/ )
{
    return boost::lexical_cast<std::string>(i);
}
#endif