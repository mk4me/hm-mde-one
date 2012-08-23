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

class IVisualizerChannel : public timeline::IChannel
{
public:
	IVisualizerChannel();
	virtual ~IVisualizerChannel();
};

class VisualizerChannel : public IVisualizerChannel
{
public:

	VisualizerChannel(const core::VisualizerTimeSeriePtr & serie);

	virtual ~VisualizerChannel();

	//! \return Sklonowany kana³
	virtual VisualizerChannel * clone() const;

	//! \return d³ugoœæ kana³u w sekundach
	virtual double getLength() const;

	//! Czas zawiera siê miêdzy 0 a getLength()
	//! \param time Aktualny, lokalny czas kana³u w sekundach
	virtual void setTime(double time);

	const core::VisualizerTimeSeriePtr & getSerie();

	const core::VisualizerTimeSerieConstPtr & getSerie() const;

private:

	core::VisualizerTimeSeriePtr serie;
	core::VisualizerTimeSerieConstPtr constSerie;
};


class VisualizerMultiChannel : public IVisualizerChannel
{
public:

	VisualizerMultiChannel(const std::vector<core::VisualizerTimeSeriePtr> & channelSeries);

	virtual ~VisualizerMultiChannel();

	//! \return Sklonowany kana³
	virtual VisualizerMultiChannel * clone() const;

	//! \return d³ugoœæ kana³u w sekundach
	virtual double getLength() const;

	//! Czas zawiera siê miêdzy 0 a getLength()
	//! \param time Aktualny, lokalny czas kana³u w sekundach
	virtual void setTime(double time);

	const std::vector<core::VisualizerTimeSeriePtr> & channelSeries() const;

private:

	std::vector<core::VisualizerTimeSeriePtr> channelSeries_;
};

typedef boost::shared_ptr<IVisualizerChannel> VisualizerChannelPtr;
typedef boost::shared_ptr<const IVisualizerChannel> VisualizerChannelConstPtr;

#endif	//	HEADER_GUARD___VISUALIZERCHANNEL_H__
