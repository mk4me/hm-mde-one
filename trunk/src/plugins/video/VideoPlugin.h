/********************************************************************
	created:  2010/09/27
	created:  27:9:2010   11:49
	filename: VideoPlugin.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__VIDEOPLUGIN_H__
#define __HEADER_GUARD__VIDEOPLUGIN_H__

class VideoPlugin : public ISystemPlugin
{
public:
  virtual void UnloadPlugin(); 
  virtual int GetWidgetsCount(); 
  virtual IWidget* GetDockWidget(int i = 0);
  virtual POSITION GetWidgetPos(int i = 0);
  virtual std::string GetPluginName() = 0; 

  virtual void RegisterServices(IServiceManager *pServiceManager); 

  virtual void SetScene(osgViewer::Scene *scene); 
  virtual void SetScene(osgViewer::Scene *scene, IServiceManager *pServiceManager); 
  virtual std::string GetWidgetName(int i = 0);
};


#endif  // __HEADER_GUARD__VIDEOPLUGIN_H__