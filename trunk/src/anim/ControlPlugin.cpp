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

#ifdef __WIN32__

int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void*)
{
    return 1;
}

IMPEXP IControlPlugin* CreateConrolPluginInstance()
{
    return (IControlPlugin*)(new ControlPlugin());
}

#endif

#ifdef __UNIX__
extern "C" IControlPlugin* CreateControlPluginInstance()
{
	return (IControlPlugin*)(new ControlPlugin());
}
#endif

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
    return QString::fromUtf8("Control wigdet (from plugin ver 2 ;) )"); 
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
Q_EXPORT_PLUGIN2(motion_controlplugin, ControlPlugin)
