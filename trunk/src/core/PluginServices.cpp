#include "PluginServices.h"
#include <iostream>
#include <QtGui/QMainWindow>
#include <QtGui/QtGui>
#include <QtGui/QMenu>
#include <QtCore/QDir>
#include <QtGui/QAction>
#include <QtCore/QVector>
#include <QtGui/QWidget>

#include "ServiceManager.h"

#ifdef __WIN32__
//#include <winbase.h>
#include <windows.h>
#endif

#ifdef __UNIX__
#include <dlfcn.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#endif

#include <stdio.h>

#define GRANT_WINDOWS_FUNCT(x) x ## A
#define GRANT_STRING_TO_FILENAME(s) s
#define GRANT_FILENAME_TO_STRING(s) s
#define GRANT_FILENAME_TEXT(x) x

// DLL function signature


typedef char filenamechar;
typedef std::string filenamestring;

using namespace std;
M_DECLARED_CLASS(PluginService, kCLASSID_PluginService);


#ifdef __UNIX__
//--------------------------------------------------------------------------------------------------
int getdir (string dir, vector<string> &files)
{
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL) {
        cout << "Error(" << errno << ") opening " << dir << endl;
        return errno;
    }

    while ((dirp = readdir(dp)) != NULL) {
        files.push_back(string(dirp->d_name));
    }
    closedir(dp);
    return 0;
}
#endif

//--------------------------------------------------------------------------------------------------
void PluginService::Clear()
{
    m_pluginFileDirList.clear();
}

//--------------------------------------------------------------------------------------------------
PluginService::PluginService()
{
    m_pluginFileDirList.clear();
}

//--------------------------------------------------------------------------------------------------
PluginService::~PluginService()
{
    Clear();
}

//--------------------------------------------------------------------------------------------------
AsyncResult PluginService::OnTick(double delta)
{ 
    return AsyncResult_Complete; 
}

//--------------------------------------------------------------------------------------------------
AsyncResult PluginService::OnAdded(IServiceManager* serviceManager)
{
    m_pServiceManager = (ServiceManager*)serviceManager;

    std::cout << "PluginService ADDED-test!" << std::endl; 

    SetPathPlugin();
    LoadPlugins();
    return AsyncResult_Complete; 
}


//--------------------------------------------------------------------------------------------------
void PluginService::convertStringPathIntoFileDirList(const std::string& paths,FilePathList& filepath)
{
#if defined(WIN32) && !defined(__CYGWIN__)
    char delimitor = ';';
#else
    char delimitor = ':';
#endif

    if (!paths.empty())
    {
        std::string::size_type start = 0;
        std::string::size_type end;
        while ((end = paths.find_first_of(delimitor,start))!=std::string::npos)
        {
            filepath.push_back(std::string(paths,start,end-start));
            start = end+1;
        }

        std::string lastPath(paths,start,std::string::npos);
        if (!lastPath.empty())
            filepath.push_back(lastPath);
    }

}

//--------------------------------------------------------------------------------------------------
FilePathList& PluginService::GetPathPlugin()
{
   return m_pluginFileDirList;
}


//--------------------------------------------------------------------------------------------------
void PluginService::SetPathPlugin()
{
#ifdef __WIN32__
    //   1. The directory from which the application loaded.
    DWORD retval = 0;
    const DWORD size = MAX_PATH;
    filenamechar path[size];
    retval = GRANT_WINDOWS_FUNCT(GetModuleFileName)(NULL, path, size);
    if (retval != 0 && retval < size)
    {
        filenamestring pathstr(path);
        filenamestring executableDir(pathstr, 0, 
            pathstr.find_last_of(GRANT_FILENAME_TEXT("\\/")));
        convertStringPathIntoFileDirList(GRANT_FILENAME_TO_STRING(executableDir), m_pluginFileDirList);
    }
    else
    {
        std::cout<< "Could not get application directory "
            "using Win32 API. It will not be searched." << std::endl;
    }

    //   2. The directory from plugins folder.
    retval = GRANT_WINDOWS_FUNCT(GetModuleFileName)(NULL, path, size);
    if (retval != 0 && retval < size)
    {
        filenamestring pathstr(path);
        filenamestring executableDir(pathstr, 0, 
            pathstr.find_last_of(GRANT_FILENAME_TEXT("\\/")));

        std::string dirpath = executableDir;
        dirpath.append("\\plugins");
        convertStringPathIntoFileDirList(GRANT_FILENAME_TO_STRING(dirpath), m_pluginFileDirList);
    }
    else
    {
        std::cout << "Could not get application directory -- plugins folder"
            "using Win32 API. It will not be searched." << std::endl;
    }

#endif
#ifdef __UNIX__
    //   1. The directory from which the application loaded.
    std::string dir = string(".");
    m_pluginFileDirList.push_back(dir);

    //   2. The directory from plugins folder.
    dir = string("plugins/.");
    m_pluginFileDirList.push_back(dir);
#endif
}

//--------------------------------------------------------------------------------------------------
PluginList& PluginService::GetPluginList()
{
    return m_pluginList;
}

//--------------------------------------------------------------------------------------------------
void PluginService::LoadPlugins()
{
    FilePathList loadPluginFolderPath = GetPathPlugin();

#ifdef __WIN32__
    HANDLE hFind;
    WIN32_FIND_DATA dataFind;
    BOOL bMoreFiles;
    char* m_chFileMask;
    m_chFileMask = "*.dll";


    for(FilePathList::const_iterator itr = loadPluginFolderPath.begin(); itr!=loadPluginFolderPath.end(); ++itr)
    {
        GRANT_WINDOWS_FUNCT(SetCurrentDirectory)(itr->c_str());
        bMoreFiles = TRUE;

        hFind = FindFirstFile(m_chFileMask,&dataFind);
        while (hFind != INVALID_HANDLE_VALUE && bMoreFiles == TRUE)
        {
            if(AddPlugIn(getRealPath(itr->c_str(), dataFind.cFileName)))
            {
                std::cout<< "Plugin Load Complete :   " << dataFind.cFileName << std::endl;
            }

            bMoreFiles = FindNextFile(hFind,&dataFind);
        }
    }
#endif
#ifdef __UNIX__
    for(FilePathList::const_iterator itr = loadPluginFolderPath.begin(); itr!=loadPluginFolderPath.end(); ++itr)
    {
        std::vector<string> files = vector<string>();

	    DIR *dp;
        struct dirent *dirp;
        if((dp  = opendir((*itr).c_str())) == NULL) 
	   {
            cout << "Error(" << errno << ") opening " << (*itr) << endl;
            return;
        }

        while ((dirp = readdir(dp)) != NULL) 
	    {
	        if(AddPlugIn(getRealPath(itr->c_str(), dirp->d_name)))
	        {
		    std::cout<< "Plugin Load Complete :   " << dirp->d_name << std::endl;
	        }
        }
        closedir(dp);
    }
#endif
}

//--------------------------------------------------------------------------------------------------
std::string PluginService::getRealPath(const std::string& path, const std::string& fileName)
{
#ifdef __WIN32__
    return path + "\\" + fileName;
#endif
#ifdef __UNIX__
    return path + "/" + fileName;
#endif
}

//--------------------------------------------------------------------------------------------------
std::string PluginService::GetPlugIn()
{
    return std::string();
}

//--------------------------------------------------------------------------------------------------
bool PluginService::AddPlugIn(std::string pluginPath/*, std::string pluginName*/)
{
    ISystemPlugin* iControlPlugin = NULL;
    typedef ISystemPlugin* (*pvFunction)();
    pvFunction createPlugin;

#ifdef __WIN32__
    HINSTANCE hdll = NULL;

    hdll = GRANT_WINDOWS_FUNCT(LoadLibrary)(TEXT(pluginPath.c_str()));		// load the dll
    createPlugin = (pvFunction)(GetProcAddress( hdll, "CreateConrolPluginInstance" ));

    if(!createPlugin)
        return false;

   // QObject *plugin = (QObject*)(CreateFoo());	// get pointer to object
   // iControlPlugin = (ISystemPlugin*)(CreateFoo());
   // iControlPlugin = qobject_cast<ISystemPlugin *>(plugin);

    iControlPlugin = (ISystemPlugin*)(createPlugin());
    if(iControlPlugin)
    {
        iControlPlugin->RegisterServices(m_pServiceManager);
        std::cout << "Loading Plugin -- Plugin Name :"<< iControlPlugin->GetPluginName() << std::endl;
        m_pluginList.push_back(iControlPlugin);
    }
  
   // Hmm... to trzeba bêdzie dok³adnei zbadaæ
   // FreeLibrary(hdll);				// free the dll
#endif
#ifdef __UNIX__

   int size_of_path = pluginPath.length();
   if(size_of_path < 4)
	return false;

   std::string expansion_of_file = pluginPath.substr((size_of_path - 2), size_of_path);
   if(expansion_of_file != "so")
	return false;


   //void* handle = dlopen(pluginPath.c_str(), RTLD_LAZY);
   void* handle = dlopen(pluginPath.c_str(), RTLD_LAZY);
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

//--------------------------------------------------------------------------------------------------
std::string PluginService::getSimpleFileName(const std::string& fileName)
{
    std::string::size_type slash1 = fileName.find_last_of('/');
    std::string::size_type slash2 = fileName.find_last_of('\\');
    if (slash1==std::string::npos) 
    {
        if (slash2==std::string::npos) return fileName;
        return std::string(fileName.begin()+slash2+1,fileName.end());
    }
    if (slash2==std::string::npos) return std::string(fileName.begin()+slash1+1,fileName.end());
    return std::string(fileName.begin()+(slash1>slash2?slash1:slash2)+1,fileName.end());
}
