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
#endif

#ifdef __UNIX__
extern "C" IControlPlugin* CreateControlPluginInstance();
#endif

#endif	// DLLEXPORTS_H

