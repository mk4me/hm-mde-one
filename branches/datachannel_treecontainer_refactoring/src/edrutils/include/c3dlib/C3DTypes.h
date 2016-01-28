/********************************************************************
	created:	2015/03/20
	created:	8:49
	filename: 	C3DTypes.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_C3DLIB__C3DTypes_H__
#define HEADER_GUARD_C3DLIB__C3DTypes_H__

#include <osg/Vec3>
#include <datachannellib/ArgumentTracker.h>
#include <datachannellib/Adapters.h>
#include <datachannellib/AccessorsCollection.h>
#include <utils/SmartPtr.h>
#include <utils/PtrPolicyStd.h>
#include <utils/ClonePolicies.h>
#include <c3dlib/C3DParser.h>
#include <c3dlib/Export.h>

namespace  c3dlib {

	//! interfejs do odczytu czasu timera
	typedef dataaccessor::IArgumentTrackerReader<float> TimerReader;
	//! wskaźnik do interfejsu do odczytu czasu timera
	typedef utils::shared_ptr<TimerReader> TimerReaderPtr;
	//! niemodyfikowalny wskaźnik do interfejsu do odczytu czasu timera
	typedef utils::shared_ptr<const TimerReader> TimerReaderConstPtr;

	//! akcesor umożliwiajacy dostanie się do danych w sposób ciągły
	typedef dataaccessor::IFunctionAccessorT<float, float> ScalarContiniousTimeAccessor;	
	
	//! interfejs umożliwiający odczyt kanału
	typedef dataaccessor::IDiscreteAccessorT<float, float> ScalarChannelReaderInterface;
	//! wkaźnik na interfejs umożliwiający odczyt kanału
	typedef utils::shared_ptr<ScalarChannelReaderInterface> ScalarChannelReaderInterfacePtr;
	//! wkaźnik na niemodyfikowalny interfejs umożliwiający odczyt kanału
	typedef utils::shared_ptr<const ScalarChannelReaderInterface> ScalarChannelReaderInterfaceConstPtr;
	
	//! interfejs umożliwiający odczyt kanału
	typedef dataaccessor::IDiscreteAccessorT<osg::Vec3, float> VectorChannelReaderInterface;
	//! wkaźnik na interfejs umożliwiający odczyt kanału
	typedef utils::shared_ptr<VectorChannelReaderInterface> VectorChannelReaderInterfacePtr;
	//! wkaźnik na niemodyfikowalny interfejs umożliwiający odczyt kanału
	typedef utils::shared_ptr<const VectorChannelReaderInterface> VectorChannelReaderInterfaceConstPtr;

	template<typename SignalType, int Type = 0, typename ValueType = utils::remove_toplevel<decltype(std::declval<SignalType>()->getValue(0))>::type>
	class C3DChannelWrapper : public dataaccessor::IIndependentDiscreteAccessorT<ValueType, float>
	{
	public:

		enum { type = Type };

	public:

		C3DChannelWrapper(const SignalType& signal, const argument_type step, const std::size_t size)
			: signal(signal),
			argumentsGenerator(step, size) {

			if (signal == nullptr) {
				throw std::runtime_error("No points signal");
			}

			attachFeature(utils::make_shared<dataaccessor::UniformArgumentsFeature<argument_type>>(argumentsGenerator.step()));
			attachFeature(dataaccessor::IFeaturePtr(dataaccessor::DescriptorFeature::create<value_type, argument_type >(signal->getLabel(), signal->getUnit(), "s")));
			attachFeature(utils::make_shared<dataaccessor::BoundedArgumentsFeature<argument_type>>(argumentsGenerator.start(), argumentsGenerator.end()));
		}

		virtual ~C3DChannelWrapper() {}

		virtual argument_type argument(const size_type idx) const override {
			return argumentsGenerator.argument(idx);
		}

		virtual value_type value(const size_type idx) const override {
			return signal->getValue(idx);
		}

		virtual size_type size() const override {
			return argumentsGenerator.size();
		}

	private:
		SignalType signal;
		dataaccessor::UniformArgumentsGenerator<argument_type> argumentsGenerator;
	};

	using C3DAnalogChannel = C3DChannelWrapper<C3DParser::IAnalogConstPtr, 0, float>;

	typedef utils::shared_ptr<C3DAnalogChannel> C3DAnalogChannelPtr;
	typedef utils::shared_ptr<const C3DAnalogChannel> C3DAnalogChannelConstPtr;

	//! kanał EMG
	class C3DLIB_EXPORT EMGChannel : public C3DAnalogChannel
	{
	public:		
		//! konstruktor
		//! \param c3dData przeparsowane dane z pliku c3d
		//! \param channelNo numer kanału w pliku c3d
		EMGChannel(const c3dlib::C3DParser& data, int channelNo);
		//! Destruktor wirtualny
		virtual ~EMGChannel() {}
	};
	typedef utils::shared_ptr<EMGChannel> EMGChannelPtr;
	typedef utils::shared_ptr<const EMGChannel> EMGChannelConstPtr;

	//! kanał GRF
	class C3DLIB_EXPORT GRFChannel : public dataaccessor::IIndependentDiscreteAccessorT<osg::Vec3, float>
	{
	public:
		//! rodzaj kanału GRF
		enum Type { Unknown, F1, M1, F2, M2 };

	public:		
		//! konstruktor
		//! \param c3dData przeparsowane dane z pliku c3d
		//! \param channelNo numer kanału w pliku c3d
		//! \param treshold próg dla którego warto analizować dane z płyty GRF
		GRFChannel(const c3dlib::C3DParser& data, int channelNo, float treshold = 200.0f);
		//! Destruktor wirtualny
		virtual ~GRFChannel();
		//! \param channelNo Numer kanału
		//! \return Typ kanału/danych
		static Type fromChannelNo(const int channelNo);

		//! \param idx Indeks próbki
		//! \return Argument dla danego indeksu
		virtual argument_type argument(const size_type idx) const override;

		//! \param idx Indeks próbki
		//! \return Wartość dla danego indeksu
		virtual value_type value(const size_type idx) const override;

		//! \return Ilość próbek w kanale
		virtual size_type size() const override;
	
		//! \return rodzaj kanału GRF
		Type getType() const;
		//! \return czy kanał ma wykryty peak, związanych z naciskiem na płytę
		bool hasStartEndData() const;
		//! \return początek danych, związanych z naciskiem na płytę
		argument_type getDataStartTime() const;
		//! \return koniec danych, związanych z naciskiem na płyę
		argument_type getDataEndTime() const;

	private:
		//! Typ kanału
		const Type type;
		//! początek danych, związanych z naciskiem na płytę
		argument_type dataStart;
		//! koniec danych, związanych z naciskiem na płytę
		argument_type dataEnd;
		//! wewnętrzne kanały
		c3dlib::C3DParser::IAnalogConstPtr xsignal;
		c3dlib::C3DParser::IAnalogConstPtr ysignal;
		c3dlib::C3DParser::IAnalogConstPtr zsignal;
		//! Generator argumentów
		dataaccessor::UniformArgumentsGenerator<argument_type> argumentsGenerator;
	};
	typedef utils::shared_ptr<GRFChannel> GRFChannelPtr;
	typedef utils::shared_ptr<const GRFChannel> GRFChannelConstPtr;

#define DECLARE_CHANNEL(name, type)	\
	class name##Channel : public C3DChannelWrapper<C3DParser::IPointConstPtr, type> { \
	public:	\
		name##Channel(const C3DParser& data, int channelNo)	\
			: C3DChannelWrapper<C3DParser::IPointConstPtr, type>(data.getPoint(channelNo),	\
			1.0 / data.getPointFrequency(), data.getNumPointFrames()) {}	\
		virtual ~name##Channel() {} \
	}; \
	typedef utils::shared_ptr<name##Channel> name##ChannelPtr;	\
	typedef utils::shared_ptr<const name##Channel> name##ChannelConstPtr;

	DECLARE_CHANNEL(Marker, 1);
	DECLARE_CHANNEL(Force, 2);
	DECLARE_CHANNEL(Moment, 3);
	DECLARE_CHANNEL(Angle, 4);
	DECLARE_CHANNEL(Power, 5);
}

#endif // HEADER_GUARD 
