#ifndef PLUGININTERFACES_H
#define PLUGININTERFACES_H

#include <QtPlugin>

QT_BEGIN_NAMESPACE
class QWidget;
class QString; 
QT_END_NAMESPACE

namespace osgViewer { class Scene; }

class IControlPlugin
{
public:
  virtual ~IControlPlugin() {}

  virtual void UnloadPlugin() = 0; 
  virtual int GetDockWidgetsCount() = 0; 
  virtual QWidget* GetDockWidget(int i) = 0;
  virtual QString GetPluginName() = 0; 

  // Tymczasowa funkcja - pozniej zamiast tego bedzie serviceManager !!
  virtual void SetScene(osgViewer::Scene *scene) = 0; 
};


QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(IControlPlugin, "com.motion.plugins.ControlPlugin/1.0")
QT_END_NAMESPACE

#endif