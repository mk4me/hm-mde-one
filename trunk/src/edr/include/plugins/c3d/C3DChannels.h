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
#include <utils/DataChannel.h>
#include <utils/DataChannelModifiers.h>
#include <utils/DataChannelStats.h>
#include <utils/DataChannelCollection.h>
#include <utils/DataChannelAccessors.h>
#include <utils/DataChannelTimers.h>
#include <core/ObjectWrapper.h>
#include <osg/Vec3>
#include <c3dlib/C3DParser.h>

typedef utils::ITimerReader<float>::TimerReaderType TimerReader;
typedef utils::ITimerReader<float>::TimerReaderPtr TimerReaderPtr;
typedef utils::ITimerReader<float>::TimerReaderConstPtr TimerReaderConstPtr;

typedef utils::ITimer<float>::TimerType Timer;
typedef utils::ITimer<float>::TimerPtr TimerPtr;
typedef utils::ITimer<float>::TimerConstPtr TimerConstPtr;

typedef utils::Timer<float> GeneralTimer;

typedef utils::DataChannelTimeAccessor<float, float> ScalarContiniousTimeAccessor;
typedef utils::CurrentValueExtractor<float, float> ScalarCurentValueExtractor;
typedef utils::ChannelAutoModifier<float, float> ScalarModifier;

typedef utils::Channel<float> ScalarChannel;
typedef ScalarChannel::Interface ScalarChannelReaderInterface;

typedef core::shared_ptr<ScalarChannelReaderInterface> ScalarChannelReaderInterfacePtr;
typedef core::shared_ptr<const ScalarChannelReaderInterface> ScalarChannelReaderInterfaceConstPtr;

typedef utils::Channel<osg::Vec3f> VectorChannel;

typedef VectorChannel::Interface VectorChannelReaderInterface;

typedef utils::DataChannelTimeAccessor<osg::Vec3f, float> VectorContiniousTimeAccessor;
typedef utils::CurrentValueExtractor<osg::Vec3f, float> VectorCurentValueExtractor;
typedef utils::ChannelAutoModifier<osg::Vec3f, float> VectorModifier;

typedef core::shared_ptr<VectorChannel> VectorChannelPtr;
typedef core::shared_ptr<const VectorChannel> VectorChannelConstPtr;

typedef core::shared_ptr<VectorChannelReaderInterface> VectorChannelReaderInterfacePtr;
typedef core::shared_ptr<const VectorChannelReaderInterface> VectorChannelReaderInterfaceConstPtr;

class VectorToScalarAdaptor : public ScalarChannelReaderInterface, public utils::IChannelDescriptorWriter
{
protected:
    VectorToScalarAdaptor(const VectorToScalarAdaptor & adaptor) : descriptor(adaptor.descriptor), vector(adaptor.vector), index(adaptor.index), name(adaptor.name) {}

public:
    VectorToScalarAdaptor(const VectorChannelReaderInterfaceConstPtr & vector, size_type idx, const std::string & name = std::string())
        : descriptor(*vector), vector(vector), index(idx), name(name)
    {
        UTILS_ASSERT((idx >= 0 && idx < 3), "Bledny index dla adaptera skalarnego kanalu wektorowego");
    }
    virtual ~VectorToScalarAdaptor() {}

    virtual const std::string& getName() const
    {
        return name;
    }

	void setName(const std::string& name)
	{
		this->name = name;
	}

    virtual VectorToScalarAdaptor * clone() const
    {
        return new VectorToScalarAdaptor(*this);
    }

    //! \return Czas trwania kana³u
    virtual time_type getLength() const
    {
        return vector->getLength();
    }

    //! \param idx Indeks probki
    //! \return Wartosc czasu dla danego indeksu
    virtual time_type argument(size_type idx) const
    {
        return vector->argument(idx);
    }

    //! \param idx Indeks probki
    //! \return Wartosc probki dla danego indeksu
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

    virtual float getSamplesPerSecond() const
    {
        return vector->getSamplesPerSecond();
    }

    virtual float getSampleDuration() const
    {
        return vector->getSampleDuration();
    }

	//! \return
	virtual const std::string& getTimeBaseUnit() const
	{ 
		return descriptor.getTimeBaseUnit();
	}

	//! \return
	virtual float getTimeScaleFactor() const
	{ 
		return descriptor.getTimeScaleFactor();
	}

	//! \return
	virtual const std::string& getValueBaseUnit() const
	{ 
		return descriptor.getValueBaseUnit();
	}

	//! \return
	virtual float getValueScaleFactor() const
	{ 
		return descriptor.getValueScaleFactor();
	}

	//! \param timeBaseUnit
	virtual void setTimeBaseUnit(const std::string& timeBaseUnit) 
	{ 
		descriptor.setTimeBaseUnit(timeBaseUnit);
	}

	//! \param timeBaseUnit
	virtual void setTimeScaleFactor(float timeScaleFactor) 
	{ 
		descriptor.setTimeScaleFactor(timeScaleFactor);
	}

	//! \param yUnit
	virtual void setValueBaseUnit(const std::string& valueBaseUnit) 
	{ 
		descriptor.setValueBaseUnit(valueBaseUnit);
	}

	//! \param timeBaseUnit
	virtual void setValueScaleFactor(float valueScaleFactor) 
	{ 
		descriptor.setValueScaleFactor(valueScaleFactor);
	}

private:
    VectorChannelReaderInterfaceConstPtr vector;
    size_type index;
    std::string name;
	utils::ChannelDescriptor descriptor;
};

class ScalarWithTimeSegment : public ScalarChannelReaderInterface, public utils::IChannelDescriptorWriter
{
protected:
    ScalarWithTimeSegment(const ScalarWithTimeSegment & adaptor) : descriptor(adaptor.descriptor), startIndex(adaptor.startIndex), endIndex(adaptor.endIndex) {}

public:
    ScalarWithTimeSegment(const ScalarChannelReaderInterfaceConstPtr & channel, time_type start, time_type end)
        : descriptor(*channel),	reader(channel)
    {
        startIndex = channel->getValueHelper(start).first;
        endIndex = channel->getValueHelper(end).second;
    }
    virtual ~ScalarWithTimeSegment() {}

    virtual const std::string& getName() const
    {
        return reader->getName();
    }

    virtual ScalarWithTimeSegment * clone() const
    {
        return new ScalarWithTimeSegment(*this);
    }

    //! \return Czas trwania kana³u
    virtual time_type getLength() const
    {
        return static_cast<time_type>(100);
    }

    size_type transformIndex(size_type idx) const
    {
        return startIndex + idx;
        float x = static_cast<float>(idx) / size();
        return static_cast<size_type>(startIndex * (1.0f - x) + endIndex * x);
    }

    //! \param idx Indeks probki
    //! \return Wartosc czasu dla danego indeksu
    virtual time_type argument(size_type idx) const
    {
        return static_cast<time_type>(idx * 100) / size();
    }

    //! \param idx Indeks probki
    //! \return Wartosc probki dla danego indeksu
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

    virtual float getSamplesPerSecond() const
    {
        return size() / getLength();
    }

    virtual float getSampleDuration() const
    {
        if (size()) {
          return 1.0 / getSamplesPerSecond();
        }

        throw std::runtime_error("empty channel");
    }

	//! \return
	virtual const std::string& getTimeBaseUnit() const
	{ 
		return descriptor.getTimeBaseUnit();
	}

	//! \return
	virtual float getTimeScaleFactor() const
	{ 
		return descriptor.getTimeScaleFactor();
	}

	//! \return
	virtual const std::string& getValueBaseUnit() const
	{ 
		return descriptor.getValueBaseUnit();
	}

	//! \return
	virtual float getValueScaleFactor() const
	{ 
		return descriptor.getValueScaleFactor();
	}

	//! \param timeBaseUnit
	virtual void setTimeBaseUnit(const std::string& timeBaseUnit) 
	{ 
		descriptor.setTimeBaseUnit(timeBaseUnit);
	}

	//! \param timeBaseUnit
	virtual void setTimeScaleFactor(float timeScaleFactor) 
	{ 
		descriptor.setTimeScaleFactor(timeScaleFactor);
	}

	//! \param yUnit
	virtual void setValueBaseUnit(const std::string& valueBaseUnit) 
	{ 
		descriptor.setValueBaseUnit(valueBaseUnit);
	}

	//! \param timeBaseUnit
	virtual void setValueScaleFactor(float valueScaleFactor) 
	{ 
		descriptor.setValueScaleFactor(valueScaleFactor);
	}

private:
    ScalarChannelReaderInterfaceConstPtr reader;
    int startIndex;
    int endIndex;
	utils::ChannelDescriptor descriptor;
};

typedef core::shared_ptr<ScalarChannel> ScalarChannelPtr;
typedef core::shared_ptr<const ScalarChannel> ScalarChannelConstPtr;

typedef utils::ChannelStats<ScalarChannel::point_type, ScalarChannel::time_type> ScalarChannelStats;

typedef core::shared_ptr<ScalarChannelStats> ScalarChannelStatsPtr;
typedef core::shared_ptr<const ScalarChannelStats> ScalarChannelStatsConstPtr;

class ScalarChannelNormalizer
{
public:

    void operator()(ScalarChannelReaderInterface::_MyExtendedWriter & modifierInterface,
        const ScalarChannelReaderInterface::_MyRawChannelReaderType & observedChannel,
        const ScalarChannelReaderInterface::_MyRawChannelReaderType & myChannel)
    {
        //uzupe³nij brakujace prboki
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


//! Podstawa dla kanalu analogowego zapisanego w pliku c3d
class C3DAnalogChannel : public ScalarChannel
{
protected:
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

    C3DAnalogChannel( int samplesPerSec ) :
    ScalarChannel(samplesPerSec)
    {}

    C3DAnalogChannel( const C3DAnalogChannel& channel ) :
    ScalarChannel(channel)
    {}
};
typedef boost::shared_ptr<C3DAnalogChannel> C3DAnalogChannelPtr;

//! Kanal EMG
class EMGChannel : public C3DAnalogChannel
{
public:
    explicit EMGChannel(int samplesPerSec) :
    C3DAnalogChannel(samplesPerSec)
    {}
    EMGChannel(const EMGChannel& channel) :
    C3DAnalogChannel(channel)
    {}
    EMGChannel(const c3dlib::C3DParser& data, int channelNo) :
    C3DAnalogChannel(data, channelNo)
    {}
        
public:
    virtual EMGChannel* clone() const
    {
        return new EMGChannel(*this);
    }
};
typedef boost::shared_ptr<EMGChannel> EMGChannelPtr;
typedef boost::shared_ptr<const EMGChannel> EMGChannelConstPtr;




//! Kanal GRF
class GRFChannel : public VectorChannel
{
friend class C3DParser;
public:
	enum Type { Unknown, F1, M1, F2, M2 };

public:
    explicit GRFChannel(int samplesPerSec, float treshold = 100.0f) :
    VectorChannel(samplesPerSec) ,
	type(Unknown),
	treshold(treshold)
    {}

    GRFChannel(const GRFChannel& channel) :
    VectorChannel(channel),
	type(channel.type)
    {}

    GRFChannel( const c3dlib::C3DParser& data, int channelNo, float treshold = 200.0f) :
    VectorChannel(static_cast<int>(data.getNumberAnalogSamplePerFrame() * data.getPointFrequency())),
        type(Unknown),
        dataStart(-1.0f),
        dataEnd(-1.0f),
        treshold(treshold)
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
        //? w pliku c3d nie pojawia sie cos takiego jak jednostka czasu, dlatego mozna przyjac sekundy
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
    virtual GRFChannel* clone() const
    {
        return new GRFChannel(*this);
    }

	GRFChannel::Type getType() const { return type; }

	bool hasStartEndData() const { return dataStart >= 0.0f && dataEnd >= 0.0f; }
	float getDataStartTime() const { UTILS_ASSERT(dataStart >= 0.0f); return dataStart; }
	float getDataEndTime() const { UTILS_ASSERT(dataEnd >= 0.0f); return dataEnd; }
	
	osg::Vec3 getEndPoint() const { UTILS_ASSERT(dataStart >= 0.0f); return endPoint; }
	osg::Vec3 getStartPoint() const { UTILS_ASSERT(dataEnd >= 0.0f); return startPoint; }
	
private:
	Type type;
	float treshold;
	float dataStart, dataEnd;
	osg::Vec3 startPoint, endPoint;
	
};
typedef boost::shared_ptr<GRFChannel> GRFChannelPtr;
typedef boost::shared_ptr<const GRFChannel> GRFChannelConstPtr;

//! Kanal zawiera dane o jednym markerze
class MarkerChannel : public VectorChannel
{
public:  
    MarkerChannel(int samplesPerSec) : VectorChannel(samplesPerSec) {}
        
    MarkerChannel(const MarkerChannel& channel) :
        VectorChannel(channel)
        { }

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
	typedef core::shared_ptr<const name##Channel> name##ChannelConstPtr;						 \
	

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