/********************************************************************
	created:	2011/11/12
	created:	12:11:2011   15:12
	filename: 	ChartVisualizer.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__CHARTVISUALIZER_H__
#define HEADER_GUARD_NEW_CHART__CHARTVISUALIZER_H__

#include <QtCore/QObject>
#include <QtGui/QWidget>
#include <QtGui/QComboBox>
#include <core/IVisualizer.h>
#include <core/IObjectSource.h>
#include <plugins/c3d/C3DChannels.h>

#include <qwt/qwt_plot.h>
#include <qwt/qwt_text.h>
#include <qwt/qwt_plot_curve.h>
#include <qwt/qwt_plot_marker.h>
#include <qwt/qwt_series_data.h>
#include <qwt/qwt_plot_grid.h>
#include <qwt/qwt_plot_zoomer.h>
#include <qwt/qwt_picker_machine.h>
#include <qwt/qwt_legend.h>
#include <qwt/qwt_plot_panner.h>
#include <qwt/qwt_plot_magnifier.h>

#include "NewChartSerie.h"
#include "NewChartState.h"

class StatsTable;

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
    };
    typedef core::shared_ptr<Segment> SegmentPtr;
    typedef core::shared_ptr<const Segment> SegmentConstPtr;
    typedef std::vector<SegmentPtr> Segments;
    typedef boost::iterator_range<Segments::const_iterator> SegmentsConstRange;

public:
    EventsHelper(EventsCollectionConstPtr events, ScalarChannelReaderInterfaceConstPtr scalar);
    EventsCollectionConstPtr getEvents() const { return events; }

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

private:
    void createSegments(std::vector<SegmentPtr>& collection, C3DEventsCollection::Context context);

private:
    std::vector<SegmentPtr> leftSegments;
    std::vector<SegmentPtr> rightSegments;
    EventsCollectionConstPtr events;
    ScalarChannelReaderInterfaceConstPtr scalar;
};
typedef core::shared_ptr<EventsHelper> EventsHelperPtr;
typedef core::shared_ptr<const EventsHelper> EventsHelperConstPtr;

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

class NewChartVisualizer : public QObject, public core::IVisualizer, public boost::enable_shared_from_this<NewChartVisualizer>
{
    friend class NewChartSerie;
    Q_OBJECT;
    UNIQUE_ID("{1122BD8A-6056-4965-9AEA-502F99CA2433}", "New Chart Visualizer");

public:
    NewChartVisualizer();
	virtual ~NewChartVisualizer();
    
public:
      QwtPlot* getPlot();
      const NewChartSerie* tryGetCurrentSerie() const;

      virtual IVisualizer* createClone() const;
      
      virtual const std::string& getName() const;
      
      core::IVisualizer::SerieBase *createSerie(const core::ObjectWrapperConstPtr& data, const std::string& name);

      core::IVisualizer::SerieBase *createSerie(const core::IVisualizer::SerieBase * serie);
      
      virtual void removeSerie(core::IVisualizer::SerieBase *serie);

      virtual QWidget* createWidget(std::vector<QObject*>& actions);

      virtual QIcon* createIcon();
      
      virtual int getMaxDataSeries(void) const;

      virtual void getInputInfo( std::vector<core::IInputDescription::InputInfo>& info);

      virtual void setUp(core::IObjectSource* source);
      virtual void update(double deltaTime);
      virtual void reset();

      void setTitle( const QString& title ) { qwtPlot->setTitle(title); }
      void setManipulation(bool val);
      bool isShowLegend() const { return showLegend; }
      void setShowLegend(bool val);
      bool eventFilter( QObject *object, QEvent *event );

      bool isEventMode() const { return eventMode; }
      
private:
      void addPlotCurve(QwtPlotCurve* curve, const Scales& scales);
      void setEvents(NewChartSerie* serie, EventsCollectionConstPtr val );
      void rescale(float t1, float t2);
      void recreateStats(ScalarChannelStatsConstPtr stats = ScalarChannelStatsConstPtr());

private slots:

        void onComboDestroy(QObject * obj);

      void setNormalized(bool normalized);
      void setActiveSerie(int idx);
      void onSerieSelected(QwtPlotItem*);
      void onStateAction();
      void onEventContext();
      void setEventMode(bool val);

private:
      QwtPlot* qwtPlot;
      QwtPlotMarker* qwtMarker;
      core::shared_ptr<QwtPlotGrid> grid;
      core::shared_ptr<QwtPlotZoomer> zoomer;
      Scales plotScales;
      QComboBox* activeSerieCombo;
      std::vector<NewChartSerie*> series;
      std::map<QAction*, NewChartStatePtr> statesMap;
      bool showLegend;
      NewChartStatePtr currentState;
      int currentSerie;
      QwtPlotPanner* plotPanner;
      QwtPlotMagnifier* plotMagnifier;
      EventsPlotItem* eventsItem;
      StatsTable* statsTable;
      bool eventMode;
      bool eventsVisible;
      C3DEventsCollection::Context context;
      std::map<NewChartSerie*, EventsHelperPtr> eventsHelpers;
};
typedef core::shared_ptr<NewChartVisualizer> NewChartVisualizerPtr;
typedef core::shared_ptr<const NewChartVisualizer> NewChartVisualizerConstPtr;


    
#endif  