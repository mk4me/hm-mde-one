/********************************************************************
	created:	2010/02/11
	created:	11:2:2010   13:05
	filename: VMPrivate.h
	author:		Piotr Gwiazdowski

	purpose:	Nag³ówek u¿ywany przez wszystkie pliki niew³¹czone do
            publicznego interfejsu biblioteki. Aby nie zaœmiecaæ
            publicznego namespace'a i unikaæ konfliktów tutaj powinna
            znajdowaæ siê wiêkszoœæ makr
*********************************************************************/

#ifndef __AVINOUTPRIVATE_H__
#define __AVINOUTPRIVATE_H__


////////////////////////////////////////////////////////////////////////////////

extern "C" {
#define __STDC_LIMIT_MACROS
#define __STDC_CONSTANT_MACROS
#include <stdint.h>
#undef INTMAX_C
#undef UINTMAX_C
}

#include <string>
#include <stdexcept>
#include <memory.h>
#include <vector>
#include <map>
#include <deque>
#include <list>
#include <algorithm>
#include <limits.h>
#include <stack>

////////////////////////////////////////////////////////////////////////////////

#include <utils/Utils.h>
#include <utils/Debug.h>
#include <utils/Profiler.h>
#include <utils/Enum.h>
#include <plugins/video/core/Config.h>
#include "Utils.h"
#include "PixelFormat.h"
#include "Picture.h"
#include "VideoBufferChunk.h"
#include "VideoBuffer.h"
#include "VideoError.h"
#include "VideoStream.h"
#include "VideoInput.h"
#include "VideoManager.h"

////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

#ifdef _DEBUG
#define VM_DEBUG
#endif // _DEBUG

//------------------------------------------------------------------------------

#ifdef  VM_ENABLE_EXCEPTIONS
#define VM_ERROR(x) { throw (x); }
#else   // AVENABLE_EXCEPTIONS
#define VM_ERROR(x) { return notifyError((x)); }
#endif  // AVENABLE_EXCEPTIONS

//------------------------------------------------------------------------------

//! Czy mo¿na u¿ywaæ nowego API?
#define VM_FFMPEG_ENABLE_NEWAPI
//! Do jakiej wartoœci dane maj¹ byæ alignowane?
#define VM_FFMPEG_ALIGN_BYTES 4

//------------------------------------------------------------------------------

#define VM_FUNCTION_PROLOG UTILS_PROFILER_ENTRY
#define VM_MEASURE_SCOPE UTILS_PROFILER_SCOPE

//------------------------------------------------------------------------------


#endif  // __AVINOUTPRIVATE_H__
