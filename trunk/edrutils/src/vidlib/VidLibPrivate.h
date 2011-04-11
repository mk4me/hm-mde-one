/********************************************************************
	created:	2010/02/11
	created:	11:2:2010   13:05
	filename:   VidLibPrivate.h
	author:		Piotr Gwiazdowski

	purpose:	Nag³ówek u¿ywany przez wszystkie pliki niew³¹czone do
            publicznego interfejsu biblioteki. Aby nie zaœmiecaæ
            publicznego namespace'a i unikaæ konfliktów tutaj powinna
            znajdowaæ siê wiêkszoœæ makr
*********************************************************************/

#ifndef HEADER_GUARD_VIDLIB__VIDLIBPRIVATE_H__
#define HEADER_GUARD_VIDLIB__VIDLIBPRIVATE_H__


#include <utils/Profiler.h>


#ifdef _DEBUG
#define VIDLIB_DEBUG
#endif // _DEBUG

//------------------------------------------------------------------------------

#ifdef  VIDLIB_ENABLE_EXCEPTIONS
#define VIDLIB_ERROR(x) { throw (x); }
#define VIDLIB_ERROR_NORETURN VIDLIB_ERROR
#else   // VIDLIB_ENABLE_EXCEPTIONS
#define VIDLIB_ERROR(x) { return notifyError((x)); }
#define VIDLIB_ERROR_NORETURN(x) notifyError((x));
#endif  // VIDLIB_ENABLE_EXCEPTIONS

//------------------------------------------------------------------------------

//! Czy mo¿na u¿ywaæ nowego API?
#define VIDLIB_FFMPEG_ENABLE_NEWAPI
//! Do jakiej wartoœci dane maj¹ byæ alignowane?
#define VIDLIB_FFMPEG_ALIGN_BYTES 4

//------------------------------------------------------------------------------

#define VIDLIB_FUNCTION_PROLOG  UTILS_PROFILER_ENTRY
#define VIDLIB_MEASURE_SCOPE    UTILS_PROFILER_SCOPE

//------------------------------------------------------------------------------


#endif  // HEADER_GUARD_VIDLIB__VIDLIBPRIVATE_H__
