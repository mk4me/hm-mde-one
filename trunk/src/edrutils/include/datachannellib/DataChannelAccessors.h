/********************************************************************
	created:  2012/04/26
	created:  26:4:2012   23:47
	filename: DataChannelAccessors.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD___DATACHANNELACCESSORS_H__
#define HEADER_GUARD___DATACHANNELACCESSORS_H__

#include <datachannellib/DataChannel.h>
#include <utils/SmartPtr.h>

namespace utils {
	//! Klasa zapewniajaca dost�p do danych w sposob ciagly (sposob realizacji "wrazenia" ciaglasci zalezny od implementacji)
	template<class PointType, class TimeType>
	class IDataChannelTimeAccessor
	{
	public:
		typedef utils::shared_ptr<IDataChannelTimeAccessor<PointType, TimeType>> IDataChannelTimeAccessorPtr;
		typedef utils::shared_ptr<const IDataChannelTimeAccessor<PointType, TimeType>> IDataChannelTimeAccessorConstPtr;

	public:
		//! Wirtualny destruktor
		virtual ~IDataChannelTimeAccessor() {}
		//! \param time Czas dla którego chemy pobrać wartość
		//! \return Wartość kanału dla zadanego czasu
		virtual PointType getValue(TimeType time) const = 0;
		//! \param time Czas dla którego chemy pobrać wartość
		//! \return Wartość kanału dla zadanego czasu
		PointType operator[](TimeType time) const
		{
			return getValue(time);
		}

		//! \return Kanał dla którego tworzymy wrażenie ciągłości
		virtual const typename IRawGeneralDataChannelReader<PointType, TimeType>::_MyChannelConstPtr & getChannel() const = 0;
		//! \param channel Kanał dla którego chcemy utworzyć wrażenie ciągłości
		virtual void setChannel(const typename IRawGeneralDataChannelReader<PointType, TimeType>::_MyChannelConstPtr & channel) = 0;
	};

	//! Obiekt obsugujący zapytania o wartości kanału dla czasu spoza zakresu pokrytego przez kanał
	//! Rzuca wyjątkami
	template <	class PointType,
	class TimeType,
		template<typename Point, typename Time> class Interpolator,
	class PointRefType = typename IRawGeneralDataChannelReader<PointType, TimeType>::point_type_reference>
	class ExceptionExtrapolator
	{
	public:
		typedef IRawGeneralDataChannelReader<PointType, TimeType> ChannelReader;
		//! Domyśłny konstruktor
		ExceptionExtrapolator() {}

		//! Konstruktor kopiujący
		ExceptionExtrapolator(const ExceptionExtrapolator & resolver) {}
		//! Destruktor polimorficzny
		~ExceptionExtrapolator() {}

	public:
		//! \param time Czas poniżej zera
		//! \param point Zwracana wartość
		static PointType timeUnderflow(TimeType time, const IRawGeneralDataChannelReader<PointType, TimeType> & channel)
		{
			throw std::runtime_error("Time value less than 0");
			return PointType();
		}

		//! \param time Czas poza długością kanału
		//! \param point Zwracana wartość
		static PointType timeOverflow(TimeType time, const ChannelReader & channel)
		{
			throw std::runtime_error("Time value greater than channel length");
			return PointType();
		}
	};

	//! Obiekt obsugujący zapytania o wartości kanału dla czasu spoza zakresu pokrytego przez kanał
	//! Wprowadza wrażenie okresowości kanału
	template <	class PointType,
	class TimeType,
		template<typename Point, typename Time> class Interpolator,
	class PointRefType = typename IRawGeneralDataChannelReader<PointType, TimeType>::point_type_reference >
	class PeriodicExtrapolator
	{
	public:
		//! Domyśłny konstruktor
		PeriodicExtrapolator() {}

		//! Konstruktor kopiujący
		PeriodicExtrapolator(const PeriodicExtrapolator & resolver) {}
		//! Destruktor polimorficzny
		~PeriodicExtrapolator() {}

	public:
		//! \param time Czas poniżej zera
		//! \param point Zwracana wartość
		static PointType timeUnderflow(TimeType time, const IRawGeneralDataChannelReader<PointType, TimeType> & channel)
		{
			auto l = channel.getLength();
			while ((time += l) < 0) {}
			return Interpolator<PointType, TimeType>::extractValue(time, channel);
		}

		//! \param time Czas poza długością kanału
		//! \param point Zwracana wartość
		static PointType timeOverflow(TimeType time, const IRawGeneralDataChannelReader<PointType, TimeType> & channel)
		{
			auto l = channel.getLength();
			while ((time -= l) > l) {}
			return Interpolator<PointType, TimeType>::extractValue(time, channel);
		}
	};

	//! Obiekt obsugujący zapytania o wartości kanału dla czasu spoza zakresu pokrytego przez kanał
	//! Powiela skrajne próbki
	template <	class PointType,
	class TimeType,
		template<typename Point, typename Time> class Interpolator,
	class PointRefType = typename IRawGeneralDataChannelReader<PointType, TimeType>::point_type_reference >
	class BorderExtrapolator
	{
	public:
		//! Domyśłny konstruktor
		BorderExtrapolator() {}

		//! Konstruktor kopiujący
		BorderExtrapolator(const BorderExtrapolator & resolver) {}
		//! Destruktor polimorficzny
		~BorderExtrapolator() {}

	public:
		//! \param time Czas poniżej zera
		//! \param point Zwracana wartość
		static PointType timeUnderflow(TimeType time, const IRawGeneralDataChannelReader<PointType, TimeType> & channel)
		{
			return channel.value(0);
		}

		//! \param time Czas poza długością kanału
		//! \param point Zwracana wartość
		static PointType timeOverflow(TimeType time, const IRawGeneralDataChannelReader<PointType, TimeType> & channel)
		{
			return channel.value(channel.size() - 1);
		}
	};

	//! Obiekt pomocniczy przy interpolacjach
	template <	class PointType,
	class TimeType,
		template<typename Point, typename Time> class Interpolator>
	struct InterpolatorHelper
	{
	public:

		//! \param time Czas dla któego chcemy interpolować wartośc kanału
		//! \param channel Kanał dla którego interpolujemy

		static PointType extractValue(TimeType time, const IRawGeneralDataChannelReader<PointType, TimeType> & channel)
		{
			auto r = channel.getValueHelper(time);
			if (channel.argument(r.first) == time){
				return channel.value(r.first);
			}
			else if (channel.argument(r.second) == time){
				return channel.value(r.second);
			}
			else{
				return Interpolator<PointType, TimeType>::extractValue(r.first, r.second, time, channel);
			}
		}
	};

	//! Manipulator danych bazujący na operatorach typu. Interpoluje liniowo.
	template <class PointType, class TimeType>
	struct LerpInterpolator
	{
		typedef typename IRawGeneralDataChannelReader<PointType, TimeType>::size_type IndexType;

		//! \param point Zwracana interpolowana wartość
		//! \param p1 próbka dla czasu poniżej lub identycznego
		//! \param próbka dla czasu powyzej lub identycznego
		//! \param time Wartość określająca procent przesuniecia względem próbki poniżej
		static PointType extractValue(IndexType idxA, IndexType idxB, TimeType time, const IRawGeneralDataChannelReader<PointType, TimeType> & channel)
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

		//! \return Domyślny epsilon dla czasu
		T getEpsilon() const
		{
			return epsilon;
		}

		//! \param
		void setEpsilon(T epsilon)
		{
			this->epsilon = epsilon;
		}

		//! \return Czy brana będzie mniejsza próbka jeśli równe
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
		//! Wartość otoczenia
		T epsilon;
		//! Którą próbkę brać jeśli równoodległe
		bool smallerIfEqual;
	};

	//! Interpolator dyskretny z obsługą otoczenia
	template <class PointType, class TimeType>
	struct DiscreteInterpolator : public EpsilonManager<TimeType>
	{
		typedef typename IRawGeneralDataChannelReader<PointType, TimeType>::size_type IndexType;

		//! \param point Zwracana interpolowana wartość
		//! \param p1 próbka dla czasu poniżej lub identycznego
		//! \param próbka dla czasu powyzej lub identycznego
		//! \param time Wartość określająca procent przesuniecia względem próbki poniżej
		static PointType extractValue(IndexType idxA, IndexType idxB, TimeType time, const IRawGeneralDataChannelReader<PointType, TimeType> & channel)
		{
			TimeType diffA = time - channel.argument(idxA);
			TimeType diffB = channel.argument(idxB) - time;

			if (diffA == diffB){
				if (getSmallerIfEqualPolicy() == true){
					return channel.value(idxA);
				}
				else{
					return channel.value(idxB);
				}
			}
			else if (diffA < diffB){
				return channel.value(idxA);
			}
			else{
				return channel.value(idxB);
			}
		}
	};

	//! Obiekt realizujący funkcjonalność ciągłości czasu w kanale - wewnątrz i na zewnątrz kanału
	template<	class PointType,
	class TimeType,
		template<typename Point, typename Time> class Interpolator = LerpInterpolator,
		template<typename Point, typename Time, template<typename Point, typename Time> class Interpolator, typename PointRefType> class Extrapolator = ExceptionExtrapolator>
	class DataChannelTimeAccessor : public IDataChannelTimeAccessor<PointType, TimeType>,
		private Interpolator<PointType, TimeType>,
		private Extrapolator<PointType, TimeType, Interpolator, typename IRawGeneralDataChannelReader<PointType, TimeType>::point_type_reference>
	{
	public:

		typedef IRawGeneralDataChannelReader<PointType, TimeType> Channel;
		typedef typename IRawGeneralDataChannelReader<PointType, TimeType>::_MyChannelConstPtr ConstChannelPtr;

	private:
		//! Kanał dla którego realizujemy ciągłość
		ConstChannelPtr constChannel;

	public:
		//! Konstruktor domyślny
		DataChannelTimeAccessor() {}
		//! Konstruktor
		//! \prama Kanał dla którego tworzymy wrażenie ciągłości
		DataChannelTimeAccessor(const ConstChannelPtr & channel) : constChannel(channel) {}
		//! Destruktor wirtualny
		virtual ~DataChannelTimeAccessor() {}
		//! \return Kanał dla którego tworzymy wrażenie ciągłości
		virtual const ConstChannelPtr & getChannel() const
		{
			return constChannel;
		}

		//virtual const typename IRawGeneralDataChannelReader<PointType, TimeType>::_MyChannelPtr & getChannel()
		//{
		//	return channel;
		//}

		//! \param channel Kanał dla którego tworzymy wrażenie ciągłości
		virtual void setChannel(const ConstChannelPtr & channel)
		{
			constChannel = channel;
		}

		//! \param time Czas dla którego chcemy pobrać wartość
		//! \param channel Kanał z którego chcemy pobrać wartość
		//! \return Wartość kanału dla zadanego czasu uwzględniająca extrapolatro i interpolator (może w szczególności rzucać wyjątkami!!!)
		static PointType getValue(TimeType time, const Channel & channel)
		{
			if (channel.empty() == true){
				throw std::runtime_error("Accessing data in empty channel");
			}

			if (time < 0){
				return Extrapolator<PointType, TimeType, Interpolator, typename Channel::point_type_reference>::timeUnderflow(time, channel);
			}
			else if (time > channel.argument(channel.size() - 1)){
				return Extrapolator<PointType, TimeType, Interpolator, typename Channel::point_type_reference>::timeOverflow(time, channel);
			}
			else{
				return InterpolatorHelper<PointType, TimeType, Interpolator>::extractValue(time, channel);
			}
		}
		//! \param time Czas dla którego chcemy pobrać wartość
		//! \return Wartość kanału dla zadanego czasu uwzględniająca extrapolatro i interpolator (może w szczególności rzucać wyjątkami!!!)
		virtual PointType getValue(TimeType time) const
		{
			if (constChannel == nullptr){
				throw std::runtime_error("Accessing uninitialized channel");
			}

			return getValue(time, *constChannel);
		}
	};
}

#endif	//	HEADER_GUARD___DATACHANNELACCESSORS_H__
