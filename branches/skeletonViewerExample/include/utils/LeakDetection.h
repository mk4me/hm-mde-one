/********************************************************************
	created:  2010/10/04
	created:  4:10:2010   15:30
	filename: LeakDetection.h
	author:	  Piotr Gwiazdowski
	
	purpose:  Nag��wek nale�y za��cza� tylko w g��wnym pliku projektu.
              Dodanie go wi�cej razy mo�e spowodowa� b��dy linkera.
*********************************************************************/
#ifndef __HEADER_GUARD__CORE_LEAKDETECTION_H__
#define __HEADER_GUARD__CORE_LEAKDETECTION_H__

#include <utils/Config.h>

#ifdef __WIN32__

#   ifdef UTILS_ENABLE_LEAK_DETECTION_VLD

// na razie VLD nie jest w��czone do projektu tak jak nale�y; trzeba
// �ci�gn�� r�cznie (http://sites.google.com/site/dmoulding/vld) oraz
// samemu skonfigurowa� (readme po instalacji)
#include <vld.h>

#   else // UTILS_ENABLE_LEAK_DETECTION_VLD

/**
 *	Prosty typ uruchamiaj�cy �ledzenie leak�w z u�yciem CRT. Poprzednio
 *  znajdowa� si� w pliku .cpp, ale zgodnie z polityk� usuwania konfiguracji z utils�w
 *  znajduje si� tutaj.
 *  Je�eliby przenosi� ten typ do .cpp, to powinno si� da� hinta linkerowi, aby
 *  instancjonowa� statyczny obiekt:
 *  #pragma comment(linker, "/include:?__leakDetector@@3V__CrtLeakDetector@@A")
 */
class __CrtLeakDetector
{
public:
    __CrtLeakDetector()
    {
        _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
    }
    ~__CrtLeakDetector()
    {
    }
} __leakDetector;

#   endif // UTILS_ENABLE_LEAK_DETECTION_VLD

#else // __WIN32__

// jak to zrobi� na unixie?

#endif // __WIN32__

#endif  // __HEADER_GUARD__CORE_PLUGIN_H__