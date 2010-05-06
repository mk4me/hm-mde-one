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
  virtual void unloadPlugin(); 
  virtual int getDockWidgetsCount(); 
  virtual QWidget *getDockWidget(int i);
  virtual QString getPluginName(); 

  // Tymczasowa funkcja - pozniej zamiast tego bedzie serviceManager !!
  virtual void setScene(osgViewer::Scene *scene); 

  // 
private: 
  OsgControlWidget *_controlWidget;  
};

#endif
