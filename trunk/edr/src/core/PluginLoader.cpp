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
#include <core/Log.h>
#include <core/PluginCommon.h>

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
    clear();
    freeLibraries();
}

void PluginLoader::clear()
{
    // wyczyszczenie œcie¿ek
    paths.clear();

    // kasowanie pluginów
    //for (size_t i = 0; i < plugins.size(); ++i) 
    //{
    //    delete plugins[i];
    //}
    plugins.clear();
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
        convertStringPathIntoFileDirList(executableDir, paths);
    } 
    else 
    {
        LOG_ERROR("Could not get application directory "
            "using Win32 API. It will not be searched.");
    }

    // katalog plugins
    if (!executableDir.empty())  
    {
        std::string dirpath = executableDir;
        dirpath.append("\\plugins");
        convertStringPathIntoFileDirList(dirpath, paths);
    } 
    else 
    {
        LOG_ERROR("Could not get application directory -- plugins folder"
            "using Win32 API. It will not be searched.");
    }
#elif defined(__UNIX__)
    // katalog uruchomieniowy
    std::string dir = string(".");
    paths.push_back(dir);

    // zagnie¿d¿ony katalog plugins
    dir = string("plugins/.");
    paths.push_back(dir);
#endif
}


void PluginLoader::load()
{
#if defined(__WIN32__)
    HANDLE file;
    WIN32_FIND_DATA dataFind;
    bool moreFiles;
    const char* fileMask = "plugin_*.dll";

    for(Paths::const_iterator itr = paths.begin(); itr!=paths.end(); ++itr) {
        std::string pattern = combinePath(*itr, fileMask);
        moreFiles = true;
        // listowanie plików
        utils::zero(dataFind);
        file = ::FindFirstFile(pattern.c_str(), &dataFind);
        while (file != INVALID_HANDLE_VALUE && moreFiles) {
            // dodanie plugina
            addPlugIn(combinePath(*itr, dataFind.cFileName));
            // czy dalej? (dziwna postaæ ¿eby pozbyæ siê warninga)
            moreFiles = (::FindNextFile(file, &dataFind) == BOOL(TRUE));
        }
    }
#elif defined(__UNIX__)
    for(Paths::const_iterator itr = paths.begin(); itr!=paths.end(); ++itr)
    {
        std::vector<string> files = vector<string>();

        DIR *dp;
        struct dirent *dirp;
        if( (dp  = opendir(itr->c_str())) == NULL) 
        {
            LOG_ERROR("Error(" << errno << ") opening " << (*itr));
            continue;;
        }

        while ((dirp = readdir(dp)) != NULL) 
        {
            addPlugIn(combinePath(*itr, dirp->d_name));
        }
        closedir(dp);
    }
#endif
}

bool PluginLoader::addPlugIn( const std::string& path )
{
#if defined(__WIN32__)
    HMODULE library = ::LoadLibrary( path.c_str() );
    if ( library ) {
        if ( checkPluginVersion(library, path) && checkLibrariesVersions(library, path) ) {
            FARPROC proc = ::GetProcAddress(library, STRINGIZE(CORE_CREATE_PLUGIN_FUNCTION_NAME));
            if ( proc ) {
                bool success = onAddPlugin(path, reinterpret_cast<uint32_t>(library),
                    reinterpret_cast<Plugin::CreateFunction>(proc));
                if ( success ) {
                    return true;
                }
            } else {
                LOG_DEBUG(path<<" is a plugin, but finding "<<STRINGIZE(CORE_CREATE_PLUGIN_FUNCTION_NAME)<<" failed.");
            }
        }   
    } else  {
        DWORD err = ::GetLastError();
        LPSTR errStr;
        if (::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, nullptr, err, 0, reinterpret_cast<LPSTR>(&errStr), 0, nullptr)) {
            // usuwanie CRLF
            LPSTR p = strchr(errStr, '\r');
            if ( p ) {
                *p = '\0';
            }
            LOG_ERROR("Error \"" << errStr << "\" (" << err << ") during loading " << path << ".");
            ::LocalFree(errStr);
        } else {
            LOG_ERROR("Unknown error (" << err << ") during loading " << path << ".");
        }
        
    }
    FreeLibrary(library);
    return false;
#elif defined(__UNIX__)
#error Dla UNIXa jeszcze nie zmienione
    ISystemPlugin* iControlPlugin = NULL;
    typedef ISystemPlugin* (*pvFunction)();
    pvFunction createPlugin;

    int size_of_path = path.length();
    if(size_of_path < 4)
        return false;

    std::string expansion_of_file = path.substr((size_of_path - 2), size_of_path);
    if(expansion_of_file != "so")
        return false;


    //void* handle = dlopen(pluginPath.c_str(), RTLD_LAZY);
    void* handle = dlopen(path.c_str(), RTLD_LAZY);
    if(!handle)
    {
        std::cout << "Error ladowanie uchwytuuuu: " << dlerror() << std::endl;
        return false;
    }

    createPlugin = (pvFunction)dlsym(handle, "CreateControlPluginInstance");
    if(!createPlugin)
    {
        std::cout << " Error pobieranie funkcji zwracajacej obiekt: " << dlerror() << std::endl;
        return false;
    }
    iControlPlugin = (ISystemPlugin*)(createPlugin());
    if(iControlPlugin)
    {
        std::cout << "Loading Plugin -- Plugin Name :"<< iControlPlugin->GetPluginName() << std::endl;
        m_pluginList.push_back(iControlPlugin);
    }
#endif

    return true;
}

bool PluginLoader::onAddPlugin( const std::string& path, uint32_t library, Plugin::CreateFunction createFunction )
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

    plugin->setPath(path);
    plugins.push_back( PluginPtr(plugin) );
    libraries.push_back(library);

    LOG_INFO("Successfully loaded plugin " << path);

    //LOG_INFO("Plugin " << plugin->getName() << " loaded from " << path);
    return true;
}

void PluginLoader::convertStringPathIntoFileDirList(const std::string& paths, Paths& filepath)
{
#ifdef __WIN32__
    char delimitor = ';';
#else
    char delimitor = ':';
#endif
    if (!paths.empty()) 
    {
        std::string::size_type start = 0;
        std::string::size_type end;
        while ( (end = paths.find_first_of(delimitor, start)) != std::string::npos) 
        {
            filepath.push_back(std::string(paths,start,end-start));
            start = end+1;
        }
        std::string lastPath(paths,start,std::string::npos);
        if (!lastPath.empty()) 
        {
            filepath.push_back(lastPath);
        }
    }

}

std::string PluginLoader::combinePath(const std::string& path, const std::string& fileName)
{
#if defined(__WIN32__)
    return path + "\\" + fileName;
#elif defined(__UNIX__)
    return path + "/" + fileName;
#endif
}

std::string PluginLoader::getFileName(const std::string& fileName)
{
    std::string::size_type slash1 = fileName.find_last_of('/');
    std::string::size_type slash2 = fileName.find_last_of('\\');
    if (slash1==std::string::npos) 
    {
        if (slash2==std::string::npos) 
        {
            return fileName;
        }
        return std::string(fileName.begin()+slash2+1,fileName.end());
    }
    if (slash2==std::string::npos) 
    {   
        return std::string(fileName.begin()+slash1+1,fileName.end());
    }
    return std::string(fileName.begin()+(slash1>slash2?slash1:slash2)+1,fileName.end());
}

void PluginLoader::freeLibraries()
{
    for (size_t i = 0; i < libraries.size(); ++i) 
    {
#if defined(__WIN32__)
        ::FreeLibrary( reinterpret_cast<HMODULE>(libraries[i]) );
#elif defined(__UNIX__)
#error Not implemented yet
#endif
    }
    libraries.clear();
}

bool PluginLoader::checkLibrariesVersions( HMODULE library, const std::string& path )
{
    // pobranie wersji bibliotek
    auto libsVerProc = reinterpret_cast<Plugin::GetLibrariesVersionFunction>(
        ::GetProcAddress(library, STRINGIZE(CORE_GET_LIBRARIES_VERSIONS_FUNCTION_NAME)));
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
    FARPROC versionProc = ::GetProcAddress(library, STRINGIZE(CORE_GET_PLUGIN_VERSION_FUNCTION_NAME));
    if ( versionProc ) {
        int version = reinterpret_cast<Plugin::GetVersionFunction>(versionProc)();
        if ( version != CORE_PLUGIN_INTERFACE_VERSION ) {
            LOG_ERROR(path<<" has obsolete interface version; should be "<<CORE_PLUGIN_INTERFACE_VERSION<<", is "<<version);
            return false;
        } else {
            return true;
        }
    } else {
        LOG_ERROR(path<<" is a .dll, but finding "<<STRINGIZE(CORE_GET_PLUGIN_VERSION_FUNCTION_NAME)<<" failed. Is it a plugin or library?");
        return false;
    }
}
////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////
