/********************************************************************
	created:	2011/11/12
	created:	12:11:2011   18:08
	filename: 	ChartSerie.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__CHARTSERIE_H__
#define HEADER_GUARD_NEW_CHART__CHARTSERIE_H__

#include <core/IVisualizer.h>
#include <core/IObjectSource.h>
#include <plugins/c3d/C3DChannels.h>
#include <qwt/qwt_plot_curve.h>

class NewChartVisualizer;

struct Scales
{
    Scales() : initialized(false) {}
    Scales(float xMin, float xMax, float yMin, float yMax) :
    xMin(xMin), xMax(xMax), yMin(yMin), yMax(yMax), initialized(true) {}

    float xMin, xMax, yMin, yMax;
    bool initialized;
};

class NewChartSerie : public core::IVisualizer::TimeSerieBase
{
    friend class NewChartVisualizer;
public:
    NewChartSerie(NewChartVisualizer * visualizer) :
      visualizer(visualizer), 
      active(false),
      xvals(nullptr),
      yvals(nullptr),
      time(0.0f)
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

    const QwtPlotCurve* getCurve() const { return curve; }

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

    bool getActive() const { return active; }
    void setActive(bool val);

    ScalarChannelStatsConstPtr getStats() const { return stats; } //return pointHelper->getStats(); }

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
    //core::shared_ptr<ScalarModifier> absChannel;
    //core::shared_ptr<ScalarModifier> integratorChannel;
};

#endif
