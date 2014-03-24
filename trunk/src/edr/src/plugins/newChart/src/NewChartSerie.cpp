#include "NewChartPCH.h"
#include <QtGui/QPainter>
#include "NewChartSerie.h"
#include "NewChartVisualizer.h"
#include "NewChartSeriesData.h"

const int ACTIVE_WIDTH = 2;
const int NON_ACTIVE_WIDTH = 1;


NewChartSerie::NewChartSerie( NewChartVisualizer * visualizer ) :
visualizer(visualizer),
    curve(nullptr),
    active(false),
    time(0.0f),
    _z(0),
    _zBase(0)
{

}

void NewChartSerie::setData(const utils::TypeInfo & requestedType, const core::ObjectWrapperConstPtr & data )
{
    this->data = data;
	this->requestedType = requestedType;
	std::string name;
	data->getMetadata("core/name", name);

    curve = new NewChartCurve(name.c_str());	
    data->tryGet(reader);    
    ScalarChannelReaderInterfacePtr nonConstChannel;
    nonConstChannel = core::const_pointer_cast<ScalarChannelReaderInterface>(reader);
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

const utils::TypeInfo & NewChartSerie::getRequestedDataType() const
{
	return requestedType;
}

void NewChartSerie::update()
{

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
	curve = nullptr;
}

void NewChartSerie::setEvents( EventsCollectionConstPtr val )
{
    UTILS_ASSERT(val);
    eventsHelper = EventsHelperPtr(new EventsHelper(val, getReader()));
    setColorsForEvents(eventsHelper->getLeftSegments(), getColor());
    setColorsForEvents(eventsHelper->getRightSegments(), getColor());
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

QwtPlotCurve* NewChartSerie::getCurve()
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
    for (auto it = range.begin(); it != range.end(); ++it) {
        QPen pen = (*it)->normalizedCurve->pen();
        pen.setColor(color);
        (*it)->normalizedCurve->setPen(pen);
    }
}

double NewChartSerie::getCurrentValue() const 
{ 
    return static_cast<double>(ScalarContiniousTimeAccessor::getValue(time, *reader)); 
} 

double NewChartSerie::getLength() const
{
    return reader->getLength();
}

double NewChartSerie::getBegin() const
{
	return 0.0;
}

double NewChartSerie::getEnd() const
{
	return getLength();
}

void NewChartSerie::setColor( int r, int g, int b, int a /*= 255*/ )
{
    setColor(QColor(r, g, b, a));
}

void NewChartSerie::setColor( const QColor& color )
{
    UTILS_ASSERT(curve);
    QPen pen = curve->pen();
    pen.setColor(color);
    curve->setPen(pen);
}

QColor NewChartSerie::getColor() const
{
    UTILS_ASSERT(curve);
    return curve->pen().color();
}

void NewChartSerie::setName( const std::string & name )
{
    this->name = name;
}

const std::string NewChartSerie::getName() const
{
    return name;
}

const core::ObjectWrapperConstPtr & NewChartSerie::getData() const
{
    return data;
}

double NewChartSerie::getTime() const
{
    return time;
}
