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
    //! wskaŸnik na zdarzenie wczytane z pliku C3D
    typedef core::shared_ptr<c3dlib::C3DParser::IEvent> EventPtr;
    //! niemodyfikowalny wskaŸnik na zdarzenie wczytane z pliku C3D
    typedef core::shared_ptr<const c3dlib::C3DParser::IEvent> EventConstPtr;
    //! kolekcja zdarzeñ wczytanych z pliku C3D
    typedef std::vector<EventPtr> Collection;
    //! iterator kolekcji zdarzeñ wczytanych z pliku C3D
    typedef Collection::iterator iterator;
    //! niemodyfikowalny iterator kolekcji zdarzeñ wczytanych z pliku C3D
    typedef Collection::const_iterator const_iterator;
    //! typ zdarzenia wczytanego z pliku C3D
    typedef c3dlib::C3DParser::IEvent IEvent;
    //! kontekst zdarzenia (strona lewa, prawa, ... )
    typedef IEvent::Context Context;

private:
    //! kolekcja przechowuje zdarzenia wczytane z pliku c3d
    std::vector<EventPtr> events;
    // wyglada na to, ze VS2010 ma blad - wrzucenie nag³ówka zawieraj¹cego funkcje lambda
    // do nag³ówków prekompilowanych skutkuje uniemo¿liwieniem korzystania z lambdy wszedzie indziej
    // porownywanie eventów odbywa siê zatem w 'klasyczny' sposób
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
    //! Konstruktor kopiuj¹cy
    C3DEventsCollection(const C3DEventsCollection& es)
    {
        int count = static_cast<int>(es.events.size());
        events.resize(count);
        for (int i = 0; i < count; ++i) {
            events[i] = EventPtr(es.events[i]->clone());
        }
    }

public:
    //! \return liczba wczytanych zdarzeñ
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
        // wymuszenie kolejnoœci zwi¹zanej z czasem
        std::sort(events.begin(), events.end(), EventFunctor());
    }


    //! Zwraca event o okreœlonym kontekscie; nastepny do danego
    //! \param event konkretny event
    //! \param context konteks, który musi mieæ zwracany event
    //! \return event, który spe³nia za³o¿enia lub pusty wskaŸnik
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

    //! Zwraca event dla podanego czasu, event musi mieæ odpowiedni kontekst,
    //! czas rozpoczêcia mniejszy lub rowny t i nie mo¿e byæ ostatnim eventem w kolekcji
    //! \param t czas, dla którego wyszukiwany jest event
    //! \param context kontekst, dla którego wyszukiwany jest event (lewy, prawy.. )
    //! \return event, który spe³nia za³o¿enia lub pusty wskaŸnik
    EventConstPtr getEvent(float t, Context context) const
    {
        // przefiltrowanie eventów wzglêdem kontekstu
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

//! Kolekcja kana³ów opartych o trójwymiarowy wektor
typedef utils::DataChannelCollection<VectorChannel> VectorChannelCollection;
typedef core::shared_ptr<VectorChannelCollection > VectorChannelCollectionPtr;
typedef core::shared_ptr<const VectorChannelCollection > VectorChannelCollectionConstPtr;

//! para liczb zmiennoprzecinkowych, s³u¿¹ca do okreœlenia przedzia³u czasowego zdarzenia
typedef core::shared_ptr<std::pair<float, float>> FloatPairPtr;
//! Metoda wydziela przedzia³y czasowe, dla których realizowana jest analiza
//! \param events zdarzenia, z których beda wyci¹gane przedzia³y
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

//! Kolekcja dostarcza równie¿ informacji o p³ytach GFR oraz u³atwia pobranie konkretnego kana³u
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
    //! Platformy pomiarowe zwi¹zane z t¹ sam¹ prób¹ pomiarow¹ co kana³y z kolekcji
    IForcePlatformCollection platforms;
};
typedef boost::shared_ptr<GRFCollection> GRFCollectionPtr;
typedef boost::shared_ptr<const GRFCollection> GRFCollectionConstPtr;

//! Kontener wszystkich markerów modelu, u³atwia obs³ugê danych, dodaje wsparcie o pliki VSK
class MarkerCollection : public VectorChannelCollection
{
public:
    //! Konstruktor
    //! \param vsk parser ze wczytanym plikiem vsk (dziêki niemu tworz¹ siê kolorowe po³¹czenia midzy markerami)
    MarkerCollection(kinematic::VskParserPtr vsk = kinematic::VskParserPtr()) :
      VectorChannelCollection(),
      vsk(vsk)
      {}
    //! Zwraca nazwê markera na podstawie indeksu
    //! \param markerNo poprawny indeks, w przeciwnym razie poleci asercja
    virtual const std::string& getMarkerName(int markerNo) const {
        return this->getChannel(markerNo)->getName();
    }
    //! próbuje pobraæ kana³ na podstawie nazwy
    //! \param name nazwa kana³u, wielkoœæ liter ma znaczenie
    //! \return kana³ lub null jeœli takiego nie ma
    VectorChannelConstPtr tryGetChannelByName(const std::string& name) {
        for (int i = this->getNumChannels() - 1; i >= 0; --i) {
            if (getMarkerName(i) == name) {
                return this->getChannel(i);
            }
        }

        return VectorChannelConstPtr();
    }
    //! \return parser vsk zwi¹zany z kolekcj¹
    kinematic::VskParserConstPtr getVsk() const { return vsk; }

private:
    //! parser vsk zwi¹zany z kolekcj¹
    kinematic::VskParserPtr vsk;
};
typedef core::shared_ptr<MarkerCollection> MarkerCollectionPtr;
typedef core::shared_ptr<const MarkerCollection> MarkerCollectionConstPtr;

//! makro u³atwia definicje dodatkowych kana³ów, osobne typy u³atwiaja otrzymanie konkretnych danych z DM
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
