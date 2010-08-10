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

#include "GlobalServicesIDs.h"
#include "SimpleFunctors.h"
#include "ToolboxMain.h"

typedef std::deque<std::string> FilePathList;
typedef std::vector<IControlPlugin* > PluginList;

//--------------------------------------------------------------------------------------------------
// Plugin Service
//--------------------------------------------------------------------------------------------------
class PluginService: public IBaseService
{
public:
    PluginService();
    virtual ~PluginService();

    virtual AsyncResult OnTick(double delta); 
    // 
    virtual AsyncResult OnAdded(); 

protected: 


private: 
    M_DECLARE_CLASS(); 

    // clear all
    void Clear();
    void convertStringPathIntoFileDirList(const std::string& paths,FilePathList& filepath);
    FilePathList& GetPathPlugin();
    void SetPathPlugin();
    void LoadPlugins();
    std::string getRealPath(const std::string& path, const std::string& fileName);
    PluginList& GetPluginList();

    std::string GetPlugIn();
    bool AddPlugIn(std::string pluginPath/*, std::string pluginName*/);
    std::string getSimpleFileName(const std::string& fileName);

 
    PluginList _PluginList;
    FilePathList _PluginFileDirList;
};


#endif //PLUGIN_SERVICE_H