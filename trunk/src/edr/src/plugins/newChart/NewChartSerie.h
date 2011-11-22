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
protected:
    class PointData: public QwtSyntheticPointData
    {
    public:
        PointData(ScalarChannelReaderInterfaceConstPtr reader, int size = 100):
          QwtSyntheticPointData(size),
              reader(reader),
              accessor(new ScalarContiniousTimeAccessor(reader))          
          {
              ScalarChannelReaderInterfacePtr nonConstChannel(core::const_pointer_cast<ScalarChannelReaderInterface>(reader));
              stats.reset(new ScalarChannelStats(nonConstChannel));
          }
          virtual double y(double x) const
          {
              if (x > 3.3f && x < 3.5f) {
                  int x = 0;
                  x = x;
              }
              if (x >= 0 && x < reader->getLength()) {

                  return accessor->getValue(x);
              } else {
                  return 0.0;
              }
          }

          Scales getScales() const { return Scales(0.0f, reader->getLength(), getMin(), getMax()); }
          float getLength() const { return reader->getLength(); }
          float getMin() const { return stats->minValue(); }
          float getMax() const { return stats->maxValue(); }

    private:
        ScalarChannelReaderInterfaceConstPtr reader;
        ScalarChannelStatsPtr stats;
        boost::shared_ptr<ScalarContiniousTimeAccessor> accessor;
    };
public:
    NewChartSerie(NewChartVisualizer * visualizer)
        : visualizer(visualizer), active(false)
    {

    }

    virtual void setTime(float time);

    virtual float getLength() const { return pointHelper->getLength(); }

    void setColor(int r, int g, int b, int a = 255) 
    {
        UTILS_ASSERT(curve);
        QPen pen = curve->pen();
        pen.setColor(QColor(r, g, b, a));
        curve->setPen(pen);
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
        UTILS_ASSERT(pointHelper);
        return pointHelper->getScales(); 
    }

    bool getActive() const { return active; }
    void setActive(bool val);

private:
    NewChartVisualizer* visualizer;
    std::string name;
    core::ObjectWrapperConstPtr data;
    QwtPlotCurve* curve;
    PointData* pointHelper;
    bool active;

};

#endif
