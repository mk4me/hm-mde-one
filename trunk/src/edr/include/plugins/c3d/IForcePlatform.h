/********************************************************************
	created:	2012/03/23
	created:	23:3:2012   10:37
	filename: 	ForcePlatform.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_C3DPLUGIN__FORCEPLATFORM_H__
#define HEADER_GUARD_C3DPLUGIN__FORCEPLATFORM_H__

#include <boost/range.hpp>
#include <osg/Vec3>
#include <core/SmartPtr.h>
#include <plugins/c3d/C3DChannels.h>

class IForcePlatform
{
public:
    class IStep {
    public:
        virtual ~IStep() {}
        virtual float getStartTime() const = 0;
        virtual float getEndTime() const = 0;
        virtual osg::Vec3 getStartPoint() const = 0;
        virtual osg::Vec3 getEndPoint() const = 0;
        virtual bool isValid() const = 0;
    };
    typedef core::shared_ptr<IStep> IStepPtr;
    typedef core::shared_ptr<const IStep> IStepConstPtr;

    typedef std::vector<IStepConstPtr> Steps;
    typedef boost::iterator_range<Steps::const_iterator> StepsConstRange;

    typedef std::vector<GRFChannelConstPtr> GRFChannels;
    typedef boost::iterator_range<GRFChannels::const_iterator> GRFChannelsConstRange;

public:
	virtual ~IForcePlatform() {}

    virtual StepsConstRange getSteps() const = 0;
    virtual osg::Vec3 getCenter() const = 0;
    virtual float getWidth() const = 0;

    virtual float getLength() const = 0;
    virtual float getSignX() const = 0;

    virtual float getSignY() const = 0;
    virtual GRFChannelConstPtr getForceChannel() const = 0;
    virtual GRFChannelConstPtr getMomentChannel() const = 0;

    float getDistanceToCenter(const osg::Vec3& vec) const {
        return (getCenter() - vec).length();
    }
};
typedef core::shared_ptr<IForcePlatform> IForcePlatformPtr;
typedef core::shared_ptr<const IForcePlatform> IForcePlatformConstPtr;
typedef std::vector<IForcePlatformPtr> IForcePlatformCollection;


#endif
