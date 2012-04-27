/********************************************************************
    created:  2012/04/26
    created:  26:4:2012   23:56
    filename: DataChannelTimers.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_UTILS__DATACHANNELTIMERS_H__
#define HEADER_GUARD_UTILS__DATACHANNELTIMERS_H__

#include <utils/DataChannel.h>
#include <utils/DataChannelAccessors.h>
#include <boost/shared_ptr.hpp>

namespace utils {

template<class TimeType>
class ITimerReader
{
public:
	typedef ITimerReader<TimeType> TimerReaderType;
	typedef boost::shared_ptr<TimerReaderType> TimerReaderPtr;
	typedef boost::shared_ptr<const TimerReaderType> TimerReaderConstPtr;

public:
	virtual ~ITimerReader() {}

	virtual TimeType getTime() const = 0;
};

template<class TimeType>
class ITimer : public ITimerReader<TimeType> 
{
public:
	typedef ITimer<TimeType> TimerType;
	typedef boost::shared_ptr<TimerType> TimerPtr;
	typedef boost::shared_ptr<const TimerType> TimerConstPtr;

public:

	virtual void setTime(TimeType time) = 0;
};

template<class TimeType>
class Timer : public ITimer<TimeType>
{
public:

	Timer() : time(0) {}

	template<class U>
	Timer(const ITimer<U> & timer) : time(timer.getTime()) {}

	template<class U>
	Timer(const Timer<U> & timer) : time(timer.time) {}

	template<class U>
	Timer(U time) : time(time) {}

	virtual ~Timer() {}

	virtual void setTime(TimeType time)
	{
		this->time = time;
	}

	virtual TimeType getTime() const
	{
		return time;
	}

private:
	TimeType time;
};

template<class PointType, class TimeType>
class CurrentValueExtractor
{
public:
	typedef typename ITimer<TimeType>::TimerType TimerType;
	typedef IDataChannelTimeAccessor<PointType, TimeType> Accessor;
	typedef IRawGeneralDataChannelReader<PointType, TimeType> DataChannel;

public:

	CurrentValueExtractor(const typename Accessor::IDataChannelTimeAccessorPtr & accessor, const typename TimerType::TimerPtr & timer = TimerType::TimerPtr(new Timer<TimeType>())) : timer(timer), constTimer(timer),
		accessor(accessor), constAccessor(accessor)
	{
		if(timer == nullptr){
			throw std::runtime_error("Wrong accessor for CurrentValueExtractor");
		}

		if(accessor == nullptr){
			throw std::runtime_error("Wrong accessor for CurrentValueExtractor");
		}

		if(accessor->getChannel() == nullptr){
			throw std::runtime_error("Accessor not initialized with channel for CurrentValueExtractor");
		}
	}

	virtual ~CurrentValueExtractor() {}

	virtual PointType getCurrentValue() const
	{
		return accessor->getValue(timer->getTime());
	}

	virtual const typename Accessor::IDataChannelTimeAccessorPtr & getDataAccesor()
	{
		return accessor;
	}

	virtual const typename Accessor::IDataChannelTimeAccessorConstPtr & getDataAccesor() const
	{
		return constAccessor;
	}

	virtual void setDataAccesor(const typename Accessor::IDataChannelTimeAccessorPtr & accessor)
	{
		if(accessor == nullptr){
			throw std::runtime_error("Wrong accessor for CurrentValueExtractor");
		}

		this->constAccessor = this->accessor = accessor;
	}

	virtual const typename TimerType::TimerPtr & getTimer()
	{
		return timer;
	}

	virtual const typename TimerType::TimerConstPtr & getTimer() const
	{
		return constTimer;
	}

	virtual void setTimer(const typename TimerType::TimerPtr & timer)
	{
		if(timer == nullptr){
			throw std::runtime_error("Wrong accessor for CurrentValueExtractor");
		}

		this->constTimer = this->timer = timer;
	}

private:
	typename TimerType::TimerPtr timer;
	typename TimerType::TimerConstPtr constTimer;

	typename Accessor::IDataChannelTimeAccessorPtr accessor;
	typename Accessor::IDataChannelTimeAccessorConstPtr constAccessor;
};

}

#endif	//	HEADER_GUARD_UTILS__DATACHANNELTIMERS_H__
