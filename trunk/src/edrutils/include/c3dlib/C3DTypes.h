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
#include <datachannellib/DataChannel.h>
#include <datachannellib/DataChannelModifiers.h>
#include <datachannellib/DataChannelStats.h>
#include <datachannellib/DataChannelCollection.h>
#include <datachannellib/DataChannelAccessors.h>
#include <datachannellib/DataChannelTimers.h>
#include <utils/SmartPtr.h>
#include <utils/PtrPolicyStd.h>
#include <utils/ClonePolicies.h>
#include <c3dlib/C3DParser.h>
#include <c3dlib/Export.h>

namespace  c3dlib {

	//! interfejs do odczytu czasu timera
	typedef utils::ITimerReader<float>::TimerReaderType TimerReader;
	//! wskaźnik do interfejsu do odczytu czasu timera
	typedef utils::ITimerReader<float>::TimerReaderPtr TimerReaderPtr;
	//! niemodyfikowalny wskaźnik do interfejsu do odczytu czasu timera
	typedef utils::ITimerReader<float>::TimerReaderConstPtr TimerReaderConstPtr;

	//! akcesor umożliwiajacy dostanie się do danych w sposób ciągły
	typedef utils::DataChannelTimeAccessor<float, float> ScalarContiniousTimeAccessor;
	//! interfejs, dzięki któremu można zmodyfikować kanał
	typedef utils::ChannelAutoModifier<float, float> ScalarModifier;

	//! kanał przechowujący liczby zmiennoprzecinkowe
	typedef utils::Channel<float> ScalarChannel;
	//! interfejs umożliwiający odczyt kanału
	typedef ScalarChannel::Interface ScalarChannelReaderInterface;
	//! wkaźnik na interfejs umożliwiający odczyt kanału
	typedef utils::shared_ptr<ScalarChannelReaderInterface> ScalarChannelReaderInterfacePtr;
	//! wkaźnik na niemodyfikowalny interfejs umożliwiający odczyt kanału
	typedef utils::shared_ptr<const ScalarChannelReaderInterface> ScalarChannelReaderInterfaceConstPtr;

	//! kanał przechowujący wektory trójwymiarowe
	typedef utils::Channel<osg::Vec3f> VectorChannel;
	//! interfejs umożliwiający odczyt kanału
	typedef VectorChannel::Interface VectorChannelReaderInterface;
	//! akcesor umożliwiajacy dostanie się do wektorów w sposób ciągły
	typedef utils::DataChannelTimeAccessor<osg::Vec3f, float> VectorContiniousTimeAccessor;
	//! interfejs, dzięki któremu można zmodyfikować kanał z wektorami
	typedef utils::ChannelAutoModifier<osg::Vec3f, float> VectorModifier;
	//! wskaźnik na kanał przechowujący wektory trójwymiarowe
	typedef utils::shared_ptr<VectorChannel> VectorChannelPtr;
	//! niemodyfikowalny wskaźnik na kanał przechowujący wektory trójwymiarowe
	typedef utils::shared_ptr<const VectorChannel> VectorChannelConstPtr;
	//! wkaźnik na interfejs umożliwiający odczyt kanału
	typedef utils::shared_ptr<VectorChannelReaderInterface> VectorChannelReaderInterfacePtr;
	//! wkaźnik na niemodyfikowalny interfejs umożliwiający odczyt kanału
	typedef utils::shared_ptr<const VectorChannelReaderInterface> VectorChannelReaderInterfaceConstPtr;

	//! Adaptor, dzięki któremu można obsłużyć kanał z wektorem trójwymiarowym tak jak skalarny
	class C3DLIB_EXPORT VectorToScalarAdaptor : public ScalarChannelReaderInterface, public utils::IChannelDescriptorWriter
	{
	protected:
		//! konstruktor kopiujący
		VectorToScalarAdaptor(const VectorToScalarAdaptor & adaptor);

	public:
		//! konstruktor
		//! \param vector adaptowany kanał z wektorem
		//! \param idx indeks kanału z wektorem, ta część będzie widziana jako skalar
		//! \param name nazwa kanału
		VectorToScalarAdaptor(const VectorChannelReaderInterfaceConstPtr & vector, size_type idx, const std::string & name = std::string());
		virtual ~VectorToScalarAdaptor();

	public:
		//! \return zwraca nazwę kanału
		virtual const std::string& getName() const;
		//! Ustawia nazwę kanału
		//! \param name nowa nazwa
		void setName(const std::string& name);
		//! \return zwraca kopię obiektu, którą trzeba samemu usunąć
		virtual VectorToScalarAdaptor * clone() const;
		//! \return Czas trwania kanału
		virtual time_type getLength() const;
		//! \param idx Indeks próbki
		//! \return Wartość czasu dla danego indeksu
		virtual time_type argument(size_type idx) const;
		//! \param idx Indeks próbki
		//! \return Wartość próbki dla danego indeksu
		virtual point_type_const_reference value(size_type idx) const;
		//! \return Ilość próbek w kanale
		virtual size_type size() const;
		//! \return Czy kanał nie zawiera danych
		virtual bool empty() const;
		//! \return liczba próbek na sekundę
		virtual float getSamplesPerSecond() const;
		//! \return czas trwania pojedynczej próbki
		virtual float getSampleDuration() const;
		//! \return nazwa jednostki czasu
		virtual const std::string& getTimeBaseUnit() const;
		//! \return skala czasu
		virtual float getTimeScaleFactor() const;
		//! \return nazwa jednostki wartości
		virtual const std::string& getValueBaseUnit() const;
		//! \return skala wartości
		virtual float getValueScaleFactor() const;
		//! ustawia nazwę jednostki czasu
		//! \param timeBaseUnit nowa nazwa
		virtual void setTimeBaseUnit(const std::string& timeBaseUnit);
		//! ustawia skalę czasu
		//! \param timeScaleFactor nowa nazwa
		virtual void setTimeScaleFactor(float timeScaleFactor);
		//! ustawia nazwę jednostki wartości
		//! \param valueBaseUnit nowa nazwa
		virtual void setValueBaseUnit(const std::string& valueBaseUnit);
		//! ustawia skalę dla wartości
		//! \param valueScaleFactor nowa skala
		virtual void setValueScaleFactor(float valueScaleFactor);

	private:
		//! adaptowany kanał z wektorem
		VectorChannelReaderInterfaceConstPtr vector;
		//! indeks wektora, który ma być widoczny jako skalar
		size_type index;
		//! nazwa kanału
		std::string name;
		//! standardowy deskryptor kanału
		utils::ChannelDescriptor descriptor;
	};

	//! modyfikator, który pobiera wycinek kanału podstawowego i normalizuje go (0 - 100%)
	class C3DLIB_EXPORT ScalarWithTimeSegment : public ScalarChannelReaderInterface, public utils::IChannelDescriptorWriter
	{
	protected:
		//! konstruktor kopiujący
		ScalarWithTimeSegment(const ScalarWithTimeSegment & adaptor);

	public:
		//! konstruktor
		//! \param channel kanał, który ma być opakowany w modyfikator
		//! \param start początek przedziału czasowego
		//! \param end koniec przedziału czasowego
		ScalarWithTimeSegment(const ScalarChannelReaderInterfaceConstPtr & channel, time_type start, time_type end);
		//! 
		virtual ~ScalarWithTimeSegment();
		//! \return nazwa kanału
		virtual const std::string& getName() const;
		//! \return zwraca klon obiektu
		virtual ScalarWithTimeSegment * clone() const;
		//! \return Czas trwania kanału
		virtual time_type getLength() const;
		//! Transformuje zmodyfikowany indeks do indeksu oryginalnego
		//! \param idx zmodyfikowany indeks
		//! \return indeks oryginalny
		size_type transformIndex(size_type idx) const;
		//! \param idx Indeks próbki
		//! \return Wartość czasu dla danego indeksu
		virtual time_type argument(size_type idx) const;
		//! \param idx Indeks próbki
		//! \return Wartość próbki dla danego indeksu
		virtual point_type_const_reference value(size_type idx) const;
		//! \return Ilość próbek w kanale
		virtual size_type size() const;
		//! \return Czy kanał nie zawiera danych
		virtual bool empty() const;
		//! \return liczba próbek na sekundę
		virtual float getSamplesPerSecond() const;
		//! \return czas trwania próbki
		virtual float getSampleDuration() const;
		//! \return nazwa jednostki czasu
		virtual const std::string& getTimeBaseUnit() const;
		//! \return skala czasu
		virtual float getTimeScaleFactor() const;
		//! \return nazwa jednostki wartości
		virtual const std::string& getValueBaseUnit() const;
		//! \return skala wartości
		virtual float getValueScaleFactor() const;
		//! ustawia nazwę jednostki czasu
		//! \param timeBaseUnit nowa nazwa
		virtual void setTimeBaseUnit(const std::string& timeBaseUnit);
		//! ustawia skalę czasu
		//! \param timeScaleFactor nowa nazwa
		virtual void setTimeScaleFactor(float timeScaleFactor);
		//! ustawia nazwę jednostki wartości
		//! \param valueBaseUnit nowa nazwa
		virtual void setValueBaseUnit(const std::string& valueBaseUnit);
		//! ustawia skalę dla wartości
		//! \param valueScaleFactor nowa skala
		virtual void setValueScaleFactor(float valueScaleFactor);

	private:
		//! opakowywany kanał
		ScalarChannelReaderInterfaceConstPtr reader;
		//! indeks początkowej próbki
		int startIndex;
		//! indeks końcowej próbki
		int endIndex;
		//! standardowy deskryptor kanału
		utils::ChannelDescriptor descriptor;
	};
	//! wskaźnik do kanału z danymi skalarnymi
	typedef utils::shared_ptr<ScalarChannel> ScalarChannelPtr;
	//! niemodyfikowalny skaźnik do kanału z danymi skalarnymi
	typedef utils::shared_ptr<const ScalarChannel> ScalarChannelConstPtr;
	//! obiekt ze statystykami do kanału z danymi skalarnymi
	typedef utils::ChannelStats<ScalarChannel::point_type, ScalarChannel::time_type> ScalarChannelStats;
	//! wskaźnik obiektu ze statystykami do kanału z danymi skalarnymi
	typedef utils::shared_ptr<ScalarChannelStats> ScalarChannelStatsPtr;
	//! niemodyfikowalny wskaźnik obiektu ze statystykami do kanału z danymi skalarnymi
	typedef utils::shared_ptr<const ScalarChannelStats> ScalarChannelStatsConstPtr;

	//! Modyfikator, który normalizuje kanał
	class C3DLIB_EXPORT ScalarChannelNormalizer
	{
	public:
		//! funktor odpowiada za modyfikację kopii kanału 
		//! \param modifierInterface writer dla zmodyfikowanego kanału
		//! \param observedChannel modyfikowany kanała
		//! \param myChannel kanał wynikowy
		void operator()(ScalarChannelReaderInterface::_MyExtendedWriter & modifierInterface,
						const ScalarChannelReaderInterface::_MyRawChannelReaderType & observedChannel,
						const ScalarChannelReaderInterface::_MyRawChannelReaderType & myChannel);
	};


	//! Podstawa dla kanału analogowego zapisanego w pliku c3d
	class C3DLIB_EXPORT C3DAnalogChannel : public ScalarChannel
	{
	protected:
		//! konstruktor
		//! \param c3dData przeparsowane dane z pliku c3d
		//! \param channelNo numer kanału w pliku c3d
		C3DAnalogChannel(const c3dlib::C3DParser& c3dData, int channelNo);
		//! konstruktor 
		//! \param samplesPerSec liczba próbek na sekundę
		C3DAnalogChannel(int samplesPerSec);
		//! konstruktor kopiujący
		//! \param channel kopiowany kanał
		C3DAnalogChannel(const C3DAnalogChannel& channel);
	};
	typedef utils::shared_ptr<C3DAnalogChannel> C3DAnalogChannelPtr;
	typedef utils::shared_ptr<const C3DAnalogChannel> C3DAnalogChannelConstPtr;

	//! kanał EMG
	class C3DLIB_EXPORT EMGChannel : public C3DAnalogChannel
	{
	public:
		//! konstruktor 
		//! \param samplesPerSec liczba próbek na sekundę
		explicit EMGChannel(int samplesPerSec);
		//! konstruktor kopiujący
		//! \param channel kopiowany kanał
		EMGChannel(const EMGChannel& channel);
		//! konstruktor
		//! \param c3dData przeparsowane dane z pliku c3d
		//! \param channelNo numer kanału w pliku c3d
		EMGChannel(const c3dlib::C3DParser& data, int channelNo);

	public:
		//! \return zwraca kopię obiektu, którą trzeba samemu usunąć
		virtual EMGChannel* clone() const;
	};
	typedef utils::shared_ptr<EMGChannel> EMGChannelPtr;
	typedef utils::shared_ptr<const EMGChannel> EMGChannelConstPtr;

	//! kanał GRF
	class C3DLIB_EXPORT GRFChannel : public VectorChannel
	{
	public:
		//! rodzaj kanału GRF
		enum Type { Unknown, F1, M1, F2, M2 };

	public:
		//! konstruktor 
		//! \param samplesPerSec liczba próbek na sekundę
		//! \param treshold próg dla którego warto analizować dane z płyty GRF
		explicit GRFChannel(int samplesPerSec, float treshold = 100.0f);
		//! konstruktor kopiujący
		//! \param channel kopiowany kanał
		GRFChannel(const GRFChannel& channel);
		//! konstruktor
		//! \param c3dData przeparsowane dane z pliku c3d
		//! \param channelNo numer kanału w pliku c3d
		//! \param treshold próg dla którego warto analizować dane z płyty GRF
		GRFChannel(const c3dlib::C3DParser& data, int channelNo, float treshold = 200.0f);

	public:
		//! \return zwraca kopię obiektu, którą trzeba samemu usunąć
		virtual GRFChannel* clone() const;
		//! \return rodzaj kanału GRF
		GRFChannel::Type getType() const;
		//! \return czy kanał ma wykryty peak, związanych z naciskiem na płytę
		bool hasStartEndData() const;
		//! \return początek danych, związanych z naciskiem na płytę
		float getDataStartTime() const;
		//! \return koniec danych, związanych z naciskiem na płyę
		float getDataEndTime() const;

	private:
		Type type;
		//! próg dla którego warto analizować dane z płyty GRF
		float treshold;
		//! początek danych, związanych z naciskiem na płytę
		float dataStart;
		//! koniec danych, związanych z naciskiem na płytę
		float dataEnd;
	};
	typedef utils::shared_ptr<GRFChannel> GRFChannelPtr;
	typedef utils::shared_ptr<const GRFChannel> GRFChannelConstPtr;

	//! kanał zawiera dane o jednym markerze
	class C3DLIB_EXPORT MarkerChannel : public VectorChannel
	{
	public:
		//! konstruktor 
		//! \param samplesPerSec liczba próbek na sekundę
		MarkerChannel(int samplesPerSec);

		MarkerChannel(const MarkerChannel& channel);

		//! konstruktor
		//! \param c3dData przeparsowane dane z pliku c3d
		//! \param channelNo numer kanału w pliku c3d
		MarkerChannel(const c3dlib::C3DParser& data, int channelNo);

	public:
		//! \return zwraca kopię obiektu, którą trzeba samemu usunąć
		virtual MarkerChannel* clone() const;
	};
	typedef utils::shared_ptr<MarkerChannel> MarkerChannelPtr;
	typedef utils::shared_ptr<const MarkerChannel> MarkerChannelConstPtr;

#define DECLARE_CHANNEL(name)																	 \
	class C3DLIB_EXPORT name##Channel : public VectorChannel 								 \
			{																							 \
	private:																					 \
		name##Channel(int samplesPerSec);														 \
																								 \
	public:        																				 \
		name##Channel(const name##Channel& channel);											 \
																								 \
		  name##Channel(const c3dlib::C3DParser& data, int channelNo );							 \
																								 \
	public:																						 \
		virtual name##Channel* clone() const;													 \
			};												 											 \
	typedef utils::shared_ptr<name##Channel> name##ChannelPtr;									 \
	typedef utils::shared_ptr<const name##Channel> name##ChannelConstPtr;

	DECLARE_CHANNEL(Force);
	DECLARE_CHANNEL(Moment);
	DECLARE_CHANNEL(Angle);
	DECLARE_CHANNEL(Power);


	typedef utils::IChannelReader<float, float, true> ScalarChannelReader;
}

#endif // HEADER_GUARD 
