/********************************************************************
    created:	2012/03/25
    created:	25:3:2012   20:35
    filename: 	C3DCollections.h
    author:		Wojciech Kniec

    purpose:
*********************************************************************/

#ifndef HEADER_GUARD_C3DPLUGIN__C3DCOLLECTIONS_H__
#define HEADER_GUARD_C3DPLUGIN__C3DCOLLECTIONS_H__

#include <plugins/c3d/Export.h>
#include <kinematiclib/VskParser.h>
#include <plugins/c3d/IForcePlatform.h>
#include <plugins/c3d/C3DChannels.h>

//! Prosta kolekcja przechowująca wszystkie zdarzenia z pliku c3d
class PLUGINC3D_EXPORT C3DEventsCollection
{
public:
    //! wskaźnik na zdarzenie wczytane z pliku C3D
    typedef utils::shared_ptr<c3dlib::C3DParser::IEvent> EventPtr;
    //! niemodyfikowalny wskaźnik na zdarzenie wczytane z pliku C3D
    typedef utils::shared_ptr<const c3dlib::C3DParser::IEvent> EventConstPtr;
    //! kolekcja zdarzeń wczytanych z pliku C3D
    typedef std::vector<EventPtr> Collection;
    //! iterator kolekcji zdarzeń wczytanych z pliku C3D
    typedef Collection::iterator iterator;
    //! niemodyfikowalny iterator kolekcji zdarzeń wczytanych z pliku C3D
    typedef Collection::const_iterator const_iterator;
    //! typ zdarzenia wczytanego z pliku C3D
    typedef c3dlib::C3DParser::IEvent IEvent;
    //! kontekst zdarzenia (strona lewa, prawa, ... )
    typedef IEvent::Context Context;

private:
    //! kolekcja przechowuje zdarzenia wczytane z pliku c3d
    std::vector<EventPtr> events;
    // wyglada na to, ze VS2010 ma blad - wrzucenie nagłówka zawierającego funkcje lambda
    // do nagłówków prekompilowanych skutkuje uniemożliwieniem korzystania z lambdy wszedzie indziej
    // porownywanie eventów odbywa się zatem w 'klasyczny' sposób
    struct PLUGINC3D_EXPORT EventFunctor : public std::binary_function<EventPtr,EventPtr,bool>
    {
        bool operator()(const EventPtr& e1, const EventPtr& e2);
    };

public:
    //! Konstruktor
    C3DEventsCollection();
    //! Konstruktor kopiujący
    C3DEventsCollection(const C3DEventsCollection& es);

public:
    //! \return liczba wczytanych zdarzeń
    int getNumEvents() const;
    //! pobranie zdarzenia
    //! \param index indeks z zakresu <0 , getNumEvents)
    EventPtr getEvent(int index);
    //! pobranie zdarzenia (wersja niemodyfikowalna)
    //! \param index indeks z zakresu <0 , getNumEvents)
    EventConstPtr getEvent(int index) const;
    //! \return przepisany, stl-owy iterator
    iterator begin();
    //! \return przepisany, stl-owy iterator
    iterator end();
    //! \return przepisany, stl-owy iterator
    const_iterator cbegin() const;
    //! \return przepisany, stl-owy iterator
    const_iterator cend() const;
    //! dodanie zdarzenia do kolekcji
    //! \param event
    void addEvent(EventPtr event);
    //! Zwraca event o określonym kontekscie; nastepny do danego
    //! \param event konkretny event
    //! \param context konteks, który musi mieć zwracany event
    //! \return event, który spełnia założenia lub pusty wskaźnik
    EventConstPtr getNextEvent(EventConstPtr event, Context context) const;
    //! Zwraca event dla podanego czasu, event musi mieć odpowiedni kontekst,
    //! czas rozpoczęcia mniejszy lub rowny t i nie może być ostatnim eventem w kolekcji
    //! \param t czas, dla którego wyszukiwany jest event
    //! \param context kontekst, dla którego wyszukiwany jest event (lewy, prawy.. )
    //! \return event, który spełnia założenia lub pusty wskaźnik
    EventConstPtr getEvent(float t, Context context) const;
};
typedef boost::shared_ptr<C3DEventsCollection> EventsCollectionPtr;
typedef boost::shared_ptr<const C3DEventsCollection> EventsCollectionConstPtr;

//! Kolekcja obiektów EMG, obsługiwana w standardowy sposób
typedef utils::DataChannelCollection<EMGChannel> EMGCollection;
typedef boost::shared_ptr<EMGCollection> EMGCollectionPtr;
typedef boost::shared_ptr<const EMGCollection> EMGCollectionConstPtr;

//! Kolekcja kanałów opartych o trójwymiarowy wektor
typedef utils::DataChannelCollection<VectorChannel> VectorChannelCollection;
typedef utils::shared_ptr<VectorChannelCollection > VectorChannelCollectionPtr;
typedef utils::shared_ptr<const VectorChannelCollection > VectorChannelCollectionConstPtr;

//! para liczb zmiennoprzecinkowych, służąca do określenia przedziału czasowego zdarzenia
typedef utils::shared_ptr<std::pair<float, float>> FloatPairPtr;
//! Metoda wydziela przedziały czasowe, dla których realizowana jest analiza
//! \param events zdarzenia, z których beda wyciągane przedziały
//! \param context kontekst kroku (lewy, prawy)
PLUGINC3D_EXPORT std::vector<FloatPairPtr> getTimeSegments(EventsCollectionConstPtr events, C3DEventsCollection::Context context);

//! Kolekcja dostarcza również informacji o płytach GFR oraz ułatwia pobranie konkretnego kanału
class PLUGINC3D_EXPORT GRFCollection : public VectorChannelCollection
{
public:
    //! \return wszystkie wczytane płyty pomiarowe związane z danymi GRF
    const IForcePlatformCollection& getPlatforms() const;
    //! Ustawienie płyt pomiarowych
    //! \param val
    void setPlatforms(const IForcePlatformCollection& val);
    //! Metoda ułatwia pobranie kanału GRF o odpowiednim typie
    //! \param type typ kanału (np. M1 lub F2)
    GRFChannelConstPtr getGRFChannel(GRFChannel::Type type) const;

private:
    //! Platformy pomiarowe związane z tą samą próbą pomiarową co kanały z kolekcji
    IForcePlatformCollection platforms;
};
typedef boost::shared_ptr<GRFCollection> GRFCollectionPtr;
typedef boost::shared_ptr<const GRFCollection> GRFCollectionConstPtr;

//! Kontener wszystkich markerów modelu, ułatwia obsługę danych, dodaje wsparcie o pliki VSK
class PLUGINC3D_EXPORT MarkerCollection : public VectorChannelCollection
{
public:
    //! Konstruktor
    //! \param vsk parser ze wczytanym plikiem vsk (dzięki niemu tworzą się kolorowe połączenia midzy markerami)
    MarkerCollection(kinematic::VskParserPtr vsk = kinematic::VskParserPtr());
    //! Zwraca nazwę markera na podstawie indeksu
    //! \param markerNo poprawny indeks, w przeciwnym razie poleci asercja
    virtual const std::string& getMarkerName(int markerNo) const;
    //! próbuje pobrać kanał na podstawie nazwy
    //! \param name nazwa kanału, wielkość liter ma znaczenie
    //! \return kanał lub null jeśli takiego nie ma
    VectorChannelConstPtr tryGetChannelByName(const std::string& name);
    //! \return parser vsk związany z kolekcją
    kinematic::VskParserConstPtr getVsk() const;

private:
    //! parser vsk związany z kolekcją
    kinematic::VskParserPtr vsk;
};
typedef utils::shared_ptr<MarkerCollection> MarkerCollectionPtr;
typedef utils::shared_ptr<const MarkerCollection> MarkerCollectionConstPtr;

//! makro ułatwia definicje dodatkowych kanałów, osobne typy ułatwiaja otrzymanie konkretnych danych z DM
#define DEFINE_CHANNEL_COLLECTION(name)                                         \
class PLUGINC3D_EXPORT name##Collection : public VectorChannelCollection {};				        \
    typedef utils::shared_ptr<name##Collection> name##CollectionPtr;				\
    typedef utils::shared_ptr<const name##Collection> name##CollectionConstPtr;


DEFINE_CHANNEL_COLLECTION(Force);
DEFINE_CHANNEL_COLLECTION(Moment);
DEFINE_CHANNEL_COLLECTION(Angle);
DEFINE_CHANNEL_COLLECTION(Power);

DEFINE_WRAPPER(EMGCollection, utils::PtrPolicyBoost, utils::ClonePolicyCopyConstructor);
DEFINE_WRAPPER(C3DEventsCollection, utils::PtrPolicyBoost, utils::ClonePolicyCopyConstructor);
DEFINE_WRAPPER(VectorChannelCollection, utils::PtrPolicyBoost, utils::ClonePolicyCopyConstructor);
DEFINE_WRAPPER_INHERITANCE(ForceCollection,  VectorChannelCollection);
DEFINE_WRAPPER_INHERITANCE(AngleCollection,  VectorChannelCollection);
DEFINE_WRAPPER_INHERITANCE(PowerCollection,  VectorChannelCollection);
DEFINE_WRAPPER_INHERITANCE(MomentCollection, VectorChannelCollection);
DEFINE_WRAPPER_INHERITANCE(MarkerCollection, VectorChannelCollection);
DEFINE_WRAPPER_INHERITANCE(GRFCollection, VectorChannelCollection);

#endif
