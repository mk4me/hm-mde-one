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
#include <vidlib/osg/OsgAdapter.h>
#include <vidlib/FFmpegVideoStream.h>

typedef vidlib::OsgStream VideoStream;
//typedef vidlib::OsgAdapter<vidlib::FFmpegVideoStream> VideoStream;
typedef osg::ref_ptr<VideoStream> VideoStreamPtr;
typedef osg::ref_ptr<const VideoStream> VideoStreamConstPtr;

CORE_DEFINE_WRAPPER(vidlib::Picture, utils::PtrPolicyBoost, utils::ClonePolicyNotImplemented);
CORE_DEFINE_WRAPPER(VideoStream, utils::PtrPolicyOSG, utils::ClonePolicyNotImplemented);
//CORE_DEFINE_WRAPPER(vidlib::VideoStream, utils::PtrPolicyBoost);

#endif  // __HEADER_GUARD_VIDEO__WRAPPERS_H__