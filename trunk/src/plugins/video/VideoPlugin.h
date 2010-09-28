/********************************************************************
	created:  2010/09/27
	created:  27:9:2010   11:49
	filename: VideoPlugin.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__VIDEOPLUGIN_H__
#define __HEADER_GUARD__VIDEOPLUGIN_H__

#include <string>
#include <osgViewer/Scene>
#include <core/PluginsInterfaces.h>

class VideoPlugin : public ISystemPlugin
{
private:
  //! Nazwa pluginu.
  std::string name;

public:
  VideoPlugin();
  virtual ~VideoPlugin();

  virtual void UnloadPlugin(); 
  virtual int GetWidgetsCount(); 
  virtual IWidget* GetDockWidget(int i = 0);
  virtual POSITION GetWidgetPos(int i = 0);
  virtual const std::string& GetPluginName(); 

  virtual void RegisterServices(IServiceManager *serviceManager); 

  virtual void SetScene(osgViewer::Scene *scene); 
  virtual void SetScene(osgViewer::Scene *scene, IServiceManager *serviceManager); 
  virtual std::string GetWidgetName(int i = 0);
};


#endif  // __HEADER_GUARD__VIDEOPLUGIN_H__