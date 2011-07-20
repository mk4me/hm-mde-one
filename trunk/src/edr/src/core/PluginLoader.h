#ifndef PLUGIN_SERVICE_H
#define PLUGIN_SERVICE_H

#include <vector>
#include <string>
#include <deque>
#include <core/Plugin.h>
#include <core/SmartPtr.h>
#include <QtOpenGL/QtOpenGL>

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

class PluginLoader
{
public:
    //! Œcie¿ki wyszukiwania.
    typedef std::deque<std::string> Paths;

private:

    typedef std::pair<PluginPtr, PluginConstPtr> PluginPair;
    //! Za³adowane pluginy.
    typedef std::vector<PluginPair> Plugins;
    //! Uchwyty do bibliotek dynamicznie ³adowanych.
    typedef std::vector<uint32_t> Handles;

private:
    //! Uchwyty do bibliotek dynamicznie ³adowanych.
    Handles libraries;
    //! Za³adowane pluginy.
    Plugins plugins;
    //! Œcie¿ki wyszukiwania.
    Paths paths;

public:
    PluginLoader();
    virtual ~PluginLoader();

public:
    //! 
    void clear();
    //! 
    void addDefaultPaths();
    //! £aduje pluginy.
    void load();

	void addPath(const std::string& path)
	{
		this->paths.push_back(path);
	}

    //! \return
    int getNumPlugins() const
    {
        return static_cast<int>(plugins.size());
    }
    //! 
    //! \param idx
    const PluginPtr & getPlugin(int idx)
    {
        return plugins[idx].first;
    }
    //! 
    //! \param idx
    const PluginConstPtr & getPlugin(int idx) const
    {
        return plugins[idx].second;
    }
    //! \return
    const Paths& getPaths() const
    { 
        return paths;
    }
    //! \param paths
    void setPaths(const Paths& paths) 
    { 
        this->paths = paths; 
    }

    void unloadPlugins();


private:
    //! 
    //! \param path
    bool addPlugIn(const std::string& path);

    //! 
    //! \param library
    //! \param path
    bool checkPluginVersion( HMODULE library, const std::string& path );
    //! 
    //! \param library
    //! \param path
    bool checkLibrariesVersions( HMODULE library, const std::string& path );
    //! 
    //! \param path
    //! \param library
    //! \param createFunction
    bool onAddPlugin(const std::string& path, uint32_t library, Plugin::CreateFunction createFunction);
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

    //! Zwalnia biblioteki. Mo¿na to wywo³aæ dopiero po zniszczeniu g³ównego okna.
    void freeLibraries();
};

typedef shared_ptr<PluginLoader> PluginLoaderPtr;

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////


#endif //PLUGIN_SERVICE_H