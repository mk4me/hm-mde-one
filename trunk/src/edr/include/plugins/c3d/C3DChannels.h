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
//! wska�nik do interfejsu do odczytu czasu timera
typedef utils::ITimerReader<float>::TimerReaderPtr TimerReaderPtr;
//! niemodyfikowalny wska�nik do interfejsu do odczytu czasu timera
typedef utils::ITimerReader<float>::TimerReaderConstPtr TimerReaderConstPtr;

//! akcesor umo�liwiajacy dostanie si� do danych w spos�b ci�g�y
typedef utils::DataChannelTimeAccessor<float, float> ScalarContiniousTimeAccessor;
//! interfejs, dzi�ki kt�remu mo�na zmodyfikowa� kana�
typedef utils::ChannelAutoModifier<float, float> ScalarModifier;

//! kana� przechowuj�cy liczby zmiennoprzecinkowe
typedef utils::Channel<float> ScalarChannel;
//! interfejs umo�liwiaj�cy odczyt kana�u
typedef ScalarChannel::Interface ScalarChannelReaderInterface;
//! wka�nik na interfejs umo�liwiaj�cy odczyt kana�u
typedef core::shared_ptr<ScalarChannelReaderInterface> ScalarChannelReaderInterfacePtr;
//! wka�nik na niemodyfikowalny interfejs umo�liwiaj�cy odczyt kana�u
typedef core::shared_ptr<const ScalarChannelReaderInterface> ScalarChannelReaderInterfaceConstPtr;

//! kana� przechowuj�cy wektory tr�jwymiarowe
typedef utils::Channel<osg::Vec3f> VectorChannel;
//! interfejs umo�liwiaj�cy odczyt kana�u
typedef VectorChannel::Interface VectorChannelReaderInterface;
//! akcesor umo�liwiajacy dostanie si� do wektor�w w spos�b ci�g�y
typedef utils::DataChannelTimeAccessor<osg::Vec3f, float> VectorContiniousTimeAccessor;
//! interfejs, dzi�ki kt�remu mo�na zmodyfikowa� kana� z wektorami
typedef utils::ChannelAutoModifier<osg::Vec3f, float> VectorModifier;
//! wska�nik na kana� przechowuj�cy wektory tr�jwymiarowe
typedef core::shared_ptr<VectorChannel> VectorChannelPtr;
//! niemodyfikowalny wska�nik na kana� przechowuj�cy wektory tr�jwymiarowe
typedef core::shared_ptr<const VectorChannel> VectorChannelConstPtr;
//! wka�nik na interfejs umo�liwiaj�cy odczyt kana�u
typedef core::shared_ptr<VectorChannelReaderInterface> VectorChannelReaderInterfacePtr;
//! wka�nik na niemodyfikowalny interfejs umo�liwiaj�cy odczyt kana�u
typedef core::shared_ptr<const VectorChannelReaderInterface> VectorChannelReaderInterfaceConstPtr;

//! Adaptor, dzi�ki kt�remu mo�na obs�u�y� kana� z wektorem tr�jwymiarowym tak jak skalarny
class VectorToScalarAdaptor : public ScalarChannelReaderInterface, public utils::IChannelDescriptorWriter
{
protected:
    //! konstruktor kopiuj�cy
    VectorToScalarAdaptor(const VectorToScalarAdaptor & adaptor) : descriptor(adaptor.descriptor), vector(adaptor.vector), index(adaptor.index), name(adaptor.name) {}

public:
    //! konstruktor
    //! \param vector adaptowany kana� z wektorem
    //! \param idx indeks kana�u z wektorem, ta cz�� b�dzie widziana jako skalar
    //! \param name nazwa kana�u
    VectorToScalarAdaptor(const VectorChannelReaderInterfaceConstPtr & vector, size_type idx, const std::string & name = std::string())
        : descriptor(*vector), vector(vector), index(idx), name(name)
    {
        UTILS_ASSERT((idx >= 0 && idx < 3), "Bledny index dla adaptera skalarnego kanalu wektorowego");
    }
    virtual ~VectorToScalarAdaptor() {}

public:
    //! \return zwraca nazw� kana�u
    virtual const std::string& getName() const
    {
        return name;
    }
	//! Ustawia nazw� kana�u
	//! \param name nowa nazwa
	void setName(const std::string& name)
	{
		this->name = name;
	}
    //! \return zwraca kopi� obiektu, kt�r� trzeba samemu usun��
    virtual VectorToScalarAdaptor * clone() const
    {
        return new VectorToScalarAdaptor(*this);
    }
    //! \return Czas trwania kana�u
    virtual time_type getLength() const
    {
        return vector->getLength();
    }
    //! \param idx Indeks pr�bki
    //! \return Warto�� czasu dla danego indeksu
    virtual time_type argument(size_type idx) const
    {
        return vector->argument(idx);
    }
    //! \param idx Indeks pr�bki
    //! \return Warto�� pr�bki dla danego indeksu
    virtual point_type_const_reference value(size_type idx) const
    {
        return vector->value(idx)[index];
    }
    //! \return Ilo�� pr�bek w kanale
    virtual size_type size() const
    {
        return vector->size();
    }
    //! \return Czy kana� nie zawiera danych
    virtual bool empty() const
    {
        return vector->empty();
    }
    //! \return liczba pr�bek na sekund�
    virtual float getSamplesPerSecond() const
    {
        return vector->getSamplesPerSecond();
    }
    //! \return czas trwania pojedynczej pr�bki
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
    //! \return nazwa jednostki warto�ci
	virtual const std::string& getValueBaseUnit() const
	{
		return descriptor.getValueBaseUnit();
	}
	//! \return skala warto�ci
	virtual float getValueScaleFactor() const
	{
		return descriptor.getValueScaleFactor();
	}
    //! ustawia nazw� jednostki czasu
	//! \param timeBaseUnit nowa nazwa
	virtual void setTimeBaseUnit(const std::string& timeBaseUnit)
	{
		descriptor.setTimeBaseUnit(timeBaseUnit);
	}
    //! ustawia skal� czasu
	//! \param timeScaleFactor nowa nazwa
	virtual void setTimeScaleFactor(float timeScaleFactor)
	{
		descriptor.setTimeScaleFactor(timeScaleFactor);
	}
    //! ustawia nazw� jednostki warto�ci
	//! \param valueBaseUnit nowa nazwa
    virtual void setValueBaseUnit(const std::string& valueBaseUnit)
	{
		descriptor.setValueBaseUnit(valueBaseUnit);
	}
    //! ustawia skal� dla warto�ci
	//! \param valueScaleFactor nowa skala
	virtual void setValueScaleFactor(float valueScaleFactor)
	{
		descriptor.setValueScaleFactor(valueScaleFactor);
	}

private:
    //! adaptowany kana� z wektorem
    VectorChannelReaderInterfaceConstPtr vector;
    //! indeks wektora, kt�ry ma by� widoczny jako skalar
    size_type index;
    //! nazwa kana�u
    std::string name;
    //! standardowy deskryptor kana�u
	utils::ChannelDescriptor descriptor;
};

//! modyfikator, kt�ry pobiera wycinek kana�u podstawowego i normalizuje go (0 - 100%)
class ScalarWithTimeSegment : public ScalarChannelReaderInterface, public utils::IChannelDescriptorWriter
{
protected:
    //! konstruktor kopiuj�cy
    ScalarWithTimeSegment(const ScalarWithTimeSegment & adaptor) : descriptor(adaptor.descriptor), startIndex(adaptor.startIndex), endIndex(adaptor.endIndex) {}

public:
    //! konstruktor
    //! \param channel kana�, kt�ry ma by� opakowany w modyfikator
    //! \param start pocz�tek przedzia�u czasowego
    //! \param end koniec przedzia�u czasowego
    ScalarWithTimeSegment(const ScalarChannelReaderInterfaceConstPtr & channel, time_type start, time_type end)
        : descriptor(*channel),	reader(channel)
    {
        startIndex = channel->getValueHelper(start).first;
        endIndex = channel->getValueHelper(end).second;
    }
    //! 
    virtual ~ScalarWithTimeSegment() {}
    //! \return nazwa kana�u
    virtual const std::string& getName() const
    {
        return reader->getName();
    }
    //! \return zwraca klon obiektu
    virtual ScalarWithTimeSegment * clone() const
    {
        return new ScalarWithTimeSegment(*this);
    }
    //! \return Czas trwania kana�u
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
    //! \param idx Indeks pr�bki
    //! \return Warto�� czasu dla danego indeksu
    virtual time_type argument(size_type idx) const
    {
        return static_cast<time_type>(idx * 100) / size();
    }
    //! \param idx Indeks pr�bki
    //! \return Warto�� pr�bki dla danego indeksu
    virtual point_type_const_reference value(size_type idx) const
    {
        return reader->value(transformIndex(idx));
    }
    //! \return Ilo�� pr�bek w kanale
    virtual size_type size() const
    {
        return endIndex - startIndex;
    }
    //! \return Czy kana� nie zawiera danych
    virtual bool empty() const
    {
        return reader->empty();
    }
    //! \return liczba pr�bek na sekund�
    virtual float getSamplesPerSecond() const
    {
        return size() / getLength();
    }
    //! \return czas trwania pr�bki
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
    //! \return nazwa jednostki warto�ci
    virtual const std::string& getValueBaseUnit() const
    {
        return descriptor.getValueBaseUnit();
    }
    //! \return skala warto�ci
    virtual float getValueScaleFactor() const
    {
        return descriptor.getValueScaleFactor();
    }
    //! ustawia nazw� jednostki czasu
    //! \param timeBaseUnit nowa nazwa
    virtual void setTimeBaseUnit(const std::string& timeBaseUnit)
    {
        descriptor.setTimeBaseUnit(timeBaseUnit);
    }
    //! ustawia skal� czasu
    //! \param timeScaleFactor nowa nazwa
    virtual void setTimeScaleFactor(float timeScaleFactor)
    {
        descriptor.setTimeScaleFactor(timeScaleFactor);
    }
    //! ustawia nazw� jednostki warto�ci
    //! \param valueBaseUnit nowa nazwa
    virtual void setValueBaseUnit(const std::string& valueBaseUnit)
    {
        descriptor.setValueBaseUnit(valueBaseUnit);
    }
    //! ustawia skal� dla warto�ci
    //! \param valueScaleFactor nowa skala
    virtual void setValueScaleFactor(float valueScaleFactor)
    {
        descriptor.setValueScaleFactor(valueScaleFactor);
    }

private:
    //! opakowywany kana�
    ScalarChannelReaderInterfaceConstPtr reader;
    //! indeks pocz�tkowej pr�bki
    int startIndex;
    //! indeks ko�cowej pr�bki
    int endIndex;
    //! standardowy deskryptor kana�u
	utils::ChannelDescriptor descriptor;
};
//! wska�nik do kana�u z danymi skalarnymi
typedef core::shared_ptr<ScalarChannel> ScalarChannelPtr;
//! niemodyfikowalny ska�nik do kana�u z danymi skalarnymi
typedef core::shared_ptr<const ScalarChannel> ScalarChannelConstPtr;
//! obiekt ze statystykami do kana�u z danymi skalarnymi
typedef utils::ChannelStats<ScalarChannel::point_type, ScalarChannel::time_type> ScalarChannelStats;
//! wska�nik obiektu ze statystykami do kana�u z danymi skalarnymi
typedef core::shared_ptr<ScalarChannelStats> ScalarChannelStatsPtr;
//! niemodyfikowalny wska�nik obiektu ze statystykami do kana�u z danymi skalarnymi
typedef core::shared_ptr<const ScalarChannelStats> ScalarChannelStatsConstPtr;

//! Modyfikator, kt�ry normalizuje kana�
class ScalarChannelNormalizer
{
public:
    //! funktor odpowiada za modyfikacj� kopii kana�u 
    //! \param modifierInterface writer dla zmodyfikowanego kana�u
    //! \param observedChannel modyfikowany kana�a
    //! \param myChannel kana� wynikowy
    void operator()(ScalarChannelReaderInterface::_MyExtendedWriter & modifierInterface,
        const ScalarChannelReaderInterface::_MyRawChannelReaderType & observedChannel,
        const ScalarChannelReaderInterface::_MyRawChannelReaderType & myChannel)
    {
        //uzupe�nij brakuj�ce prboki
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
            //aktualizacja pr�bek
            for(ScalarChannelReaderInterface::size_type idx = 0; idx < myChannel.size(); ++idx){
                modifierInterface.setIndexData(idx, (observedChannel.value(idx) - minVal) / diff);
            }
        }
    }
};


//! Podstawa dla kana�u analogowego zapisanego w pliku c3d
class C3DAnalogChannel : public ScalarChannel
{
protected:
    //! konstruktor
    //! \param c3dData przeparsowane dane z pliku c3d
    //! \param channelNo numer kana�u w pliku c3d
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
    //! \param samplesPerSec liczba pr�bek na sekund�
    C3DAnalogChannel( int samplesPerSec ) :
    ScalarChannel(samplesPerSec)
    {}
    //! konstruktor kopiuj�cy
    //! \param channel kopiowany kana�
    C3DAnalogChannel( const C3DAnalogChannel& channel ) :
    ScalarChannel(channel)
    {}
};
typedef boost::shared_ptr<C3DAnalogChannel> C3DAnalogChannelPtr;

//! kana� EMG
class EMGChannel : public C3DAnalogChannel
{
public:
    //! konstruktor 
    //! \param samplesPerSec liczba pr�bek na sekund�
    explicit EMGChannel(int samplesPerSec) :
    C3DAnalogChannel(samplesPerSec)
    {}
    //! konstruktor kopiuj�cy
    //! \param channel kopiowany kana�
    EMGChannel(const EMGChannel& channel) :
    C3DAnalogChannel(channel)
    {}
    //! konstruktor
    //! \param c3dData przeparsowane dane z pliku c3d
    //! \param channelNo numer kana�u w pliku c3d
    EMGChannel(const c3dlib::C3DParser& data, int channelNo) :
    C3DAnalogChannel(data, channelNo)
    {}

public:
    //! \return zwraca kopi� obiektu, kt�r� trzeba samemu usun��
    virtual EMGChannel* clone() const
    {
        return new EMGChannel(*this);
    }
};
typedef boost::shared_ptr<EMGChannel> EMGChannelPtr;
typedef boost::shared_ptr<const EMGChannel> EMGChannelConstPtr;

//! kana� GRF
class GRFChannel : public VectorChannel
{
friend class C3DParser;
public:
    //! rodzaj kana�u GRF
	enum Type { Unknown, F1, M1, F2, M2 };

public:
    //! konstruktor 
    //! \param samplesPerSec liczba pr�bek na sekund�
    //! \param treshold pr�g dla kt�rego warto analizowa� dane z p�yty GRF
    explicit GRFChannel(int samplesPerSec, float treshold = 100.0f) :
        VectorChannel(samplesPerSec) ,
        type(Unknown),
        treshold(treshold)
    {}
    //! konstruktor kopiuj�cy
    //! \param channel kopiowany kana�
    GRFChannel(const GRFChannel& channel) :
    VectorChannel(channel),
	type(channel.type)
    {}
    //! konstruktor
    //! \param c3dData przeparsowane dane z pliku c3d
    //! \param channelNo numer kana�u w pliku c3d
    //! \param treshold pr�g dla kt�rego warto analizowa� dane z p�yty GRF
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
        //? w pliku c3d nie pojawia si� cos takiego jak jednostka czasu, dlatego mo�na przyj�� sekundy
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
    //! \return zwraca kopi� obiektu, kt�r� trzeba samemu usun��
    virtual GRFChannel* clone() const
    {
        return new GRFChannel(*this);
    }
    //! \return rodzaj kana�u GRF
	GRFChannel::Type getType() const { return type; }
	//! \return czy kana� ma wykryty peak, zwi�zanych z naciskiem na p�yt�
	bool hasStartEndData() const { return dataStart >= 0.0f && dataEnd >= 0.0f; }
	//! \return pocz�tek danych, zwi�zanych z naciskiem na p�yt�
	float getDataStartTime() const { UTILS_ASSERT(dataStart >= 0.0f); return dataStart; }
	//! \return koniec danych, zwi�zanych z naciskiem na p�y�
	float getDataEndTime() const { UTILS_ASSERT(dataEnd >= 0.0f); return dataEnd; }
	
private:
    Type type;
    //! pr�g dla kt�rego warto analizowa� dane z p�yty GRF
    float treshold;
    //! pocz�tek danych, zwi�zanych z naciskiem na p�yt�
	float dataStart;
    //! koniec danych, zwi�zanych z naciskiem na p�yt�
    float dataEnd;
};
typedef boost::shared_ptr<GRFChannel> GRFChannelPtr;
typedef boost::shared_ptr<const GRFChannel> GRFChannelConstPtr;

//! kana� zawiera dane o jednym markerze
class MarkerChannel : public VectorChannel
{
public:
    //! konstruktor 
    //! \param samplesPerSec liczba pr�bek na sekund�
    MarkerChannel(int samplesPerSec) : VectorChannel(samplesPerSec) {}

    MarkerChannel(const MarkerChannel& channel) :
        VectorChannel(channel)
        { }

    //! konstruktor
    //! \param c3dData przeparsowane dane z pliku c3d
    //! \param channelNo numer kana�u w pliku c3d
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
    //! \return zwraca kopi� obiektu, kt�r� trzeba samemu usun��
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
