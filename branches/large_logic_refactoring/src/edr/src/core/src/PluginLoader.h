#ifndef PLUGIN_SERVICE_H
#define PLUGIN_SERVICE_H

#include <vector>
#include "Plugin.h"
#include <corelib/SmartPtr.h>

// rev
#ifdef WIN32
#   include "Windows.h"
#else
#   define HMODULE void *
#endif

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

class PluginLoader
{
public:
    //! Ścieżki wyszukiwania.
    typedef std::list<Filesystem::Path> Paths;

private:

	//! Struktura opisująca załadowany plugin
	struct PluginData {
		//! Wskaźnik do pluginu
		PluginPtr plugin;
		//! Wskaźnik do dedykowanej implementacji interfejcu core::IApplication
		shared_ptr<IApplication> coreApplication;
		//! Uchwyt do biblioteki
		HMODULE handle;
	};

    //! Załadowane pluginy.
    typedef std::vector<PluginData> Plugins;

private:
    //! Załadowane pluginy.
    Plugins plugins;
    //! Ścieżki wyszukiwania.
    Paths paths;

public:
    PluginLoader(const Filesystem::Path & pluginsPath);
    virtual ~PluginLoader();

public:
    //!
    void clear();
    //! Ładuje pluginy.
    void load();

	void addPath(const Filesystem::Path& path)
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
    PluginPtr getPlugin(int idx)
    {
        return plugins[idx].plugin;
    }
    //!
    //! \param idx
    PluginConstPtr getPlugin(int idx) const
    {
        return plugins[idx].plugin;
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

	static HMODULE loadSharedLibrary(const Filesystem::Path & path);
	static void unloadSharedLibrary(HMODULE library);
	static const std::string lastLoadSharedLibraryError();

	template<typename T>
	static T loadProcedure(HMODULE library, const char * procName)
	{
		//UTILS_STATIC_ASSERT((std::is_member_function_pointer<T>::value || std::is_function<T>::value), "Casting type must be member funtion pointer or function");
		#if defined(__WIN32__)
			return reinterpret_cast<T>(::GetProcAddress(library, procName));
		#elif defined(__UNIX__)
			return reinterpret_cast<Plugin::CreateFunction>(dlsym(library, procName));
		#else
		#error "Unsupported system for unloading shared libraries"
			return 0;
		#endif
	}


private:
    //!
    //! \param path
    bool addPlugIn(const Filesystem::Path& path);

    //!
    //! \param library
    //! \param path
    bool checkPluginVersion( HMODULE library, const Filesystem::Path& path );

    //!
    //! \param library
    //! \param path
    bool checkPluginBuildType( HMODULE library, const Filesystem::Path& path );
    //!
    //! \param library
    //! \param path
    bool checkLibrariesVersions( HMODULE library, const Filesystem::Path& path );
    //!
    //! \param path
    //! \param library
    //! \param createFunction
    bool onAddPlugin(PluginPtr plugin, HMODULE library, Plugin::FillFunction fillFunction);
};

typedef shared_ptr<PluginLoader> PluginLoaderPtr;

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////


#endif //PLUGIN_SERVICE_H
