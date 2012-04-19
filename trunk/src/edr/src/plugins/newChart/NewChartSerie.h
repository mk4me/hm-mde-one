/********************************************************************
	created:	2011/11/12
	created:	12:11:2011   18:08
	filename: 	ChartSerie.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__NEWCHARTSERIE_H__
#define HEADER_GUARD_NEW_CHART__NEWCHARTSERIE_H__

#include <core/IVisualizer.h>
#include <core/IObjectSource.h>
#include <qwt/qwt_scale_map.h>
#include <qwt/qwt_plot_curve.h>
#include <plugins/c3d/C3DCollections.h>
#include <plugins/c3d/EventSerieBase.h>
#include "NewChartCurve.h"
#include "NewChartEvents.h"
#include "Scales.h"

class NewChartCurve;

class NewChartSerie : public EventSerieBase
{
    friend class NewChartVisualizer;
public:
    NewChartSerie(NewChartVisualizer * visualizer) :
      visualizer(visualizer), 
      active(false),
      //xvals(nullptr),
      //yvals(nullptr),
      time(0.0f),
      _z(0),
      _zBase(0)
    {

    }

    virtual ~NewChartSerie();

public:
    double getTime() const { return time; }
    double getCurrentValue() const 
    { 
        return accessor->getValue(time); 
    } 
    virtual void setTime(double time);

    virtual double getLength() const { return reader->getLength(); } 
    virtual void setEvents(EventsCollectionConstPtr val);

    const QwtPlotCurve* getCurve() const; 

    void setVisible(bool visible);

    bool isVisible() const;

    EventsHelperPtr getEventsHelper() const { return eventsHelper; }

    void setColor(int r, int g, int b, int a = 255)
    {
        setColor(QColor(r, g, b, a));
    }

    void setColor(const QColor& color)
    {
        UTILS_ASSERT(curve);
        QPen pen = curve->pen();
        pen.setColor(color);
        curve->setPen(pen);
    }
    

    QColor getColor() const
    {
        UTILS_ASSERT(curve);
        return curve->pen().color();
    }

    void setWidth(int width);

    virtual void setName(const std::string & name)
    {
        this->name = name;
    }

    virtual const std::string & getName() const
    {
        return name;
    }

    virtual void setData(const core::ObjectWrapperConstPtr & data);

    virtual const core::ObjectWrapperConstPtr & getData() const
    {
        return data;
    }

    Scales getScales() const; 
    

    bool isActive() const { return active; }
    void setActive(bool val);

    ScalarChannelStatsConstPtr getStats() const { return stats; } //return pointHelper->getStats(); }
    ScalarChannelReaderInterfaceConstPtr getReader() const { return reader; }

    void removeItemsFromPlot();

    void setZ(double z, bool replot = false);
    double z() const;

    double getXScale() const;
    void setXScale(double val);
    double getYScale() const;
    void setYScale(double val);

    double getXOffset() const;
    void setXOffset(double val);
    double getYOffset() const;
    void setYOffset(double val);

    QPointF getOffset() const;
    void setOffset(const QPointF& offset);

private:
    void setColorsForEvents(EventsHelper::SegmentsRange range, const QColor& color);

private:
    NewChartVisualizer* visualizer;
    std::string name;
    core::ObjectWrapperConstPtr data;
    NewChartCurve* curve;
    ScalarChannelStatsPtr stats;
    core::shared_ptr<ScalarContiniousTimeAccessor> accessor;
    ScalarChannelReaderInterfaceConstPtr reader;
    EventsHelperPtr eventsHelper;
    

    //double* xvals;
    //double* yvals;
    bool active;
    double time;
    double _z;
    double _zBase;    
};

#endif
