/********************************************************************
	created:  2011/02/17
	created:  17:2:2011   14:06
	filename: C3DChannels.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_C3D__C3DCHANNELS_H__
#define __HEADER_GUARD_C3D__C3DCHANNELS_H__
#include <boost/smart_ptr.hpp>
#include <core/SmartPtr.h>
#include <utils/DataChannel.h>
#include <core/ObjectWrapper.h>
#include <osg/Vec3>
#include <osg/Math>
#include <c3dlib/C3DParser.h>
#include <kinematiclib/KinematicModel.h>

typedef utils::DataChannel<osg::Vec3, float> VectorChannel;
typedef boost::shared_ptr<VectorChannel> VectorChannelPtr;

typedef utils::DataChannel<float, float> ScalarChannel;
typedef boost::shared_ptr<ScalarChannel> ScalarChannelPtr;
typedef boost::shared_ptr<const ScalarChannel> ScalarChannelConstPtr;

//! Podstawa dla kanalu analogowego zapisanego w pliku c3d
class C3DAnalogChannel : public ScalarChannel
{
protected:
    C3DAnalogChannel(int samplesPerSec);
    C3DAnalogChannel(const C3DAnalogChannel& channel);
    C3DAnalogChannel(const c3dlib::C3DParser& data, int channelNo);
};
typedef boost::shared_ptr<C3DAnalogChannel> C3DAnalogChannelPtr;

//! Kanal EMG
class EMGChannel : public C3DAnalogChannel
{
public:
    explicit EMGChannel(int samplesPerSec) :
    C3DAnalogChannel(samplesPerSec)
    {}
    EMGChannel(const EMGChannel& channel) :
    C3DAnalogChannel(channel)
    {}
    EMGChannel(const c3dlib::C3DParser& data, int channelNo) :
    C3DAnalogChannel(data, channelNo)
    {}
        
public:
    virtual ScalarChannel* clone() const
    {
        return new EMGChannel(*this);
    }
};
typedef boost::shared_ptr<EMGChannel> EMGChannelPtr;
typedef boost::shared_ptr<const EMGChannel> EMGChannelConstPtr;

//! Kanal GRF
class GRFChannel : public C3DAnalogChannel
{
public:
    explicit GRFChannel(int samplesPerSec) :
    C3DAnalogChannel(samplesPerSec)
    {}
    GRFChannel(const GRFChannel& channel) :
    C3DAnalogChannel(channel)
    {}
    GRFChannel(const c3dlib::C3DParser& data, int channelNo) :
    C3DAnalogChannel(data, channelNo)
    {}

public:
    virtual ScalarChannel* clone() const
    {
        return new GRFChannel(*this);
    }
};
typedef boost::shared_ptr<GRFChannel> GRFChannelPtr;
typedef boost::shared_ptr<const GRFChannel> GRFChannelConstPtr;

//! Kanal zawiera dane o jednym markerze
class MarkerChannel : public utils::DataChannel<osg::Vec3f>
{
private:
    MarkerChannel(int samplesPerSec) : DataChannel(samplesPerSec) {}
        
public:        
    MarkerChannel(const MarkerChannel& channel) :
        DataChannel(channel)
        { }
    MarkerChannel(const c3dlib::C3DParser& data, int channelNo);
public:
    virtual MarkerChannel* clone() const
    {
        return new MarkerChannel(*this);
    }
};
typedef boost::shared_ptr<MarkerChannel> MarkerChannelPtr;
typedef boost::shared_ptr<const MarkerChannel> MarkerChannelConstPtr;

//! Kontener wszystkich markerow modelu
class MarkerSet : public kinematic::IMarkerSet
{
public:
	MarkerSet() {
		markers.reserve(53);
	}

public:
	void addMarker(MarkerChannelConstPtr marker) {
		markers.push_back(marker);
	}

public:
	virtual int getMarkersCount() const { return markers.size(); }
	virtual osg::Vec3 getPosition(int markerNo, double normalizedTime) const {
		double time = normalizedTime * markers[markerNo]->getLength();

		osg::Vec3 res = markers[markerNo]->getValue(osg::minimum(time, markers[markerNo]->getLength()));
		return res * static_cast<float>(getScale());
	}
	virtual std::string getMarkerName(int markerNo) const {
		return markers[markerNo]->getName();
	}
	virtual double getDuration() const {
		double duration = markers[0]->getLength();
		return duration;
	}

private:
	std::vector<MarkerChannelConstPtr> markers;
};
typedef core::shared_ptr<MarkerSet> MarkerSetPtr;
typedef core::shared_ptr<const MarkerSet> MarkerSetConstPtr;

CORE_DEFINE_WRAPPER(MarkerSet, utils::PtrPolicyBoost, utils::ClonePolicyCopyConstructor);
CORE_DEFINE_WRAPPER(ScalarChannel, utils::PtrPolicyBoost, utils::ClonePolicyVirtualCloneMethod);
CORE_DEFINE_WRAPPER_INHERITANCE(C3DAnalogChannel, ScalarChannel);
CORE_DEFINE_WRAPPER_INHERITANCE(EMGChannel, C3DAnalogChannel);
CORE_DEFINE_WRAPPER_INHERITANCE(GRFChannel, C3DAnalogChannel);
CORE_DEFINE_WRAPPER(MarkerChannel, utils::PtrPolicyBoost, utils::ClonePolicyVirtualCloneMethod);


#endif  // __HEADER_GUARD_CORE__C3DCHANNELS_H__