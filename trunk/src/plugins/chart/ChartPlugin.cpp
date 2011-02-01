#include <core/Plugin.h>
#include <plugins/chart/EMGService.h>
#include <plugins/chart/GRFService.h>

CORE_PLUGIN_BEGIN("chart", UniqueID('CHAR', 'PLUG'))
CORE_PLUGIN_ADD_SERVICE(EMGService)
CORE_PLUGIN_ADD_SERVICE(GRFService)
CORE_PLUGIN_END

#if 0

#include <sstream>

#include <QtGui/QWidget>
#include <QtCore/QString>
#include <osgViewer/Scene>
#include <osg/Group>
#include <osg/Geode>
#include <osg/NodeVisitor>

#include <core/PluginDataInterface.h>
#include <core/PluginsInterfaces.h>
#include <core/WidgetInterface.h>
#include <core/IServiceManager.h>
#include <core/DllExports.h>



#include "ChartPlugin.h"
#include "ui_ChartWidget.h"
#include "ChartWidget.h"


PLUGIN_REGISTER(ChartPlugin)

ChartPlugin::ChartPlugin()
: name("Chart Plugin")
{
    service = new ChartService();
}

ChartPlugin::~ChartPlugin()
{
  UnloadPlugin();
}

void ChartPlugin::UnloadPlugin()
{

}

int ChartPlugin::GetWidgetsCount()
{
  return 1;
}

IWidget* ChartPlugin::GetDockWidget( int i /*= 0*/ )
{
  return (IWidget*)service->getWidget();
}

POSITION ChartPlugin::GetWidgetPos( int i /*= 0*/ )
{
  return UP;
}

const std::string& ChartPlugin::GetPluginName()
{
  return name;
}

void ChartPlugin::RegisterServices( IServiceManager *serviceManager )
{
  serviceManager->registerService(service);
}

std::string ChartPlugin::GetWidgetName( int i /*= 0*/ )
{
    return utils::toString(i);
}

#endif