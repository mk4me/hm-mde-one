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
#include <utils/DataChannel.h>
////////////////////////////////////////////////////////////////////////////////
namespace utils {
////////////////////////////////////////////////////////////////////////////////

//! Klasa agreguje klasy DataChannel, wszystkie dodawane kanaly powinny miec tyle samo wpisow
template <class Channel>
class DataChannelCollection
{
public:
	typedef Channel ChannelType;
    typedef typename Channel::time_type TimeType;
    typedef typename Channel::point_type PointType;
	typedef boost::shared_ptr<ChannelType> ChannelPtr;
	typedef boost::shared_ptr<const ChannelType> ChannelConstPtr;
	typedef std::vector<ChannelPtr> Collection;
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
		for (int i = 0; i < count; i++) {
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
		UTILS_ASSERT(index >= 0 && index < channels.size());
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


	//! \return dlugosc kanalu w sekundach; gdy nie ma danych zwracane jest zero, jest to odejscie od konwecji, ale dzialanie calkiem naturalne.
	TimeType getLength() const
	{
		if (channels.size() == 0) {
			return TimeType(0);
		} else {
			return channels[0]->getLength();
		}
	}

	//! \param time czas, dla ktorego pobieramy wartosci
	//! \return wartosci wszystkich kanalow dla konkretnego czasu
	std::vector<PointType> getValues(TimeType time) const
	{
		int count = static_cast<int>(channels.size());
		std::vector<PointType> res(channels.size());
		for (int i = 0; i < count; i++) {
			res[i] = channels[i]->getValue(time);
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
	
	//! \param index indeks kanalu, dla ktorego pobieramy wartosc
	//! \param time czas, dla ktorego bedzie zwrocona wartosc
	//! \return wartosc kanalu
	PointType getValue(int index, TimeType time) const
	{
		return getChannel(index)->getValue(time);
	}

	//! \return maksymalna wartosc w calej dziedzinie dla wszystkich kanalow
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
	//! \return minimalna wartosc w calej dziedzinie dla wszystkich kanalow
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
		for (int i = 0; i < count; i++) {
			m[i] = fillCollection(i);
		}
		return static_cast<int>(std::distance(m.begin(), indexGetter(m.begin(), m.end())));
	}
};
////////////////////////////////////////////////////////////////////////////////
} //namespace utils 
////////////////////////////////////////////////////////////////////////////////

#endif
