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
#include <qwt/qwt_plot_curve.h>
#include <plugins/c3d/C3DChannels.h>

class Scales
{
public:
    Scales() : 
      initialized(false) 
      {}
    Scales(float xMin, float xMax, float yMin, float yMax) :
        xMin(xMin),
        xMax(xMax),
        yMin(yMin),
        yMax(yMax),
        initialized(true) 
    {}

public:
    float getYMax() const { return yMax; }
    float getYMin() const { return yMin; }
    float getXMax() const { return xMax; }
    float getXMin() const { return xMin; }
    bool isInitialized() const { return initialized; }

public:
    void clear() { initialized = false; }
    void merge(const Scales& scales);

private:
    float xMin, xMax, yMin, yMax;
    bool initialized;
};

class NewChartSerie : public EventSerieBase
{
    friend class NewChartVisualizer;
public:
    NewChartSerie(NewChartVisualizer * visualizer) :
      visualizer(visualizer), 
      active(false),
      xvals(nullptr),
      yvals(nullptr),
      time(0.0f),
      _z(0),
      _zBase(0)
    {

    }

    virtual ~NewChartSerie();

public:
    float getTime() const { return time; }
    float getCurrentValue() const 
    { 
        return accessor->getValue(time); 
    } 
    virtual void setTime(float time);

    virtual float getLength() const { return reader->getLength(); } 
    virtual void setEvents(EventsCollectionConstPtr val);

    const QwtPlotCurve* getCurve() const { return curve; }

    void setVisible(bool visible)
    {
        curve->setVisible(visible);
    }

    bool isVisible() const 
    {
        return curve->isVisible();
    }

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

    QColor getColor() const {
        UTILS_ASSERT(curve);
        return curve->pen().color();
    }

    void setWidth(int width)
    {
        UTILS_ASSERT(curve);
        QPen pen = curve->pen();
        pen.setWidth(width);
        curve->setPen(pen);
    }

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

    Scales getScales() const 
    { 
        return Scales(0.0f, reader->getLength(), getStats()->minValue(), getStats()->maxValue()); 
    }

    bool isActive() const { return active; }
    void setActive(bool val);

    ScalarChannelStatsConstPtr getStats() const { return stats; } //return pointHelper->getStats(); }
    ScalarChannelReaderInterfaceConstPtr getReader() const { return reader; }

    void removeItemsFromPlot();

    void setZ(double z, bool replot = false);
    double z() const;

private:
    NewChartVisualizer* visualizer;
    std::string name;
    core::ObjectWrapperConstPtr data;
    QwtPlotCurve* curve;
    ScalarChannelStatsPtr stats;
    core::shared_ptr<ScalarContiniousTimeAccessor> accessor;
    ScalarChannelReaderInterfaceConstPtr reader;
    
    double* xvals;
    double* yvals;
    //PointData* pointHelper;
    bool active;
    float time;
    double _z;
    double _zBase;
    //core::shared_ptr<ScalarModifier> absChannel;
    //core::shared_ptr<ScalarModifier> integratorChannel;
};

#endif
