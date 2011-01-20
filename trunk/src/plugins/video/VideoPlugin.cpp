#include "VideoPCH.h"
#include <core/Plugin.h>
#include <plugins/video/VideoService.h>
#include <core/Log.h>

DEFINE_DEFAULT_LOGGER("edr.video")
CORE_PLUGIN_BEGIN("Video", UniqueID('VIDE', 'PLUG'))
CORE_PLUGIN_ADD_SERVICE(VideoService)
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



#include "VideoPlugin.h"
#include "ui_video.h"
#include "VideoWidget.h"


PLUGIN_REGISTER(VideoPlugin)

VideoPlugin::VideoPlugin()
: name("Video Plugin")
{
    service = new VideoService();
}

VideoPlugin::~VideoPlugin()
{
  UnloadPlugin();
}

void VideoPlugin::UnloadPlugin()
{

}

int VideoPlugin::GetWidgetsCount()
{
  return 1;
}

IWidget* VideoPlugin::GetDockWidget( int i /*= 0*/ )
{
  return (IWidget*)service->getWidget();
}

POSITION VideoPlugin::GetWidgetPos( int i /*= 0*/ )
{
  return UP;
}

const std::string& VideoPlugin::GetPluginName()
{
  return name;
}

void VideoPlugin::RegisterServices( IServiceManager *serviceManager )
{
  serviceManager->registerService(service);
}

std::string VideoPlugin::GetWidgetName( int i /*= 0*/ )
{
    return utils::toString(i);
}

#endif