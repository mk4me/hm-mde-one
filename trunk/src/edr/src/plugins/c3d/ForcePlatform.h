/********************************************************************
	created:	2012/03/24
	created:	24:3:2012   13:12
	filename: 	ForcePlatform.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_ForcePlaform__FORCEPLATFORM_H__
#define HEADER_GUARD_ForcePlaform__FORCEPLATFORM_H__

#include "C3DPCH.h"
#include <c3dlib/C3DParser.h>
#include <plugins/c3d/IForcePlatform.h>
#include <plugins/c3d/C3DChannels.h>
#include <plugins/c3d/C3DCollections.h>


class ForcePlatform : public IForcePlatform, private utils::GeneralDataChannelTimeAccessor<osg::Vec3f, float>
{
public:
    typedef c3dlib::C3DParser::IEvent IEvent;
    typedef c3dlib::C3DParser::IEvent::Context Context;

    class Step : public IStep
    {
    public:
        Step() : startTime(-1.0f), endTime(-1.0f), context(ForcePlatform::IEvent::General) {}
        Step(float t1, float t2, ForcePlatform::Context c) : startTime(t1), endTime(t2), context(c) {}

    public:
        virtual float getStartTime() const { return startTime; }
        virtual void setStartTime(float val) { startTime = val; }
        virtual float getEndTime() const { return endTime; }
        virtual void setEndTime(float val) { endTime = val; }
        virtual osg::Vec3 getStartPoint() const { return startPoint; }
        virtual void setStartPoint(const osg::Vec3& val) { startPoint = val; }
        virtual osg::Vec3 getEndPoint() const { return endPoint; }
        virtual void setEndPoint(const osg::Vec3& val) { endPoint = val; }
        virtual bool isValid() const { return startTime >= 0.0f && endTime >= 0.0f; }
        ForcePlatform::Context getContext() const { return context; }
        void setContext(ForcePlatform::Context val) { context = val; }

    private:
        float startTime;
        float endTime;
        osg::Vec3 startPoint;
        osg::Vec3 endPoint;
        Context context;
    };

public:
    ForcePlatform(c3dlib::ForcePlatformPtr platform);

    virtual StepsConstRange getSteps() const;

    virtual osg::Vec3 getCenter() const;

    virtual float getWidth() const;

    virtual float getLength() const;

    virtual float getSignX() const;

    virtual float getSignY() const;

    void computeSteps( MarkerCollectionPtr markers, EventsCollectionConstPtr events );

    virtual GRFChannelConstPtr getForceChannel() const;
    virtual GRFChannelConstPtr getMomentChannel() const;

    virtual void setForceChannel(GRFChannelConstPtr val)  { force = val; }
    virtual void setMomentChannel(GRFChannelConstPtr val) { moment = val; }

private:
    bool isInsideXY( const osg::Vec3& v) const;

private:
    c3dlib::ForcePlatform platform;
    Steps steps;
    GRFChannelConstPtr force;
    GRFChannelConstPtr moment;
};
typedef core::shared_ptr<ForcePlatform> ForcePlatformPtr;
typedef core::shared_ptr<const ForcePlatform> ForcePlatformConstPtr;


#endif
