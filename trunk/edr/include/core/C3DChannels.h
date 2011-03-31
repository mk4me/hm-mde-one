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
#include <core/C3D_Data.h>
#include <kinematiclib/KinematicModel.h>

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////
    template <>
    struct DataOperatorsManipulator<osg::Vec3, float, osg::Vec3>
    {
        bool isLower(const osg::Vec3& p1, const osg::Vec3& p2)
        {
            return p1 < p2;
        }
        osg::Vec3 normalize(const osg::Vec3& p, const osg::Vec3& min, const osg::Vec3& max) const
        {
            UTILS_ASSERT(false);
            return p;
        }
        osg::Vec3 interpolate(const osg::Vec3& p1, const osg::Vec3& p2, float t) const
        {
            osg::Vec3 copy1 = p1;
            osg::Vec3 copy2 = p2;
            return copy1 + (copy2 - copy1) * t;
        }
    };

    typedef DataChannel<osg::Vec3, float, DataOperatorsManipulator<osg::Vec3, float, const osg::Vec3&> > VectorChannel;
    typedef shared_ptr<VectorChannel> VectorChannelPtr;

    typedef DataChannel<float, float> ScalarChannel;
    typedef shared_ptr<ScalarChannel> ScalarChannelPtr;
    typedef shared_ptr<const ScalarChannel> ScalarChannelConstPtr;

    class C3DAnalogChannel : public ScalarChannel
    {
    protected:
        C3DAnalogChannel(int samplesPerSec);
        C3DAnalogChannel(const C3DAnalogChannel& channel);
        C3DAnalogChannel(const C3D_Data& data, int channelNo);
    };

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
        virtual ScalarChannel* clone()
        {
            return new EMGChannel(*this);
        }
    };
    typedef shared_ptr<EMGChannel> EMGChannelPtr;



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
        virtual ScalarChannel* clone()
        {
            return new GRFChannel(*this);
        }
    };
    typedef shared_ptr<GRFChannel> GRFChannelPtr;
    typedef shared_ptr<const GRFChannel> GRFChannelConstPtr;


    class MarkerChannel : public VectorChannel 
    {
    private:
        MarkerChannel(int samplesPerSec);
        
    public:        
        MarkerChannel(const MarkerChannel& channel) :
            VectorChannel(channel)
            { }
        MarkerChannel(C3D_Data& c3dData, int channelNo);
    public:
        virtual MarkerChannel* clone()
        {
            return new MarkerChannel(*this);
        }
    };
    typedef shared_ptr<MarkerChannel> MarkerChannelPtr;
    typedef shared_ptr<const MarkerChannel> MarkerChannelConstPtr;



////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////

CORE_DEFINE_WRAPPER(ScalarChannel, utils::PtrPolicyBoost);
CORE_DEFINE_WRAPPER(EMGChannel, ScalarChannel);
CORE_DEFINE_WRAPPER(GRFChannel, ScalarChannel);
CORE_DEFINE_WRAPPER(MarkerChannel, utils::PtrPolicyBoost);


#endif  // __HEADER_GUARD_CORE__C3DCHANNELS_H__