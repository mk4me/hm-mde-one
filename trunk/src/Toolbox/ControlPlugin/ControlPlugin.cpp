#include "ControlPlugin.h"

#include <QtGui/QWidget>
#include <QtCore/QString>

#include <osgViewer/Scene>
#include <osg/Group>
#include <osg/Geode>
#include <osg/NodeVisitor>

#include "OsgControlWidget.h"

ControlPlugin::ControlPlugin(void):
  _controlWidget(0)
{
}

ControlPlugin::~ControlPlugin(void)
{
  unloadPlugin(); 
}

void ControlPlugin::unloadPlugin()
{
  if(_controlWidget != NULL)
  {
    delete _controlWidget; 
    _controlWidget = 0; 
  }
}

int ControlPlugin::getDockWidgetsCount()
{
  return 1; 
}

QWidget *ControlPlugin::getDockWidget( int i )
{
  if(!_controlWidget) 
    _controlWidget = new OsgControlWidget(); 
  return (QWidget *)_controlWidget; 
}

QString ControlPlugin::getPluginName()
{
  return QString::fromUtf8("Control wigdet (from plugin ;) )"); 
}

void ControlPlugin::setScene( osgViewer::Scene *scene )
{
  if(!_controlWidget) return; 
  _controlWidget->setScene(scene); 
}

Q_EXPORT_PLUGIN2(motion_controlplugin, ControlPlugin)