/********************************************************************
	created:  2010/10/04
	created:  4:10:2010   15:30
	filename: LeakDetection.h
	author:	  Piotr Gwiazdowski
	
	purpose:  Nag��wek nale�y za��cza� tylko w g��wnym pliku projektu.
*********************************************************************/
#ifndef __HEADER_GUARD__CORE_LEAKDETECTION_H__
#define __HEADER_GUARD__CORE_LEAKDETECTION_H__

#include <utils/Config.h>

#if defined(UTILS_ENABLE_LEAK_DETECTION) && (defined(_DEBUG) || defined(DEBUG))
#   ifdef __WIN32__
#       ifdef UTILS_ENABLE_LEAK_DETECTION_VLD

// na razie VLD nie jest w��czone do projektu tak jak nale�y; trzeba
// �ci�gn�� r�cznie (http://sites.google.com/site/dmoulding/vld) oraz
// samemu skonfigurowa� (readme po instalacji)
#include <vld.h>

#       else // UTILS_ENABLE_LEAK_DETECTION_VLD

// prosty leak detection z u�yciem CRT
#define __UTILS_ENABLE_LEAD_DETECTION_CRT
#pragma comment(linker, "/include:?__leakDetector@@3V__CrtLeakDetector@@A")


#       endif // UTILS_ENABLE_LEAK_DETECTION_VLD
#   else // __WIN32__
// jak to zrobi� na unixie?
#   endif // __WIN32__
#endif

#endif  // __HEADER_GUARD__CORE_PLUGIN_H__