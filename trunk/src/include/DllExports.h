#ifndef DLLEXPORTS_H
#define DLLEXPORTS_H
#define WIN32_LEAN_AND_MEAN

#include <iostream>
#ifdef __dll__
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif 	// __dll__


#ifdef __WIN32__         
extern "C" IMPEXP IControlPlugin* CreateConrolPluginInstance();  

#define PLUGIN_REGISTER(className)                                          \
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void*)      \
{                                                                           \
    return 1;                                                               \
}                                                                           \
                                                                            \
IMPEXP IControlPlugin* CreateConrolPluginInstance()                         \
{                                                                           \
return (IControlPlugin*)(new className());                                  \
}        

#endif     
                   
                          

#ifdef __UNIX__        
extern "C" IControlPlugin* CreateControlPluginInstance();

#define PLUGIN_REGISTER(className)                                          \
extern "C" IControlPlugin* CreateControlPluginInstance()                    \
{                                                                           \
    return (IControlPlugin*)(new className());                              \
}                                                                           
#endif

#endif	// DLLEXPORTS_H

