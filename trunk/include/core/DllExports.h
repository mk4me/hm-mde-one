#ifndef DLLEXPORTS_H
#define DLLEXPORTS_H
#define WIN32_LEAN_AND_MEAN

#include <iostream>
#ifdef __LIBRARY_DYNAMIC__
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif 	// __LIBRARY_DYNAMIC__


#ifdef __WIN32__         
extern "C" IMPEXP ISystemPlugin* CreateConrolPluginInstance();  

#define PLUGIN_REGISTER(className)                                          \
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void*)      \
{                                                                           \
    return 1;                                                               \
}                                                                           \
                                                                            \
IMPEXP ISystemPlugin* CreateConrolPluginInstance()                          \
{                                                                           \
return (ISystemPlugin*)(new className());                                   \
}        

#endif     
                   
                          

#ifdef __UNIX__        
extern "C" ISystemPlugin* CreateControlPluginInstance();

#define PLUGIN_REGISTER(className)                                          \
extern "C" ISystemPlugin* CreateControlPluginInstance()                    \
{                                                                           \
    return (ISystemPlugin*)(new className());                              \
}                                                                           
#endif

#endif	// DLLEXPORTS_H

