#include "PluginServices.h"
#include <iostream>
#include <QtGui/QMainWindow>
#include <QtGui>
#include <QMenu>
#include <QDir>
#include <QAction>
#include <QVector>
#include <QWidget>

#include "../anim/ControlPlugin.h"

#ifdef __WIN32__
//#include <winbase.h>
#include <windows.h>
#endif

#ifdef __UNIX__
#include <dlfcn.h>
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


//--------------------------------------------------------------------------------------------------
void PluginService::Clear()
{
    _PluginFileDirList.clear();
}

//--------------------------------------------------------------------------------------------------
PluginService::PluginService()
{
    _PluginFileDirList.clear();
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
AsyncResult PluginService::OnAdded()
{
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
   return _PluginFileDirList;
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
        convertStringPathIntoFileDirList(GRANT_FILENAME_TO_STRING(executableDir), _PluginFileDirList);
    }
//     else
//     {
//         osg::notify(osg::WARN) << "Could not get application directory "
//             "using Win32 API. It will not be searched." << std::endl;
//     }
#endif
#ifdef __UNIX__

#endif
}

//--------------------------------------------------------------------------------------------------
PluginList& PluginService::GetPluginList()
{
    return _PluginList;
}

//--------------------------------------------------------------------------------------------------
void PluginService::LoadPlugins()
{
    FilePathList loadPluginFolderPath = GetPathPlugin();

#ifdef __WIN32__
    HANDLE hFind;
    WIN32_FIND_DATA dataFind;
    BOOL bMoreFiles = TRUE;
    char* m_chFileMask;
    m_chFileMask = "*.dll";


    for(FilePathList::const_iterator itr = loadPluginFolderPath.begin(); itr!=loadPluginFolderPath.end(); ++itr)
    {
        GRANT_WINDOWS_FUNCT(SetCurrentDirectory)(itr->c_str());


        hFind = FindFirstFile(m_chFileMask,&dataFind);
        while (hFind != INVALID_HANDLE_VALUE && bMoreFiles == TRUE)
        {
            if(AddPlugIn(getRealPath(itr->c_str(), dataFind.cFileName)))
            {
                int nie_dupa = 1;
            }

            bMoreFiles = FindNextFile(hFind,&dataFind);
        }
    }
#endif
}

//--------------------------------------------------------------------------------------------------
std::string PluginService::getRealPath(const std::string& path, const std::string& fileName)
{
    return path + "\\" + fileName;
}

//--------------------------------------------------------------------------------------------------
std::string PluginService::GetPlugIn()
{
    return std::string();
}

//--------------------------------------------------------------------------------------------------
bool PluginService::AddPlugIn(std::string pluginPath/*, std::string pluginName*/)
{
    IControlPlugin* iControlPlugin = NULL;
    typedef IControlPlugin* (*pvFunction)();
    pvFunction createPlugin;

#ifdef __WIN32__
    HINSTANCE hdll = NULL;

    hdll = GRANT_WINDOWS_FUNCT(LoadLibrary)(TEXT(pluginPath.c_str()));		// load the dll
    createPlugin = (pvFunction)(GetProcAddress( hdll, "CreateConrolPluginInstance" ));

    if(!CreateFoo)
        return false;

   // QObject *plugin = (QObject*)(CreateFoo());	// get pointer to object
   // iControlPlugin = (IControlPlugin*)(CreateFoo());
   // iControlPlugin = qobject_cast<IControlPlugin *>(plugin);

    iControlPlugin = (IControlPlugin*)(createPlugin());
    if(iControlPlugin)
    {
        _PluginList.push_back(iControlPlugin);
    }
  
   // Hmm... to trzeba bêdzie dok³adnei zbadaæ
   // FreeLibrary(hdll);				// free the dll
#endif
#ifdef __UNIX__
   //void* handle = dlopen(pluginPath.c_str(), RTLD_LAZY);
   void* handle = dlopen(pluginPath.c_str(), RTLD_NOW);
   if(!handle)
   {

	std::cout << "Error: " << dlerror() << std::endl;
        return false;
   }

  
   createPlugin = (pvFunction)dlsym(handle, "CreateControlPluginInstance");
   if(!createPlugin)
   {
	std::cout << " Error: " << dlerror() << std::endl;
        return false;
   }
   iControlPlugin = (IControlPlugin*)(createPlugin());
    if(iControlPlugin)
    {
	std::cout << "Zaladowal :"<< iControlPlugin->GetPluginName().toStdString() << std::endl;
        _PluginList.push_back(iControlPlugin);
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
