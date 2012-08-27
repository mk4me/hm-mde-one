﻿/********************************************************************
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

//! Klasa agreguje klasy DataChannel, wszystkie dodawane kanały powinny mieć tyle samo wpisow
//template <class Channel, template<typename Point, typename Time> class Interpolator = LerpInterpolator>
template <class Channel, class TimeAccessor = DataChannelTimeAccessor<typename Channel::point_type, typename Channel::time_type>>
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
protected:
	Collection channels;
	//! numer konfiguracji
	int configurationID;

public:
	DataChannelCollection() {}
	virtual ~DataChannelCollection() {}
public:
	virtual DataChannelCollection* clone()
	{
		DataChannelCollection* obj = new DataChannelCollection();
		int count = static_cast<int>(channels.size());
		obj->channels.resize(count);
		for (int i = 0; i < count; ++i) {
			obj->channels[i] = ChannelPtr(this->channels[i]->clone());
		}
		return obj;
	}
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

	double getFrequency()
	{
		UTILS_ASSERT(channels.size());
		return channels[0]->size();
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

	//! \param time czas, dla którego pobieramy wartości
	//! \return wartości wszystkich kanałów dla konkretnego czasu
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

	int getConfigurationID() const
	{
		return configurationID;
	}


	void setConfigurationID(int id)
	{
		configurationID = id;
	}

	//! \param index indeks kanału, dla którego pobieramy wartość
	//! \param time czas, dla którego będzie zwrócona wartość
	//! \return wartość kanału
	PointType getValue(int index, TimeType time) const
	{
		return TimeAccessor::getValue(time, *getChannel(index));
	}

	//! \return maksymalna wartość w calej dziedzinie dla wszystkich kanałów
	boost::tuple<PointType, TimeType, int> getMaxValue() const
	{
		int index = _getIndex(
			[&](int i) { return channels[i]->getMaxValue(); },
			[&](iterator b, iterator e) -> int { return std::max_element(channels.begin(), channels.end()); }
		);
		TimeType time = channels[index]->getMaxValueTime();
		PointType val = channels[index]->getMaxValue();
		return make_tuple(val, time, index);
	}
	//! \return minimalna wartość w calej dziedzinie dla wszystkich kanałów
	boost::tuple<PointType, TimeType, int> getMinValue() const
	{
		int index = _getIndex(
			[&](int i) { return channels[i]->getMinValue(); },
			[&](iterator b, iterator e) -> int { return std::min_element(channels.begin(), channels.end()); }
		);
		TimeType time = channels[index]->getMinValueTime();
		PointType val = channels[index]->getMinValue();
		return make_tuple(val, time, index);
	}

	//!
	//! \param t
	boost::tuple<PointType, int> getMaxValue(TimeType t) const
	{
		// todo
		UTILS_ASSERT(false);
	}

private:
	template <class _FwdIt>
	int _getIndex(std::function<PointType (int)> fillColletion, std::function<_FwdIt (_FwdIt, _FwdIt)> indexGetter) {
		UTILS_ASSERT(channels.size());
		int count = static_cast<int>(channels.size());
		std::vector<PointType> m(count);
		for (int i = 0; i < count; ++i) {
			m[i] = fillCollection(i);
		}
		return static_cast<int>(std::distance(m.begin(), indexGetter(m.begin(), m.end())));
	}
};
////////////////////////////////////////////////////////////////////////////////
} //namespace utils
////////////////////////////////////////////////////////////////////////////////

#endif
