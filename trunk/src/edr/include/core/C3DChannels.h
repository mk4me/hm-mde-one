/********************************************************************
	created:  2011/02/17
	created:  17:2:2011   14:06
	filename: C3DChannels.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_CORE__C3DCHANNELS_H__
#define __HEADER_GUARD_CORE__C3DCHANNELS_H__

#include <core/SmartPtr.h>
#include <utils/DataChannel.h>
#include <core/ObjectWrapper.h>
#include <core/C3D_Data.h>
#include <osg/Vec3>

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////
    typedef utils::DataChannel<osg::Vec3, float> VectorChannel;
    typedef shared_ptr<VectorChannel> VectorChannelPtr;

    typedef utils::DataChannel<float, float> ScalarChannel;
    typedef shared_ptr<ScalarChannel> ScalarChannelPtr;
    typedef shared_ptr<const ScalarChannel> ScalarChannelConstPtr;

    class C3DAnalogChannel : public ScalarChannel
    {
    protected:
        C3DAnalogChannel(int samplesPerSec);
        C3DAnalogChannel(const C3DAnalogChannel& channel);
        C3DAnalogChannel(const C3D_Data& data, int channelNo);
    };
    typedef shared_ptr<C3DAnalogChannel> C3DAnalogChannelPtr;

    class EMGChannel : public C3DAnalogChannel
    {
    public:
        explicit EMGChannel(int samplesPerSec) :
        C3DAnalogChannel(samplesPerSec)
        {}
        EMGChannel(const EMGChannel& channel) :
        C3DAnalogChannel(channel)
        {}
        EMGChannel(const C3D_Data& data, int channelNo) :
        C3DAnalogChannel(data, channelNo)
        {}
        
    public:
        virtual ScalarChannel* clone() const
        {
            return new EMGChannel(*this);
        }
    };
    typedef shared_ptr<EMGChannel> EMGChannelPtr;
    typedef shared_ptr<const EMGChannel> EMGChannelConstPtr;



    class GRFChannel : public C3DAnalogChannel
    {
    public:
        explicit GRFChannel(int samplesPerSec) :
        C3DAnalogChannel(samplesPerSec)
        {}
        GRFChannel(const GRFChannel& channel) :
        C3DAnalogChannel(channel)
        {}
        GRFChannel(const C3D_Data& data, int channelNo) :
        C3DAnalogChannel(data, channelNo)
        {}

    public:
        virtual ScalarChannel* clone() const
        {
            return new GRFChannel(*this);
        }
    };
    typedef shared_ptr<GRFChannel> GRFChannelPtr;
    typedef shared_ptr<const GRFChannel> GRFChannelConstPtr;


    class MarkerChannel : public utils::DataChannel<osg::Vec3f>
    {
    private:
        MarkerChannel(int samplesPerSec) : DataChannel(samplesPerSec) {}
        
    public:        
        MarkerChannel(const MarkerChannel& channel) :
            DataChannel(channel)
            { }
        MarkerChannel(C3D_Data& c3dData, int channelNo);
    public:
        virtual MarkerChannel* clone() const
        {
            return new MarkerChannel(*this);
        }
    };
    typedef shared_ptr<MarkerChannel> MarkerChannelPtr;
    typedef shared_ptr<const MarkerChannel> MarkerChannelConstPtr;



////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////

CORE_DEFINE_WRAPPER(ScalarChannel, utils::PtrPolicyBoost, utils::ClonePolicyVirtualCloneMethod);
CORE_DEFINE_WRAPPER_INHERITANCE(C3DAnalogChannel, ScalarChannel);
CORE_DEFINE_WRAPPER_INHERITANCE(EMGChannel, C3DAnalogChannel);
CORE_DEFINE_WRAPPER_INHERITANCE(GRFChannel, C3DAnalogChannel);
CORE_DEFINE_WRAPPER(MarkerChannel, utils::PtrPolicyBoost, utils::ClonePolicyVirtualCloneMethod);


#endif  // __HEADER_GUARD_CORE__C3DCHANNELS_H__