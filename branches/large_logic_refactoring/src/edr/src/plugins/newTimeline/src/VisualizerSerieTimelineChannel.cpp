#include "TimelinePCH.h"
#include <plugins/newTimeline/VisualizerSerieTimelineChannel.h>

VisualizerSerieTimelineChannel::VisualizerSerieTimelineChannel(Visualizer * visualizer, Visualizer::VisualizerSerie * serie)
    : visualizer(visualizer), serie(serie)
{

}

VisualizerSerieTimelineChannel::~VisualizerSerieTimelineChannel()
{

}

void VisualizerSerieTimelineChannel::setTime(double time)
{
    serie->timeSerieFeatures()->setTime(time);
}

double VisualizerSerieTimelineChannel::getLength() const
{
    return serie->timeSerieFeatures()->getLength();
}

void VisualizerSerieTimelineChannel::offsetChanged(double newOffset)
{
	serie->timeSerieFeatures()->setOffset(newOffset);
}

void VisualizerSerieTimelineChannel::scaleChanged(double newScale)
{
	serie->timeSerieFeatures()->setScale(newScale);
}

VisualizerSerieTimelineChannel * VisualizerSerieTimelineChannel::clone() const
{
    //TODO
    //można tutaj wprowadzic zarządzanie klonowaniem serii

    return nullptr;
}

const Visualizer::VisualizerSerie * VisualizerSerieTimelineChannel::getSerie() const
{
    return serie;
}

const Visualizer * VisualizerSerieTimelineChannel::getVisualizer() const
{
    return visualizer;
}

VisualizerSerieTimelineMultiChannel::VisualizerSerieTimelineMultiChannel(const VisualizersSeries & visualizersSeries)
    : visualizersSeries(visualizersSeries)
{
    UTILS_ASSERT((visualizersSeries.empty() == false), "Nie podano żadnych serii dla kanalu");
}

VisualizerSerieTimelineMultiChannel::~VisualizerSerieTimelineMultiChannel()
{

}

void VisualizerSerieTimelineMultiChannel::setTime(double time)
{
    for(auto it = visualizersSeries.begin(); it != visualizersSeries.end(); ++it){
		for(auto serieIT = it->second.begin(); serieIT != it->second.end(); ++serieIT){
			(*serieIT)->timeSerieFeatures()->setTime(time > (*serieIT)->timeSerieFeatures()->getLength() ? (*serieIT)->timeSerieFeatures()->getLength() : time);
		}
    }
}

double VisualizerSerieTimelineMultiChannel::getLength() const
{
	double length = 0;

	for(auto it = visualizersSeries.begin(); it != visualizersSeries.end(); ++it){
		for(auto serieIT = it->second.begin(); serieIT != it->second.end(); ++serieIT){
			length = std::max(length, (*serieIT)->timeSerieFeatures()->getLength());
		}
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