#ifndef CONTROL_PLUGIN_H
#define CONTROL_PLUGIN_H

#include <QObject>

#include <PluginsInterfaces.h>

class OsgControlWidget; 

class ControlPlugin : public QObject, public IControlPlugin
{
    Q_OBJECT
    Q_INTERFACES(IControlPlugin)
public:
    ControlPlugin(void);
    virtual ~ControlPlugin(void);

    // IControlPlugin 
    virtual void UnloadPlugin(); 
    virtual int GetDockWidgetsCount(); 
    virtual QWidget *GetDockWidget(int i);
    virtual QString GetPluginName(); 
    // Tymczasowa funkcja - pozniej zamiast tego bedzie serviceManager !!
    virtual void SetScene(osgViewer::Scene *scene); 
    virtual std::string GetPluginType();

    // 
private: 
    OsgControlWidget *_controlWidget;  
};

#endif
