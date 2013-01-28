/********************************************************************
	created:  2010/10/19
	created:  19:10:2010   11:43
	filename: Export.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__UTILS_EXPORT_H__
#define __HEADER_GUARD__UTILS_EXPORT_H__

#if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__) || defined( __BCPLUSPLUS__)  || defined( __MWERKS__)
#   if defined( __LIBRARY_STATIC__ ) 
#       define UTILS_EXPORT
#   elif defined( __LIBRARY_DYNAMIC__ ) || defined(_WINDLL)
#       define UTILS_EXPORT   __declspec(dllexport)
#   else
#       define UTILS_EXPORT   __declspec(dllimport)
#   endif
#else
#   define UTILS_EXPORT
#endif

#endif  // __HEADER_GUARD__UTILS_EXPORT_H__
