/********************************************************************
	created:  2011/02/04
	created:  4:2:2011   13:02
	filename: DataWrappers.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_VIDEO__WRAPPERS_H__
#define __HEADER_GUARD_VIDEO__WRAPPERS_H__

#include <utils/PtrPolicyStd.h>
#include <utils/ClonePolicies.h>
#include <corelib/BaseDataTypes.h>
#include <vidlib/osg/VideoImageStream.h>
#include <vidlib/osg/OsgAdapter.h>
#include <vidlib/FFmpegVideoStream.h>
#include <utils/DataChannel.h>
#include <utils/DataChannelCollection.h>


typedef osg::ref_ptr<vidlib::VideoImage> VideoImageOsgPtr;
typedef vidlib::OsgStream VideoStream;
typedef osg::ref_ptr<VideoStream> VideoStreamPtr;
typedef osg::ref_ptr<const VideoStream> VideoStreamConstPtr;
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

	virtual ~VideoChannel()
	{

	}

	virtual VideoChannel* clone() const
	{
		//VideoStreamPtr v(new VideoStream(videoStream->clone()));
		VideoStreamPtr v(videoStream->clone());
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
	//! \return Liczba punktów pomiarowych.
	virtual size_type size() const
	{
		return videoStream->getFrameCount();
	}

    //! \return Czy kanał nie zawiera danych
    virtual bool empty() const
    {
        return videoStream->getFrameCount() == 0;
    }

    //! \param idx Indeks próbki
    //! \return Wartość czasu dla danego indeksu
    virtual time_type argument(size_type idx) const
    {
        UTILS_ASSERT((idx >= 0),"Błędny indeks dla kanały - musi być większy lub równy 0");
        return static_cast<time_type>(videoStream->getFrameDuration() * idx);
    }

    //! \param idx Indeks próbki
    //! \return Wartość próbki dla danego indeksu
    virtual point_type_const_reference value(size_type idx) const
    {
        static VideoImageOsgPtr ret;

        videoStream->setTime(idx * videoStream->getFrameDuration());

        return ret = videoStream->getImage(vidlib::PixelFormatARGB);
    }

	VideoStreamPtr getVideoStream() {
		return videoStream;
	}

	VideoStreamConstPtr getVideoStream() const {
		return videoStream;
	}

	VideoChannel::View getType() const { return type; }

protected:
	//! \param time
	//! \return Zinterpolowana wartość dla zadanego czasu.
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
typedef utils::shared_ptr<VideoChannel> VideoChannelPtr;
typedef utils::shared_ptr<const VideoChannel> VideoChannelConstPtr;

//utils::DataChannelTimeAccessor<VideoChannel::point_type, VideoChannel::time_type, utils::DiscreteInterpolator, utils::BorderExtrapolator>

class VideoCollection : public utils::DataChannelCollection<VideoChannel>
{

};
typedef utils::shared_ptr<VideoCollection> VideoCollectionPtr;
typedef utils::shared_ptr<const VideoCollection> VideoCollectionConstPtr;

DEFINE_WRAPPER(VideoChannel, utils::PtrPolicyStd, utils::ClonePolicyVirtualCloneMethod);
DEFINE_WRAPPER(vidlib::Picture, utils::PtrPolicyStd, utils::ClonePolicyNotImplemented);
DEFINE_WRAPPER(VideoStream, utils::PtrPolicyOSG, utils::ClonePolicyVirtualCloneMethod);

#endif  // __HEADER_GUARD_VIDEO__WRAPPERS_H__
