/********************************************************************
	created:  2010/10/28
	created:  28:10:2010   23:45
	filename: ChartPlugin.h
	author:	  Michal Szafarski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__CHARTPLUGIN_H__
#define __HEADER_GUARD__CHARTPLUGIN_H__
#if 0
#include <string>
#include <osgViewer/Scene>
#include <core/PluginsInterfaces.h>
#include <plugins/chart/ChartService.h>

class VideoPlugin : public ISystemPlugin
{
private:
  //! Nazwa pluginu.
  std::string name;
  //!
  ChartService* service;

public:
  ChartPlugin();
  virtual ~ChartPlugin();

  virtual void UnloadPlugin(); 
  virtual int GetWidgetsCount(); 
  virtual IWidget* GetDockWidget(int i = 0);
  virtual POSITION GetWidgetPos(int i = 0);
  virtual const std::string& GetPluginName(); 

  virtual void RegisterServices(IServiceManager *serviceManager); 
  virtual std::string GetWidgetName(int i = 0);
};


#endif  // __HEADER_GUARD__VIDEOPLUGIN_H__
#endif