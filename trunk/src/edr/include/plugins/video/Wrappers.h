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

typedef utils::IRawUniformDataChannelReader<VideoImageOsgPtr, float> VideoChannelReaderInterface;

class VideoChannel : public VideoChannelReaderInterface
{
public:
	enum View { Unknown, Left, Right, Front, Back };

private:
	mutable VideoStreamPtr videoStream;
	View type;
    float samplesPerSecond;
    float invSamplesPerSecond;
    std::string name;

public:
	VideoChannel(VideoStreamPtr video, const std::string & name = std::string()) :
	videoStream(video), samplesPerSecond(static_cast<float>(videoStream->getFramerate())), name(name)
	{
        invSamplesPerSecond = 1.0f / samplesPerSecond;
    }

	virtual VideoChannel* clone() const
	{
		VideoStreamPtr v(new VideoStream(videoStream)); //videoStream->clone();
		return new VideoChannel(v,name);
	}

    virtual const std::string & getName() const
    {
        return name;
    }

    virtual void setName(const std::string & name)
    {
        this->name = name;
    }

    virtual float getSamplesPerSecond() const
    {
        return samplesPerSecond;
    }

    virtual float getSampleDuration() const
    {
        return invSamplesPerSecond;
    }

	virtual time_type getLength() const
	{
		return static_cast<time_type>(videoStream->getDuration());
	}
	//! \return Liczba punkt�w pomiarowych.
	virtual size_type size() const
	{
		return videoStream->getFrameCount();
	}

    //! \return Czy kana� nie zawiera danych
    virtual bool empty() const
    {
        return videoStream->getFrameCount() == 0;
    }

    //! \param idx Indeks probki
    //! \return Wartosc czasu dla danego indeksu
    virtual time_type argument(size_type idx) const
    {
        UTILS_ASSERT((idx >= 0),"B��dny indeks dla kana�y - musi by� wi�kszy lub r�wny 0");
        return static_cast<time_type>(videoStream->getFrameDuration() * idx);
    }

    //! \param idx Indeks probki
    //! \return Wartosc probki dla danego indeksu
    virtual point_type_const_reference value(size_type idx) const
    {
        static VideoImageOsgPtr ret;

        videoStream->setTime(idx * videoStream->getFrameDuration());

        return ret = videoStream->getImage(vidlib::PixelFormatARGB);
    }

	VideoStreamConstPtr getVideoStream() const {
		return videoStream;
	}

	VideoChannel::View getType() const { return type; }

protected:
	//! \param time
	//! \return Zinterpolowana warto�� dla zadanego czasu.
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

class VideoCollection : public utils::DataChannelCollection<VideoChannel, utils::DataChannelTimeAccessor<VideoChannel::point_type, VideoChannel::time_type, utils::DiscreteInterpolator, utils::BorderExtrapolator>>
{

};
typedef boost::shared_ptr<VideoCollection> VideoCollectionPtr;
typedef boost::shared_ptr<const VideoCollection> VideoCollectionConstPtr;

CORE_DEFINE_WRAPPER(VideoChannel, utils::PtrPolicyBoost, utils::ClonePolicyVirtualCloneMethod);
CORE_DEFINE_WRAPPER(vidlib::Picture, utils::PtrPolicyBoost, utils::ClonePolicyNotImplemented);
CORE_DEFINE_WRAPPER(VideoStream, utils::PtrPolicyOSG, utils::ClonePolicyVirtualCloneMethod);

#endif  // __HEADER_GUARD_VIDEO__WRAPPERS_H__