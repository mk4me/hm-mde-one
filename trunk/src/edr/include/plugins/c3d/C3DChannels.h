/********************************************************************
	created:  2011/02/17
	created:  17:2:2011   14:06
	filename: C3DChannels.h
	author:	  Piotr Gwiazdowski

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_C3D__C3DCHANNELS_H__
#define __HEADER_GUARD_C3D__C3DCHANNELS_H__

#include <plugins/c3d/Export.h>
#include <osg/Vec3>
#include <utils/DataChannel.h>
#include <utils/DataChannelModifiers.h>
#include <utils/DataChannelStats.h>
#include <utils/DataChannelCollection.h>
#include <utils/DataChannelAccessors.h>
#include <utils/DataChannelTimers.h>
#include <c3dlib/C3DParser.h>
#include <corelib/SmartPtr.h>
#include <utils/PtrPolicyBoost.h>
#include <utils/ClonePolicies.h>
#include <corelib/BaseDataTypes.h>

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
typedef core::shared_ptr<ScalarChannelReaderInterface> ScalarChannelReaderInterfacePtr;
//! wkaźnik na niemodyfikowalny interfejs umożliwiający odczyt kanału
typedef core::shared_ptr<const ScalarChannelReaderInterface> ScalarChannelReaderInterfaceConstPtr;

//! kanał przechowujący wektory trójwymiarowe
typedef utils::Channel<osg::Vec3f> VectorChannel;
//! interfejs umożliwiający odczyt kanału
typedef VectorChannel::Interface VectorChannelReaderInterface;
//! akcesor umożliwiajacy dostanie się do wektorów w sposób ciągły
typedef utils::DataChannelTimeAccessor<osg::Vec3f, float> VectorContiniousTimeAccessor;
//! interfejs, dzięki któremu można zmodyfikować kanał z wektorami
typedef utils::ChannelAutoModifier<osg::Vec3f, float> VectorModifier;
//! wskaźnik na kanał przechowujący wektory trójwymiarowe
typedef core::shared_ptr<VectorChannel> VectorChannelPtr;
//! niemodyfikowalny wskaźnik na kanał przechowujący wektory trójwymiarowe
typedef core::shared_ptr<const VectorChannel> VectorChannelConstPtr;
//! wkaźnik na interfejs umożliwiający odczyt kanału
typedef core::shared_ptr<VectorChannelReaderInterface> VectorChannelReaderInterfacePtr;
//! wkaźnik na niemodyfikowalny interfejs umożliwiający odczyt kanału
typedef core::shared_ptr<const VectorChannelReaderInterface> VectorChannelReaderInterfaceConstPtr;

//! Adaptor, dzięki któremu można obsłużyć kanał z wektorem trójwymiarowym tak jak skalarny
class PLUGINC3D_EXPORT VectorToScalarAdaptor : public ScalarChannelReaderInterface, public utils::IChannelDescriptorWriter
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
class PLUGINC3D_EXPORT ScalarWithTimeSegment : public ScalarChannelReaderInterface, public utils::IChannelDescriptorWriter
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
typedef core::shared_ptr<ScalarChannel> ScalarChannelPtr;
//! niemodyfikowalny skaźnik do kanału z danymi skalarnymi
typedef core::shared_ptr<const ScalarChannel> ScalarChannelConstPtr;
//! obiekt ze statystykami do kanału z danymi skalarnymi
typedef utils::ChannelStats<ScalarChannel::point_type, ScalarChannel::time_type> ScalarChannelStats;
//! wskaźnik obiektu ze statystykami do kanału z danymi skalarnymi
typedef core::shared_ptr<ScalarChannelStats> ScalarChannelStatsPtr;
//! niemodyfikowalny wskaźnik obiektu ze statystykami do kanału z danymi skalarnymi
typedef core::shared_ptr<const ScalarChannelStats> ScalarChannelStatsConstPtr;

//! Modyfikator, który normalizuje kanał
class PLUGINC3D_EXPORT ScalarChannelNormalizer
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
class PLUGINC3D_EXPORT C3DAnalogChannel : public ScalarChannel
{
protected:
    //! konstruktor
    //! \param c3dData przeparsowane dane z pliku c3d
    //! \param channelNo numer kanału w pliku c3d
    C3DAnalogChannel( const c3dlib::C3DParser& c3dData, int channelNo );
    //! konstruktor 
    //! \param samplesPerSec liczba próbek na sekundę
    C3DAnalogChannel( int samplesPerSec );
    //! konstruktor kopiujący
    //! \param channel kopiowany kanał
    C3DAnalogChannel( const C3DAnalogChannel& channel );
};
typedef core::shared_ptr<C3DAnalogChannel> C3DAnalogChannelPtr;
typedef core::shared_ptr<const C3DAnalogChannel> C3DAnalogChannelConstPtr;

//! kanał EMG
class PLUGINC3D_EXPORT EMGChannel : public C3DAnalogChannel
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
typedef core::shared_ptr<EMGChannel> EMGChannelPtr;
typedef core::shared_ptr<const EMGChannel> EMGChannelConstPtr;

//! kanał GRF
class PLUGINC3D_EXPORT GRFChannel : public VectorChannel
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
    GRFChannel( const c3dlib::C3DParser& data, int channelNo, float treshold = 200.0f);

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
typedef core::shared_ptr<GRFChannel> GRFChannelPtr;
typedef core::shared_ptr<const GRFChannel> GRFChannelConstPtr;

//! kanał zawiera dane o jednym markerze
class PLUGINC3D_EXPORT MarkerChannel : public VectorChannel
{
public:
    //! konstruktor 
    //! \param samplesPerSec liczba próbek na sekundę
    MarkerChannel(int samplesPerSec);

    MarkerChannel(const MarkerChannel& channel);

    //! konstruktor
    //! \param c3dData przeparsowane dane z pliku c3d
    //! \param channelNo numer kanału w pliku c3d
    MarkerChannel(const c3dlib::C3DParser& data, int channelNo );

public:
    //! \return zwraca kopię obiektu, którą trzeba samemu usunąć
    virtual MarkerChannel* clone() const;
};
typedef core::shared_ptr<MarkerChannel> MarkerChannelPtr;
typedef core::shared_ptr<const MarkerChannel> MarkerChannelConstPtr;

#define DECLARE_CHANNEL(name)																	 \
	class PLUGINC3D_EXPORT name##Channel : public VectorChannel 								 \
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
	typedef core::shared_ptr<name##Channel> name##ChannelPtr;									 \
	typedef core::shared_ptr<const name##Channel> name##ChannelConstPtr;

DECLARE_CHANNEL(Force);
DECLARE_CHANNEL(Moment);
DECLARE_CHANNEL(Angle);
DECLARE_CHANNEL(Power);

DEFINE_WRAPPER(VectorChannelReaderInterface, utils::PtrPolicyBoost, utils::ClonePolicyVirtualCloneMethod);
DEFINE_WRAPPER_INHERITANCE(VectorChannel, VectorChannelReaderInterface);
DEFINE_WRAPPER_INHERITANCE(MarkerChannel,VectorChannel);
DEFINE_WRAPPER_INHERITANCE(ForceChannel, VectorChannel);
DEFINE_WRAPPER_INHERITANCE(MomentChannel,VectorChannel);
DEFINE_WRAPPER_INHERITANCE(AngleChannel, VectorChannel);
DEFINE_WRAPPER_INHERITANCE(PowerChannel, VectorChannel);
DEFINE_WRAPPER(ScalarChannelReaderInterface, utils::PtrPolicyBoost, utils::ClonePolicyVirtualCloneMethod);
DEFINE_WRAPPER_INHERITANCE(ScalarChannel, ScalarChannelReaderInterface);
DEFINE_WRAPPER_INHERITANCE(VectorToScalarAdaptor, ScalarChannelReaderInterface);
DEFINE_WRAPPER_INHERITANCE(C3DAnalogChannel, ScalarChannel);
DEFINE_WRAPPER_INHERITANCE(EMGChannel, C3DAnalogChannel);
DEFINE_WRAPPER_INHERITANCE(GRFChannel, VectorChannel);

#endif  // __HEADER_GUARD_CORE__C3DCHANNELS_H__
