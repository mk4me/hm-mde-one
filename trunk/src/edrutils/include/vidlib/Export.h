/********************************************************************
	created:  2011/01/26
	created:  26:1:2011   12:19
	filename: Export.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_VIDLIB__EXPORT_H__
#define HEADER_GUARD_VIDLIB__EXPORT_H__

#include <vidlib/Config.h>

#if defined(_MSC_VER)
#   if defined (VIDLIB_LIBRARY_STATIC)
#       define VIDLIB_EXPORT
#   elif defined (VIDLIB_LIBRARY)
#       define VIDLIB_EXPORT __declspec(dllexport)
#   else
#       define VIDLIB_EXPORT __declspec(dllimport)
#   endif
#else
#   define VIDLIB_EXPORT
#endif

#endif  // HEADER_GUARD_VIDLIB__EXPORT_H__
