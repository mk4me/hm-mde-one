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

//! Klasa wizualizuje eventy na wykresie. S� one reprezentowane jako szerokie pasy.
//! Czerwone dla event�w lewych i zielone dla prawych
class EventsPlotItem : public QwtPlotItem
{
public:
    //! Obiekt tworzony jest od razu z eventami 
    //! \param events wska�nik do poprawnych danych
    EventsPlotItem(EventsCollectionConstPtr events);
    virtual ~EventsPlotItem() {}

public:
    //! Metoda dziedziczona z QwtPlotItem, odrysowywuje obiekt na wykresie
    //! \param painter obiekt paintera
    //! \param xMap zapewnia konwersje wsp�rz�dnych w poziomie
    //! \param yMap zapewnia konwersje wsp�rz�dnych w pionie
    //! \param canvasRect obszar, kt�ry mo�na zarysowa�
    virtual void draw( QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRectF &canvasRect ) const;
    //! \return kolekcja z rysowanymi eventami
    EventsCollectionConstPtr getEvents() const { return events; }

private:
    //! odrysowywane eventy
    EventsCollectionConstPtr events;

};

//! Klasa s�u�y do u�atwienia obs�ugi event�w C3D w wizualizatorze
class EventsHelper
{
public:
    //! typ u�ywany do reprezentacji czasu
    typedef float timeType;
    //! niemodyfikowalny wska�nik do wczytanego z pliku C3D zdarzenia
    typedef C3DEventsCollection::EventConstPtr EventConstPtr;
    //! wska�nik do wczytanego z pliku C3D zdarzenia
    typedef C3DEventsCollection::EventPtr EventPtr;
    //! wska�nik do krzywej wykresu
    typedef boost::shared_ptr<QwtPlotCurve> PlotCurvePtr;
    //! segment oznaczaj�cy przedzia� czasowy zwi�zany z eventami
    struct Segment 
    {
        //! pierwszy event
        EventConstPtr event1;
        //! drugi event
        EventConstPtr event2;
        //! pocz�tek czasu, w kt�rym zaczyna si� event
        timeType begin;
        //! koniec czasu, w kt�rym event si� ko�czy
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
    //! konstruktor pobiera dane i zwi�zane z nimi eventy
    EventsHelper(EventsCollectionConstPtr events, ScalarChannelReaderInterfaceConstPtr scalar);
    //! \return eventy C3D
    EventsCollectionConstPtr getEvents() const { return events; }
    //! \return element wykresu odpowiedzialny za rysowanie event�w
    const EventsPlotItem* getEventsItem() const { return eventsItem; }
    //! \return element wykresu odpowiedzialny za rysowanie event�w
    EventsPlotItem* getEventsItem() { return eventsItem; }
    //! \return odpowiedni segment dla podanego czasu lub nullptr je�li takiego nie ma
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
    //! tworzy segmenty na podstawie dostarczonych event�w i danych 
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
