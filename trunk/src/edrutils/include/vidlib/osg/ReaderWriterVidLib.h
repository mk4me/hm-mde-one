/********************************************************************
	created:  2011/01/27
	created:  27:1:2011   10:47
	filename: ReaderWriterVidLib.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_VIDLIB__READERWRITERVIDLIB_H__
#define HEADER_GUARD_VIDLIB__READERWRITERVIDLIB_H__

#include <vidlib/Export.h>
#include <utils/Utils.h>
UTILS_PUSH_WARNINGS
#include <osgDB/Registry>
UTILS_POP_WARNINGS

//! Pomocnicze makro dla modu��w, kt�re chc� za�adowa� plugin tak, jakby by� zbudowany statycznie.
#ifdef VIDLIB_LIBRARY_STATIC
#   define USE_OSGPLUGIN_VIDLIB() USE_OSGPLUGIN(vidlib)
#else 
#   define USE_OSGPLUGIN_VIDLIB() osgDB::PluginFunctionProxy proxy_vidlib(osgdb_vidlib_init);
#endif

extern VIDLIB_EXPORT void osgdb_vidlib_init(void);

#endif  // HEADER_GUARD_VIDLIB__READERWRITERVIDLIB_H__