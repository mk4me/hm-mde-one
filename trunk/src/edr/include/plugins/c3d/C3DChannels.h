/********************************************************************
	created:  2011/02/17
	created:  17:2:2011   14:06
	filename: C3DChannels.h
	author:	  Piotr Gwiazdowski

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_C3D__C3DCHANNELS_H__
#define __HEADER_GUARD_C3D__C3DCHANNELS_H__

#include <core/SmartPtr.h>
#include <stdexcept>
#include <utils/Debug.h>
#include <utils/DataChannel.h>
#include <utils/DataChannelModifiers.h>
#include <utils/DataChannelStats.h>
#include <utils/DataChannelCollection.h>
#include <utils/DataChannelAccessors.h>
#include <utils/DataChannelTimers.h>
#include <utils/PtrPolicyBoost.h>
#include <core/ObjectWrapper.h>
#include <c3dlib/C3DParser.h>

//! interfejs do odczytu czasu timera
typedef utils::ITimerReader<float>::TimerReaderType TimerReader;
//! wskaŸnik do interfejsu do odczytu czasu timera
typedef utils::ITimerReader<float>::TimerReaderPtr TimerReaderPtr;
//! niemodyfikowalny wskaŸnik do interfejsu do odczytu czasu timera
typedef utils::ITimerReader<float>::TimerReaderConstPtr TimerReaderConstPtr;

//! akcesor umo¿liwiajacy dostanie siê do danych w sposób ci¹g³y
typedef utils::DataChannelTimeAccessor<float, float> ScalarContiniousTimeAccessor;
//! interfejs, dziêki któremu mo¿na zmodyfikowaæ kana³
typedef utils::ChannelAutoModifier<float, float> ScalarModifier;

//! kana³ przechowuj¹cy liczby zmiennoprzecinkowe
typedef utils::Channel<float> ScalarChannel;
//! interfejs umo¿liwiaj¹cy odczyt kana³u
typedef ScalarChannel::Interface ScalarChannelReaderInterface;
//! wkaŸnik na interfejs umo¿liwiaj¹cy odczyt kana³u
typedef core::shared_ptr<ScalarChannelReaderInterface> ScalarChannelReaderInterfacePtr;
//! wkaŸnik na niemodyfikowalny interfejs umo¿liwiaj¹cy odczyt kana³u
typedef core::shared_ptr<const ScalarChannelReaderInterface> ScalarChannelReaderInterfaceConstPtr;

//! kana³ przechowuj¹cy wektory trójwymiarowe
typedef utils::Channel<osg::Vec3f> VectorChannel;
//! interfejs umo¿liwiaj¹cy odczyt kana³u
typedef VectorChannel::Interface VectorChannelReaderInterface;
//! akcesor umo¿liwiajacy dostanie siê do wektorów w sposób ci¹g³y
typedef utils::DataChannelTimeAccessor<osg::Vec3f, float> VectorContiniousTimeAccessor;
//! interfejs, dziêki któremu mo¿na zmodyfikowaæ kana³ z wektorami
typedef utils::ChannelAutoModifier<osg::Vec3f, float> VectorModifier;
//! wskaŸnik na kana³ przechowuj¹cy wektory trójwymiarowe
typedef core::shared_ptr<VectorChannel> VectorChannelPtr;
//! niemodyfikowalny wskaŸnik na kana³ przechowuj¹cy wektory trójwymiarowe
typedef core::shared_ptr<const VectorChannel> VectorChannelConstPtr;
//! wkaŸnik na interfejs umo¿liwiaj¹cy odczyt kana³u
typedef core::shared_ptr<VectorChannelReaderInterface> VectorChannelReaderInterfacePtr;
//! wkaŸnik na niemodyfikowalny interfejs umo¿liwiaj¹cy odczyt kana³u
typedef core::shared_ptr<const VectorChannelReaderInterface> VectorChannelReaderInterfaceConstPtr;

//! Adaptor, dziêki któremu mo¿na obs³u¿yæ kana³ z wektorem trójwymiarowym tak jak skalarny
class VectorToScalarAdaptor : public ScalarChannelReaderInterface, public utils::IChannelDescriptorWriter
{
protected:
    //! konstruktor kopiuj¹cy
    VectorToScalarAdaptor(const VectorToScalarAdaptor & adaptor) : descriptor(adaptor.descriptor), vector(adaptor.vector), index(adaptor.index), name(adaptor.name) {}

public:
    //! konstruktor
    //! \param vector adaptowany kana³ z wektorem
    //! \param idx indeks kana³u z wektorem, ta czêœæ bêdzie widziana jako skalar
    //! \param name nazwa kana³u
    VectorToScalarAdaptor(const VectorChannelReaderInterfaceConstPtr & vector, size_type idx, const std::string & name = std::string())
        : descriptor(*vector), vector(vector), index(idx), name(name)
    {
        UTILS_ASSERT((idx >= 0 && idx < 3), "Bledny index dla adaptera skalarnego kanalu wektorowego");
    }
    virtual ~VectorToScalarAdaptor() {}

public:
    //! \return zwraca nazwê kana³u
    virtual const std::string& getName() const
    {
        return name;
    }
	//! Ustawia nazwê kana³u
	//! \param name nowa nazwa
	void setName(const std::string& name)
	{
		this->name = name;
	}
    //! \return zwraca kopiê obiektu, któr¹ trzeba samemu usun¹æ
    virtual VectorToScalarAdaptor * clone() const
    {
        return new VectorToScalarAdaptor(*this);
    }
    //! \return Czas trwania kana³u
    virtual time_type getLength() const
    {
        return vector->getLength();
    }
    //! \param idx Indeks próbki
    //! \return Wartoœæ czasu dla danego indeksu
    virtual time_type argument(size_type idx) const
    {
        return vector->argument(idx);
    }
    //! \param idx Indeks próbki
    //! \return Wartoœæ próbki dla danego indeksu
    virtual point_type_const_reference value(size_type idx) const
    {
        return vector->value(idx)[index];
    }
    //! \return Iloœæ próbek w kanale
    virtual size_type size() const
    {
        return vector->size();
    }
    //! \return Czy kana³ nie zawiera danych
    virtual bool empty() const
    {
        return vector->empty();
    }
    //! \return liczba próbek na sekundê
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
    //! \return nazwa jednostki wartoœci
	virtual const std::string& getValueBaseUnit() const
	{
		return descriptor.getValueBaseUnit();
	}
	//! \return skala wartoœci
	virtual float getValueScaleFactor() const
	{
		return descriptor.getValueScaleFactor();
	}
    //! ustawia nazwê jednostki czasu
	//! \param timeBaseUnit nowa nazwa
	virtual void setTimeBaseUnit(const std::string& timeBaseUnit)
	{
		descriptor.setTimeBaseUnit(timeBaseUnit);
	}
    //! ustawia skalê czasu
	//! \param timeScaleFactor nowa nazwa
	virtual void setTimeScaleFactor(float timeScaleFactor)
	{
		descriptor.setTimeScaleFactor(timeScaleFactor);
	}
    //! ustawia nazwê jednostki wartoœci
	//! \param valueBaseUnit nowa nazwa
    virtual void setValueBaseUnit(const std::string& valueBaseUnit)
	{
		descriptor.setValueBaseUnit(valueBaseUnit);
	}
    //! ustawia skalê dla wartoœci
	//! \param valueScaleFactor nowa skala
	virtual void setValueScaleFactor(float valueScaleFactor)
	{
		descriptor.setValueScaleFactor(valueScaleFactor);
	}

private:
    //! adaptowany kana³ z wektorem
    VectorChannelReaderInterfaceConstPtr vector;
    //! indeks wektora, który ma byæ widoczny jako skalar
    size_type index;
    //! nazwa kana³u
    std::string name;
    //! standardowy deskryptor kana³u
	utils::ChannelDescriptor descriptor;
};

//! modyfikator, który pobiera wycinek kana³u podstawowego i normalizuje go (0 - 100%)
class ScalarWithTimeSegment : public ScalarChannelReaderInterface, public utils::IChannelDescriptorWriter
{
protected:
    //! konstruktor kopiuj¹cy
    ScalarWithTimeSegment(const ScalarWithTimeSegment & adaptor) : descriptor(adaptor.descriptor), startIndex(adaptor.startIndex), endIndex(adaptor.endIndex) {}

public:
    //! konstruktor
    //! \param channel kana³, który ma byæ opakowany w modyfikator
    //! \param start pocz¹tek przedzia³u czasowego
    //! \param end koniec przedzia³u czasowego
    ScalarWithTimeSegment(const ScalarChannelReaderInterfaceConstPtr & channel, time_type start, time_type end)
        : descriptor(*channel),	reader(channel)
    {
        startIndex = channel->getValueHelper(start).first;
        endIndex = channel->getValueHelper(end).second;
    }
    //! 
    virtual ~ScalarWithTimeSegment() {}
    //! \return nazwa kana³u
    virtual const std::string& getName() const
    {
        return reader->getName();
    }
    //! \return zwraca klon obiektu
    virtual ScalarWithTimeSegment * clone() const
    {
        return new ScalarWithTimeSegment(*this);
    }
    //! \return Czas trwania kana³u
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
    //! \return Wartoœæ czasu dla danego indeksu
    virtual time_type argument(size_type idx) const
    {
        return static_cast<time_type>(idx * 100) / size();
    }
    //! \param idx Indeks próbki
    //! \return Wartoœæ próbki dla danego indeksu
    virtual point_type_const_reference value(size_type idx) const
    {
        return reader->value(transformIndex(idx));
    }
    //! \return Iloœæ próbek w kanale
    virtual size_type size() const
    {
        return endIndex - startIndex;
    }
    //! \return Czy kana³ nie zawiera danych
    virtual bool empty() const
    {
        return reader->empty();
    }
    //! \return liczba próbek na sekundê
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
    //! \return nazwa jednostki wartoœci
    virtual const std::string& getValueBaseUnit() const
    {
        return descriptor.getValueBaseUnit();
    }
    //! \return skala wartoœci
    virtual float getValueScaleFactor() const
    {
        return descriptor.getValueScaleFactor();
    }
    //! ustawia nazwê jednostki czasu
    //! \param timeBaseUnit nowa nazwa
    virtual void setTimeBaseUnit(const std::string& timeBaseUnit)
    {
        descriptor.setTimeBaseUnit(timeBaseUnit);
    }
    //! ustawia skalê czasu
    //! \param timeScaleFactor nowa nazwa
    virtual void setTimeScaleFactor(float timeScaleFactor)
    {
        descriptor.setTimeScaleFactor(timeScaleFactor);
    }
    //! ustawia nazwê jednostki wartoœci
    //! \param valueBaseUnit nowa nazwa
    virtual void setValueBaseUnit(const std::string& valueBaseUnit)
    {
        descriptor.setValueBaseUnit(valueBaseUnit);
    }
    //! ustawia skalê dla wartoœci
    //! \param valueScaleFactor nowa skala
    virtual void setValueScaleFactor(float valueScaleFactor)
    {
        descriptor.setValueScaleFactor(valueScaleFactor);
    }

private:
    //! opakowywany kana³
    ScalarChannelReaderInterfaceConstPtr reader;
    //! indeks pocz¹tkowej próbki
    int startIndex;
    //! indeks koñcowej próbki
    int endIndex;
    //! standardowy deskryptor kana³u
	utils::ChannelDescriptor descriptor;
};
//! wskaŸnik do kana³u z danymi skalarnymi
typedef core::shared_ptr<ScalarChannel> ScalarChannelPtr;
//! niemodyfikowalny skaŸnik do kana³u z danymi skalarnymi
typedef core::shared_ptr<const ScalarChannel> ScalarChannelConstPtr;
//! obiekt ze statystykami do kana³u z danymi skalarnymi
typedef utils::ChannelStats<ScalarChannel::point_type, ScalarChannel::time_type> ScalarChannelStats;
//! wskaŸnik obiektu ze statystykami do kana³u z danymi skalarnymi
typedef core::shared_ptr<ScalarChannelStats> ScalarChannelStatsPtr;
//! niemodyfikowalny wskaŸnik obiektu ze statystykami do kana³u z danymi skalarnymi
typedef core::shared_ptr<const ScalarChannelStats> ScalarChannelStatsConstPtr;

//! Modyfikator, który normalizuje kana³
class ScalarChannelNormalizer
{
public:
    //! funktor odpowiada za modyfikacjê kopii kana³u 
    //! \param modifierInterface writer dla zmodyfikowanego kana³u
    //! \param observedChannel modyfikowany kana³a
    //! \param myChannel kana³ wynikowy
    void operator()(ScalarChannelReaderInterface::_MyExtendedWriter & modifierInterface,
        const ScalarChannelReaderInterface::_MyRawChannelReaderType & observedChannel,
        const ScalarChannelReaderInterface::_MyRawChannelReaderType & myChannel)
    {
        //uzupe³nij brakuj¹ce prboki
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


//! Podstawa dla kana³u analogowego zapisanego w pliku c3d
class C3DAnalogChannel : public ScalarChannel
{
protected:
    //! konstruktor
    //! \param c3dData przeparsowane dane z pliku c3d
    //! \param channelNo numer kana³u w pliku c3d
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
    //! \param samplesPerSec liczba próbek na sekundê
    C3DAnalogChannel( int samplesPerSec ) :
    ScalarChannel(samplesPerSec)
    {}
    //! konstruktor kopiuj¹cy
    //! \param channel kopiowany kana³
    C3DAnalogChannel( const C3DAnalogChannel& channel ) :
    ScalarChannel(channel)
    {}
};
typedef boost::shared_ptr<C3DAnalogChannel> C3DAnalogChannelPtr;

//! kana³ EMG
class EMGChannel : public C3DAnalogChannel
{
public:
    //! konstruktor 
    //! \param samplesPerSec liczba próbek na sekundê
    explicit EMGChannel(int samplesPerSec) :
    C3DAnalogChannel(samplesPerSec)
    {}
    //! konstruktor kopiuj¹cy
    //! \param channel kopiowany kana³
    EMGChannel(const EMGChannel& channel) :
    C3DAnalogChannel(channel)
    {}
    //! konstruktor
    //! \param c3dData przeparsowane dane z pliku c3d
    //! \param channelNo numer kana³u w pliku c3d
    EMGChannel(const c3dlib::C3DParser& data, int channelNo) :
    C3DAnalogChannel(data, channelNo)
    {}

public:
    //! \return zwraca kopiê obiektu, któr¹ trzeba samemu usun¹æ
    virtual EMGChannel* clone() const
    {
        return new EMGChannel(*this);
    }
};
typedef boost::shared_ptr<EMGChannel> EMGChannelPtr;
typedef boost::shared_ptr<const EMGChannel> EMGChannelConstPtr;

//! kana³ GRF
class GRFChannel : public VectorChannel
{
friend class C3DParser;
public:
    //! rodzaj kana³u GRF
	enum Type { Unknown, F1, M1, F2, M2 };

public:
    //! konstruktor 
    //! \param samplesPerSec liczba próbek na sekundê
    //! \param treshold próg dla którego warto analizowaæ dane z p³yty GRF
    explicit GRFChannel(int samplesPerSec, float treshold = 100.0f) :
        VectorChannel(samplesPerSec) ,
        type(Unknown),
        treshold(treshold)
    {}
    //! konstruktor kopiuj¹cy
    //! \param channel kopiowany kana³
    GRFChannel(const GRFChannel& channel) :
    VectorChannel(channel),
	type(channel.type)
    {}
    //! konstruktor
    //! \param c3dData przeparsowane dane z pliku c3d
    //! \param channelNo numer kana³u w pliku c3d
    //! \param treshold próg dla którego warto analizowaæ dane z p³yty GRF
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
        //? w pliku c3d nie pojawia siê cos takiego jak jednostka czasu, dlatego mo¿na przyj¹æ sekundy
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
    //! \return zwraca kopiê obiektu, któr¹ trzeba samemu usun¹æ
    virtual GRFChannel* clone() const
    {
        return new GRFChannel(*this);
    }
    //! \return rodzaj kana³u GRF
	GRFChannel::Type getType() const { return type; }
	//! \return czy kana³ ma wykryty peak, zwi¹zanych z naciskiem na p³ytê
	bool hasStartEndData() const { return dataStart >= 0.0f && dataEnd >= 0.0f; }
	//! \return pocz¹tek danych, zwi¹zanych z naciskiem na p³ytê
	float getDataStartTime() const { UTILS_ASSERT(dataStart >= 0.0f); return dataStart; }
	//! \return koniec danych, zwi¹zanych z naciskiem na p³yê
	float getDataEndTime() const { UTILS_ASSERT(dataEnd >= 0.0f); return dataEnd; }
	
private:
    Type type;
    //! próg dla którego warto analizowaæ dane z p³yty GRF
    float treshold;
    //! pocz¹tek danych, zwi¹zanych z naciskiem na p³ytê
	float dataStart;
    //! koniec danych, zwi¹zanych z naciskiem na p³ytê
    float dataEnd;
};
typedef boost::shared_ptr<GRFChannel> GRFChannelPtr;
typedef boost::shared_ptr<const GRFChannel> GRFChannelConstPtr;

//! kana³ zawiera dane o jednym markerze
class MarkerChannel : public VectorChannel
{
public:
    //! konstruktor 
    //! \param samplesPerSec liczba próbek na sekundê
    MarkerChannel(int samplesPerSec) : VectorChannel(samplesPerSec) {}

    MarkerChannel(const MarkerChannel& channel) :
        VectorChannel(channel)
        { }

    //! konstruktor
    //! \param c3dData przeparsowane dane z pliku c3d
    //! \param channelNo numer kana³u w pliku c3d
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
    //! \return zwraca kopiê obiektu, któr¹ trzeba samemu usun¹æ
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
