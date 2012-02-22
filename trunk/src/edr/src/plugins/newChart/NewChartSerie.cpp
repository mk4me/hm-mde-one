#include "NewChartPCH.h"
#include <QtGui/QPainter>
#include <qwt/qwt_curve_fitter.h>
#include "NewChartSerie.h"
#include "NewChartVisualizer.h"
#include "NewChartSeriesData.h"

const int ACTIVE_WIDTH = 2;
const int NON_ACTIVE_WIDTH = 1;

void NewChartSerie::setData( const core::ObjectWrapperConstPtr & data )
{
    this->data = data;
    curve = new NewChartCurve(data->getName().c_str());
    //curve = new QwtPlotCurve(data->getName().c_str());
    reader = data->get();
    accessor.reset(new ScalarContiniousTimeAccessor(reader));
    ScalarChannelReaderInterfacePtr nonConstChannel(core::const_pointer_cast<ScalarChannelReaderInterface>(reader));
    stats.reset(new ScalarChannelStats(nonConstChannel));
    //int count = reader->size();
    //xvals = new double[count];
    //yvals = new double[count];
    //for (int i = 0; i < count; i++) {
    //    xvals[i] = reader->argument(i);
    //    yvals[i] = reader->value(i);
    //}

    //curve->setRawSamples(xvals, yvals, count);
    curve->setSamples(new NewChartSeriesData(reader));
    int r = rand() % 256;
    int g = rand() % 256;
    int b = rand() % 256;
    setColor(r, g, b);
    setWidth(active ? ACTIVE_WIDTH : NON_ACTIVE_WIDTH);
    curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    curve->setPaintAttribute(QwtPlotCurve::ClipPolygons, false);
    curve->setItemAttribute(QwtPlotItem::AutoScale, true);
    curve->setItemAttribute(QwtPlotItem::Legend, true);
    //QwtWeedingCurveFitter* fitter = new QwtWeedingCurveFitter(0.0);
    //fitter->setFitMode(QwtSplineCurveFitter::ParametricSpline);
    curve->setCurveFitter(nullptr);
//    curve->setPaintAttribute(QwtPlotCurve::CacheSymbols, true);
    //curve->setCurveAttribute( QwtPlotCurve::Fitted );
    visualizer->addPlotCurve(curve, getScales());

    _zBase = curve->z();
    curve->setZ(_zBase + _z);
}

void NewChartSerie::setTime( float time )
{
    this->time = time;
}

void NewChartSerie::setZ(double z, bool replot)
{
    _z = z;
    if(replot == true && curve != nullptr){
        curve->setZ(_zBase + _z);
    }
}

double NewChartSerie::z() const
{
    return _z;
}

void NewChartSerie::setActive( bool val )
{
    this->active = val;
    if (curve) {
        setWidth(val ? ACTIVE_WIDTH : NON_ACTIVE_WIDTH);
    }
}

NewChartSerie::~NewChartSerie()
{
    delete[] xvals;
    delete[] yvals;
}

void NewChartSerie::setEvents( EventsCollectionConstPtr val )
{
    UTILS_ASSERT(val);
    /*EventsPlotItem* eventsItem = new EventsPlotItem(val);
    eventsItem->attach(visualizer->getPlot());*/
    visualizer->setEvents(this, val);
}

void NewChartSerie::removeItemsFromPlot()
{
    curve->detach();
}

const QwtPlotCurve* NewChartSerie::getCurve() const
{
    return curve;
}

void NewChartSerie::setVisible( bool visible )
{
    curve->setVisible(visible);
}

bool NewChartSerie::isVisible() const
{
    return curve->isVisible();
}

QColor NewChartSerie::getColor() const
{
    UTILS_ASSERT(curve);
    return curve->pen().color();
}

void NewChartSerie::setWidth( int width )
{
    UTILS_ASSERT(curve);
    QPen pen = curve->pen();
    pen.setWidth(width);
    curve->setPen(pen);
}

double NewChartSerie::getXOffset() const
{
    return curve->getXOffset();
}

void NewChartSerie::setXOffset( double val )
{
    curve->setXOffset(val);
}

double NewChartSerie::getYOffset() const
{
    return curve->getYOffset();
}

void NewChartSerie::setYOffset( double val )
{
    curve->setYOffset(val);
}

QPointF NewChartSerie::getOffset() const
{
    return curve->getOffset();
}

void NewChartSerie::setOffset( const QPointF& offset )
{
    curve->setOffset(offset);
}
double NewChartSerie::getXScale() const
{
    return curve->getXScale();
}

void NewChartSerie::setXScale( double val )
{
    curve->setXScale(val);
}

double NewChartSerie::getYScale() const
{
    return curve->getYScale();
}

void NewChartSerie::setYScale( double val )
{
    curve->setYScale(val);
}

Scales NewChartSerie::getScales() const
{ 
    return Scales(0.0f, reader->getLength(), getStats()->minValue(), getStats()->maxValue()); 
}


