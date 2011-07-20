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
#include <utils/DataChannel.h>

typedef osg::ref_ptr<vidlib::VideoImage> VideoImageOsgPtr;
typedef vidlib::OsgStream VideoStream;
typedef osg::ref_ptr<VideoStream> VideoStreamPtr;
typedef osg::ref_ptr<const VideoStream> VideoStreamConstPtr;

class VideoManipulator
{
public:
	virtual bool isLower(const VideoImageOsgPtr& q1, const VideoImageOsgPtr& q2)
	{
		return q1 < q2;
	}
	virtual VideoImageOsgPtr normalize(const VideoImageOsgPtr& q, const VideoImageOsgPtr& min, const VideoImageOsgPtr& max) const
	{
		UTILS_ASSERT("Unable to normalize VideoImage");
		throw std::runtime_error("Unable to normalize VideoImage");
	}
	virtual VideoImageOsgPtr interpolate(const VideoImageOsgPtr& q1, const VideoImageOsgPtr& q2, float t) const
	{
		return q1;
	}
};

class VideoChannel : utils::DataChannel<VideoImageOsgPtr, float, VideoManipulator>
{
private:
	mutable VideoStreamPtr videoStream;
public:
	VideoChannel(int samplesPerSecond, VideoStreamPtr video) :
	DataChannel(samplesPerSecond) ,
	videoStream(video)
	{}

	virtual VideoChannel* clone() const
	{
		VideoStreamPtr v(new VideoStream(videoStream)); //videoStream->clone();
		return new VideoChannel(this->getSamplesPerSec(), v);
	}

	virtual time_type getLength() const
	{
		return videoStream->getDuration();
	}
	//! \return Liczba punktów pomiarowych.
	virtual int getNumPoints() const
	{
		return videoStream->getFrameCount();
	}
	
	//! \param time
	//! \return Zinterpolowana wartoœæ dla zadanego czasu.
	virtual point_type getValue(time_type time) const
	{
		videoStream->setTime(time);
		videoStream->getImage(vidlib::PixelFormatARGB);
	}

	virtual void addPoint(point_type point) 
	{
		UTILS_ASSERT(false);
	}
};


CORE_DEFINE_WRAPPER(VideoChannel, utils::PtrPolicyBoost, utils::ClonePolicyVirtualCloneMethod);
CORE_DEFINE_WRAPPER(vidlib::Picture, utils::PtrPolicyBoost, utils::ClonePolicyNotImplemented);
CORE_DEFINE_WRAPPER(VideoStream, utils::PtrPolicyOSG, utils::ClonePolicyVirtualCloneMethod);

#endif  // __HEADER_GUARD_VIDEO__WRAPPERS_H__