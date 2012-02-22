#include "NewChartPCH.h"
#include "NewChartEvents.h"

const QColor leftColor1(255, 0, 0, 15);
const QColor leftColor2(128, 0, 0, 15);
const QColor rightColor1(0, 255, 0, 15);
const QColor rightColor2(0, 128, 0, 15);

EventsPlotItem::EventsPlotItem( EventsCollectionConstPtr events ) : 
events(events) 
{
    UTILS_ASSERT(events);
}


void EventsPlotItem::draw( QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRectF &canvasRect ) const
{
    int count = events->getNumEvents();
    C3DEventsCollection::EventConstPtr lastLeftEvent;
    C3DEventsCollection::EventConstPtr lastRightEvent;

    int half = (canvasRect.bottom() - canvasRect.top())/ 2 + canvasRect.top();

    for (int i = 0; i < count; i++) {
        C3DEventsCollection::EventConstPtr event = events->getEvent(i);
        if (event->getContext() != C3DEventsCollection::Context::Left && event->getContext() != C3DEventsCollection::Context::Right) {
            continue;
        }
        int x = static_cast<int>(xMap.transform(event->getTime()));

        bool left = event->getContext() == C3DEventsCollection::Context::Left;
        int top = left ? canvasRect.top() : half;
        int bottom = left ? half : canvasRect.bottom();

        painter->setFont(QFont("Tahoma", 8));
        painter->setPen(QPen(QColor(0, 0, 0, 70)));
        painter->drawLine(x, top, x, bottom);

        painter->save();
        painter->translate(x + 10, bottom - 30);
        painter->rotate(-90); // or 270
        painter->drawText(0, 0, QString(event->getLabel().c_str()));
        painter->restore();
        //painter->drawText(x + 2, bottom - 30, QString(event->getLabel().c_str()));


        if (lastLeftEvent && left) {
            painter->setBrush(QBrush(i % 2 ? leftColor1 : leftColor2));
            int lastX = static_cast<int>(xMap.transform(lastLeftEvent->getTime()));
            painter->drawRect(lastX, top, x - lastX, bottom - top);
        } else if (lastRightEvent && !left) {
            painter->setBrush(QBrush(i % 2 ? rightColor1 : rightColor2));
            int lastX = static_cast<int>(xMap.transform(lastRightEvent->getTime()));
            painter->drawRect(lastX, top, x - lastX, bottom - top);
        } 
        if (event->getContext() == C3DEventsCollection::Context::Left) {
            lastLeftEvent = event;
        } else {
            lastRightEvent = event;
        }
    }

    painter->setFont(QFont("Tahoma", 32));  
    painter->save();

    painter->translate(canvasRect.left() + 30, half - 10);
    painter->rotate(-90); 
    painter->setPen(QPen(QColor(255, 0, 0, 55)));
    painter->drawText(0, 0, "Left");
    painter->restore();

    painter->save();
    painter->translate(canvasRect.left() + 30, canvasRect.bottom() - 10);
    painter->rotate(-90); 
    painter->setPen(QPen(QColor(0, 255, 0, 55)));
    painter->drawText(0, 0, "Right");
    painter->restore();
}



void EventsHelper::createSegments(std::vector<SegmentPtr>& collection, C3DEventsCollection::Context context)
{
    SegmentPtr currentSegment;
    for( auto it = events->cbegin(); it != events->cend(); it++) {
        EventConstPtr event = *it;
        if (event->getContext() == context) {
            if (event->getLabel() == "Foot Strike") {
                if (currentSegment) {
                    UTILS_ASSERT(currentSegment->event1);
                    currentSegment->end = event->getTime();
                    ScalarChannelReaderInterfacePtr nonConstChannel(core::const_pointer_cast<ScalarChannelReaderInterface>(scalar));
                    currentSegment->stats = ScalarChannelStatsPtr(new ScalarChannelStats(nonConstChannel, currentSegment->begin, currentSegment->end));
                    collection.push_back(currentSegment);
                }

                currentSegment.reset(new Segment());
                currentSegment->event1 = event;
                currentSegment->begin = event->getTime();
            } else if (currentSegment && event->getLabel() == "Foot Off") {
                currentSegment->event2 = event;
            }
        } 
    }
}

EventsHelper::EventsHelper( EventsCollectionConstPtr events, ScalarChannelReaderInterfaceConstPtr scalar ) :
events(events),
    scalar(scalar)
{
    createSegments(leftSegments, C3DEventsCollection::Context::Left);
    createSegments(rightSegments, C3DEventsCollection::Context::Right);
}
