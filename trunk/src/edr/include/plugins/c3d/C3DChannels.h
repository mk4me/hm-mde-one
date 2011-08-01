/********************************************************************
	created:  2011/02/17
	created:  17:2:2011   14:06
	filename: C3DChannels.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_C3D__C3DCHANNELS_H__
#define __HEADER_GUARD_C3D__C3DCHANNELS_H__
#include <boost/smart_ptr.hpp>
#include <core/SmartPtr.h>
#include <utils/DataChannel.h>
#include <utils/DataChannelCollection.h>
#include <core/ObjectWrapper.h>
#include <osg/Vec3>
#include <osg/Math>
#include <c3dlib/C3DParser.h>
#include <kinematiclib/JointAnglesCollection.h>

typedef utils::DataChannel<osg::Vec3, float> VectorChannel;
typedef boost::shared_ptr<VectorChannel> VectorChannelPtr;

typedef utils::DataChannel<float, float> ScalarChannel;
typedef boost::shared_ptr<ScalarChannel> ScalarChannelPtr;
typedef boost::shared_ptr<const ScalarChannel> ScalarChannelConstPtr;


//! Prosta kolekcja przechowujaca wszystkie zdarzenia z pliku c3d
class EventsCollection
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
	EventsCollection()
	{}
	//! Konstruktor kopiujacy
	//! 
	EventsCollection(const EventsCollection& es)
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
typedef boost::shared_ptr<EventsCollection> EventsCollectionPtr;
typedef boost::shared_ptr<const EventsCollection> EventsCollectionConstPtr;

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
    virtual ScalarChannel* clone() const
    {
        return new EMGChannel(*this);
    }
};
typedef boost::shared_ptr<EMGChannel> EMGChannelPtr;
typedef boost::shared_ptr<const EMGChannel> EMGChannelConstPtr;

class EMGCollection : public utils::DataChannelCollection<float, float>
{

};
typedef boost::shared_ptr<EMGCollection> EMGCollectionPtr;
typedef boost::shared_ptr<const EMGCollection> EMGCollectionConstPtr;


//! Kanal GRF
class GRFChannel : public C3DAnalogChannel
{
public:
    explicit GRFChannel(int samplesPerSec) :
    C3DAnalogChannel(samplesPerSec)
    {}
    GRFChannel(const GRFChannel& channel) :
    C3DAnalogChannel(channel)
    {}
    GRFChannel(const c3dlib::C3DParser& data, int channelNo) :
    C3DAnalogChannel(data, channelNo)
    {}

public:
    virtual ScalarChannel* clone() const
    {
        return new GRFChannel(*this);
    }
};
typedef boost::shared_ptr<GRFChannel> GRFChannelPtr;
typedef boost::shared_ptr<const GRFChannel> GRFChannelConstPtr;

class GRFCollection : public utils::DataChannelCollection<float, float>
{

};
typedef boost::shared_ptr<GRFCollection> GRFCollectionPtr;
typedef boost::shared_ptr<const GRFCollection> GRFCollectionConstPtr;

//! Kanal zawiera dane o jednym markerze
class MarkerChannel : public utils::DataChannel<osg::Vec3f, float>
{
private:
    MarkerChannel(int samplesPerSec) : DataChannel(samplesPerSec) {}
        
public:        
    MarkerChannel(const MarkerChannel& channel) :
        DataChannel(channel)
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
class MarkerCollection : public utils::DataChannelCollection<osg::Vec3f, float>
{
public:
	virtual std::string getMarkerName(int markerNo) const {
		return this->getChannel(markerNo)->getName();
	}
};
typedef core::shared_ptr<MarkerCollection> MarkerCollectionPtr;
typedef core::shared_ptr<const MarkerCollection> MarkerCollectionConstPtr;



CORE_DEFINE_WRAPPER(MarkerChannel, utils::PtrPolicyBoost, utils::ClonePolicyVirtualCloneMethod);
CORE_DEFINE_WRAPPER(MarkerCollection, utils::PtrPolicyBoost, utils::ClonePolicyCopyConstructor);
CORE_DEFINE_WRAPPER(ScalarChannel, utils::PtrPolicyBoost, utils::ClonePolicyVirtualCloneMethod);
CORE_DEFINE_WRAPPER_INHERITANCE(C3DAnalogChannel, ScalarChannel);
CORE_DEFINE_WRAPPER_INHERITANCE(EMGChannel, C3DAnalogChannel);
CORE_DEFINE_WRAPPER_INHERITANCE(GRFChannel, C3DAnalogChannel);
CORE_DEFINE_WRAPPER(GRFCollection, utils::PtrPolicyBoost, utils::ClonePolicyVirtualCloneMethod);
CORE_DEFINE_WRAPPER(EMGCollection, utils::PtrPolicyBoost, utils::ClonePolicyVirtualCloneMethod);
CORE_DEFINE_WRAPPER(EventsCollection, utils::PtrPolicyBoost, utils::ClonePolicyCopyConstructor);
#endif  // __HEADER_GUARD_CORE__C3DCHANNELS_H__