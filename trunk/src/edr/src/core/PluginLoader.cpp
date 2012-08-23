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

#include <core/PluginLoader.h>
#include <core/Plugin.h>
#include <core/PluginCommon.h>
#include <regex>

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

PluginLoader::PluginLoader()
{
    // dodaje domyœlne œcie¿ki
    addDefaultPaths();
}

PluginLoader::~PluginLoader()
{
    unloadPlugins();
}

void PluginLoader::clear()
{
    // wyczyszczenie œcie¿ek
    Paths().swap(paths);

    //wyczyszczenie pluginów
    Plugins().swap(plugins);
}

void PluginLoader::unloadPlugins()
{
    try{
        clear();
        freeLibraries();
    }
    catch(std::runtime_error& e){
        LOG_ERROR("PluginLoader: Error unloading plugins " << e.what());
    }
    catch(std::invalid_argument& e){
        LOG_ERROR("PluginLoader: Error unloading plugins " << e.what());
    }
    catch(std::exception& e){
        LOG_ERROR("PluginLoader: Error unloading plugins " << e.what());
    }
    catch(...){
        LOG_ERROR("PluginLoader: Error unloading plugins ");
    }
}

void PluginLoader::addDefaultPaths()
{
#if defined(__WIN32__)
	// katalog uruchomieniowy
	DWORD retval = 0;
	const DWORD size = MAX_PATH;
	char path[size];
	std::string executableDir;

	retval = ::GetModuleFileName(NULL, path, size);
	if (retval != 0 && retval < size)
	{
		std::string pathstr(path);
		executableDir = std::string( pathstr, 0, pathstr.find_last_of("\\/") );
		paths.push_back(executableDir);
	}
	else
	{
		LOG_ERROR("Could not get application directory "
			"using Win32 API. It will not be searched.");
	}

	// katalog plugins
	if (!executableDir.empty())
	{
		boost::filesystem::path dirpath(executableDir);
		dirpath /= "plugins";
		paths.push_back(dirpath.string());
	}
	else
	{
		LOG_ERROR("Could not get application directory -- plugins folder"
			"using Win32 API. It will not be searched.");
	}
#elif defined(__UNIX__)
	// katalog uruchomieniowy
	std::string dir = std::string(".");
	paths.push_back(dir);

	// zagnie¿d¿ony katalog plugins
	dir = std::string("plugins/.");
	paths.push_back(dir);
#endif
}


void PluginLoader::load()
{

#if defined(__WIN32__)
	static const std::regex pluginFilter("^plugin_.*\.dll$");
#elif defined(__UNIX__)
    // GCC cos nie radzi sobie ze znakami pocz¹tku i konca linii !?
	static const std::regex pluginFilter("libplugin_.*\.so");
#endif

	for(auto pathIT = paths.begin(); pathIT != paths.end(); ++pathIT) {
        //std::unique_ptr<std::string> pathIT(new std::string("/home/wojtek/programming/WORK/EDR/_out/edrCB/bin"));
		std::vector<std::string> localFiles = core::Filesystem::listFiles(*pathIT, true);

		for(auto fileIT = localFiles.begin(); fileIT != localFiles.end(); ++fileIT){

			// Skip if no match
			if( !std::regex_match( core::Filesystem::Path(*fileIT).leaf().string(), pluginFilter) ) continue;

			addPlugIn(*fileIT);
		}
	}
}

HMODULE PluginLoader::loadSharedLibrary(const std::string & path)
{
#if defined(__WIN32__)
	return ::LoadLibrary( path.c_str() );
#elif defined(__UNIX__)
	return dlopen(path.c_str(), RTLD_LAZY);
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

		std::string ret(p);

		::LocalFree(errStr);

		return ret + ". Error code: " + boost::lexical_cast<std::string>(err);
	}

	return std::string("Unknown error");
#elif defined(__UNIX__)
	return std::string(dlerror());
#else
#error "Unsupported system for unloading shared libraries"
	return std::string("Unknown error");
#endif
}

bool PluginLoader::addPlugIn( const std::string& path )
{
    HMODULE library = loadSharedLibrary(path);
    if ( library ) {
        try{
            if (checkPluginVersion(library, path) && checkLibrariesVersions(library, path) && checkPluginBuildType(library, path) ) {
                auto proc = loadProcedure<Plugin::CreateFunction>(library, STRINGIZE(CORE_CREATE_PLUGIN_FUNCTION_NAME));
                if ( proc ) {
                    bool success = onAddPlugin(path, library, proc);
                    if ( success ) {
                        return true;
                    }
                } else {
                    LOG_DEBUG(path<<" is a plugin, but finding "<<STRINGIZE(CORE_CREATE_PLUGIN_FUNCTION_NAME)<<" failed.");
                }
            }
        }catch(std::exception & e){
            LOG_DEBUG(path << " is a plugin, but trying to check version or libraries failed with error: " << e.what());
        }catch(...){
            LOG_DEBUG(path << " is a plugin, but trying to check version or libraries failed with UNKNOWN error");
        }
    } else  {
        LOG_ERROR("Error \"" << lastLoadSharedLibraryError() << "\" during loading " << path << ".");
    }

	unloadSharedLibrary(library);
    return false;
}

bool PluginLoader::onAddPlugin( const std::string& path, HMODULE library, Plugin::CreateFunction createFunction )
{
    Plugin* plugin = NULL;

    LOG_INFO("Loading plugin " << path);

    // próba za³adowania
    try {
        plugin = createFunction(&__instanceInfo);
    } catch ( std::exception& ex ) {
        LOG_ERROR("Error loading plugin "<<path<<": "<<ex.what());
        return false;
    } catch ( ... ) {
        LOG_ERROR("Error loading plugin "<<path<<": Unknown");
        return false;
    }

    // czy uda³o siê wczytaæ?
    if ( !plugin ) {
        LOG_ERROR("Error loading plugin "<<path<<": Plugin not created");
        return false;
    }


    bool pluginIDFound = false;
    core::PluginPtr collidingPlugin;
    //szukamy pluginu o podanym ID - jeœli nie ma ladujemy, w przeciwnym wypadku info i nie dodajemy
    for(auto it = plugins.begin(); it != plugins.end(); ++it){
        if( (*it).first->getID() == plugin->getID()){
            pluginIDFound = true;
            collidingPlugin = (*it).first;
            break;
        }
    }

    if(pluginIDFound == false){

        plugin->setPath(path);

        // musi tak byæ, inaczej dwa smart pointery do jendego obiektu!!
        PluginPair p;
        p.first.reset(plugin);
        p.second = p.first;

        plugins.push_back( p );
        libraries.push_back(library);

        LOG_INFO("Successfully loaded plugin " << path);

    }else{
        LOG_WARNING("Plugin with given ID " << plugin->getID() << " already exist. Plugin " << path << " NOT loaded to application! Collision with plugin loaded from: " << collidingPlugin->getPath() );
    }

    return !pluginIDFound;
}

void PluginLoader::freeLibraries()
{
    for (size_t i = 0; i < libraries.size(); ++i)
    {
		unloadSharedLibrary(libraries[i]);
    }
    libraries.clear();
}

bool PluginLoader::checkLibrariesVersions( HMODULE library, const std::string& path )
{
    // pobranie wersji bibliotek
    auto libsVerProc = loadProcedure<Plugin::GetLibrariesVersionFunction>(library, STRINGIZE(CORE_GET_LIBRARIES_VERSIONS_FUNCTION_NAME));
    if ( libsVerProc ) {
        int boostVer, qtVer, stlVer;
        libsVerProc(&boostVer, &qtVer, &stlVer);
        LOG_DEBUG(path << " boost: " << boostVer << "; Qt: " << std::hex << qtVer << "; STL: " << std::dec << stlVer);
        bool success = true;
        // boost/version.hpp - sprawdzenie major i minor
        if ( boostVer / 100 != BOOST_VERSION / 100 ) {
            LOG_ERROR(path << " has incompatible boost version: " << boostVer);
            success = false;
        }
        // QtCore/QGlobal.h - sprawdzenie tylko wersji Major
        if ( (qtVer >> 16) != (QT_VERSION >> 16) ) {
            LOG_ERROR(path << " has incompatible Qt version: " << qtVer);
            success = false;
        }
        //
        if ( stlVer != CORE_CPPLIB_VER ) {
            if ( stlVer == -1 ) {
                LOG_ERROR(path << " has incompatible STL version: " << "unknown");
            } else {
                LOG_ERROR(path << " has incompatible STL version: " << stlVer);
            }
            success = false;
        }
        return success;
    } else {
        LOG_ERROR(path << " is a plugin, but finding " << STRINGIZE(CORE_GET_LIBRARIES_VERSIONS_FUNCTION_NAME) << " failed");
        return false;
    }
}

bool PluginLoader::checkPluginVersion( HMODULE library, const std::string& path )
{
	auto versionProc = loadProcedure<Plugin::GetVersionFunction>(library, STRINGIZE(CORE_GET_PLUGIN_VERSION_FUNCTION_NAME));
	if ( versionProc ) {
		int version = versionProc();
		if ( version != CORE_PLUGIN_INTERFACE_VERSION ) {
			LOG_ERROR(path<<" has obsolete interface version; should be "<<CORE_PLUGIN_INTERFACE_VERSION<<", is "<<version);
			return false;
		} else {
			return true;
		}
	} else {
		LOG_ERROR(path<<" is a shared library, but finding "<<STRINGIZE(CORE_GET_PLUGIN_VERSION_FUNCTION_NAME)<<" failed. Is it a plugin or library?");
		return false;
	}
}

bool PluginLoader::checkPluginBuildType( HMODULE library, const std::string& path )
{
    auto buildTypeProc = loadProcedure<Plugin::GetBuildTypeFunction>(library, STRINGIZE(CORE_GET_PLUGIN_BUILD_TYPE_FUNCTION_NAME));
    if ( buildTypeProc ) {
        int buildType = buildTypeProc();
        if ( buildType != CORE_PLUGIN_BUILD_TYPE ) {
            LOG_ERROR(path<<" has obsolete interface buildType; should be "<<CORE_PLUGIN_BUILD_TYPE<<", is "<<buildType);
            return false;
        } else {
            return true;
        }
    } else {
        LOG_ERROR(path<<" is a shared library, but finding "<<STRINGIZE(CORE_GET_PLUGIN_BUILD_TYPE_FUNCTION_NAME)<<" failed. Is it a plugin or library?");
        return false;
    }
}
////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////
