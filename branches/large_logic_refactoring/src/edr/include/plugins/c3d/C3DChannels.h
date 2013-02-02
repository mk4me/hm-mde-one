/********************************************************************
	created:  2011/02/17
	created:  17:2:2011   14:06
	filename: C3DChannels.h
	author:	  Piotr Gwiazdowski

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_C3D__C3DCHANNELS_H__
#define __HEADER_GUARD_C3D__C3DCHANNELS_H__

#include <corelib/SmartPtr.h>
#include <stdexcept>
#include <utils/Debug.h>
#include <utils/DataChannel.h>
#include <utils/DataChannelModifiers.h>
#include <utils/DataChannelStats.h>
#include <utils/DataChannelCollection.h>
#include <utils/DataChannelAccessors.h>
#include <utils/DataChannelTimers.h>
#include <utils/PtrPolicyBoost.h>
#include <corelib/BaseDataTypes.h>
#include <c3dlib/C3DParser.h>

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
class VectorToScalarAdaptor : public ScalarChannelReaderInterface, public utils::IChannelDescriptorWriter
{
protected:
    //! konstruktor kopiujący
    VectorToScalarAdaptor(const VectorToScalarAdaptor & adaptor) : descriptor(adaptor.descriptor), vector(adaptor.vector), index(adaptor.index), name(adaptor.name) {}

public:
    //! konstruktor
    //! \param vector adaptowany kanał z wektorem
    //! \param idx indeks kanału z wektorem, ta część będzie widziana jako skalar
    //! \param name nazwa kanału
    VectorToScalarAdaptor(const VectorChannelReaderInterfaceConstPtr & vector, size_type idx, const std::string & name = std::string())
        : descriptor(*vector), vector(vector), index(idx), name(name)
    {
        UTILS_ASSERT((idx >= 0 && idx < 3), "Bledny index dla adaptera skalarnego kanalu wektorowego");
    }
    virtual ~VectorToScalarAdaptor() {}

public:
    //! \return zwraca nazwę kanału
    virtual const std::string& getName() const
    {
        return name;
    }
	//! Ustawia nazwę kanału
	//! \param name nowa nazwa
	void setName(const std::string& name)
	{
		this->name = name;
	}
    //! \return zwraca kopię obiektu, którą trzeba samemu usunąć
    virtual VectorToScalarAdaptor * clone() const
    {
        return new VectorToScalarAdaptor(*this);
    }
    //! \return Czas trwania kanału
    virtual time_type getLength() const
    {
        return vector->getLength();
    }
    //! \param idx Indeks próbki
    //! \return Wartość czasu dla danego indeksu
    virtual time_type argument(size_type idx) const
    {
        return vector->argument(idx);
    }
    //! \param idx Indeks próbki
    //! \return Wartość próbki dla danego indeksu
    virtual point_type_const_reference value(size_type idx) const
    {
        return vector->value(idx)[index];
    }
    //! \return Ilość próbek w kanale
    virtual size_type size() const
    {
        return vector->size();
    }
    //! \return Czy kanał nie zawiera danych
    virtual bool empty() const
    {
        return vector->empty();
    }
    //! \return liczba próbek na sekundę
    virtual float getSamplesPerSecond() const
    {
        return vector->getSamplesPerSecond();
    }
    //! \return czas trwania pojedynczej próbki
    virtual float getSampleDuration() const
    {
        return vector->getSampleDuration();
    }
	//! \return nazwa jednostki czasu
	virtual const std::string& getTimeBaseUnit() const
	{
		return descriptor.getTimeBaseUnit();
	}
	//! \return skala czasu
	virtual float getTimeScaleFactor() const
	{
		return descriptor.getTimeScaleFactor();
	}
    //! \return nazwa jednostki wartości
	virtual const std::string& getValueBaseUnit() const
	{
		return descriptor.getValueBaseUnit();
	}
	//! \return skala wartości
	virtual float getValueScaleFactor() const
	{
		return descriptor.getValueScaleFactor();
	}
    //! ustawia nazwę jednostki czasu
	//! \param timeBaseUnit nowa nazwa
	virtual void setTimeBaseUnit(const std::string& timeBaseUnit)
	{
		descriptor.setTimeBaseUnit(timeBaseUnit);
	}
    //! ustawia skalę czasu
	//! \param timeScaleFactor nowa nazwa
	virtual void setTimeScaleFactor(float timeScaleFactor)
	{
		descriptor.setTimeScaleFactor(timeScaleFactor);
	}
    //! ustawia nazwę jednostki wartości
	//! \param valueBaseUnit nowa nazwa
    virtual void setValueBaseUnit(const std::string& valueBaseUnit)
	{
		descriptor.setValueBaseUnit(valueBaseUnit);
	}
    //! ustawia skalę dla wartości
	//! \param valueScaleFactor nowa skala
	virtual void setValueScaleFactor(float valueScaleFactor)
	{
		descriptor.setValueScaleFactor(valueScaleFactor);
	}

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
class ScalarWithTimeSegment : public ScalarChannelReaderInterface, public utils::IChannelDescriptorWriter
{
protected:
    //! konstruktor kopiujący
    ScalarWithTimeSegment(const ScalarWithTimeSegment & adaptor) : descriptor(adaptor.descriptor), startIndex(adaptor.startIndex), endIndex(adaptor.endIndex) {}

public:
    //! konstruktor
    //! \param channel kanał, który ma być opakowany w modyfikator
    //! \param start początek przedziału czasowego
    //! \param end koniec przedziału czasowego
    ScalarWithTimeSegment(const ScalarChannelReaderInterfaceConstPtr & channel, time_type start, time_type end)
        : descriptor(*channel),	reader(channel)
    {
        startIndex = channel->getValueHelper(start).first;
        endIndex = channel->getValueHelper(end).second;
    }
    //! 
    virtual ~ScalarWithTimeSegment() {}
    //! \return nazwa kanału
    virtual const std::string& getName() const
    {
        return reader->getName();
    }
    //! \return zwraca klon obiektu
    virtual ScalarWithTimeSegment * clone() const
    {
        return new ScalarWithTimeSegment(*this);
    }
    //! \return Czas trwania kanału
    virtual time_type getLength() const
    {
        return static_cast<time_type>(100);
    }
    //! Transformuje zmodyfikowany indeks do indeksu oryginalnego
    //! \param idx zmodyfikowany indeks
    //! \return indeks oryginalny
    size_type transformIndex(size_type idx) const
    {
        return startIndex + idx;
        float x = static_cast<float>(idx) / size();
        return static_cast<size_type>(startIndex * (1.0f - x) + endIndex * x);
    }
    //! \param idx Indeks próbki
    //! \return Wartość czasu dla danego indeksu
    virtual time_type argument(size_type idx) const
    {
        return static_cast<time_type>(idx * 100) / size();
    }
    //! \param idx Indeks próbki
    //! \return Wartość próbki dla danego indeksu
    virtual point_type_const_reference value(size_type idx) const
    {
        return reader->value(transformIndex(idx));
    }
    //! \return Ilość próbek w kanale
    virtual size_type size() const
    {
        return endIndex - startIndex;
    }
    //! \return Czy kanał nie zawiera danych
    virtual bool empty() const
    {
        return reader->empty();
    }
    //! \return liczba próbek na sekundę
    virtual float getSamplesPerSecond() const
    {
        return size() / getLength();
    }
    //! \return czas trwania próbki
    virtual float getSampleDuration() const
    {
        if (size()) {
          return 1.0 / getSamplesPerSecond();
        }

        throw std::runtime_error("empty channel");
    }
    //! \return nazwa jednostki czasu
    virtual const std::string& getTimeBaseUnit() const
    {
        return descriptor.getTimeBaseUnit();
    }
    //! \return skala czasu
    virtual float getTimeScaleFactor() const
    {
        return descriptor.getTimeScaleFactor();
    }
    //! \return nazwa jednostki wartości
    virtual const std::string& getValueBaseUnit() const
    {
        return descriptor.getValueBaseUnit();
    }
    //! \return skala wartości
    virtual float getValueScaleFactor() const
    {
        return descriptor.getValueScaleFactor();
    }
    //! ustawia nazwę jednostki czasu
    //! \param timeBaseUnit nowa nazwa
    virtual void setTimeBaseUnit(const std::string& timeBaseUnit)
    {
        descriptor.setTimeBaseUnit(timeBaseUnit);
    }
    //! ustawia skalę czasu
    //! \param timeScaleFactor nowa nazwa
    virtual void setTimeScaleFactor(float timeScaleFactor)
    {
        descriptor.setTimeScaleFactor(timeScaleFactor);
    }
    //! ustawia nazwę jednostki wartości
    //! \param valueBaseUnit nowa nazwa
    virtual void setValueBaseUnit(const std::string& valueBaseUnit)
    {
        descriptor.setValueBaseUnit(valueBaseUnit);
    }
    //! ustawia skalę dla wartości
    //! \param valueScaleFactor nowa skala
    virtual void setValueScaleFactor(float valueScaleFactor)
    {
        descriptor.setValueScaleFactor(valueScaleFactor);
    }

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
class ScalarChannelNormalizer
{
public:
    //! funktor odpowiada za modyfikację kopii kanału 
    //! \param modifierInterface writer dla zmodyfikowanego kanału
    //! \param observedChannel modyfikowany kanała
    //! \param myChannel kanał wynikowy
    void operator()(ScalarChannelReaderInterface::_MyExtendedWriter & modifierInterface,
        const ScalarChannelReaderInterface::_MyRawChannelReaderType & observedChannel,
        const ScalarChannelReaderInterface::_MyRawChannelReaderType & myChannel)
    {
        //uzupełnij brakujące prboki
        if(myChannel.size() < observedChannel.size()){
            for(auto idx = myChannel.size(); idx < observedChannel.size(); ++idx){
                modifierInterface.addPoint(observedChannel.argument(idx), observedChannel.value(idx));
            }
        }

        auto i = 0;

        //min i max
        auto minVal = observedChannel.value(0);
        auto maxVal = minVal;

        ++i;

        for( ; i != observedChannel.size(); ++i){
            if(observedChannel.value(i) < minVal){
                minVal = observedChannel.value(i);
            }else if(observedChannel.value(i) > maxVal){
                maxVal = observedChannel.value(i);
            }
        }

        ScalarChannelReaderInterface::point_type diff = maxVal - minVal;

        if(diff != 0){
            //aktualizacja próbek
            for(ScalarChannelReaderInterface::size_type idx = 0; idx < myChannel.size(); ++idx){
                modifierInterface.setIndexData(idx, (observedChannel.value(idx) - minVal) / diff);
            }
        }
    }
};


//! Podstawa dla kanału analogowego zapisanego w pliku c3d
class C3DAnalogChannel : public ScalarChannel
{
protected:
    //! konstruktor
    //! \param c3dData przeparsowane dane z pliku c3d
    //! \param channelNo numer kanału w pliku c3d
    C3DAnalogChannel( const c3dlib::C3DParser& c3dData, int channelNo ) :
         ScalarChannel( static_cast<int>(c3dData.getNumberAnalogSamplePerFrame() * c3dData.getPointFrequency()) )
         {
             if ( c3dData.getNumAnalogFrames() == 0) {
                 throw std::runtime_error("Incorrect number of frames.");
             }

             c3dlib::C3DParser::IAnalogConstPtr analog = c3dData.getAnalog(channelNo);

             int numSamples = c3dData.getNumAnalogFrames();
             for (int i = 0; i < numSamples; ++i) {
                 addPoint(static_cast<float>(analog->getValue(i)));
             }

             setName(analog->getLabel());
             setValueBaseUnit(analog->getUnit());
             setTimeBaseUnit("s");
         }
    //! konstruktor 
    //! \param samplesPerSec liczba próbek na sekundę
    C3DAnalogChannel( int samplesPerSec ) :
    ScalarChannel(samplesPerSec)
    {}
    //! konstruktor kopiujący
    //! \param channel kopiowany kanał
    C3DAnalogChannel( const C3DAnalogChannel& channel ) :
    ScalarChannel(channel)
    {}
};
typedef boost::shared_ptr<C3DAnalogChannel> C3DAnalogChannelPtr;

//! kanał EMG
class EMGChannel : public C3DAnalogChannel
{
public:
    //! konstruktor 
    //! \param samplesPerSec liczba próbek na sekundę
    explicit EMGChannel(int samplesPerSec) :
    C3DAnalogChannel(samplesPerSec)
    {}
    //! konstruktor kopiujący
    //! \param channel kopiowany kanał
    EMGChannel(const EMGChannel& channel) :
    C3DAnalogChannel(channel)
    {}
    //! konstruktor
    //! \param c3dData przeparsowane dane z pliku c3d
    //! \param channelNo numer kanału w pliku c3d
    EMGChannel(const c3dlib::C3DParser& data, int channelNo) :
    C3DAnalogChannel(data, channelNo)
    {}

public:
    //! \return zwraca kopię obiektu, którą trzeba samemu usunąć
    virtual EMGChannel* clone() const
    {
        return new EMGChannel(*this);
    }
};
typedef boost::shared_ptr<EMGChannel> EMGChannelPtr;
typedef boost::shared_ptr<const EMGChannel> EMGChannelConstPtr;

//! kanał GRF
class GRFChannel : public VectorChannel
{
friend class C3DParser;
public:
    //! rodzaj kanału GRF
	enum Type { Unknown, F1, M1, F2, M2 };

public:
    //! konstruktor 
    //! \param samplesPerSec liczba próbek na sekundę
    //! \param treshold próg dla którego warto analizować dane z płyty GRF
    explicit GRFChannel(int samplesPerSec, float treshold = 100.0f) :
        VectorChannel(samplesPerSec) ,
        type(Unknown),
        treshold(treshold)
    {}
    //! konstruktor kopiujący
    //! \param channel kopiowany kanał
    GRFChannel(const GRFChannel& channel) :
    VectorChannel(channel),
	type(channel.type)
    {}
    //! konstruktor
    //! \param c3dData przeparsowane dane z pliku c3d
    //! \param channelNo numer kanału w pliku c3d
    //! \param treshold próg dla którego warto analizować dane z płyty GRF
    GRFChannel( const c3dlib::C3DParser& data, int channelNo, float treshold = 200.0f) :
    VectorChannel(static_cast<int>(data.getNumberAnalogSamplePerFrame() * data.getPointFrequency())),
        type(Unknown),
        dataStart(-1.0f),
        dataEnd(-1.0f)
    {
        if ( data.getNumAnalogFrames() == 0) {
            throw std::runtime_error("Incorrect number of frames.");
        }

        float treshold2 = treshold * treshold;

        c3dlib::C3DParser::IAnalogConstPtr x = data.getAnalog(3 * channelNo + 0);
        c3dlib::C3DParser::IAnalogConstPtr y = data.getAnalog(3 * channelNo + 1);
        c3dlib::C3DParser::IAnalogConstPtr z = data.getAnalog(3 * channelNo + 2);

        bool dataStartSet = false;
        bool dataEndSet = false;

        int startIndex, endIndex;

        int numSamples = data.getNumAnalogFrames();
        for (int i = 0; i < numSamples; ++i) {
            osg::Vec3 val = osg::Vec3(x->getValue(i), y->getValue(i), z->getValue(i));
            if (val.length2() > treshold2) {
                if (!dataStartSet) {
                    dataStartSet = true;
                    startIndex = i;
                }
            } else {
                if (dataStartSet && !dataEndSet) {
                    endIndex = i;
                    dataEndSet = true;
                }
            }
            addPoint(val);
        }

        std::string name = x->getLabel();
        auto it = std::remove(name.begin(), name.end(), 'x');
        if (it == name.end()) {
            it = std::remove(name.begin(), name.end(), 'X');
        }

        if (it != name.end()) {
            name.erase(it);
        }

        //std::string name = x->getLabel().erase( remove( str.begin(), str.end(), 'x' ), str.end() );
        setName(name);
        setValueBaseUnit(x->getUnit());
        //? w pliku c3d nie pojawia się cos takiego jak jednostka czasu, dlatego można przyjąć sekundy
        setTimeBaseUnit("s");

        if (dataStartSet) {
            dataStart = (startIndex / static_cast<float>(numSamples)) * getLength();
        }
        if (dataEndSet) {
            dataEnd = (endIndex / static_cast<float>(numSamples)) * getLength();
        }

        switch (channelNo) {
        case 0:		type = F1;		break;
        case 1:		type = M1;		break;
        case 2:		type = F2;		break;
        case 3:		type = M2;		break;
        default:	type = Unknown; break;
        }
    }

public:
    //! \return zwraca kopię obiektu, którą trzeba samemu usunąć
    virtual GRFChannel* clone() const
    {
        return new GRFChannel(*this);
    }
    //! \return rodzaj kanału GRF
	GRFChannel::Type getType() const { return type; }
	//! \return czy kanał ma wykryty peak, związanych z naciskiem na płytę
	bool hasStartEndData() const { return dataStart >= 0.0f && dataEnd >= 0.0f; }
	//! \return początek danych, związanych z naciskiem na płytę
	float getDataStartTime() const { UTILS_ASSERT(dataStart >= 0.0f); return dataStart; }
	//! \return koniec danych, związanych z naciskiem na płyę
	float getDataEndTime() const { UTILS_ASSERT(dataEnd >= 0.0f); return dataEnd; }
	
private:
    Type type;
    //! próg dla którego warto analizować dane z płyty GRF
    float treshold;
    //! początek danych, związanych z naciskiem na płytę
	float dataStart;
    //! koniec danych, związanych z naciskiem na płytę
    float dataEnd;
};
typedef boost::shared_ptr<GRFChannel> GRFChannelPtr;
typedef boost::shared_ptr<const GRFChannel> GRFChannelConstPtr;

//! kanał zawiera dane o jednym markerze
class MarkerChannel : public VectorChannel
{
public:
    //! konstruktor 
    //! \param samplesPerSec liczba próbek na sekundę
    MarkerChannel(int samplesPerSec) : VectorChannel(samplesPerSec) {}

    MarkerChannel(const MarkerChannel& channel) :
        VectorChannel(channel)
        { }

    //! konstruktor
    //! \param c3dData przeparsowane dane z pliku c3d
    //! \param channelNo numer kanału w pliku c3d
    MarkerChannel(const c3dlib::C3DParser& data, int channelNo ) :
        VectorChannel( data.getPointFrequency())
        {
            c3dlib::C3DParser::IPointConstPtr point = data.getPoint(channelNo);

            int numSamples = data.getNumPointFrames();

            for (int i = 0; i < numSamples; ++i) {
                osg::Vec3 val = point->getValue(i);
                addPoint(val);
            }
            setName(point->getLabel());
            setValueBaseUnit(point->getUnit());
            setTimeBaseUnit("s");
        }
public:
    //! \return zwraca kopię obiektu, którą trzeba samemu usunąć
    virtual MarkerChannel* clone() const
    {
        return new MarkerChannel(*this);
    }
};
typedef boost::shared_ptr<MarkerChannel> MarkerChannelPtr;
typedef boost::shared_ptr<const MarkerChannel> MarkerChannelConstPtr;




#define DEFINE_CHANNEL(name)																	 \
	class name##Channel : public VectorChannel 													 \
	{																							 \
	private:																					 \
		name##Channel(int samplesPerSec) : VectorChannel(samplesPerSec) {}						 \
																								 \
	public:        																				 \
		name##Channel(const name##Channel& channel) :											 \
		  VectorChannel(channel)																 \
		  { }																					 \
																								 \
		  name##Channel(const c3dlib::C3DParser& data, int channelNo ) :						 \
		  VectorChannel( data.getPointFrequency())												 \
		  {																						 \
			  c3dlib::C3DParser::IPointConstPtr point = data.getPoint(channelNo);				 \
																								 \
			  int numSamples = data.getNumPointFrames();										 \
																								 \
			  for (int i = 0; i < numSamples; ++i) {											 \
				  addPoint(point->getValue(i));													 \
			  }																					 \
			  setName(point->getLabel());														 \
              setValueBaseUnit(point->getUnit());                                                \
		  }																						 \
																								 \
	public:																						 \
		virtual name##Channel* clone() const {  return new name##Channel(*this); }				 \
	};												 											 \
	typedef core::shared_ptr<name##Channel> name##ChannelPtr;									 \
	typedef core::shared_ptr<const name##Channel> name##ChannelConstPtr;


DEFINE_CHANNEL(Force);
DEFINE_CHANNEL(Moment);
DEFINE_CHANNEL(Angle);
DEFINE_CHANNEL(Power);

CORE_DEFINE_WRAPPER(VectorChannelReaderInterface, utils::PtrPolicyBoost, utils::ClonePolicyVirtualCloneMethod);
CORE_DEFINE_WRAPPER_INHERITANCE(VectorChannel, VectorChannelReaderInterface);
CORE_DEFINE_WRAPPER_INHERITANCE(MarkerChannel,VectorChannel);
CORE_DEFINE_WRAPPER_INHERITANCE(ForceChannel, VectorChannel);
CORE_DEFINE_WRAPPER_INHERITANCE(MomentChannel,VectorChannel);
CORE_DEFINE_WRAPPER_INHERITANCE(AngleChannel, VectorChannel);
CORE_DEFINE_WRAPPER_INHERITANCE(PowerChannel, VectorChannel);
CORE_DEFINE_WRAPPER(ScalarChannelReaderInterface, utils::PtrPolicyBoost, utils::ClonePolicyVirtualCloneMethod);
CORE_DEFINE_WRAPPER_INHERITANCE(ScalarChannel, ScalarChannelReaderInterface);
CORE_DEFINE_WRAPPER_INHERITANCE(VectorToScalarAdaptor, ScalarChannelReaderInterface);
CORE_DEFINE_WRAPPER_INHERITANCE(C3DAnalogChannel, ScalarChannel);
CORE_DEFINE_WRAPPER_INHERITANCE(EMGChannel, C3DAnalogChannel);
CORE_DEFINE_WRAPPER_INHERITANCE(GRFChannel, VectorChannel);



#endif  // __HEADER_GUARD_CORE__C3DCHANNELS_H__
