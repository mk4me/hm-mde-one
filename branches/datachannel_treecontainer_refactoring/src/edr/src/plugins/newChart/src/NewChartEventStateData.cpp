#include "NewChartPCH.h"
#include "NewChartEventStateData.h"
#include <datachannellib/IBoundedArgumentsFeature.h>
#include <datachannellib/IBoundedValuesFeature.h>

QRectF channelBoundingRect(c3dlib::ScalarChannelReaderInterfaceConstPtr channel)
{
	if (channel == nullptr){
		return QRectF();
	}

	QRectF ret(0, 1000, 20, 2000);

	auto baf = channel->getOrCreateArgumentFeature<datachannel::IBoundedArgumentsFeature>();	
	ret.setLeft(baf->minArgument());
	ret.setRight(baf->maxArgument());
	
	auto bvf = channel->getOrCreateValueFeature<datachannel::IBoundedValuesFeature>();	
	ret.setBottom(bvf->minValue());
	ret.setTop(bvf->maxValue());	

	return ret;
}

NewChartEventStateData::NewChartEventStateData(c3dlib::ScalarChannelReaderInterfaceConstPtr channel, float startTime, float endTime) :
	channel(channel), startIndex(datachannel::NearestArgumentsFinder::range(*channel, startTime).first),
	endIndex(datachannel::NearestArgumentsFinder::range(*channel, startTime).second),
	boundingRect_(channelBoundingRect(channel))
{	
	
}

size_t NewChartEventStateData::size() const
{
    return endIndex - startIndex;
}

QPointF NewChartEventStateData::sample( size_t i ) const
{
    const float arg = (100.0f * i) / size();
    const float val = channel->value(i + startIndex);
    return QPointF(arg, val);
}

QRectF NewChartEventStateData::boundingRect() const
{
	return boundingRect_;
}
