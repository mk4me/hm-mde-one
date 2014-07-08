#include "NewChartPCH.h"
#include "NewChartEventStateData.h"

NewChartEventStateData::NewChartEventStateData( ScalarChannelReaderInterfaceConstPtr channel, float startTime, float endTime ) :
    channel(channel)
{
    startIndex = channel->getValueHelper(startTime).first;
    endIndex = channel->getValueHelper(endTime).second;
}

size_t NewChartEventStateData::size() const
{
    return endIndex - startIndex;
}

QPointF NewChartEventStateData::sample( size_t i ) const
{
    float arg = (100.0f * i) / size();
    float val = channel->value(i + startIndex);
    return QPointF(arg, val);
}

QRectF NewChartEventStateData::boundingRect() const
{
    return QRectF(0, 1000, 20, 2000);
}
