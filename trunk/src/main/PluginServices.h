#ifndef PLUGIN_SERVICE_H
#define PLUGIN_SERVICE_H


#include <string>
#include <deque>
#include <PluginsInterfaces.h>

#include <QtGui/QMainWindow>
#include <QtGui>
#include <QMenu>
#include <QDir>
#include <QAction>
#include <QVector>

#include "../include/GlobalServicesIDs.h"
#include "../include/SimpleFunctors.h"
#include "ToolboxMain.h"

typedef std::deque<std::string> FilePathList;
typedef std::vector<ISystemPlugin* > PluginList;


//--------------------------------------------------------------------------------------------------
// Plugin Service
//--------------------------------------------------------------------------------------------------
class PluginService: public IBaseService
{
public:
    PluginService();
    virtual ~PluginService();

    virtual AsyncResult OnTick(double delta); 
    virtual AsyncResult OnAdded(IServiceManager* serviceManager); 

private: 
    M_DECLARE_CLASS(); 

    void convertStringPathIntoFileDirList(const std::string& paths,FilePathList& filepath);
    void SetPathPlugin();
    void LoadPlugins();
    void Clear();

    std::string getRealPath(const std::string& path, const std::string& fileName);
    std::string getSimpleFileName(const std::string& fileName);
    std::string GetPlugIn();

    bool AddPlugIn(std::string pluginPath/*, std::string pluginName*/);

    FilePathList& GetPathPlugin();
    PluginList& GetPluginList();

    PluginList m_pluginList;
    FilePathList m_pluginFileDirList;

    ServiceManager* m_pServiceManager;
};

#endif //PLUGIN_SERVICE_H