#ifndef PLUGININTERFACES_H
#define PLUGININTERFACES_H

#include <QtPlugin>
#include "Enumerators.h"

class IWidget;
class IServiceManager;
class ServiceManager;

namespace osgViewer { class Scene; }

class ISystemPlugin
{
public:
  virtual ~ISystemPlugin() {}

  virtual void UnloadPlugin() = 0; 
  virtual int GetWidgetsCount() = 0; 
  virtual IWidget* GetDockWidget(int i = 0) = 0;
  virtual POSITION GetWidgetPos(int i = 0) = 0;
  virtual std::string GetPluginName() = 0; 

  virtual void RegisterServices(IServiceManager *pServiceManager) = 0; 

  virtual void SetScene(osgViewer::Scene *scene) = 0; 
  virtual void SetScene(osgViewer::Scene *scene, IServiceManager *pServiceManager) = 0; 
  virtual std::string GetWidgetName(int i = 0) = 0;
};


#endif // PLUGININTERFACES_H
