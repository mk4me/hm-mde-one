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

//! Klasa wizualizuje eventy na wykresie. Są one reprezentowane jako szerokie pasy.
//! Czerwone dla eventów lewych i zielone dla prawych
class EventsPlotItem : public QwtPlotItem
{
public:
    //! Obiekt tworzony jest od razu z eventami 
    //! \param events wskaźnik do poprawnych danych
    EventsPlotItem(EventsCollectionConstPtr events);
    virtual ~EventsPlotItem() {}

public:
    //! Metoda dziedziczona z QwtPlotItem, odrysowywuje obiekt na wykresie
    //! \param painter obiekt paintera
    //! \param xMap zapewnia konwersje współrzędnych w poziomie
    //! \param yMap zapewnia konwersje współrzędnych w pionie
    //! \param canvasRect obszar, który można zarysować
    virtual void draw( QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRectF &canvasRect ) const;
    //! \return kolekcja z rysowanymi eventami
    EventsCollectionConstPtr getEvents() const { return events; }

private:
    //! odrysowywane eventy
    EventsCollectionConstPtr events;

};

//! Klasa służy do ułatwienia obsługi eventów C3D w wizualizatorze
class EventsHelper
{
public:
    //! typ używany do reprezentacji czasu
    typedef float timeType;
    //! niemodyfikowalny wskaźnik do wczytanego z pliku C3D zdarzenia
    typedef C3DEventsCollection::EventConstPtr EventConstPtr;
    //! wskaźnik do wczytanego z pliku C3D zdarzenia
    typedef C3DEventsCollection::EventPtr EventPtr;
    //! wskażnik do krzywej wykresu
    typedef boost::shared_ptr<QwtPlotCurve> PlotCurvePtr;
    //! segment oznaczający przedział czasowy związany z eventami
    struct Segment 
    {
        //! pierwszy event
        EventConstPtr event1;
        //! drugi event
        EventConstPtr event2;
        //! początek czasu, w którym zaczyna się event
        timeType begin;
        //! koniec czasu, w którym event się kończy
        timeType end;
        //! dane ze statystykami (znormalizowanego odcinka)
        ScalarChannelStatsPtr stats;
        //! znormalizowana krzywa
        PlotCurvePtr normalizedCurve;
    };
    //! 
    typedef core::shared_ptr<Segment> SegmentPtr;
    //!
    typedef core::shared_ptr<const Segment> SegmentConstPtr;
    //! 
    typedef std::vector<SegmentPtr> Segments;
    //!
    typedef boost::iterator_range<Segments::const_iterator> SegmentsConstRange;
    //!
    typedef boost::iterator_range<Segments::iterator> SegmentsRange;

public:
    //! konstruktor pobiera dane i związane z nimi eventy
    EventsHelper(EventsCollectionConstPtr events, ScalarChannelReaderInterfaceConstPtr scalar);
    //! \return eventy C3D
    EventsCollectionConstPtr getEvents() const { return events; }
    //! \return element wykresu odpowiedzialny za rysowanie eventów
    const EventsPlotItem* getEventsItem() const { return eventsItem; }
    //! \return element wykresu odpowiedzialny za rysowanie eventów
    EventsPlotItem* getEventsItem() { return eventsItem; }
    //! \return odpowiedni segment dla podanego czasu lub nullptr jeśli takiego nie ma
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
    //! tworzy segmenty na podstawie dostarczonych eventów i danych 
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
