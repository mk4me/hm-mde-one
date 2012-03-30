/********************************************************************
	created:	2012/02/22
	created:	22:2:2012   10:17
	filename: 	NewChartEvents.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__NEWCHARTEVENTS_H__
#define HEADER_GUARD_NEW_CHART__NEWCHARTEVENTS_H__

#include <plugins/c3d/C3DChannels.h>
#include <plugins/c3d/C3DCollections.h>

class EventsPlotItem : public QwtPlotItem
{
public:
    EventsPlotItem(EventsCollectionConstPtr events);
    ~EventsPlotItem() {}

public:
    virtual void draw( QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRectF &canvasRect ) const;
    EventsCollectionConstPtr getEvents() const { return events; }

private:
    EventsCollectionConstPtr events;

};

class EventsHelper
{
public:
    typedef float timeType;
    typedef C3DEventsCollection::EventConstPtr EventConstPtr;
    typedef C3DEventsCollection::EventPtr EventPtr;
    struct Segment 
    {
        EventConstPtr event1;
        EventConstPtr event2;
        timeType begin;
        timeType end;
        ScalarChannelStatsPtr stats;
        QwtPlotCurve* normalizedCurve;
    };
    typedef core::shared_ptr<Segment> SegmentPtr;
    typedef core::shared_ptr<const Segment> SegmentConstPtr;
    typedef std::vector<SegmentPtr> Segments;
    typedef boost::iterator_range<Segments::const_iterator> SegmentsConstRange;
    typedef boost::iterator_range<Segments::iterator> SegmentsRange;

public:
    EventsHelper(EventsCollectionConstPtr events, ScalarChannelReaderInterfaceConstPtr scalar);
    EventsCollectionConstPtr getEvents() const { return events; }
    const EventsPlotItem* getEventsItem() const { return eventsItem; }
    EventsPlotItem* getEventsItem() { return eventsItem; }
    SegmentConstPtr getSegment(timeType time, C3DEventsCollection::Context context)
    {
        UTILS_ASSERT(context == C3DEventsCollection::Context::Left || context == C3DEventsCollection::Context::Right);
        std::vector<SegmentPtr>& segments = context == C3DEventsCollection::Context::Left ? leftSegments : rightSegments;
        for (auto it = segments.cbegin(); it != segments.cend(); it++) {
            if (time >= (*it)->begin && time <= (*it)->end) {
                return *it;
            }
        }
        return SegmentConstPtr();
    }

    SegmentsConstRange getRightSegments() const { return boost::make_iterator_range(rightSegments.cbegin(), rightSegments.cend()); }
    SegmentsConstRange getLeftSegments() const { return boost::make_iterator_range(leftSegments.cbegin(), leftSegments.cend());  }
    SegmentsRange getRightSegments() { return boost::make_iterator_range(rightSegments.begin(), rightSegments.end()); }
    SegmentsRange getLeftSegments() { return boost::make_iterator_range(leftSegments.begin(), leftSegments.end());  }

private:
    void createSegments(std::vector<SegmentPtr>& collection, C3DEventsCollection::Context context);

private:
    std::vector<SegmentPtr> leftSegments;
    std::vector<SegmentPtr> rightSegments;
    EventsCollectionConstPtr events;
    ScalarChannelReaderInterfaceConstPtr scalar;
    EventsPlotItem* eventsItem;
    
    
};
typedef core::shared_ptr<EventsHelper> EventsHelperPtr;
typedef core::shared_ptr<const EventsHelper> EventsHelperConstPtr;


#endif
