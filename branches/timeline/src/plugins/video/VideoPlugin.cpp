#include "VideoPCH.h"
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

#include <plugins/video/VideoService.h>

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
  serviceManager->RegisterServiceAs(service, service->getID().major);
}

void VideoPlugin::SetScene( osgViewer::Scene *scene )
{

}

void VideoPlugin::SetScene( osgViewer::Scene *scene, IServiceManager *serviceManager )
{
}

std::string VideoPlugin::GetWidgetName( int i /*= 0*/ )
{
  std::ostringstream out;
  out << i;
  return out.str();
}
