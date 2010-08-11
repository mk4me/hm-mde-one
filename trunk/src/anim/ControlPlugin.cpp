#include "ControlPlugin.h"

#include <QtGui/QWidget>
#include <QtCore/QString>

#include <osgViewer/Scene>
#include <osg/Group>
#include <osg/Geode>
#include <osg/NodeVisitor>

#include "OsgControlWidget.h"

//-------- DllMain.cpp --------//
#define __dll__
#include "DllExports.h"


PLUGIN_REGISTER(ControlPlugin)

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
std::string ControlPlugin::GetPluginName()
{
    return std::string("Control wigdet (from plugin ver ;) )"); 
}

//--------------------------------------------------------------------------------------------------
void ControlPlugin::SetScene( osgViewer::Scene *scene )
{
    if(!_controlWidget) return; 
    _controlWidget->SetScene(scene); 
}

//--------------------------------------------------------------------------------------------------
std::string ControlPlugin::GetPluginType()
{
    return "GRANT-PLUGIN2";
}
