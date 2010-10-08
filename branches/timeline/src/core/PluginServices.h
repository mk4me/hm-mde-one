#ifndef PLUGIN_SERVICE_H
#define PLUGIN_SERVICE_H


#include <string>
#include <deque>
#include <core/PluginsInterfaces.h>

#include <QtGui/QMainWindow>
#include <QtGui/QtGui>
#include <QtGui/QMenu>
#include <QtCore/QDir>
#include <QtGui/QAction>
#include <QtCore/QVector>

#include <core/GlobalServicesIDs.h>
#include <core/SimpleFunctors.h>
#include "ToolboxMain.h"

typedef std::deque<std::string> FilePathList;
typedef std::vector<ISystemPlugin* > PluginList;

class IModel;
class IDataManager;

//--------------------------------------------------------------------------------------------------
// Plugin Service
//--------------------------------------------------------------------------------------------------
class PluginService: public IBaseService
{
    UNIQUE_ID('PLUG','SRVC');
public:
    PluginService();
    virtual ~PluginService();

    virtual AsyncResult OnTick(double delta); 
    virtual AsyncResult OnAdded(IServiceManager* serviceManager); 

    virtual void SetModel(IDataManager* dataManager);

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