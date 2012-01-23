#include "NewChartPCH.h"
#include <QtGui/QPainter>
#include "NewChartSerie.h"
#include "NewChartVisualizer.h"
#include <qwt/qwt_curve_fitter.h>

const int ACTIVE_WIDTH = 2;
const int NON_ACTIVE_WIDTH = 1;

void NewChartSerie::setData( const core::ObjectWrapperConstPtr & data )
{
    this->data = data;
    curve = new QwtPlotCurve(data->getName().c_str());
    reader = data->get();
    accessor.reset(new ScalarContiniousTimeAccessor(reader));
    ScalarChannelReaderInterfacePtr nonConstChannel(core::const_pointer_cast<ScalarChannelReaderInterface>(reader));
    stats.reset(new ScalarChannelStats(nonConstChannel));
    int count = reader->size();
    xvals = new double[count];
    yvals = new double[count];
    for (int i = 0; i < count; i++) {
        xvals[i] = reader->argument(i);
        yvals[i] = reader->value(i);
    }

    curve->setRawSamples(xvals, yvals, count);
    int r = rand() % 256;
    int g = rand() % 256;
    int b = rand() % 256;
    setColor(r, g, b);
    setWidth(active ? ACTIVE_WIDTH : NON_ACTIVE_WIDTH);
    curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    curve->setPaintAttribute(QwtPlotCurve::ClipPolygons, false);
    curve->setItemAttribute(QwtPlotItem::AutoScale, true);
    QwtWeedingCurveFitter* fitter = new QwtWeedingCurveFitter(0.0);
    //fitter->setFitMode(QwtSplineCurveFitter::ParametricSpline);
    curve->setCurveFitter(nullptr);
//    curve->setPaintAttribute(QwtPlotCurve::CacheSymbols, true);
    //curve->setCurveAttribute( QwtPlotCurve::Fitted );
    visualizer->addPlotCurve(curve, getScales());
}

void NewChartSerie::setTime( float time )
{
    this->time = time;
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
