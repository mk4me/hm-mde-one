/********************************************************************
	created:  2011/02/04
	created:  4:2:2011   13:02
	filename: DataWrappers.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_VIDEO__WRAPPERS_H__
#define __HEADER_GUARD_VIDEO__WRAPPERS_H__

#include <core/ObjectWrapper.h>
#include <vidlib/osg/VideoImageStream.h>

CORE_DEFINE_WRAPPER(vidlib::VideoImageStream, utils::PtrPolicyOSG);

#endif  // __HEADER_GUARD_VIDEO__WRAPPERS_H__