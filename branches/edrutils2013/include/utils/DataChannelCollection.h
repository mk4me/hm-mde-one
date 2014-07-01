/********************************************************************
	created:	2011/07/04
	created:	4:7:2011   20:56
	filename: 	DataChannelCollection.h
	author:		Wojciech Kniec

	purpose:
*********************************************************************/

#ifndef HEADER_GUARD_UTILS__DATACHANNELCOLLECTION_H__
#define HEADER_GUARD_UTILS__DATACHANNELCOLLECTION_H__

#include <vector>
#include <algorithm>
#include <iterator>
#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include <utils/DataChannelDescriptors.h>
#include <utils/DataChannelAccessors.h>
////////////////////////////////////////////////////////////////////////////////
namespace utils {
////////////////////////////////////////////////////////////////////////////////

//TODO
//cała ta klasa do rev -> ciężko ją było skompilować budując np. plugin_c3d w edr!!

//! Klasa agreguje klasy DataChannel, wszystkie dodawane kanały powinny mieć tyle samo wpisow
template <class Channel>
class DataChannelCollection
{
    UTILS_STATIC_ASSERT((boost::is_base_of<IRawGeneralDataChannelReader<typename Channel::point_type, typename Channel::time_type>, Channel>::value), "Base class should inherit from IRawGeneralDataChannelReader");

public:
	typedef Channel ChannelType;
    typedef typename Channel::time_type TimeType;
    typedef typename Channel::point_type PointType;
	typedef boost::shared_ptr<ChannelType> ChannelPtr;
	typedef boost::shared_ptr<const ChannelType> ChannelConstPtr;
	typedef std::vector<ChannelPtr> Collection;
	typedef typename Collection::iterator iterator;
	typedef typename Collection::const_iterator const_iterator;
	typedef boost::tuple<PointType, TimeType, int> PointTimeIndex;
	typedef DataChannelTimeAccessor<typename Channel::point_type, typename Channel::time_type> DefaultTimeAccessor;
protected:
	Collection channels;
	//! numer konfiguracji
	int configurationID;

public:
	DataChannelCollection() {}
	DataChannelCollection(const DataChannelCollection& dc) 
	{
		int count = static_cast<int>(dc.channels.size());
		channels.resize(count);
		for (int i = 0; i < count; ++i) {
			channels[i] = ChannelPtr(dc.channels[i]->clone());
		}
	}
	virtual ~DataChannelCollection() {}

public:
	void addChannel(const ChannelPtr & ptr)
	{
		UTILS_ASSERT(channels.size() == 0 || ptr->size() == channels[0]->size());
		channels.push_back(ptr);
	}

	const ChannelPtr & getChannel(int index)
	{
		UTILS_ASSERT(index >= 0 && index < static_cast<int>(channels.size()));
		return channels[index];
	}

	ChannelConstPtr getChannel(int index) const
	{
		UTILS_ASSERT(index >= 0 && index < static_cast<int>(channels.size()));
		return channels[index];
	}

    ChannelConstPtr getChannel(const std::string& name) const
    {
        for (int i = channels.size() - 1; i >= 0; --i) {
            if (channels[i]->getName() == name) {
                return channels[i];
            }
        }

        throw std::runtime_error("Unknown channel name");
    }

	int getNumPointsPerChannel() const
	{
		UTILS_ASSERT(channels.size());
		return channels[0]->size();
	}

	int getNumChannels() const
	{
		return channels.size();
	}
	
	//! \return długość kanału w sekundach; gdy nie ma danych zwracane jest zero, jest to odejście od konwecji, ale dzialanie całkiem naturalne.
	TimeType getLength() const
	{
		if (channels.size() == 0) {
			return TimeType(0);
		} else {
			return channels[0]->getLength();
		}
	}

	//! \tparam Typ akcesora czasu
	//! \param time czas, dla którego pobieramy wartości
	//! \return wartości wszystkich kanałów dla konkretnego czasu
	template<typename TimeAccessor>
	std::vector<PointType> getValues(TimeType time) const
	{
		int count = static_cast<int>(channels.size());
		std::vector<PointType> res(channels.size());
		for (int i = 0; i < count; ++i) {
			//res[i] = channels[i]->getValue(time);
            res[i] = TimeAccessor::getValue(time, *channels[i]);
		}
		return res;
	}

	//! \param time czas, dla którego pobieramy wartości
	//! \return wartości wszystkich kanałów dla konkretnego czasu
	std::vector<PointType> getValues(TimeType time) const
	{
		return getValues<DefaultTimeAccessor>(time);
	}

	int getConfigurationID() const
	{
		return configurationID;
	}


	void setConfigurationID(int id)
	{
		configurationID = id;
	}

	//! \tparam Typ akcesora czasu
	//! \param index indeks kanału, dla którego pobieramy wartość
	//! \param time czas, dla którego będzie zwrócona wartość
	//! \return wartość kanału
	template<typename TimeAccessor>		
	PointType getValue(int index, TimeType time) const
	{
		return TimeAccessor::getValue(time, *getChannel(index));
	}

	//! \param index indeks kanału, dla którego pobieramy wartość
	//! \param time czas, dla którego będzie zwrócona wartość
	//! \return wartość kanału
	PointType getValue(int index, TimeType time) const
	{
		return getValue<DefaultTimeAccessor>(index, time);
	}
	
	//! \return krotka z maksymalną wartością, jej czasem i indeksem kanału w calej dziedzinie dla wszystkich kanałów
	template<class _Fun>
	PointTimeIndex getValue(_Fun f) const
	{
		std::vector<PointTimeIndex> values;

		int channelsCount = getNumChannels();
		for (int channelIdx = 0; channelIdx < channelsCount; ++channelIdx) {
			PointType chosenVal = channels[channelIdx]->value(0);
			int chosenIdx = 0;
			int count = channels[channelIdx]->size();
			for(int i = 1; i < count; ++i) {
				auto val = channels[channelIdx]->value(i);
				if (f(chosenVal, val)) {
					chosenVal = val;
					chosenIdx = i;
				}
			}
			values.push_back(boost::make_tuple(
				channels[channelIdx]->value(chosenIdx),
				channels[channelIdx]->argument(chosenIdx),
				channelIdx));
		}
		
		int count = values.size();
		auto chosenVal = values[0].template get<0>();
		int chosenIdx = 0;
		for (int i = 1; i < count; ++i) {
			auto val = values[i].template get<0>();
			if (f(chosenVal, val)) {
				chosenIdx = i;
				chosenVal = val;
			}
		}

		return values[chosenIdx];
	}

	//! \return krotka z minimalną wartością, jej czasem i indeksem kanału w calej dziedzinie dla wszystkich kanałów
	PointTimeIndex getMinValue() const
	{
		return getValue([&](const PointType& p1, const PointType& p2) { return p2 < p1; });
	}

	//! \return krotka z maksymalną wartością, jej czasem i indeksem kanału w calej dziedzinie dla wszystkich kanałów
	PointTimeIndex getMaxValue() const
	{
		return getValue([&](const PointType& p1, const PointType& p2) { return p1 < p2; });
	}

};
////////////////////////////////////////////////////////////////////////////////
} //namespace utils
////////////////////////////////////////////////////////////////////////////////

#endif
