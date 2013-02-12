/********************************************************************
    created:  2012/04/05
    created:  5:4:2012   8:45
    filename: VisualizerSerieTimelineChannel.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___VISUALIZERCHANNEL_H__
#define HEADER_GUARD___VISUALIZERCHANNEL_H__

#include <plugins/newTimeline/Export.h>
#include <string>
#include <corelib/Visualizer.h>
#include <timelinelib/IChannel.h>

class ITimelineService;

class PLUGINTIMELINE_EXPORT VisualizerSerieTimelineChannel : public timeline::IChannel
{
public:

	VisualizerSerieTimelineChannel(core::Visualizer * visualizer, core::Visualizer::VisualizerSerie * serie);

	virtual ~VisualizerSerieTimelineChannel();

	//! \return Sklonowany kanał
	virtual VisualizerSerieTimelineChannel * clone() const;

	//! \return długość kanału w sekundach
	virtual double getLength() const;

	//! Czas zawiera się między 0 a getLength()
	//! \param time Aktualny, lokalny czas kanału w sekundach
	virtual void setTime(double time);

	//! \param newOffset Nowy offset ustawiony kanałowi
	virtual void offsetChanged(double newOffset);

	//! \param newScale Nowa skala ustawiona kanałowi
	virtual void scaleChanged(double newScale);

	const core::Visualizer::VisualizerSerie * getSerie() const;

	const core::Visualizer * getVisualizer() const;

private:

	core::Visualizer * visualizer;
	core::Visualizer::VisualizerSerie * serie;
};


class PLUGINTIMELINE_EXPORT VisualizerSerieTimelineMultiChannel : public timeline::IChannel
{
public:

	typedef std::list<core::Visualizer::VisualizerSerie*> VisualizerSeries;
	typedef std::map<core::Visualizer*, VisualizerSeries> VisualizersSeries;

public:

	VisualizerSerieTimelineMultiChannel(const VisualizersSeries & visualizersSeries);

	virtual ~VisualizerSerieTimelineMultiChannel();

	//! \return Sklonowany kanał
	virtual VisualizerSerieTimelineMultiChannel * clone() const;

	//! \return długość kanału w sekundach
	virtual double getLength() const;

	//! Czas zawiera się między 0 a getLength()
	//! \param time Aktualny, lokalny czas kanału w sekundach
	virtual void setTime(double time);

	//! \param newOffset Nowy offset ustawiony kanałowi
	virtual void offsetChanged(double newOffset);

	//! \param newScale Nowa skala ustawiona kanałowi
	virtual void scaleChanged(double newScale);

	const VisualizersSeries & getVisualizersSeries() const;

private:

	VisualizersSeries visualizersSeries;
};

class PLUGINTIMELINE_EXPORT VisualizerTimelineHelper : public core::Visualizer::IVisualizerObserver
{
public:
	VisualizerTimelineHelper(ITimelineService * timeline);
	virtual ~VisualizerTimelineHelper();

public:
	virtual void update(core::Visualizer::VisualizerSerie * serie, core::Visualizer::SerieModyfication modyfication);

private:
	ITimelineService * timeline;
	std::map<core::Visualizer::VisualizerSerie *, std::pair<int, core::shared_ptr<timeline::IChannel>>> seriesToChannels;
};

#endif	//	HEADER_GUARD___VISUALIZERCHANNEL_H__
