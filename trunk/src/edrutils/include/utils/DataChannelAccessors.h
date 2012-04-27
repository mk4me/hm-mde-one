/********************************************************************
    created:  2012/04/26
    created:  26:4:2012   23:47
    filename: DataChannelAccessors.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___DATACHANNELACCESSORS_H__
#define HEADER_GUARD___DATACHANNELACCESSORS_H__

#include <utils/DataChannel.h>
#include <boost/shared_ptr.hpp>

namespace utils {

//! Klasa zapewniajaca dostep do danych w sposob ciagly (sposob realizacji "wrazenia" ciaglasci zalezny od implementacji)
template<class PointType, class TimeType>
class IDataChannelTimeAccessor
{
public:
	typedef boost::shared_ptr<IDataChannelTimeAccessor<PointType, TimeType>> IDataChannelTimeAccessorPtr;
	typedef boost::shared_ptr<const IDataChannelTimeAccessor<PointType, TimeType>> IDataChannelTimeAccessorConstPtr;

public:
	virtual ~IDataChannelTimeAccessor() {}

	virtual PointType getValue(TimeType time) const = 0;

	PointType operator[](TimeType time) const
	{
		return getValue(time);
	}

	//virtual const typename IRawGeneralDataChannelReader<PointType, TimeType>::_MyChannelPtr & getChannel() = 0;
	virtual const typename IRawGeneralDataChannelReader<PointType, TimeType>::_MyChannelConstPtr & getChannel() const = 0;
	virtual void setChannel(const typename IRawGeneralDataChannelReader<PointType, TimeType>::_MyChannelConstPtr & channel) = 0;
};

template <	class PointType,
			class TimeType,
			template<typename Point, typename Time> class Interpolator,
			class PointRefType = typename IRawGeneralDataChannelReader<PointType, TimeType>::point_type_reference>
class ExceptionExtrapolator
{
public:
	//! Domy��ny konstruktor
	ExceptionExtrapolator() {}

	//! Konstruktor kopiujacy
	ExceptionExtrapolator(const ExceptionExtrapolator & resolver) {}
	//! Destruktor polimorficzny
	~ExceptionExtrapolator() {}

public:
	//! \param time Czas ponizej zera
	//! \param point Zwracana wartosc
	static PointType timeUnderflow(TimeType time, typename const IRawGeneralDataChannelReader<PointType, TimeType> & channel)
	{
		throw std::runtime_error("Time value less than 0");
		return PointType();
	}

	//! \param time Czas poza dlugoscia kanalu
	//! \param point Zwracana wartosc
	static PointType timeOverflow(TimeType time, typename const IRawGeneralDataChannelReader<PointType, TimeType> & channel)
	{
		throw std::runtime_error("Time value greater than channel length");
		return PointType();
	}
};

template <	class PointType,
			class TimeType,
			template<typename Point, typename Time> class Interpolator,
			class PointRefType = typename IRawGeneralDataChannelReader<PointType, TimeType>::point_type_reference >
class PeriodicExtrapolator
{
public:
	//! Domy��ny konstruktor
	PeriodicExtrapolator() {}

	//! Konstruktor kopiujacy
	PeriodicExtrapolator(const PeriodicExtrapolator & resolver) {}
	//! Destruktor polimorficzny
	~PeriodicExtrapolator() {}

public:
	//! \param time Czas ponizej zera
	//! \param point Zwracana wartosc
	static PointType timeUnderflow(TimeType time, typename const IRawGeneralDataChannelReader<PointType, TimeType> & channel)
	{
		auto l = channel.getLength();
		while(time += l < 0) {}
		return Interpolator<PointType, TimeType>::extractValue(time, channel);
	}

	//! \param time Czas poza dlugoscia kanalu
	//! \param point Zwracana wartosc
	static PointType timeOverflow(TimeType time, typename const IRawGeneralDataChannelReader<PointType, TimeType> & channel)
	{
		auto l = channel.getLength();
		while(time -= l > l) {}
		return Interpolator<PointType, TimeType>::extractValue(time, channel);
	}
};

template <	class PointType,
			class TimeType,
			template<typename Point, typename Time> class Interpolator,
			class PointRefType = typename IRawGeneralDataChannelReader<PointType, TimeType>::point_type_reference >
class BorderExtrapolator
{
public:
	//! Domy��ny konstruktor
	BorderExtrapolator() {}

	//! Konstruktor kopiujacy
	BorderExtrapolator(const BorderExtrapolator & resolver) {}
	//! Destruktor polimorficzny
	~BorderExtrapolator() {}

public:
	//! \param time Czas ponizej zera
	//! \param point Zwracana wartosc
	static PointType timeUnderflow(TimeType time, typename const IRawGeneralDataChannelReader<PointType, TimeType> & channel)
	{
		return channel.value(0);
	}

	//! \param time Czas poza dlugoscia kanalu
	//! \param point Zwracana wartosc
	static PointType timeOverflow(TimeType time, typename const IRawGeneralDataChannelReader<PointType, TimeType> & channel)
	{
		return channel.value(channel.size() - 1);
	}
};

template <	class PointType,
			class TimeType,
			template<typename Point, typename Time> class Interpolator>
struct InterpolatorHelper 
{
public:

	static PointType extractValue(TimeType time, const typename IRawGeneralDataChannelReader<PointType, TimeType> & channel)
	{
		auto r = channel.getValueHelper(time);
		if(channel.argument(r.first) == time){
			return channel.value(r.first);
		}else if(channel.argument(r.second) == time){
			return channel.value(r.second);
		}else{
			return Interpolator<PointType, TimeType>::extractValue(r.first, r.second, time, channel);
		}
	}
};

//! Manipulator danych bazuj�cy na operatorach typu. Interpoluje liniowo.
template <class PointType, class TimeType>
struct LerpInterpolator
{
	typedef typename IRawGeneralDataChannelReader<PointType, TimeType>::size_type IndexType;

	//! \param point Zwracana interpolowana wartosc
	//! \param p1 Probka dla czasu ponizej lub identycznego
	//! \param Probka dla czasu powyzej lub identycznego
	//! \param time Wartosc okreslajaca procent przesuniecia wzgledem probki ponizej
	static PointType extractValue(IndexType idxA, IndexType idxB, TimeType time, const typename IRawGeneralDataChannelReader<PointType, TimeType> & channel)
	{
		//// lerp
		const auto & valA = channel.value(idxA);
		auto timeA = channel.argument(idxA);
		return valA + (channel.value(idxB) - valA) * (time - timeA) / (channel.argument(idxB) - timeA);
	}
};

template<class T>
class EpsilonManager
{
public:
	EpsilonManager() : epsilon(0), smallerIfEqual(false) {}
	EpsilonManager(const EpsilonManager & manager) : epsilon(manager.epsilon), smallerIfEqual(manager.smallerIfEqual) {}
	EpsilonManager(T epsilon, bool smallerIfEqual) : epsilon(epsilon), smallerIfEqual(smallerIfEqual) {}

	//! \return Domy�lny epsilon dla czasu
	T getEpsilon() const
	{
		return epsilon;
	}

	//! \param
	void setEpsilon(T epsilon)
	{
		this->epsilon = epsilon;
	}

	//! \return Czy brana b�dzie mniejsza pr�bka je�li r�wne
	bool getSmallerIfEqualPolicy() const
	{
		return smallerIfEqual;
	}


	//! \param
	void setSmallerIfEqualPolicy(bool smallerIfEqual)
	{
		this->smallerIfEqual = smallerIfEqual;
	}

private:
	T epsilon;
	bool smallerIfEqual;
};

template <class PointType, class TimeType>
struct DiscreteInterpolator : public EpsilonManager<TimeType>
{
	typedef typename IRawGeneralDataChannelReader<PointType, TimeType>::size_type IndexType;

	//! \param point Zwracana interpolowana wartosc
	//! \param p1 Probka dla czasu ponizej lub identycznego
	//! \param Probka dla czasu powyzej lub identycznego
	//! \param time Wartosc okreslajaca procent przesuniecia wzgledem probki ponizej
	static PointType extractValue(IndexType idxA, IndexType idxB, TimeType time, const typename IRawGeneralDataChannelReader<PointType, TimeType> & channel)
	{
		TimeType diffA = time - channel.argument(idxA);
		TimeType diffB = channel.argument(idxB) - time;

		if(diffA == diffB){
			if(getSmallerIfEqualPolicy() == true){
				return channel.value(idxA);
			}else{
				return channel.value(idxB);
			}
		}else if(diffA < diffB){
			return channel.value(idxA);
		}else{
			return channel.value(idxB);
		}
	}
};

template<	class PointType,
			class TimeType,
			template<typename Point, typename Time> class Interpolator = LerpInterpolator,
			template<typename Point, typename Time,	template<typename Point, typename Time> class Interpolator,	typename PointRefType> class Extrapolator = ExceptionExtrapolator>
class DataChannelTimeAccessor : public IDataChannelTimeAccessor<PointType, TimeType>,
	private Interpolator<PointType, TimeType>,
	private Extrapolator<PointType, TimeType, Interpolator, typename IRawGeneralDataChannelReader<PointType, TimeType>::point_type_reference>
{
public:

	typedef typename IRawGeneralDataChannelReader<PointType, TimeType> Channel;
	typedef typename IRawGeneralDataChannelReader<PointType, TimeType>::_MyChannelConstPtr ConstChannelPtr;

private:

	ConstChannelPtr constChannel;

public:

	DataChannelTimeAccessor() {}
	DataChannelTimeAccessor(const ConstChannelPtr & channel) : constChannel(channel) {}

	virtual ~DataChannelTimeAccessor() {}

	virtual const ConstChannelPtr & getChannel() const
	{
		return constChannel;
	}

	//virtual const typename IRawGeneralDataChannelReader<PointType, TimeType>::_MyChannelPtr & getChannel()
	//{
	//	return channel;
	//}

	virtual void setChannel(const ConstChannelPtr & channel)
	{
		constChannel = channel;
	}

public:

	static PointType getValue(TimeType time, const Channel & channel)
	{
		if(channel.empty() == true){
			throw std::runtime_error("Accessing data in empty channel");
		}

		if(time < 0){
			return Extrapolator<PointType, TimeType, Interpolator, typename Channel::point_type_reference>::timeUnderflow(time, channel);
		}else if(time > channel.argument(channel.size() - 1)){
			return Extrapolator<PointType, TimeType, Interpolator, typename Channel::point_type_reference>::timeOverflow(time, channel);
		}else{
			return InterpolatorHelper<PointType, TimeType, Interpolator>::extractValue(time, channel);
		}   
	}

	virtual PointType getValue(TimeType time) const
	{
		if(constChannel == nullptr){
			throw std::runtime_error("Accessing uninitialized channel");
		}

		return getValue(time, *constChannel);
	}
};

}

#endif	//	HEADER_GUARD___DATACHANNELACCESSORS_H__
