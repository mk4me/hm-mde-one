#ifndef PLUGIN_SERVICE_H
#define PLUGIN_SERVICE_H

#include <vector>
#include <string>
#include <deque>
#include <core/Plugin.h>
#include <core/SmartPtr.h>

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

class PluginLoader
{
public:
    //! Za�adowane pluginy.
    typedef std::vector<PluginPtr> Plugins;
    //! Uchwyty do bibliotek dynamicznie �adowanych.
    typedef std::vector<uint32_t> Handles;
    //! �cie�ki wyszukiwania.
    typedef std::deque<std::string> Paths;

private:
    //! Uchwyty do bibliotek dynamicznie �adowanych.
    static Handles libraries;
    //! Za�adowane pluginy.
    Plugins plugins;
    //! �cie�ki wyszukiwania.
    Paths paths;

public:
    PluginLoader();
    virtual ~PluginLoader();

public:
    //! Zwalnia biblioteki. Mo�na to wywo�a� dopiero po zniszczeniu g��wnego okna.
    static void freeLibraries();
    //! 
    void clear();
    //! 
    void addDefaultPaths();
    //! �aduje pluginy.
    void load();

    //! \return
    size_t getNumPlugins() const
    {
        return plugins.size();
    }
    //! 
    //! \param idx
    PluginPtr getPlugin(size_t idx)
    {
        return plugins[idx];
    }
    //! 
    //! \param idx
    const PluginPtr getPlugin(size_t idx) const
    {
        return plugins[idx];
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


private:
    //! 
    //! \param path
    bool addPlugIn(const std::string& path);
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
};

typedef CORE_SHARED_PTR(PluginLoader) PluginLoaderPtr;

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////


#endif //PLUGIN_SERVICE_H