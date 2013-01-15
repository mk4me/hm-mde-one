#include "CorePCH.h"
#include "VisualizerChannel.h"
#include "VisualizerManager.h"

namespace core {

IVisualizerChannel::IVisualizerChannel()
{

}

IVisualizerChannel::~IVisualizerChannel()
{
	VisualizerManager::getInstance()->notifyDestroyed(this);
}

VisualizerChannel::VisualizerChannel(const plugin::VisualizerTimeSeriePtr & serie)
    : serie(serie), constSerie(serie)
{

}

VisualizerChannel::~VisualizerChannel()
{

}

void VisualizerChannel::setTime(double time)
{
    serie->setTime(time);
}

double VisualizerChannel::getLength() const
{
    return serie->getLength();
}

VisualizerChannel * VisualizerChannel::clone() const
{
    //TODO
    //można tutaj wprowadzic zarządzanie klonowaniem serii

    return nullptr;
}

const plugin::VisualizerTimeSeriePtr & VisualizerChannel::getSerie()
{
    return serie;
}

const plugin::VisualizerTimeSerieConstPtr & VisualizerChannel::getSerie() const
{
    return constSerie;
}

VisualizerMultiChannel::VisualizerMultiChannel(const std::vector<plugin::VisualizerTimeSeriePtr> & channelSeries)
    : channelSeries_(channelSeries)
{
    UTILS_ASSERT((channelSeries.empty() == false), "Nie podano żadnych serii dla kanalu");
}

VisualizerMultiChannel::~VisualizerMultiChannel()
{

}

void VisualizerMultiChannel::setTime(double time)
{
    for(auto it = channelSeries_.begin(); it != channelSeries_.end(); ++it){
        (*it)->setTime(time > (*it)->getLength() ? (*it)->getLength() : time);
    }
}

double VisualizerMultiChannel::getLength() const
{
	auto it = channelSeries_.begin();

	auto length = (*it)->getLength();

	++it;

	for( ; it != channelSeries_.end(); ++it){
		length = (std::max)(length, (*it)->getLength());
	}

	return length;
}

VisualizerMultiChannel * VisualizerMultiChannel::clone() const
{
    //TODO
    //można tutaj wprowadzic zarządzanie klonowaniem serii

    return nullptr;
}

const std::vector<plugin::VisualizerTimeSeriePtr> & VisualizerMultiChannel::channelSeries() const
{
    return channelSeries_;
}

}
