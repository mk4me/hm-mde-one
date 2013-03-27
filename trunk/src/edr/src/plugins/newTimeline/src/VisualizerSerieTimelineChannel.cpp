#include "TimelinePCH.h"
#include <plugins/newTimeline/VisualizerSerieTimelineChannel.h>
#include <plugins/newTimeline/ITimelineService.h>

VisualizerSerieTimelineChannel::VisualizerSerieTimelineChannel(core::Visualizer * visualizer, core::Visualizer::VisualizerSerie * serie)
    : visualizer(visualizer), serie(serie)
{
	if(serie->timeAvareSerieFeatures() == nullptr){
		throw std::runtime_error("Serie not aware of time");
	}
}

VisualizerSerieTimelineChannel::~VisualizerSerieTimelineChannel()
{

}

void VisualizerSerieTimelineChannel::setTime(double time)
{
    serie->timeAvareSerieFeatures()->setTime(time);
}

double VisualizerSerieTimelineChannel::getLength() const
{
    return serie->timeAvareSerieFeatures()->getLength();
}

void VisualizerSerieTimelineChannel::offsetChanged(double newOffset)
{
	if(serie->timeEditableSerieFeatures()){
		serie->timeEditableSerieFeatures()->setOffset(newOffset);
	}
}

void VisualizerSerieTimelineChannel::scaleChanged(double newScale)
{
	if(serie->timeEditableSerieFeatures()){
		serie->timeEditableSerieFeatures()->setScale(newScale);
	}	
}

VisualizerSerieTimelineChannel * VisualizerSerieTimelineChannel::clone() const
{
    //TODO
    //można tutaj wprowadzic zarządzanie klonowaniem serii

    return nullptr;
}

const core::Visualizer::VisualizerSerie * VisualizerSerieTimelineChannel::getSerie() const
{
    return serie;
}

const core::Visualizer * VisualizerSerieTimelineChannel::getVisualizer() const
{
    return visualizer;
}

VisualizerSerieTimelineMultiChannel::VisualizerSerieTimelineMultiChannel(const VisualizersSeries & visualizersSeries)
    : visualizersSeries(visualizersSeries)
{
    UTILS_ASSERT((visualizersSeries.empty() == false), "Nie podano żadnych serii dla kanalu");
	for(auto it = visualizersSeries.begin(); it != visualizersSeries.end(); ++it){
		if((*it)->timeAvareSerieFeatures() == nullptr){
			throw std::runtime_error("Serie not aware of time");
		}
	}
}

VisualizerSerieTimelineMultiChannel::~VisualizerSerieTimelineMultiChannel()
{

}

void VisualizerSerieTimelineMultiChannel::setTime(double time)
{
    for(auto it = visualizersSeries.begin(); it != visualizersSeries.end(); ++it){
		(*it)->timeAvareSerieFeatures()->setTime(time > (*it)->timeAvareSerieFeatures()->getLength() ? (*it)->timeAvareSerieFeatures()->getLength() : time);
    }
}

double VisualizerSerieTimelineMultiChannel::getLength() const
{
	double length = 0;

	for(auto it = visualizersSeries.begin(); it != visualizersSeries.end(); ++it){
		length = std::max(length, (*it)->timeAvareSerieFeatures()->getLength());
	}

	return length;
}

VisualizerSerieTimelineMultiChannel * VisualizerSerieTimelineMultiChannel::clone() const
{
    //TODO
    //można tutaj wprowadzic zarządzanie klonowaniem serii

    return nullptr;
}

const VisualizerSerieTimelineMultiChannel::VisualizersSeries & VisualizerSerieTimelineMultiChannel::getVisualizersSeries() const
{
    return visualizersSeries;
}

void VisualizerSerieTimelineMultiChannel::offsetChanged(double newOffset)
{
	for(auto it = visualizersSeries.begin(); it != visualizersSeries.end(); ++it){
		if((*it)->timeEditableSerieFeatures()){
			(*it)->timeEditableSerieFeatures()->setOffset(newOffset);
		}
	}
}

void VisualizerSerieTimelineMultiChannel::scaleChanged(double newScale)
{
	for(auto it = visualizersSeries.begin(); it != visualizersSeries.end(); ++it){
		if((*it)->timeEditableSerieFeatures()){
			(*it)->timeEditableSerieFeatures()->setScale(newScale);
		}
	}
}

VisualizerTimelineHelper::VisualizerTimelineHelper(ITimelineService * timeline) : timeline(timeline)
{

}

VisualizerTimelineHelper::~VisualizerTimelineHelper()
{
	for(auto it = seriesToChannels.begin(); it != seriesToChannels.end(); ++it){
		timeline->removeChannel(boost::lexical_cast<std::string>(it->second.first));
		it->first->visualizer()->removeObserver(this);
	}
}

void VisualizerTimelineHelper::update(core::Visualizer::VisualizerSerie * serie, core::Visualizer::SerieModyfication modyfication)
{
	static int idx = 0;
	if(modyfication == core::Visualizer::ADD_SERIE){
		//utwórz kanał
		auto channel = utils::shared_ptr<timeline::IChannel>(new VisualizerSerieTimelineChannel(serie->visualizer(), serie));
		//dodaj do timeline
		timeline->addChannel(boost::lexical_cast<std::string>(idx), channel);
		//zapamiętaj
		seriesToChannels[serie] = std::make_pair(idx++, channel);
	}else{
		//usuń kanał
		auto it = seriesToChannels.find(serie);
		timeline->removeChannel(boost::lexical_cast<std::string>(it->second.first));
		seriesToChannels.erase(serie);
	}
}