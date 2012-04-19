#include "NewChartPCH.h"
#include "NewChartSeriesData.h"
#include "NewChartCurve.h"

NewChartCurve::NewChartCurve( const QString &title /*= QString::null */ ) :
QwtPlotCurve(title),
    xOffset(0.0),
    yOffset(0.0),
    xScale(1.0),
    yScale(1.0)
{
}

void NewChartCurve::drawCurve( QPainter *p, int style, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRectF &canvasRect, int from, int to ) const
{
    QwtPlotCurve::drawCurve(p, style, xMap, yMap, canvasRect, from, to);
}

void NewChartCurve::setSamples( NewChartSeriesData* data )
{
    setData(data);
}

void NewChartCurve::setXOffset( double val )
{
    setOffset(QPointF(val, yOffset));
}

void NewChartCurve::setYOffset( double val )
{
    setOffset(QPointF(xOffset, val));
}

void NewChartCurve::setOffset(const QPointF& point)
{
    xOffset = point.x();
    yOffset = point.y();
    NewChartSeriesData* samples = dynamic_cast<NewChartSeriesData*>(data());
    UTILS_ASSERT(samples);
    if (samples) {
        samples->setOffset(point);
    }
}

QPointF NewChartCurve::getOffset() const
{
    return QPointF(getXOffset(), getYOffset());
}

void NewChartCurve::setXScale( double val )
{
     xScale = val;
     NewChartSeriesData* samples = dynamic_cast<NewChartSeriesData*>(data());
     UTILS_ASSERT(samples);
     if (samples) {
         samples->setScaleX(xScale);
     }
}

void NewChartCurve::setYScale( double val )
{
    yScale = val; 
    NewChartSeriesData* samples = dynamic_cast<NewChartSeriesData*>(data());
    UTILS_ASSERT(samples);
    if (samples) {
        samples->setScaleY(yScale);
    }
}

double NewChartCurve::OffsetTransformation::xForm( double s, double s1, double s2, double p1, double p2 ) const
{
    return p1 - offset + ( p2 - p1 ) / ( s2 - s1 ) * ( s - s1 );
}

double NewChartCurve::OffsetTransformation::invXForm( double p, double p1, double p2, double s1, double s2 ) const
{
    return s1 + offset + ( s2 - s1 ) / ( p2 - p1 ) * ( p - p1 );
}

QwtScaleTransformation * NewChartCurve::OffsetTransformation::copy() const
{
    return new NewChartCurve::OffsetTransformation(getOffset());
}



