/********************************************************************
	created:	2011/06/26
	created:	26:6:2011   12:15
	filename: 	Export.h
	author:		Wojciech Knieć
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_C3DLIB__EXPORT_H__
#define HEADER_GUARD_C3DLIB__EXPORT_H__

#if defined(_MSC_VER)
#    define C3DLIB_EXPORT __declspec(dllexport)
#else		
#    define C3DLIB_EXPORT
#endif

#endif
