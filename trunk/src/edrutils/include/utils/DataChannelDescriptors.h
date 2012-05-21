/********************************************************************
    created:  2012/04/26
    created:  26:4:2012   23:59
    filename: DataChannelDescriptors.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_UTILS__DATACHANNELDESCRIPTORS_H__
#define HEADER_GUARD_UTILS__DATACHANNELDESCRIPTORS_H__

#include <utils/DataChannel.h>

namespace utils {

	//! Interfejs do czytania opisu kana³u - osi x i y oraz nazwy kana³u.
	class IChannelDescriptorReader
	{
	public:
		virtual ~IChannelDescriptorReader() {}

		//! \return
		virtual const std::string& getTimeBaseUnit() const = 0;

		virtual float getTimeScaleFactor() const = 0;

		//! \return
		virtual const std::string& getValueBaseUnit() const = 0;

		virtual float getValueScaleFactor() const = 0;
	};

	//! Interfejs do modyfikacji opisu kana³u - osi x i y oraz nazwy kana³u.
	class IChannelDescriptorWriter
	{
	public:
		virtual ~IChannelDescriptorWriter() {}

		//! \param xUnit
		virtual void setTimeBaseUnit(const std::string& xUnit) = 0;

		virtual void setTimeScaleFactor(float factor) = 0;

		//! \param yUnit
		virtual void setValueBaseUnit(const std::string& yUnit) = 0;

		virtual void setValueScaleFactor(float factor) = 0;
	};


	class IChannelDescriptor : public virtual IChannelDescriptorReader, public virtual IChannelDescriptorWriter
	{
	public:
		virtual ~IChannelDescriptor() {}

	};

	//! Implementacja interfejsu IChannelDescriptor
	class ChannelDescriptor : public IChannelDescriptor
	{
	private:
		//! Skala czasu
		float timeScaleFactor;
		//! Jednostka czasu
		std::string timeBaseUnit;
		//! Skala wartoœci
		float valueScaleFactor;
		//! Jendostka wartoœci
		std::string valueBaseUnit;

	public:

		ChannelDescriptor() : timeScaleFactor(1.0f), valueScaleFactor(1.0f) {}

		ChannelDescriptor(const std::string & valueBaseUnit,
			float valueScaleFactor = 1.0f, float timeScaleFactor = 1.0f,
			const std::string & timeBaseUnit = std::string("s"))
			: valueBaseUnit(valueBaseUnit), valueScaleFactor(valueScaleFactor),
			timeScaleFactor(timeScaleFactor), timeBaseUnit(timeBaseUnit)
		{

		}

		ChannelDescriptor(const ChannelDescriptor & channel)
			: valueBaseUnit(channel.valueBaseUnit),
			valueScaleFactor(channel.valueScaleFactor),
			timeScaleFactor(channel.timeScaleFactor),
			timeBaseUnit(channel.timeBaseUnit)
		{

		}

		ChannelDescriptor(const IChannelDescriptorReader & channel)
			: valueBaseUnit(channel.getValueBaseUnit()),
			valueScaleFactor(channel.getValueScaleFactor()),
			timeScaleFactor(channel.getTimeScaleFactor()),
			timeBaseUnit(channel.getTimeBaseUnit())
		{

		}

	public:

		virtual ~ChannelDescriptor() {}

		//! \return
		virtual const std::string& getTimeBaseUnit() const
		{ 
			return timeBaseUnit;
		}

		//! \return
		virtual float getTimeScaleFactor() const
		{ 
			return timeScaleFactor;
		}

		//! \return
		virtual const std::string& getValueBaseUnit() const
		{ 
			return valueBaseUnit;
		}

		//! \return
		virtual float getValueScaleFactor() const
		{ 
			return valueScaleFactor;
		}

		//! \param timeBaseUnit
		virtual void setTimeBaseUnit(const std::string& timeBaseUnit) 
		{ 
			this->timeBaseUnit = timeBaseUnit;
		}

		//! \param timeBaseUnit
		virtual void setTimeScaleFactor(float timeScaleFactor) 
		{ 
			this->timeScaleFactor = timeScaleFactor;
		}

		//! \param yUnit
		virtual void setValueBaseUnit(const std::string& valueBaseUnit) 
		{ 
			this->valueBaseUnit = valueBaseUnit;
		}

		//! \param timeBaseUnit
		virtual void setValueScaleFactor(float valueScaleFactor) 
		{ 
			this->valueScaleFactor = valueScaleFactor;
		}
	};


	//! Klasa rozszerzajaca podstawowe kana³y o opis ich danych
	template<class Channel>
	class ChannelWithDescriptor : public Channel, public ChannelDescriptor
	{
		UTILS_STATIC_ASSERT((boost::is_base_of<IRawGeneralDataChannelReader<typename Channel::point_type, typename Channel::time_type>, Channel>::value), "Base class should inherit from IRawGeneralDataChannelReader");
		UTILS_STATIC_ASSERT(!(boost::is_base_of<IChannelDescriptorReader, Channel>::value), "Channel is already extended with ChannelDescriptor");

	public:

		ChannelWithDescriptor() {}
		ChannelWithDescriptor(const IChannelDescriptorReader & descriptor) : ChannelDescriptor(descriptor) {}
		ChannelWithDescriptor(const Channel & channel, const IChannelDescriptorReader & descriptor = ChannelDescriptor()) : Channel(channel), ChannelDescriptor(descriptor) {}
		ChannelWithDescriptor(const IRawGeneralDataChannelReader<typename Channel::point_type, typename Channel::time_type> & channel,
			const IChannelDescriptorReader & descriptor = ChannelDescriptor()) : Channel(channel), ChannelDescriptor(descriptor) {}
		ChannelWithDescriptor(const IRawUniformDataChannelReader<typename Channel::point_type, typename Channel::time_type> & channel,
			const IChannelDescriptorReader & descriptor = ChannelDescriptor()) : Channel(channel), ChannelDescriptor(descriptor) {}
		ChannelWithDescriptor(const ChannelWithDescriptor & channel) : Channel(channel), ChannelDescriptor(channel) {}
		ChannelWithDescriptor(float samplesPerSecond, const IChannelDescriptorReader & descriptor = ChannelDescriptor()) : Channel(samplesPerSecond), ChannelDescriptor(descriptor) {}

		virtual ~ChannelWithDescriptor() {}

		virtual ChannelWithDescriptor * clone() const
		{
			return new ChannelWithDescriptor(*this);
		}

		virtual ChannelWithDescriptor * create() const
		{
			boost::scoped_ptr<Channel> entity(this->Channel::create());
			return new ChannelWithDescriptor(*entity, *this);
		}

	};


	//! Obiekt realizuj¹cy interfejs do czytania zawartoœci kana³u ogólnego wraz z jego opisem
	template<class PointType, class TimeType, bool>
	class IChannelReader : public utils::IRawGeneralDataChannelReader<PointType, TimeType>, public virtual IChannelDescriptorReader
	{
	public:
		virtual ~IChannelReader() {}

		typedef utils::RawGeneralDataChannel<PointType, TimeType> RawChannel;
		typedef utils::ChannelWithDescriptor<RawChannel> DefaultImplementation;
	};
	//! Obiekt realizuj¹cy interfejs do czytania zawartoœci kana³u z równoroz³o¿onymi próbkami wraz z jego opisem
	template<class PointType, class TimeType>
	class IChannelReader<PointType, TimeType, true> : public utils::IRawUniformDataChannelReader<PointType, TimeType>, public virtual  IChannelDescriptorReader
	{
	public:
		virtual ~IChannelReader() {}

		typedef utils::RawUniformDataChannel<PointType, TimeType> RawChannel;
		typedef utils::ChannelWithDescriptor<RawChannel> DefaultImplementation;
	};


	//! Klasa realizuj¹ca kana³ z opisem
	template<class PointType, class TimeType = float, bool uniform = true>
	class Channel : public IChannelReader<PointType, TimeType, uniform>, public IRawDataChannelBasicWriter<PointType, TimeType>, public IChannelDescriptor
	{
	public:
		typedef IChannelReader<PointType, TimeType, uniform> Interface;

	private:

		typedef typename Interface::DefaultImplementation Impl;

	public:

		Channel() {}

		Channel(const Channel & channel) : impl(channel.impl) {}    
		Channel(const IChannelDescriptorReader & descriptor) : impl(descriptor) {}
		Channel(const Interface & channel) : impl(channel, channel) {}
		Channel(const Impl & channel) : impl(channel) {}
		Channel(float samplesPerSecond, const IChannelDescriptorReader & descriptor = ChannelDescriptor()) : impl(samplesPerSecond, descriptor) {}

		virtual ~Channel() {}

		virtual Channel * clone() const
		{
			return new Channel(*this);
		}

		virtual Channel * create() const
		{
			boost::scoped_ptr<Impl> entity(impl.create());
			return new Channel(*entity);
		}

		//! \return Nazwa kana³u
		virtual const std::string& getName() const
		{
			return impl.getName();
		}

		void setName(const std::string & name)
		{
			impl.setName(name);
		}

		//! \return Czas trwania kana³u
		virtual time_type getLength() const
		{
			return impl.getLength();
		}

		//! \param idx Indeks probki
		//! \return Wartosc czasu dla danego indeksu
		virtual time_type argument(size_type idx) const
		{
			return impl.argument(idx);
		}

		//! \param idx Indeks probki
		//! \return Wartosc probki dla danego indeksu
		virtual point_type_const_reference value(size_type idx) const
		{
			return impl.value(idx);
		}

		//! \return Iloœæ próbek w kanale
		virtual size_type size() const
		{
			return impl.size();
		}

		//! \return Czy kana³ nie zawiera danych
		virtual bool empty() const
		{
			return impl.empty();
		}
		//! \param time Czas dla ktorego chemy uzyskac dwie najblizsze probki
		//! \return Para indeksow, pierwszy wskazujke probke o czasie mniejszym lub rownym zadanemu czasowi, drugi wskazuje probke o czasie wiekszym lub rownym zadanemu
		//! Ta implementacja dzia³a w czasie O(log(N)) - podzia³ binarny bo dane sa u³o¿one chronologicznie
		virtual data_range getValueHelper(time_type time) const
		{
			return impl.getValueHelper(time);
		}

		virtual float getSamplesPerSecond() const
		{
			return impl.getSamplesPerSecond();
		}

		virtual float getSampleDuration() const
		{
			return impl.getSampleDuration();
		}

		virtual void addPoint(time_type time, point_type_const_reference point)
		{
			impl.addPoint(time, point);
			notify();
		}

		virtual void addPoint(point_type_const_reference point)
		{
			impl.addPoint(point);
			notify();
		}

		//! \return
		virtual const std::string& getTimeBaseUnit() const
		{ 
			return impl.getTimeBaseUnit();
		}

		//! \return
		virtual float getTimeScaleFactor() const
		{ 
			return impl.getTimeScaleFactor();
		}

		//! \return
		virtual const std::string& getValueBaseUnit() const
		{ 
			return impl.getValueBaseUnit();
		}

		//! \return
		virtual float getValueScaleFactor() const
		{ 
			return impl.getValueScaleFactor();
		}

		//! \param timeBaseUnit
		virtual void setTimeBaseUnit(const std::string& timeBaseUnit) 
		{ 
			impl.setTimeBaseUnit(timeBaseUnit);
		}

		//! \param timeBaseUnit
		virtual void setTimeScaleFactor(float timeScaleFactor) 
		{ 
			impl.setTimeScaleFactor(timeScaleFactor);
		}

		//! \param yUnit
		virtual void setValueBaseUnit(const std::string& valueBaseUnit) 
		{ 
			impl.setValueBaseUnit(valueBaseUnit);
		}

		//! \param timeBaseUnit
		virtual void setValueScaleFactor(float valueScaleFactor) 
		{ 
			impl.setValueScaleFactor(valueScaleFactor);
		}

	private:

		Impl impl;
	};

}

#endif	//	HEADER_GUARD_UTILS__DATACHANNELDESCRIPTORS_H__
