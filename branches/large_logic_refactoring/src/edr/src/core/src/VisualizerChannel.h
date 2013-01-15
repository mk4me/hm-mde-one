/********************************************************************
    created:  2012/04/05
    created:  5:4:2012   8:45
    filename: VisualizerChannel.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___VISUALIZERCHANNEL_H__
#define HEADER_GUARD___VISUALIZERCHANNEL_H__

#include <string>
#include "Visualizer.h"
#include <timelinelib/IChannel.h>

namespace core {

class IVisualizerChannel : public timeline::IChannel
{
public:
	IVisualizerChannel();
	virtual ~IVisualizerChannel();
};

class VisualizerChannel : public IVisualizerChannel
{
public:

	VisualizerChannel(const plugin::VisualizerTimeSeriePtr & serie);

	virtual ~VisualizerChannel();

	//! \return Sklonowany kanał
	virtual VisualizerChannel * clone() const;

	//! \return długość kanału w sekundach
	virtual double getLength() const;

	//! Czas zawiera się między 0 a getLength()
	//! \param time Aktualny, lokalny czas kanału w sekundach
	virtual void setTime(double time);

	const plugin::VisualizerTimeSeriePtr & getSerie();

	const plugin::VisualizerTimeSerieConstPtr & getSerie() const;

private:

	plugin::VisualizerTimeSeriePtr serie;
	plugin::VisualizerTimeSerieConstPtr constSerie;
};


class VisualizerMultiChannel : public IVisualizerChannel
{
public:

	VisualizerMultiChannel(const std::vector<plugin::VisualizerTimeSeriePtr> & channelSeries);

	virtual ~VisualizerMultiChannel();

	//! \return Sklonowany kanał
	virtual VisualizerMultiChannel * clone() const;

	//! \return długość kanału w sekundach
	virtual double getLength() const;

	//! Czas zawiera się między 0 a getLength()
	//! \param time Aktualny, lokalny czas kanału w sekundach
	virtual void setTime(double time);

	const std::vector<plugin::VisualizerTimeSeriePtr> & channelSeries() const;

private:

	std::vector<plugin::VisualizerTimeSeriePtr> channelSeries_;
};

typedef shared_ptr<IVisualizerChannel> VisualizerChannelPtr;
typedef shared_ptr<const IVisualizerChannel> VisualizerChannelConstPtr;

}

#endif	//	HEADER_GUARD___VISUALIZERCHANNEL_H__
