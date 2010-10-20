/********************************************************************
	created:  2010/10/07
	created:  7:10:2010   13:27
	filename: TimelinePlugin.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
/*#ifndef __HEADER_GUARD__TIMELINEPLUGIN_H__
#define __HEADER_GUARD__TIMELINEPLUGIN_H__

#include <string>
#include <core/PluginsInterfaces.h>
#include <plugins/timeline/TimelineService.h>

class TimelinePlugin : public ISystemPlugin
{
private:
    //! Nazwa pluginu.
    std::string name;
    //!
    TimelineService* service;

public:
    TimelinePlugin();
    virtual ~TimelinePlugin();

    virtual void UnloadPlugin(); 
    virtual int GetWidgetsCount(); 
    virtual IWidget* GetDockWidget(int i = 0);
    virtual POSITION GetWidgetPos(int i = 0);
    virtual const std::string& GetPluginName(); 

    virtual void RegisterServices(IServiceManager *serviceManager); 

    virtual std::string GetWidgetName(int i = 0);
};


#endif  // __HEADER_GUARD__TIMELINEPLUGIN_H__*/