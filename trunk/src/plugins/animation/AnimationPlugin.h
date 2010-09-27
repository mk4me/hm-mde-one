#ifndef ANIMATION_PLUGIN_H
#define ANIMATION_PLUGIN_H

#include <QtCore/QObject>
#include <core/PluginsInterfaces.h>
#include <vector>
#include <string>

class IWidget;
class OsgControlWidget; 
class PluginData;
class QtWidget;

class AnimationPlugin : public ISystemPlugin
{
public:
    AnimationPlugin(void);
    virtual ~AnimationPlugin(void);

    // ISystemPlugin 
    virtual void UnloadPlugin(); 
    virtual int GetWidgetsCount(); 
    virtual IWidget *GetDockWidget(int i = 0);
    virtual POSITION GetWidgetPos(int i = 0);
    virtual const std::string& GetPluginName(); 
    virtual void RegisterServices(IServiceManager *pServiceManager);

    virtual void SetScene(osgViewer::Scene *scene); 
    virtual void SetScene(osgViewer::Scene *scene, IServiceManager *pServiceManager);
    virtual std::string GetWidgetName(int i = 0);

    // 
private: 
    std::string _name;
    std::vector<QtWidget* >_controlWidgetList;  
    std::vector<PluginData* > _pluginDataList;
};

#endif //ANIMATION_PLUGIN_H
