/********************************************************************
	created:  2010/10/13
	created:  13:10:2010   15:17
	filename: ITimeline.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__ITIMELINESERVICE_H__
#define __HEADER_GUARD__ITIMELINESERVICE_H__

#include <core/IService.h>
#include <core/SmartPtr.h>
#include "Stream.h"

class ITimeline
{
public:
    virtual ~ITimeline() {}

    virtual void addStream(timeline::StreamPtr stream) = 0;
    virtual void removeStream(timeline::StreamPtr stream) = 0;
    virtual bool isPlaying() const = 0;
    virtual void setPlaying(bool playing) = 0;
    virtual double getTime() const = 0;
    virtual void setTime(double time) = 0;
    virtual double getLength() const = 0;
    virtual double getNormalizedTime() const = 0;
    virtual void setNormalizedTime(double time) = 0;
};

typedef core::shared_ptr<ITimeline> ITimelinePtr;
typedef core::shared_ptr<const ITimeline> ITimelineConstPtr;
typedef core::weak_ptr<ITimeline> ITimelineWeakPtr;



#endif  // __HEADER_GUARD__ITIMELINESERVICE_H__