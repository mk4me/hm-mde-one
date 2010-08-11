#ifndef PLUGININTERFACES_H
#define PLUGININTERFACES_H

#include <QtPlugin>

QT_BEGIN_NAMESPACE
class QWidget;
QT_END_NAMESPACE

namespace osgViewer { class Scene; }

class IControlPlugin
{
public:
  virtual ~IControlPlugin() {}

  virtual void UnloadPlugin() = 0; 
  virtual int GetDockWidgetsCount() = 0; 
  virtual QWidget* GetDockWidget(int i) = 0;
  virtual std::string GetPluginName() = 0; 

  // Tymczasowa funkcja - pozniej zamiast tego bedzie serviceManager !!
  virtual void SetScene(osgViewer::Scene *scene) = 0; 
  virtual std::string GetPluginType() = 0;
};


#endif // PLUGININTERFACES_H