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

//! Klasa wizualizuje eventy na wykresie. S¹ one reprezentowane jako szerokie pasy.
//! Czerwone dla eventow lewych i zielone dla prawych
class EventsPlotItem : public QwtPlotItem
{
public:
    //! Obiekt tworzony jest od razu z eventami 
    //! \param events wskaŸnik do poprawnych danych
    EventsPlotItem(EventsCollectionConstPtr events);
    virtual ~EventsPlotItem() {}

public:
    //! Metoda dziedziczona z QwtPlotItem, odrysowywuje obiekt na wykresie
    //! \param painter obiekt paintera
    //! \param xMap zapewnia konwersje wspó³rzêdnych w poziomie
    //! \param yMap zapewnia konwersje wspó³rzêdnych w pionie
    //! \param canvasRect obszar, który mo¿na zarysowac
    virtual void draw( QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRectF &canvasRect ) const;
    //! \return kolekcja z rysowanymi eventami
    EventsCollectionConstPtr getEvents() const { return events; }

private:
    //! odrysowywane eventy
    EventsCollectionConstPtr events;

};

//! Klasa s³u¿y do ulatwienia obs³ugi eventow C3D w wizualizatorze
class EventsHelper
{
public:
    typedef float timeType;
    typedef C3DEventsCollection::EventConstPtr EventConstPtr;
    typedef C3DEventsCollection::EventPtr EventPtr;
    typedef boost::shared_ptr<QwtPlotCurve> PlotCurvePtr;
    //! segment oznaczajacy przedzia³ czasowy zwiazany z eventami
    struct Segment 
    {
        //! pierszy event
        EventConstPtr event1;
        //! durgi event
        EventConstPtr event2;
        //! pocz¹tek czasu, w którym zaczyna siê event
        timeType begin;
        //! koniec czasu, w którym event siê konczy
        timeType end;
        //! dane ze statystykami (znormalizowanego odcinka)
        ScalarChannelStatsPtr stats;
        //! znormalizowana krzywa
        PlotCurvePtr normalizedCurve;
    };
    typedef core::shared_ptr<Segment> SegmentPtr;
    typedef core::shared_ptr<const Segment> SegmentConstPtr;
    typedef std::vector<SegmentPtr> Segments;
    typedef boost::iterator_range<Segments::const_iterator> SegmentsConstRange;
    typedef boost::iterator_range<Segments::iterator> SegmentsRange;

public:
    //! konstruktor pobiera dane i zwi¹zane z nimi eventy
    EventsHelper(EventsCollectionConstPtr events, ScalarChannelReaderInterfaceConstPtr scalar);
    //! \return eventy C3D
    EventsCollectionConstPtr getEvents() const { return events; }
    //! \return element wykresu odpowiedzialny za rysowanie eventow
    const EventsPlotItem* getEventsItem() const { return eventsItem; }
    //! \return element wykresu odpowiedzialny za rysowanie eventow
    EventsPlotItem* getEventsItem() { return eventsItem; }
    //! \return odpowiedni segment dla podanego czasu lub nullptr jeœli takiego nie ma
    SegmentConstPtr getSegment(timeType time, C3DEventsCollection::Context context);
    //! \return zwraca segmenty dla prawej nogi
    SegmentsConstRange getRightSegments() const { return boost::make_iterator_range(rightSegments.cbegin(), rightSegments.cend()); }
    //! \return zwraca segmenty dla lewej nogi
    SegmentsConstRange getLeftSegments() const { return boost::make_iterator_range(leftSegments.cbegin(), leftSegments.cend());  }
    //! \return zwraca segmenty dla prawej nogi
    SegmentsRange getRightSegments() { return boost::make_iterator_range(rightSegments.begin(), rightSegments.end()); }
    //! \return zwraca segmenty dla lewej nogi
    SegmentsRange getLeftSegments() { return boost::make_iterator_range(leftSegments.begin(), leftSegments.end());  }

private:
    //! tworzy segmenty na podstawie dostarczonych eventow i danych 
    void createSegments(std::vector<SegmentPtr>& collection, C3DEventsCollection::Context context);

private:
    //! kolekcja z lewymi segmentami
    std::vector<SegmentPtr> leftSegments;
    //! kolekcja z prawymi segmentami
    std::vector<SegmentPtr> rightSegments;
    //! dostarczone eventy c3d
    EventsCollectionConstPtr events;
    //! dostarczone dane
    ScalarChannelReaderInterfaceConstPtr scalar;
    //! element rysowany na wykresie
    EventsPlotItem* eventsItem;
};
typedef core::shared_ptr<EventsHelper> EventsHelperPtr;
typedef core::shared_ptr<const EventsHelper> EventsHelperConstPtr;


#endif
