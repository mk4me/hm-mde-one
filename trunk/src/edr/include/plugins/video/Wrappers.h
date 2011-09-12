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
#include <utils/DataChannelCollection.h>

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
	virtual void interpolate(VideoImageOsgPtr & ret, const VideoImageOsgPtr& q1, const VideoImageOsgPtr& q2, float t) const
	{
		ret = q1;
	}
};

class VideoChannel : public utils::BaseChannel<VideoImageOsgPtr, float, VideoManipulator>
{
public:
	enum View { Unknown, Left, Right, Front, Back };

private:
	mutable VideoStreamPtr videoStream;
	View type;
	

public:
	VideoChannel(int samplesPerSecond, VideoStreamPtr video) :
	BaseChannel(samplesPerSecond) ,
	videoStream(video)
	{}

	virtual VideoChannel* clone() const
	{
		VideoStreamPtr v(new VideoStream(videoStream)); //videoStream->clone();
		return new VideoChannel(this->getSamplesPerSecond(), v);
	}

	virtual time_type getLength() const
	{
		return videoStream->getDuration();
	}
	//! \return Liczba punktów pomiarowych.
	virtual size_type size() const
	{
		return videoStream->getFrameCount();
	}

    //! \return Czy kana³ nie zawiera danych
    virtual bool empty() const
    {
        return videoStream->getFrameCount() == 0;
    }

	VideoStreamConstPtr getVideoStream() const {
		return videoStream;
	}

	VideoChannel::View getType() const { return type; }

protected:
	//! \param time
	//! \return Zinterpolowana wartoœæ dla zadanego czasu.
	virtual point_type innerGetValue(time_type time) const
	{
		videoStream->setTime(time);
		return videoStream->getImage(vidlib::PixelFormatARGB);
	}

	virtual void innerAddPoint(time_type time, point_type_const_reference point) 
	{
		UTILS_ASSERT(false);
	}
};
typedef boost::shared_ptr<VideoChannel> VideoChannelPtr;
typedef boost::shared_ptr<const VideoChannel> VideoChannelConstPtr;

class VideoCollection : public utils::DataChannelCollection<VideoChannel>
{
};
typedef boost::shared_ptr<VideoCollection> VideoCollectionPtr;
typedef boost::shared_ptr<const VideoCollection> VideoCollectionConstPtr;

CORE_DEFINE_WRAPPER(VideoChannel, utils::PtrPolicyBoost, utils::ClonePolicyVirtualCloneMethod);
CORE_DEFINE_WRAPPER(vidlib::Picture, utils::PtrPolicyBoost, utils::ClonePolicyNotImplemented);
CORE_DEFINE_WRAPPER(VideoStream, utils::PtrPolicyOSG, utils::ClonePolicyVirtualCloneMethod);

#endif  // __HEADER_GUARD_VIDEO__WRAPPERS_H__