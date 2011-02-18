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
#include <core/DataChannel.h>
#include <core/ObjectWrapper.h>

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

    typedef DataChannel<osg::Vec3, float> VectorChannel;
    typedef shared_ptr<VectorChannel> VectorChannelPtr;

    typedef DataChannel<float, float> ScalarChannel;
    typedef shared_ptr<ScalarChannel> ScalarChannelPtr;

    class EMGChannel : public ScalarChannel
    {
    public:
        EMGChannel(int samplesPerSec) :
        DataChannel(samplesPerSec)
        {}
        EMGChannel(const EMGChannel& channel) :
        DataChannel(channel)
        {}
        
    public:
        virtual DataChannel* clone()
        {
            return new EMGChannel(*this);
        }
    };
    typedef shared_ptr<EMGChannel> EMGChannelPtr;



    class GRFChannel : public ScalarChannel
    {
    public:
        GRFChannel(int samplesPerSec) :
        DataChannel(samplesPerSec)
        {}
        GRFChannel(const GRFChannel& channel) :
        DataChannel(channel)
        {}

    public:
        virtual DataChannel* clone()
        {
            return new GRFChannel(*this);
        }
    };
    typedef shared_ptr<GRFChannel> GRFChannelPtr;


////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////

CORE_DEFINE_WRAPPER(ScalarChannel, utils::PtrPolicyBoost);
CORE_DEFINE_WRAPPER(EMGChannel, utils::PtrPolicyBoost);
CORE_DEFINE_WRAPPER(GRFChannel, utils::PtrPolicyBoost);

#endif  // __HEADER_GUARD_CORE__C3DCHANNELS_H__