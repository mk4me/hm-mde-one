#include "ControlPlugin.h"

#include <QtGui/QWidget>
#include <QtCore/QString>

#include <osgViewer/Scene>
#include <osg/Group>
#include <osg/Geode>
#include <osg/NodeVisitor>

#include "OsgControlWidget.h"

ControlPlugin::ControlPlugin(void):
  controlWidget_(0)
{
}

ControlPlugin::~ControlPlugin(void)
{
  unloadPlugin(); 
}

void ControlPlugin::unloadPlugin()
{
  if(controlWidget_ != NULL)
  {
    delete controlWidget_; 
    controlWidget_ = 0; 
  }
}

int ControlPlugin::getDockWidgetsCount()
{
  return 1; 
}

QWidget *ControlPlugin::getDockWidget( int i )
{
  if(!controlWidget_) 
    controlWidget_ = new OsgControlWidget(); 
  return (QWidget *)controlWidget_; 
}

QString ControlPlugin::getPluginName()
{
  return QString::fromUtf8("Control wigdet (from plugin ;) )"); 
}

void ControlPlugin::setScene( osgViewer::Scene *scene )
{
  if(!controlWidget_) return; 
  controlWidget_->setScene(scene); 
}

Q_EXPORT_PLUGIN2(motion_controlplugin, ControlPlugin)