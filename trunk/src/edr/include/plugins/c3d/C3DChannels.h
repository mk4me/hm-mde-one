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

typedef utils::BaseChannel<osg::Vec3f> VectorChannel;
typedef core::shared_ptr<VectorChannel> VectorChannelPtr;
typedef core::shared_ptr<const VectorChannel> VectorChannelConstPtr;

typedef utils::BaseChannel<float> ScalarChannel;
typedef core::shared_ptr<ScalarChannel> ScalarChannelPtr;
typedef core::shared_ptr<const ScalarChannel> ScalarChannelConstPtr;

typedef utils::ChannelStats<ScalarChannel::point_type, ScalarChannel::time_type> ScalarChannelStats;
typedef core::shared_ptr<ScalarChannelStats> ScalarChannelStatsPtr;
typedef core::shared_ptr<const ScalarChannelStats> ScalarChannelStatsConstPtr;

class ScalarChannelNormalizer
{
public:

    void operator()(utils::AutoModifier::ChannelModifierInterface<ScalarChannel> & modifierInterface,
        const ScalarChannel & myChannel,
        const ScalarChannel & observedChannel)
    {
        //uzupe³nij brakujace prboki
        if(myChannel.size() < observedChannel.size()){
            for(auto idx = myChannel.size(); idx < observedChannel.size(); idx++){
                modifierInterface.addPoint(observedChannel[idx].first, observedChannel[idx].second);
            }
        }

        auto it = observedChannel.begin();

        //min i max
        ScalarChannel::point_type minVal = it->second;
        ScalarChannel::point_type maxVal = it->second;

        it++;

        for( ; it != observedChannel.end(); it++){
            if(it->second < minVal){
                minVal = it->second;
            }else if(it->second > maxVal){
                maxVal = it->second;
            }
        }

        ScalarChannel::point_type diff = maxVal - minVal;

        if(diff != 0){
            //aktualizacja próbek
            for(ScalarChannel::size_type idx = 0; idx < myChannel.size(); idx++){
                modifierInterface.setIndexValue(idx, (observedChannel[idx].second - minVal) / diff);
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
public:
	enum Type { Unknown, F1, M1, F2, M2 };

public:
    explicit GRFChannel(int samplesPerSec) :
    VectorChannel(samplesPerSec) ,
	type(Unknown)
    {}
    GRFChannel(const GRFChannel& channel) :
    VectorChannel(channel),
	type(channel.type)
    {}
    GRFChannel(const c3dlib::C3DParser& data, int channelNo);

public:
    virtual GRFChannel* clone() const
    {
        return new GRFChannel(*this);
    }

	GRFChannel::Type getType() const { return type; }
	
private:
	Type type;
	
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
	virtual std::string getMarkerName(int markerNo) const {
		return this->getChannel(markerNo)->getName();
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

CORE_DEFINE_WRAPPER(VectorChannel, utils::PtrPolicyBoost, utils::ClonePolicyVirtualCloneMethod);
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
CORE_DEFINE_WRAPPER(ScalarChannel, utils::PtrPolicyBoost, utils::ClonePolicyVirtualCloneMethod);
CORE_DEFINE_WRAPPER_INHERITANCE(C3DAnalogChannel, ScalarChannel);
CORE_DEFINE_WRAPPER_INHERITANCE(EMGChannel, C3DAnalogChannel);
CORE_DEFINE_WRAPPER_INHERITANCE(GRFChannel, VectorChannel);
CORE_DEFINE_WRAPPER(GRFCollection, utils::PtrPolicyBoost, utils::ClonePolicyVirtualCloneMethod);
CORE_DEFINE_WRAPPER(EMGCollection, utils::PtrPolicyBoost, utils::ClonePolicyVirtualCloneMethod);
CORE_DEFINE_WRAPPER(C3DEventsCollection, utils::PtrPolicyBoost, utils::ClonePolicyCopyConstructor);
#endif  // __HEADER_GUARD_CORE__C3DCHANNELS_H__