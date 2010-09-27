#include <QtGui/QWidget>
#include <QtCore/QString>

#include <osgViewer/Scene>
#include <osg/Group>
#include <osg/Geode>
#include <osg/NodeVisitor>

#include <core/PluginDataInterface.h>
#include <core/WidgetInterface.h>
#include <core/IServiceManager.h>

#include "VideoPlugin.h"

VideoPlugin::VideoPlugin()
{

}

VideoPlugin::~VideoPlugin()
{

}

void VideoPlugin::UnloadPlugin()
{

}

int VideoPlugin::GetWidgetsCount()
{
  return 0;
}

IWidget* VideoPlugin::GetDockWidget( int i /*= 0*/ )
{
  return 0;
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
  
}

void VideoPlugin::SetScene( osgViewer::Scene *scene )
{

}

void VideoPlugin::SetScene( osgViewer::Scene *scene, IServiceManager *serviceManager )
{

}

std::string VideoPlugin::GetWidgetName( int i /*= 0*/ )
{
  return "NONE";
}

