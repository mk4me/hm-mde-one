#include "NewChartPCH.h"
#include "NewChartSeriesData.h"

NewChartSeriesData::NewChartSeriesData( ScalarChannelReaderInterfaceConstPtr channel, 
    QPointF offset, double scaleX , double scaleY) :
    channel(channel),
    offset(offset),
    scaleX(scaleX),
    scaleY(scaleY)
{

}

size_t NewChartSeriesData::size() const
{
    return channel->size();
}

QPointF NewChartSeriesData::sample( size_t i ) const
{
    return QPointF(channel->argument(i) * scaleX, channel->value(i) * scaleY) + offset;
}

QRectF NewChartSeriesData::boundingRect() const
{
    return QRectF(0, 1000, 20, 2000);
}
