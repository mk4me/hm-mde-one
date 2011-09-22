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
#include <utils/DataChannelCollection.h>
#include <core/ObjectWrapper.h>
#include <osg/Vec3>
#include <osg/Math>
#include <c3dlib/C3DParser.h>
#include <kinematiclib/JointAnglesCollection.h>

typedef utils::ITimerReader<float>::TimerReaderType TimerReader;
typedef utils::ITimerReader<float>::TimerReaderPtr TimerReaderPtr;
typedef utils::ITimerReader<float>::TimerReaderConstPtr TimerReaderConstPtr;

typedef utils::ITimer<float>::TimerType Timer;
typedef utils::ITimer<float>::TimerPtr TimerPtr;
typedef utils::ITimer<float>::TimerConstPtr TimerConstPtr;

typedef utils::Timer<float> GeneralTimer;

typedef utils::TimerOnTimer<float> TimerOnTimer;


typedef utils::GeneralDataChannelTimeAccessor<float, float> ScalarContiniousTimeAccessor;
typedef utils::CurrentValueExtractor<float, float> ScalarCurentValueExtractor;
typedef utils::ChannelAutoModifier<float, float> ScalarModifier;

typedef utils::Channel<float> ScalarChannel;
typedef ScalarChannel::Interface ScalarChannelReaderInterface;

typedef core::shared_ptr<ScalarChannelReaderInterface> ScalarChannelReaderInterfacePtr;
typedef core::shared_ptr<const ScalarChannelReaderInterface> ScalarChannelReaderInterfaceConstPtr;

typedef utils::Channel<osg::Vec3f> VectorChannel;

typedef VectorChannel::Interface VectorChannelReaderInterface;

typedef utils::GeneralDataChannelTimeAccessor<osg::Vec3f, float> VectorContiniousTimeAccessor;
typedef utils::CurrentValueExtractor<osg::Vec3f, float> VectorCurentValueExtractor;
typedef utils::ChannelAutoModifier<osg::Vec3f, float> VectorModifier;

typedef core::shared_ptr<VectorChannel> VectorChannelPtr;
typedef core::shared_ptr<const VectorChannel> VectorChannelConstPtr;

typedef core::shared_ptr<VectorChannelReaderInterface> VectorChannelReaderInterfacePtr;
typedef core::shared_ptr<const VectorChannelReaderInterface> VectorChannelReaderInterfaceConstPtr;

class VectorToScalarAdaptor : public ScalarChannelReaderInterface, public utils::ChannelDescriptor
{
protected:
    VectorToScalarAdaptor(const VectorToScalarAdaptor & adaptor) : utils::ChannelDescriptor(adaptor), vector(adaptor.vector), index(adaptor.index), name(adaptor.name) {}

public:
    VectorToScalarAdaptor(const VectorChannelReaderInterfaceConstPtr & vector, size_type idx, const std::string & name = std::string()) : utils::ChannelDescriptor(*vector), vector(vector), index(idx), name(name)
    {
        UTILS_ASSERT((idx >= 0 && idx < 3), "Bledny index dla adaptera skalarnego kanalu wektorowego");
    }
    virtual ~VectorToScalarAdaptor() {}

    virtual const std::string& getName() const
    {
        return name;
    }

    virtual VectorToScalarAdaptor * clone() const
    {
        return new VectorToScalarAdaptor(*this);
    }

    //! \return Czas trwania kana�u
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

    virtual float getSamplesPerSecond() const
    {
        return vector->getSamplesPerSecond();
    }

    virtual float getSampleDuration() const
    {
        return vector->getSampleDuration();
    }

private:
    VectorChannelReaderInterfaceConstPtr vector;
    size_type index;
    std::string name;
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
        //uzupe�nij brakujace prboki
        if(myChannel.size() < observedChannel.size()){
            for(auto idx = myChannel.size(); idx < observedChannel.size(); idx++){
                modifierInterface.addPoint(observedChannel.argument(idx), observedChannel.value(idx));
            }
        }

        auto i = 0;

        //min i max
        auto minVal = observedChannel.value(0);
        auto maxVal = minVal;

        i++;

        for( ; i != observedChannel.size(); i++){
            if(observedChannel.value(i) < minVal){
                minVal = observedChannel.value(i);
            }else if(observedChannel.value(i) > maxVal){
                maxVal = observedChannel.value(i);
            }
        }

        ScalarChannelReaderInterface::point_type diff = maxVal - minVal;

        if(diff != 0){
            //aktualizacja pr�bek
            for(ScalarChannelReaderInterface::size_type idx = 0; idx < myChannel.size(); idx++){
                modifierInterface.setIndexData(idx, (observedChannel.value(idx) - minVal) / diff);
            }
        }
    }
};


//! Prosta kolekcja przechowujaca wszystkie zdarzenia z pliku c3d
class C3DEventsCollection
{
public: 
	// pomocnicze typy
	typedef c3dlib::C3DParser::IEventPtr EventPtr;
	typedef c3dlib::C3DParser::IEventConstPtr EventConstPtr;
	typedef std::vector<EventPtr> Collection;
	typedef Collection::iterator iterator;
	typedef Collection::const_iterator const_iterator;

private:
	//! kolekcja przechowuje zdarzenia wczytane z pliku c3d
	std::vector<EventPtr> events;

public:
	//! Konstruktor
	C3DEventsCollection()
	{}
	//! Konstruktor kopiujacy
	//! 
	C3DEventsCollection(const C3DEventsCollection& es)
	{
		int count = static_cast<int>(es.events.size());
		events.resize(count);
		for (int i = 0; i < count; i++) {
			events[i] = es.events[i]->clone();
		}
	}

public:
	//! \return liczba wczytanych zdarzen
	int getNumEvents() const { return events.size(); }
	//! pobranie zdarzenia
	//! \param index indeks z zakresu <0 , getNumEvents)
	EventPtr getEvent(int index) 
	{
		UTILS_ASSERT(index >= 0 && index < getNumEvents());
		return events[index];
	}
	//! pobranie zdarzenia (wersja niemodyfikowalna)
	//! \param index indeks z zakresu <0 , getNumEvents)
	EventConstPtr getEvent(int index) const
	{
		UTILS_ASSERT(index >= 0 && index < getNumEvents());
		return events[index];
	}
	//! \return przepisany, stl-owy iterator
	iterator begin() { return events.begin(); }
	//! \return przepisany, stl-owy iterator
	iterator end() { return events.end(); }
	//! \return przepisany, stl-owy iterator
	const_iterator cbegin() const { return events.cbegin(); }
	//! \return przepisany, stl-owy iterator
	const_iterator cend() const { return events.cend(); }
	//! dodanie zdarzenia do kolekcji
	//! \param event 
	void addEvent(EventPtr event) 
	{
		events.push_back(event);
		std::sort(events.begin(), events.end(), 
		[](const EventPtr& e1, const EventPtr& e2) -> bool
		{
			return e1->getTime() < e2->getTime();
		});
	}
};
typedef boost::shared_ptr<C3DEventsCollection> EventsCollectionPtr;
typedef boost::shared_ptr<const C3DEventsCollection> EventsCollectionConstPtr;

//! Podstawa dla kanalu analogowego zapisanego w pliku c3d
class C3DAnalogChannel : public ScalarChannel
{
protected:
    C3DAnalogChannel(int samplesPerSec);
    C3DAnalogChannel(const C3DAnalogChannel& channel);
    C3DAnalogChannel(const c3dlib::C3DParser& data, int channelNo);
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

typedef utils::DataChannelCollection<EMGChannel> EMGCollection;
typedef boost::shared_ptr<EMGCollection> EMGCollectionPtr;
typedef boost::shared_ptr<const EMGCollection> EMGCollectionConstPtr;


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
    GRFChannel(const c3dlib::C3DParser& data, int channelNo, float treshold = 200.0f);

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

class GRFCollection : public utils::DataChannelCollection<GRFChannel> 
{
public:
	c3dlib::ForcePlatformCollection getPlatforms() const { return platforms; }
	void setPlatforms(c3dlib::ForcePlatformCollection& val) { platforms = val; }

	GRFChannelConstPtr getGRFChannel(GRFChannel::Type type) const
	{
		for (auto it = channels.begin(); it != channels.end(); it++) {
			if ((*it)->getType() == type) {
				return *it;
			}
		}

		throw std::runtime_error("Wrong GRF channel type");
	}

private:
	c3dlib::ForcePlatformCollection platforms;
};
typedef boost::shared_ptr<GRFCollection> GRFCollectionPtr;
typedef boost::shared_ptr<const GRFCollection> GRFCollectionConstPtr;

//! Kanal zawiera dane o jednym markerze
class MarkerChannel : public VectorChannel
{
private:
    MarkerChannel(int samplesPerSec) : VectorChannel(samplesPerSec) {}
        
public:        
    MarkerChannel(const MarkerChannel& channel) :
        VectorChannel(channel)
        { }
    MarkerChannel(const c3dlib::C3DParser& data, int channelNo);
public:
    virtual MarkerChannel* clone() const
    {
        return new MarkerChannel(*this);
    }
};
typedef boost::shared_ptr<MarkerChannel> MarkerChannelPtr;
typedef boost::shared_ptr<const MarkerChannel> MarkerChannelConstPtr;

//! Kontener wszystkich markerow modelu
class MarkerCollection : public utils::DataChannelCollection<MarkerChannel>
{
public:
	virtual const std::string& getMarkerName(int markerNo) const {
		return this->getChannel(markerNo)->getName();
	}

	MarkerChannelConstPtr tryGetChannelByName(const std::string& name) {
		for (int i = this->getNumChannels() - 1; i >= 0; --i) {
			if (getMarkerName(i) == name) {
				return this->getChannel(i);
			}
		}

		MarkerChannelConstPtr empty;
		return empty;
	}
};
typedef core::shared_ptr<MarkerCollection> MarkerCollectionPtr;
typedef core::shared_ptr<const MarkerCollection> MarkerCollectionConstPtr;


//class C3DMisc
//{
//public:
//	c3dlib::ForcePlatformCollection getPlatforms() const { return platforms; }
//	void setPlatforms(c3dlib::ForcePlatformCollection& val) { platforms = val; }
//	
//private:
//	c3dlib::ForcePlatformCollection platforms;
//};
//typedef boost::shared_ptr<C3DMisc> C3DMiscPtr;
//typedef boost::shared_ptr<const C3DMisc> C3DMiscConstPtr;


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
			  for (int i = 0; i < numSamples; i++) {											 \
				  addPoint(point->getValue(i));													 \
			  }																					 \
			  setName(point->getLabel());														 \
		  }																						 \
																								 \
	public:																						 \
		virtual name##Channel* clone() const {  return new name##Channel(*this); }				 \
	};												 											 \
	class name##Collection : public utils::DataChannelCollection<name##Channel> {};				 \
	typedef core::shared_ptr<name##Channel> name##ChannelPtr;									 \
	typedef core::shared_ptr<const name##Channel> name##ChannelConstPtr;						 \
	typedef core::shared_ptr<name##Collection> name##CollectionPtr;								 \
	typedef core::shared_ptr<const name##Collection> name##CollectionConstPtr;	  

DEFINE_CHANNEL(Force);
DEFINE_CHANNEL(Moment);
DEFINE_CHANNEL(Angle);
DEFINE_CHANNEL(Power);

CORE_DEFINE_WRAPPER(VectorChannelReaderInterface, utils::PtrPolicyBoost, utils::ClonePolicyVirtualCloneMethod);
//CORE_DEFINE_WRAPPER(VectorChannel, utils::PtrPolicyBoost, utils::ClonePolicyVirtualCloneMethod);
CORE_DEFINE_WRAPPER_INHERITANCE(VectorChannel, VectorChannelReaderInterface);
CORE_DEFINE_WRAPPER_INHERITANCE(MarkerChannel,VectorChannel);
CORE_DEFINE_WRAPPER_INHERITANCE(ForceChannel, VectorChannel);
CORE_DEFINE_WRAPPER_INHERITANCE(MomentChannel,VectorChannel);
CORE_DEFINE_WRAPPER_INHERITANCE(AngleChannel, VectorChannel);
CORE_DEFINE_WRAPPER_INHERITANCE(PowerChannel, VectorChannel);
CORE_DEFINE_WRAPPER(ForceCollection, utils::PtrPolicyBoost, utils::ClonePolicyVirtualCloneMethod);
CORE_DEFINE_WRAPPER(AngleCollection, utils::PtrPolicyBoost, utils::ClonePolicyVirtualCloneMethod);
CORE_DEFINE_WRAPPER(PowerCollection, utils::PtrPolicyBoost, utils::ClonePolicyVirtualCloneMethod);
CORE_DEFINE_WRAPPER(MomentCollection, utils::PtrPolicyBoost, utils::ClonePolicyVirtualCloneMethod);
//CORE_DEFINE_WRAPPER(C3DMisc, utils::PtrPolicyBoost, utils::ClonePolicyCopyConstructor);

CORE_DEFINE_WRAPPER(MarkerCollection, utils::PtrPolicyBoost, utils::ClonePolicyCopyConstructor);
CORE_DEFINE_WRAPPER(ScalarChannelReaderInterface, utils::PtrPolicyBoost, utils::ClonePolicyVirtualCloneMethod);
//CORE_DEFINE_WRAPPER(ScalarChannel, utils::PtrPolicyBoost, utils::ClonePolicyVirtualCloneMethod);
CORE_DEFINE_WRAPPER_INHERITANCE(ScalarChannel, ScalarChannelReaderInterface);
CORE_DEFINE_WRAPPER_INHERITANCE(VectorToScalarAdaptor, ScalarChannelReaderInterface);
CORE_DEFINE_WRAPPER_INHERITANCE(C3DAnalogChannel, ScalarChannel);
CORE_DEFINE_WRAPPER_INHERITANCE(EMGChannel, C3DAnalogChannel);
CORE_DEFINE_WRAPPER_INHERITANCE(GRFChannel, VectorChannel);
CORE_DEFINE_WRAPPER(GRFCollection, utils::PtrPolicyBoost, utils::ClonePolicyVirtualCloneMethod);
CORE_DEFINE_WRAPPER(EMGCollection, utils::PtrPolicyBoost, utils::ClonePolicyVirtualCloneMethod);
CORE_DEFINE_WRAPPER(C3DEventsCollection, utils::PtrPolicyBoost, utils::ClonePolicyCopyConstructor);
#endif  // __HEADER_GUARD_CORE__C3DCHANNELS_H__