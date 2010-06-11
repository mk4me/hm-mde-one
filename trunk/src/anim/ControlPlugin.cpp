#include "ControlPlugin.h"

#include <QtGui/QWidget>
#include <QtCore/QString>

#include <osgViewer/Scene>
#include <osg/Group>
#include <osg/Geode>
#include <osg/NodeVisitor>

#include "OsgControlWidget.h"

//--------------------------------------------------------------------------------------------------
ControlPlugin::ControlPlugin(void):
_controlWidget(0)
{
}

//--------------------------------------------------------------------------------------------------
ControlPlugin::~ControlPlugin(void)
{
    UnloadPlugin(); 
}

//--------------------------------------------------------------------------------------------------
void ControlPlugin::UnloadPlugin()
{
    if(_controlWidget != NULL)
    {
        delete _controlWidget; 
        _controlWidget = 0; 
    }
}

//--------------------------------------------------------------------------------------------------
int ControlPlugin::GetDockWidgetsCount()
{
    return 1; 
}

//--------------------------------------------------------------------------------------------------
QWidget *ControlPlugin::GetDockWidget( int i )
{
    if(!_controlWidget) 
        _controlWidget = new OsgControlWidget(); 
    return (QWidget *)_controlWidget; 
}

//--------------------------------------------------------------------------------------------------
QString ControlPlugin::GetPluginName()
{
    return QString::fromUtf8("Control wigdet (from plugin ;) )"); 
}

//--------------------------------------------------------------------------------------------------
void ControlPlugin::SetScene( osgViewer::Scene *scene )
{
    if(!_controlWidget) return; 
    _controlWidget->SetScene(scene); 
}

Q_EXPORT_PLUGIN2(motion_controlplugin, ControlPlugin)