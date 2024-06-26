#include "CorePCH.h"
#if defined(__WIN32__)
#include <windows.h>
#elif defined(__UNIX__)
#include <stdio.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#endif

#include <corelib/Version.h>
#include "PluginLoader.h"
#include <corelib/IPlugin.h>
#include <corelib/PluginCommon.h>
#include "PluginApplication.h"
#include "ApplicationCommon.h"
#include <regex>

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

PluginLoader::PluginLoader(const Filesystem::Path & pluginsPath)
{
    paths.push_back(pluginsPath);
}

PluginLoader::~PluginLoader()
{
    unloadPlugins();
}

void PluginLoader::clear()
{
    // wyczyszczenie ścieżek
    Paths().swap(paths);

	for(auto it = plugins.begin(); it != plugins.end(); ++it)
	{
		CORE_LOG_DEBUG("Unloading plugin " << (*it).plugin->ID() << " name " << (*it).plugin->name() << " from " << (*it).plugin->getPath());
		(*it).plugin.reset();
		unloadSharedLibrary((*it).handle);
		(*it).coreApplication.reset();
		CORE_LOG_DEBUG("Plugin unloaded successfully");
	}

	Plugins().swap(plugins);
}

void PluginLoader::unloadPlugins()
{
    try{
        clear();
    }
    catch(std::exception& e){
        CORE_LOG_ERROR("PluginLoader: Error unloading plugins " << e.what());
    }
    catch(...){
        CORE_LOG_ERROR("PluginLoader: UNKNOWN error unloading plugins");
    }
}

void PluginLoader::load()
{

#if defined(__WIN32__)
	static const std::regex pluginFilter("^plugin_.*\\.dll$");
#elif defined(__UNIX__)
    // GCC cos nie radzi sobie ze znakami początku i końca linii !?
	static const std::regex pluginFilter("libplugin_.*\\.so");
#endif

	for(auto pathIT = paths.begin(); pathIT != paths.end(); ++pathIT) {
		auto localFiles = core::Filesystem::listFiles(*pathIT, true);

		for(auto fileIT = localFiles.begin(); fileIT != localFiles.end(); ++fileIT){

			// Skip if no match
			if( !std::regex_match( (*fileIT).leaf().string(), pluginFilter) ) continue;

			addPlugIn(*fileIT);
		}
	}
}

HMODULE PluginLoader::loadSharedLibrary(const Filesystem::Path & path)
{
#if defined(__WIN32__)
	return ::LoadLibrary( path.string().c_str() );
#elif defined(__UNIX__)
	return dlopen(path.string().c_str(), RTLD_LAZY);
#else
#error "Unsupported system for loading shared libraries"
	return 0;
#endif
}

void PluginLoader::unloadSharedLibrary(HMODULE library)
{
	if(library){
#if defined(__WIN32__)
		FreeLibrary(library);
#elif defined(__UNIX__)
		dlclose(library);
#else
	#error "Unsupported system for unloading shared libraries"
#endif
	}
}

const std::string PluginLoader::lastLoadSharedLibraryError()
{
#if defined(__WIN32__)
	DWORD err = ::GetLastError();
	LPSTR errStr;
	if (::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, nullptr, err, 0, reinterpret_cast<LPSTR>(&errStr), 0, nullptr)) {
		// usuwanie CRLF
		LPSTR p = strchr(errStr, '\r');
		if ( p ) {
			*p = '\0';
		}

		std::string ret(errStr);

		::LocalFree(errStr);

		return ret + " Error code: " + boost::lexical_cast<std::string>(err);
	}

	return std::string("Unknown error");
#elif defined(__UNIX__)
	return std::string(dlerror());
#else
#error "Unsupported system for loading shared libraries"
	return std::string("Unknown error");
#endif
}

bool PluginLoader::addPlugIn( const Filesystem::Path& path )
{
    HMODULE library = loadSharedLibrary(path.string());
    if ( library != 0) {
        try{
            if (checkPluginVersion(library, path) && checkLibrariesVersions(library, path) && checkPluginBuildType(library, path) ) {
                PluginPtr plugin(new Plugin());
				plugin->setPath(path);
				auto procSetDescription = loadProcedure<Plugin::SetPluginDescriptionFunction>(library, STRINGIZE(CORE_INITIALIZE_PLUGIN_DESCRIPTION_FUNCTION_NAME));
				if( procSetDescription != 0 ){
					procSetDescription(plugin.get());
				}else{
					CORE_LOG_DEBUG(path << " is a plugin, but finding " << STRINGIZE(CORE_INITIALIZE_PLUGIN_DESCRIPTION_FUNCTION_NAME) << " failed.");
					return false;
				}

				auto procInitializeAndLoad = loadProcedure<Plugin::InitializeAndLoadFunction>(library, STRINGIZE(CORE_INITIALIZE_AND_LOAD_PLUGIN_FUNCTION_NAME));
                if ( procInitializeAndLoad != 0) {
                    return onAddPlugin(plugin, library, procInitializeAndLoad);
                } else {
                    CORE_LOG_DEBUG(path << " is a plugin, but finding " << STRINGIZE(CORE_INITIALIZE_AND_LOAD_PLUGIN_FUNCTION_NAME) << " failed.");
                }
            }
        }catch(std::exception & e){
            CORE_LOG_DEBUG(path << " is a plugin, but trying to check version or libraries failed with error: " << e.what());
        }catch(...){
            CORE_LOG_DEBUG(path << " is a plugin, but trying to check version or libraries failed with UNKNOWN error");
        }
    } else  {
        CORE_LOG_ERROR("Error \"" << lastLoadSharedLibraryError() << "\" during loading " << path << ".");
    }

	unloadSharedLibrary(library);
    return false;
}

bool PluginLoader::onAddPlugin( PluginPtr plugin, HMODULE library, Plugin::InitializeAndLoadFunction initializeAndLoadFunction )
{
	PluginData pData;
    CORE_LOG_INFO("Loading plugin " << plugin->getPath());

	auto pluginName = plugin->name();
	if(pluginName.empty() == true){
		plugin->setName(plugin->getPath().stem().string());
		CORE_LOG_WARNING("Plugin name for plugin loaded from " << plugin->getPath() << " was empty. Setting dynamic library file name as plugin name: " << plugin->name() );
	}

	pData.coreApplication.reset(new PluginApplication(plugin->name()));

    // próba załadowania zawartosci pluginu i zainicjowania kontekstu aplikacji
    try {
		
		initializeAndLoadFunction(plugin.get(), pData.coreApplication.get());
        
    } catch ( std::exception& ex ) {
        CORE_LOG_ERROR("Error loading plugin content and initializing appliaction context for " << plugin->getPath() << ": " << ex.what());
        return false;
    } catch ( ... ) {
        CORE_LOG_ERROR("Unknown error loading plugin content and initializing appliaction context for " << plugin->getPath());
        return false;
    }

	if(plugin->empty() == true){
		CORE_LOG_INFO("Skipping loading of an empty plugin " << plugin->getPath());
		return false;
	}

	pData.plugin = plugin;		
	pData.handle = library;

    bool pluginIDFound = false;
    core::PluginPtr collidingPlugin;
    //szukamy pluginu o podanym ID - jeśli nie ma ladujemy, w przeciwnym wypadku info i nie dodajemy
    for(auto it = plugins.begin(); it != plugins.end(); ++it){
        if( (*it).plugin->ID() == pData.plugin->ID()){
            pluginIDFound = true;
            collidingPlugin = (*it).plugin;
            break;
        }
    }

    if(pluginIDFound == false){	

        plugins.push_back( pData );

        CORE_LOG_INFO("Successfully loaded plugin " << plugin->getPath());

    }else{
        CORE_LOG_WARNING("Plugin with given ID " << pData.plugin->ID() << " already exist. Plugin " << plugin->getPath() << " NOT loaded to application! Collision with plugin loaded from: " << collidingPlugin->getPath() );
    }

    return !pluginIDFound;
}

bool PluginLoader::checkLibrariesVersions( HMODULE library, const Filesystem::Path& path )
{
    // pobranie wersji bibliotek
    auto libsVerProc = loadProcedure<Plugin::GetLibrariesVersionFunction>(library, STRINGIZE(CORE_GET_LIBRARIES_VERSIONS_FUNCTION_NAME));
    if ( libsVerProc ) {
        int boostVer, qtVer, stlVer;
        libsVerProc(&boostVer, &qtVer, &stlVer);
        CORE_LOG_DEBUG(path << " boost: " << boostVer << "; Qt: " << std::hex << qtVer << "; STL: " << std::dec << stlVer);
        bool success = true;
        // boost/version.hpp - sprawdzenie major i minor
        if ( boostVer / 100 != BOOST_VERSION / 100 ) {
            CORE_LOG_ERROR(path << " has incompatible boost version: " << boostVer);
            success = false;
        }
        // QtCore/QGlobal.h - sprawdzenie tylko wersji Major
        if ( (qtVer >> 16) != (QT_VERSION >> 16) ) {
            CORE_LOG_ERROR(path << " has incompatible Qt version: " << qtVer);
            success = false;
        }
        //
        if ( stlVer != CORE_CPPLIB_VER ) {
            if ( stlVer == -1 ) {
                CORE_LOG_ERROR(path << " has incompatible STL version: " << "unknown");
            } else {
                CORE_LOG_ERROR(path << " has incompatible STL version: " << stlVer);
            }
            success = false;
        }
        return success;
    } else {
        CORE_LOG_ERROR(path << " is a plugin, but finding " << STRINGIZE(CORE_GET_LIBRARIES_VERSIONS_FUNCTION_NAME) << " failed");
        return false;
    }
}

bool PluginLoader::checkPluginVersion( HMODULE library, const Filesystem::Path& path )
{
	auto versionProc = loadProcedure<Plugin::GetAPIVersionFunction>(library, STRINGIZE(CORE_GET_PLUGIN_API_VERSION_FUNCTION_NAME));
	if ( versionProc ) {
		int pMajor;
		int pMinor;
		int pPatch;
		versionProc(&pMajor, &pMinor, &pPatch);
		if ( pMajor != CORE_API_MAJOR || pMinor > CORE_API_MINOR) {
			CORE_LOG_ERROR(path << " has obsolete interface version - should be "
				<< Version::formatedVersion() << ", is "
				<< Version::formatedVersion(Version(pMajor, pMinor, pPatch)));
			return false;
		} else {
			return true;
		}
	} else {
		CORE_LOG_ERROR(path<<" is a shared library, but finding "<<STRINGIZE(CORE_GET_PLUGIN_API_VERSION_FUNCTION_NAME)<<" failed. Is it a plugin or library?");
		return false;
	}
}

bool PluginLoader::checkPluginBuildType( HMODULE library, const Filesystem::Path& path )
{
    auto buildTypeProc = loadProcedure<Plugin::GetBuildTypeFunction>(library, STRINGIZE(CORE_GET_PLUGIN_BUILD_TYPE_FUNCTION_NAME));
    if ( buildTypeProc ) {
        int buildType = buildTypeProc();
        if ( buildType != CORE_PLUGIN_BUILD_TYPE ) {
            CORE_LOG_ERROR(path<<" has obsolete interface buildType; should be "<<CORE_PLUGIN_BUILD_TYPE<<", is "<<buildType);
            return false;
        } else {
            return true;
        }
    } else {
        CORE_LOG_ERROR(path<<" is a shared library, but finding "<<STRINGIZE(CORE_GET_PLUGIN_BUILD_TYPE_FUNCTION_NAME)<<" failed. Is it a plugin or library?");
        return false;
    }
}
////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////
