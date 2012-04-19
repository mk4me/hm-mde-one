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
    curve->setCurveFitter(nullptr);
    visualizer->addPlotCurve(curve, getScales());

    _zBase = curve->z();
    curve->setZ(_zBase + _z);
}

void NewChartSerie::setTime( double time )
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
    //delete[] xvals;
    //delete[] yvals;
}

void NewChartSerie::setEvents( EventsCollectionConstPtr val )
{
    UTILS_ASSERT(val);
    eventsHelper = EventsHelperPtr(new EventsHelper(val, getReader()));
    setColorsForEvents(eventsHelper->getLeftSegments(), getColor());
    setColorsForEvents(eventsHelper->getRightSegments(), getColor());
    visualizer->setEvents(this, val);

    //EventsHelperPtr helper(new EventsHelper(val, getReader()));
    //eventsHelpers[serie] = helper;
    //int no = 0;
    //for (auto segment = helper->getLeftSegments().begin(); segment != helper->getLeftSegments().end(); segment++) {
    //    statsTable->addEntry(tr("Left"), tr("%1: Left step %2").arg(serie->getName().c_str()).arg(++no),(*segment)->stats, QColor(255, 200, 200));
    //}
    //no = 0;
    //for (auto segment = helper->getRightSegments().begin(); segment != helper->getRightSegments().end(); segment++) {
    //    statsTable->addEntry(tr("Right"), tr("%1: Right step %2").arg(serie->getName().c_str()).arg(++no),(*segment)->stats, QColor(200, 255, 200));
    //}
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

void NewChartSerie::setColorsForEvents( EventsHelper::SegmentsRange range, const QColor& color )
{
    for (auto it = range.begin(); it != range.end(); it++) {
        QPen pen = (*it)->normalizedCurve->pen();
        pen.setColor(color);
        (*it)->normalizedCurve->setPen(pen);
    }
}

