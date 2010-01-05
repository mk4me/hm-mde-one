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

  virtual void unloadPlugin() = 0; 
  virtual int getDockWidgetsCount() = 0; 
  virtual QWidget *getDockWidget(int i) = 0;
  virtual QString getPluginName() = 0; 

  // Tymczasowa funkcja - pozniej zamiast tego bedzie serviceManager !!
  virtual void setScene(osgViewer::Scene *scene) = 0; 
};


QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(IControlPlugin, "com.motion.plugins.ControlPlugin/1.0")
QT_END_NAMESPACE

#endif