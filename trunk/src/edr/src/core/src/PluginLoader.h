#ifndef PLUGIN_SERVICE_H
#define PLUGIN_SERVICE_H

#include <vector>
#include <string>
#include <deque>
#include <core/Plugin.h>
#include <core/SmartPtr.h>
#include <utils/Debug.h>
#include <type_traits>


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
    typedef std::deque<std::string> Paths;

private:

    typedef std::pair<PluginPtr, PluginConstPtr> PluginPair;
    //! Załadowane pluginy.
    typedef std::vector<PluginPair> Plugins;
    //! Uchwyty do bibliotek dynamicznie ładowanych.
    typedef std::vector<HMODULE> Handles;

private:
    //! Uchwyty do bibliotek dynamicznie ładowanych.
    Handles libraries;
    //! Załadowane pluginy.
    Plugins plugins;
    //! Ścieżki wyszukiwania.
    Paths paths;

public:
    PluginLoader();
    virtual ~PluginLoader();

public:
    //!
    void clear();
    //!
    void addDefaultPaths();
    //! Ładuje pluginy.
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

	static HMODULE loadSharedLibrary(const std::string & path);
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
    bool addPlugIn(const std::string& path);

    //!
    //! \param library
    //! \param path
    bool checkPluginVersion( HMODULE library, const std::string& path );

    //!
    //! \param library
    //! \param path
    bool checkPluginBuildType( HMODULE library, const std::string& path );
    //!
    //! \param library
    //! \param path
    bool checkLibrariesVersions( HMODULE library, const std::string& path );
    //!
    //! \param path
    //! \param library
    //! \param createFunction
    bool onAddPlugin(const std::string& path, HMODULE library, Plugin::CreateFunction createFunction);
    //!
    //! \param fileName
    std::string getFileName(const std::string& fileName);

    //! Zwalnia biblioteki. Można to wywołać dopiero po zniszczeniu głównego okna.
    void freeLibraries();
};

typedef shared_ptr<PluginLoader> PluginLoaderPtr;

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////


#endif //PLUGIN_SERVICE_H