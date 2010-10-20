#ifndef PLUGIN_SERVICE_H
#define PLUGIN_SERVICE_H


#include <string>
#include <deque>

#include <boost/shared_ptr.hpp>
#include <core/PluginsInterfaces.h>
#include <core/Plugin.h>


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
class PluginService
{
public:
    //! Za³adowane pluginy.
    typedef std::vector<core::Plugin*> Plugins;
    //! Uchwyty do bibliotek dynamicznie ³adowanych.
    typedef std::vector<uint32_t> Handles;
    //! Œcie¿ki wyszukiwania.
    typedef std::deque<std::string> Paths;

private:
    //! Uchwyty do bibliotek dynamicznie ³adowanych.
    Handles libraries;
    //! Za³adowane pluginy.
    Plugins plugins;
    //! Œcie¿ki wyszukiwania.
    Paths paths;

public:
    PluginService();
    virtual ~PluginService();

public:
    //! 
    void clear();
    //! 
    void addDefaultPaths();
    //! £aduje pluginy.
    void load();
    //! \return Œcie¿ki wyszukiwania.
    Paths& getPaths();
    //! \return Œcie¿ki wyszukiwania.
    const Paths& getPaths() const;

    //! \return
    size_t getNumPlugins() const
    {
        return plugins.size();
    }
    //! 
    //! \param idx
    core::Plugin* getPlugin(size_t idx)
    {
        return plugins[idx];
    }
    //! 
    //! \param idx
    const core::Plugin* getPlugin(size_t idx) const
    {
        return plugins[idx];
    }

private:
    //! 
    //! \param path
    bool addPlugIn(const std::string& path);
    //! 
    //! \param path
    //! \param library
    //! \param createFunction
    bool onAddPlugin(const std::string& path, uint32_t library, core::Plugin::CreateFunction createFunction);
    //! 
    //! \param paths
    //! \param filepath
    void convertStringPathIntoFileDirList(const std::string& paths, Paths& filepath);
    //! 
    //! \param path
    //! \param fileName
    std::string combinePath(const std::string& path, const std::string& fileName);
    //! 
    //! \param fileName
    std::string getFileName(const std::string& fileName);
};

#endif //PLUGIN_SERVICE_H