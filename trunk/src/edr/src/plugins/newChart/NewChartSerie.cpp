#include "NewChartPCH.h"
#include "NewChartSerie.h"
#include "NewChartVisualizer.h"

const int ACTIVE_WIDTH = 2;
const int NON_ACTIVE_WIDTH = 1;

void NewChartSerie::setData( const core::ObjectWrapperConstPtr & data )
{
    this->data = data;
    curve = new QwtPlotCurve(data->getName().c_str());
    ScalarChannelReaderInterfaceConstPtr reader = data->get();

    pointHelper = new PointData(reader, 500);

    curve->setData(pointHelper);
    int r = rand() % 256;
    int g = rand() % 256;
    int b = rand() % 256;
    setColor(r, g, b);
    setWidth(active ? ACTIVE_WIDTH : NON_ACTIVE_WIDTH);
    curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    //curve->setCurveAttribute( QwtPlotCurve::Fitted );
    visualizer->addPlotCurve(curve, getScales());
}

void NewChartSerie::setTime( float time )
{
    this->time = time;
    //visualizer->markerX = time;
    //visualizer->markerLabel = QString("y(%1) = %2").arg(time).arg(pointHelper->y(time));
}

void NewChartSerie::setActive( bool val )
{
    this->active = val;
    if (curve) {
        setWidth(val ? ACTIVE_WIDTH : NON_ACTIVE_WIDTH);
    }
}
