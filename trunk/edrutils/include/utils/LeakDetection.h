/********************************************************************
	created:  2010/10/04
	created:  4:10:2010   15:30
	filename: LeakDetection.h
	author:	  Piotr Gwiazdowski
	
	purpose:  Nag³ówek nale¿y za³¹czaæ tylko w g³ównym pliku projektu.
              Dodanie go wiêcej razy mo¿e spowodowaæ b³êdy linkera.
*********************************************************************/
#ifndef __HEADER_GUARD__CORE_LEAKDETECTION_H__
#define __HEADER_GUARD__CORE_LEAKDETECTION_H__

#include <utils/Config.h>

#ifdef __WIN32__

#   ifdef UTILS_ENABLE_LEAK_DETECTION_VLD

// na razie VLD nie jest w³¹czone do projektu tak jak nale¿y; trzeba
// œci¹gn¹æ rêcznie (http://sites.google.com/site/dmoulding/vld) oraz
// samemu skonfigurowaæ (readme po instalacji)
#include <vld.h>

#   else // UTILS_ENABLE_LEAK_DETECTION_VLD

/**
 *	Prosty typ uruchamiaj¹cy œledzenie leaków z u¿yciem CRT. Poprzednio
 *  znajdowa³ siê w pliku .cpp, ale zgodnie z polityk¹ usuwania konfiguracji z utilsów
 *  znajduje siê tutaj.
 *  Je¿eliby przenosiæ ten typ do .cpp, to powinno siê daæ hinta linkerowi, aby
 *  instancjonowa³ statyczny obiekt:
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

// jak to zrobiæ na unixie?

#endif // __WIN32__

#endif  // __HEADER_GUARD__CORE_PLUGIN_H__