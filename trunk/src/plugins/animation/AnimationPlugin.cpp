#include "AnimationPlugin.h"

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
#include <plugins/animation/AnimationService.h>

//-------- DllMain.cpp --------//
#include <core/DllExports.h>


PLUGIN_REGISTER(AnimationPlugin)

//--------------------------------------------------------------------------------------------------
AnimationPlugin::AnimationPlugin(void)
{
    _name = "Animation Service"; 

    QtWidget* widget = new QtWidget();
    _controlWidgetList.push_back(widget);
}

//--------------------------------------------------------------------------------------------------
AnimationPlugin::~AnimationPlugin(void)
{
    UnloadPlugin(); 
}

//--------------------------------------------------------------------------------------------------
void AnimationPlugin::UnloadPlugin()
{
    _controlWidgetList.clear();
    _pluginDataList.clear();
}

//--------------------------------------------------------------------------------------------------
int AnimationPlugin::GetWidgetsCount()
{
    return _controlWidgetList.size();
}

//--------------------------------------------------------------------------------------------------
IWidget *AnimationPlugin::GetDockWidget( int i )
{
    if((_controlWidgetList.size()) > i)
        return (IWidget*)_controlWidgetList[i]->GetWidget();

    return NULL;
}

//--------------------------------------------------------------------------------------------------
POSITION AnimationPlugin::GetWidgetPos( int i /*= 0*/ )
{
    if((_pluginDataList.size()) > i)
        return _pluginDataList[i]->GetPosition();

    return RIGHT;
}

//--------------------------------------------------------------------------------------------------
std::string AnimationPlugin::GetPluginName()
{
    return _name;
}

//--------------------------------------------------------------------------------------------------
void AnimationPlugin::RegisterServices(IServiceManager *pServiceManager )
{
    AnimationService* animationService = new AnimationService();
    pServiceManager->RegisterServiceAs(animationService, AnimationService::CLASS_ID);
}

//--------------------------------------------------------------------------------------------------
void AnimationPlugin::SetScene( osgViewer::Scene *scene )
{
    for(std::vector<QtWidget* >::const_iterator it = _controlWidgetList.begin(); it != _controlWidgetList.end(); it++)
    {
        (*it)->SetData(scene);
    }
}

//--------------------------------------------------------------------------------------------------
void AnimationPlugin::SetScene( osgViewer::Scene *scene, IServiceManager *pServiceManager )
{
    for(std::vector<QtWidget* >::const_iterator it = _controlWidgetList.begin(); it != _controlWidgetList.end(); it++)
    {
        (*it)->SetData(scene,pServiceManager);
    }
}
//--------------------------------------------------------------------------------------------------
std::string AnimationPlugin::GetWidgetName(int i)
{
    if(_pluginDataList.size() > i)
        return _pluginDataList[i]->GetName();

    return "GRANT-PLUGIN1";
}
