/********************************************************************
    created:	2012/03/25
    created:	25:3:2012   20:35
    filename: 	C3DCollections.h
    author:		Wojciech Kniec

    purpose:
*********************************************************************/

#ifndef HEADER_GUARD_C3DPLUGIN__C3DCOLLECTIONS_H__
#define HEADER_GUARD_C3DPLUGIN__C3DCOLLECTIONS_H__

#include <kinematiclib/VskParser.h>
#include <plugins/c3d/IForcePlatform.h>
#include <plugins/c3d/C3DChannels.h>

//! Prosta kolekcja przechowuj¹ca wszystkie zdarzenia z pliku c3d
class C3DEventsCollection
{
public:
    // pomocnicze typy
    typedef core::shared_ptr<c3dlib::C3DParser::IEvent> EventPtr;
    typedef core::shared_ptr<const c3dlib::C3DParser::IEvent> EventConstPtr;
    typedef std::vector<EventPtr> Collection;
    typedef Collection::iterator iterator;
    typedef Collection::const_iterator const_iterator;
    typedef c3dlib::C3DParser::IEvent IEvent;
    typedef IEvent::Context Context;

private:
    //! kolekcja przechowuje zdarzenia wczytane z pliku c3d
    std::vector<EventPtr> events;
    // wyglada na to, ze VS2010 ma blad - wrzucenie naglowka zawierajacego funkcje lambda
    // do naglowkow prekompilowanych skutkuje uniemo¿liwieniem korzystania z lambdy wszedzie indziej
    // porownywanie eventow odbywa siê zatem w 'klasyczny' sposób
    struct EventFunctor : public std::binary_function<EventPtr,EventPtr,bool>
    {
        inline bool operator()(const EventPtr& e1, const EventPtr& e2)
        {
            return e1->getTime() < e2->getTime();
        }
    };

public:
    //! Konstruktor
    C3DEventsCollection()
    {}
    //! Konstruktor kopiujacy
    C3DEventsCollection(const C3DEventsCollection& es)
    {
        int count = static_cast<int>(es.events.size());
        events.resize(count);
        for (int i = 0; i < count; ++i) {
            events[i] = EventPtr(es.events[i]->clone());
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
        // wymuszenie kolejnosci zwi¹zanej z czasem
        std::sort(events.begin(), events.end(), EventFunctor());
    }


    //! Zwraca event o okreslonym kontekscie; nastepny do danego
    //! \param event konkretny event
    //! \param context konteks, który musi miec zwracany event
    //! \return event, który spe³nia zalozenia lub pusty wskaŸnik
    EventConstPtr getNextEvent(EventConstPtr event, Context context) const
    {
        // szukanie elementu 'event' w kolekcji
        std::vector<EventPtr>::const_iterator it = events.cbegin();
        for (; it != events.cend(); ++it) {
            if (*it == event) {
                break;
            }
        }
        // jeœli znaleziono event;
        if (it != events.cend()) {
            for (it++; it != events.cend(); ++it) {
                if ((*it)->getContext() == context) {
                    return *it;
                }
            }
        }

        return EventConstPtr();
    }

    //! Zwraca event dla podanego czasu, event musi miec odpowiedni kontekst,
    //! czas rozpoczecia mniejszy lub rowny t i nie mo¿e byæ ostatnim eventem w kolekcji
    //! \param t czas, dla którego wyszukiwany jest event
    //! \param context kontekst, dla którego wyszukiwany jest event (lewy, prawy.. )
    //! \return event, który spe³nia zalozenia lub pusty wskaŸnik
    EventConstPtr getEvent(float t, Context context) const
    {
        // przefiltrowanie eventow wzgledem kontekstu
        std::vector<EventPtr> temp;
        for (auto it = events.cbegin(); it != events.cend(); ++it) {
            if ((*it)->getContext() == context) {
                temp.push_back(*it);
            }
        }

        EventConstPtr found;
        // poszukiwanie odpowiedniego eventu
        for (auto it = temp.begin(); it != temp.end(); ++it) {

            if ((*it)->getTime() <= t) {
                auto check = it;
                ++check;
                if (check != temp.end()) {
                    found = *it;
                } else {
                    // event znaleziony, ale jest ostatni
                    return EventConstPtr();
                }
            }

        }
        // nie znaleziono odpowiedniego eventu
        return found;
    }
};
typedef boost::shared_ptr<C3DEventsCollection> EventsCollectionPtr;
typedef boost::shared_ptr<const C3DEventsCollection> EventsCollectionConstPtr;

//! Kolekcja obiektów EMG, obs³ugiwana w standardowy sposób
typedef utils::DataChannelCollection<EMGChannel> EMGCollection;
typedef boost::shared_ptr<EMGCollection> EMGCollectionPtr;
typedef boost::shared_ptr<const EMGCollection> EMGCollectionConstPtr;

//! Kolekcja kana³ów opartych o trojwymiarowy wektor
typedef utils::DataChannelCollection<VectorChannel> VectorChannelCollection;
typedef core::shared_ptr<VectorChannelCollection > VectorChannelCollectionPtr;
typedef core::shared_ptr<const VectorChannelCollection > VectorChannelCollectionConstPtr;

typedef core::shared_ptr<std::pair<float, float>> FloatPairPtr;
//! Metoda wydziela przedzia³y czasowe, dla których realizowana jest analiza
//! \param events zdarzenia, z których beda wyciagnane przedzia³y
//! \param context kontekst kroku (lewy, prawy)
static std::vector<FloatPairPtr> getTimeSegments(EventsCollectionConstPtr events, C3DEventsCollection::Context context)
{
    std::vector<FloatPairPtr> ret;

    FloatPairPtr currentSegment;
    for( auto it = events->cbegin(); it != events->cend(); ++it) {
        C3DEventsCollection::EventConstPtr event = *it;
        if (event->getContext() == context) {
            if (event->getLabel() == "Foot Strike") {
                if (currentSegment) {
                    currentSegment->second = static_cast<float>(event->getTime());
                    ret.push_back(currentSegment);
                }

                currentSegment.reset(new std::pair<float, float>());
                currentSegment->first = static_cast<float>(event->getTime());
            } else if (currentSegment && event->getLabel() == "Foot Off") {
                //currentSegment->event2 = event;
            }
        }
    }

    return ret;
}

//! Kolekcja dostarcza rowniez informacji o p³ytach GFR oraz u³atwia pobranie konkretnego kana³u
class GRFCollection : public VectorChannelCollection
{
public:
    //! \return wszystkie wczytane p³yty pomiarowe zwi¹zane z danymi GRF
    const IForcePlatformCollection& getPlatforms() const { return platforms; }
    //! Ustawienie p³yt pomiarowych
    //! \param val
    void setPlatforms(const IForcePlatformCollection& val) { platforms = val; }
    //! Metoda u³atwia pobranie kana³u GRF o odpowiednim typie
    //! \param type typ kana³u (np. M1 lub F2)
    GRFChannelConstPtr getGRFChannel(GRFChannel::Type type) const
    {
        for (auto it = channels.begin(); it != channels.end(); ++it) {
            GRFChannelConstPtr channel = core::dynamic_pointer_cast<const GRFChannel>(*it);
            if (channel->getType() == type) {
                return channel;
            }
        }

        throw std::runtime_error("Wrong GRF channel type");
    }

private:
    //! Platformy pomiarowe zwi¹zane z ta sama próba pomiarowa co kana³y z kolekcji
    IForcePlatformCollection platforms;
};
typedef boost::shared_ptr<GRFCollection> GRFCollectionPtr;
typedef boost::shared_ptr<const GRFCollection> GRFCollectionConstPtr;

//! Kontener wszystkich markerów modelu, u³atwia obsluge danych, dodaje wsparcie o pliki VSK
class MarkerCollection : public VectorChannelCollection
{
public:
    //! Konstruktor
    //! \param vsk parser ze wczytanym plikiem vsk (dziêki niemu tworza siê kolorowe po³¹czenia midzy markerami)
    MarkerCollection(kinematic::VskParserPtr vsk = kinematic::VskParserPtr()) :
      VectorChannelCollection(),
      vsk(vsk)
      {}
    //! Zwrace nazwê markera na podstawie indeksu
    //! \param markerNo poprawny indeks, w przeciwnym razie poleci asercja
    virtual const std::string& getMarkerName(int markerNo) const {
        return this->getChannel(markerNo)->getName();
    }
    //! próbuje pobraæ kana³ na podstawie nazwy
    //! \param name nazwa kana³u, wilkosc liter ma znaczenie
    //! \return kana³ lub null jeœli takiego nie ma
    VectorChannelConstPtr tryGetChannelByName(const std::string& name) {
        for (int i = this->getNumChannels() - 1; i >= 0; --i) {
            if (getMarkerName(i) == name) {
                return this->getChannel(i);
            }
        }

        return VectorChannelConstPtr();
    }
    //! \return parser vsk zwiazany z kolekcja
    kinematic::VskParserConstPtr getVsk() const { return vsk; }

private:
    kinematic::VskParserPtr vsk;
};
typedef core::shared_ptr<MarkerCollection> MarkerCollectionPtr;
typedef core::shared_ptr<const MarkerCollection> MarkerCollectionConstPtr;

//! makro u³atwia definicje dodatkowych kana³ów, osobne typy ulatwiaja otrzymanie konkretnych danych z DM
#define DEFINE_CHANNEL_COLLECTION(name)                                         \
class name##Collection : public VectorChannelCollection {};				        \
    typedef core::shared_ptr<name##Collection> name##CollectionPtr;				\
    typedef core::shared_ptr<const name##Collection> name##CollectionConstPtr;


DEFINE_CHANNEL_COLLECTION(Force);
DEFINE_CHANNEL_COLLECTION(Moment);
DEFINE_CHANNEL_COLLECTION(Angle);
DEFINE_CHANNEL_COLLECTION(Power);


CORE_DEFINE_WRAPPER(EMGCollection, utils::PtrPolicyBoost, utils::ClonePolicyVirtualCloneMethod);
CORE_DEFINE_WRAPPER(C3DEventsCollection, utils::PtrPolicyBoost, utils::ClonePolicyCopyConstructor);
CORE_DEFINE_WRAPPER(VectorChannelCollection, utils::PtrPolicyBoost, utils::ClonePolicyVirtualCloneMethod);
CORE_DEFINE_WRAPPER_INHERITANCE(ForceCollection,  VectorChannelCollection);
CORE_DEFINE_WRAPPER_INHERITANCE(AngleCollection,  VectorChannelCollection);
CORE_DEFINE_WRAPPER_INHERITANCE(PowerCollection,  VectorChannelCollection);
CORE_DEFINE_WRAPPER_INHERITANCE(MomentCollection, VectorChannelCollection);
CORE_DEFINE_WRAPPER_INHERITANCE(MarkerCollection, VectorChannelCollection);
CORE_DEFINE_WRAPPER_INHERITANCE(GRFCollection, VectorChannelCollection);

#endif
