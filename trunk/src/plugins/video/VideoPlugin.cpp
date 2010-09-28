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
  static QWidget* widget[1] = { NULL };
  if ( widget[0] == NULL ) {
    for (int i = 0; i < 1; ++i) {
      widget[i] = new VideoWidget();
    }
  }
  return (IWidget*)widget[i];
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
  VideoService* videoService = new VideoService();
  serviceManager->RegisterServiceAs(videoService, VideoService::getClassID().major);
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

