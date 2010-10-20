#include <plugins/animation/AnimationService.h>
#include <core/Plugin.h>

CORE_PLUGIN_BEGIN("Animation", UniqueID('ANIM', 'PLUG'))
CORE_PLUGIN_ADD_SERVICE(AnimationService)
CORE_PLUGIN_END

#if 0

#include "AnimationPlugin.h"

#include <boost/lexical_cast.hpp>

#include <QtGui/QWidget>
#include <QtCore/QString>

#include <osgViewer/Scene>
#include <osg/Group>
#include <osg/Geode>
#include <osg/NodeVisitor>

#include "QtWidget.h"
#include "PluginData.h"
#include <core/PluginDataInterface.h>
#include <core/WidgetInterface.h>
#include <core/IServiceManager.h>
//-------- DllMain.cpp --------//
#include <core/DllExports.h>

PLUGIN_REGISTER(AnimationPlugin)

//--------------------------------------------------------------------------------------------------
AnimationPlugin::AnimationPlugin(void)
: name("Animation Plugin")
{
    service = new AnimationService();
//     _name = "Animation Service"; 
// 
//     QtWidget* widget = new QtWidget();
//     _controlWidgetList.push_back(widget);
}

//--------------------------------------------------------------------------------------------------
AnimationPlugin::~AnimationPlugin(void)
{
    UnloadPlugin(); 
}

//--------------------------------------------------------------------------------------------------
void AnimationPlugin::UnloadPlugin()
{
    delete service;
}

//--------------------------------------------------------------------------------------------------
int AnimationPlugin::GetWidgetsCount()
{
    return 1;
}

//--------------------------------------------------------------------------------------------------
IWidget *AnimationPlugin::GetDockWidget( int i )
{
    return service->getWidget();
}

//--------------------------------------------------------------------------------------------------
POSITION AnimationPlugin::GetWidgetPos( int i /*= 0*/ )
{
    return RIGHT;
}

//--------------------------------------------------------------------------------------------------
const std::string& AnimationPlugin::GetPluginName()
{
    return name;
}

//--------------------------------------------------------------------------------------------------
void AnimationPlugin::RegisterServices(IServiceManager *pServiceManager )
{
    pServiceManager->registerService(service);
}

//--------------------------------------------------------------------------------------------------
std::string AnimationPlugin::GetWidgetName(int i)
{
    return boost::lexical_cast<std::string>(i);
}
#endif