#ifndef PLUGIN_SERVICE_H
#define PLUGIN_SERVICE_H

#include <vector>
#include "Plugin.h"
#include <utils/SmartPtr.h>

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
		utils::shared_ptr<IApplication> coreApplication;
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
    //! Czyści pluginy
    void clear();
    //! Ładuje pluginy.
    void load();
	//! \param path Ścieżka gdzie szukać wtyczek
	void addPath(const Filesystem::Path& path)
	{
		this->paths.push_back(path);
	}

    //! \return Ilośc wczytanych pluginów
    int getNumPlugins() const
    {
        return static_cast<int>(plugins.size());
    }
    
    //! \param idx Indeks pluginu który pobieramy
    //! \return Plugin o zadanym indeksie
    PluginPtr getPlugin(int idx)
    {
        return plugins[idx].plugin;
    }
    
	//! \param idx Indeks pluginu który pobieramy
	//! \return Plugin o zadanym indeksie
    PluginConstPtr getPlugin(int idx) const
    {
        return plugins[idx].plugin;
    }
    //! \return Zbiór ścieżek w których poszukiwane będa pluginy
    const Paths& getPaths() const
    {
        return paths;
    }
    //! \param paths Zbiór ścieżek w których poszukiwane będa pluginy
    void setPaths(const Paths& paths)
    {
        this->paths = paths;
    }

	//! Wyładowuje wszystkie pluginy
    void unloadPlugins();

	//! \param path Ścieżka do pluginu
	//! \return Uchwyt do załadowanej biblioteki z pluginem
	static HMODULE loadSharedLibrary(const Filesystem::Path & path);
	//! \param library Uchwyt do zwalnianej biblioteki z pluginem
	static void unloadSharedLibrary(HMODULE library);
	//! \return Ostatni błąd
	static const std::string lastLoadSharedLibraryError();

	//! \tparam T Typ funkcji jaki próbujemy wyciągnąć z biblioteki
	//! \param library Uchwyt do zwalnianej biblioteki z pluginem
	//! \param procName Nazwa procedury o która pytamy
	template<typename T>
	static T loadProcedure(HMODULE library, const char * procName)
	{
		//UTILS_STATIC_ASSERT((std::is_member_function_pointer<T>::value || std::is_function<T>::value), "Casting type must be member funtion pointer or function");
		#if defined(__WIN32__)
			return reinterpret_cast<T>(::GetProcAddress(library, procName));
		#elif defined(__UNIX__)
			return reinterpret_cast<T>(dlsym(library, procName));
		#else
		#error "Unsupported system for unloading shared libraries"
			return 0;
		#endif
	}


private:
    //!
    //! \param path Ścieżka biblioteki z pluginem
    bool addPlugIn(const Filesystem::Path& path);

    //!
    //! \param library Uchwyt do zwalnianej biblioteki z pluginem
    //! \param path  Ścieżka biblioteki z pluginem
    //! \return Czy wersj apluginu jest akceptowalna
    bool checkPluginVersion( HMODULE library, const Filesystem::Path& path );

    //!
	//! \param library Uchwyt do zwalnianej biblioteki z pluginem
	//! \param path  Ścieżka biblioteki z pluginem
    //! \return Czy dany typ budowy pluginu jest obsługiwany
    bool checkPluginBuildType( HMODULE library, const Filesystem::Path& path );
    //!
	//! \param library Uchwyt do zwalnianej biblioteki z pluginem
	//! \param path  Ścieżka biblioteki z pluginem
	//! \return Czy wersje bibliotek są kompatybilne z corem
    bool checkLibrariesVersions( HMODULE library, const Filesystem::Path& path );
    //!
	//! \param library Uchwyt do zwalnianej biblioteki z pluginem
	//! \param path  Ścieżka biblioteki z pluginem
    //! \param createFunction Funkcja tworząca plugin
    bool onAddPlugin(PluginPtr plugin, HMODULE library, Plugin::InitializeAndLoadFunction fillFunction);
};

typedef utils::shared_ptr<PluginLoader> PluginLoaderPtr;

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////


#endif //PLUGIN_SERVICE_H
