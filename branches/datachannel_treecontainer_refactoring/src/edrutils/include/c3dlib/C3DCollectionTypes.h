/********************************************************************
	created:	2015/03/20
	created:	10:23
	filename: 	C3DCollectionTypes.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_C3DLIB__C3DCollectionTypes_H__
#define HEADER_GUARD_C3DLIB__C3DCollectionTypes_H__

#include <viconformatslib/VskParser.h>
#include <c3dlib/IForcePlatform.h>
#include <c3dlib/C3DTypes.h>
#include <c3dlib/Export.h>

namespace  c3dlib {

	//! Prosta kolekcja przechowuj�ca wszystkie zdarzenia z pliku c3d
	class C3DLIB_EXPORT C3DEventsCollection
	{
	public:
		//! wska�nik na zdarzenie wczytane z pliku C3D
		typedef utils::shared_ptr<c3dlib::C3DParser::IEvent> EventPtr;
		//! niemodyfikowalny wska�nik na zdarzenie wczytane z pliku C3D
		typedef utils::shared_ptr<const c3dlib::C3DParser::IEvent> EventConstPtr;
		//! kolekcja zdarze� wczytanych z pliku C3D
		typedef std::vector<EventPtr> Collection;
		//! iterator kolekcji zdarze� wczytanych z pliku C3D
		typedef Collection::iterator iterator;
		//! niemodyfikowalny iterator kolekcji zdarze� wczytanych z pliku C3D
		typedef Collection::const_iterator const_iterator;
		//! typ zdarzenia wczytanego z pliku C3D
		typedef c3dlib::C3DParser::IEvent IEvent;
		//! kontekst zdarzenia (strona lewa, prawa, ... )
		typedef IEvent::Context Context;

	private:
		//! kolekcja przechowuje zdarzenia wczytane z pliku c3d
		std::vector<EventPtr> events;
		// wyglada na to, ze VS2010 ma blad - wrzucenie nag��wka zawieraj�cego funkcje lambda
		// do nag��wk�w prekompilowanych skutkuje uniemo�liwieniem korzystania z lambdy wszedzie indziej
		// porownywanie event�w odbywa si� zatem w 'klasyczny' spos�b
		struct C3DLIB_EXPORT EventFunctor : public std::binary_function < EventPtr, EventPtr, bool >
		{
			bool operator()(const EventPtr& e1, const EventPtr& e2);
		};

	public:
		//! Konstruktor
		C3DEventsCollection();
		//! Konstruktor kopiuj�cy
		C3DEventsCollection(const C3DEventsCollection& es);

	public:
		//! \return liczba wczytanych zdarze�
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
		//! Zwraca event o okre�lonym kontekscie; nastepny do danego
		//! \param event konkretny event
		//! \param context konteks, kt�ry musi mie� zwracany event
		//! \return event, kt�ry spe�nia za�o�enia lub pusty wska�nik
		EventConstPtr getNextEvent(EventConstPtr event, Context context) const;
		//! Zwraca event dla podanego czasu, event musi mie� odpowiedni kontekst,
		//! czas rozpocz�cia mniejszy lub rowny t i nie mo�e by� ostatnim eventem w kolekcji
		//! \param t czas, dla kt�rego wyszukiwany jest event
		//! \param context kontekst, dla kt�rego wyszukiwany jest event (lewy, prawy.. )
		//! \return event, kt�ry spe�nia za�o�enia lub pusty wska�nik
		EventConstPtr getEvent(float t, Context context) const;
	};
	typedef utils::shared_ptr<C3DEventsCollection> EventsCollectionPtr;
	typedef utils::shared_ptr<const C3DEventsCollection> EventsCollectionConstPtr;

	//! Kolekcja obiekt�w EMG, obs�ugiwana w standardowy spos�b
	typedef datachannel::AccessorsCollection<EMGChannel> EMGCollection;
	typedef utils::shared_ptr<EMGCollection> EMGCollectionPtr;
	typedef utils::shared_ptr<const EMGCollection> EMGCollectionConstPtr;

	//! Kolekcja kana��w opartych o tr�jwymiarowy wektor
	typedef datachannel::AccessorsCollection<VectorChannelReaderInterface> VectorChannelCollection;
	typedef utils::shared_ptr<VectorChannelCollection > VectorChannelCollectionPtr;
	typedef utils::shared_ptr<const VectorChannelCollection > VectorChannelCollectionConstPtr;

	//! para liczb zmiennoprzecinkowych, s�u��ca do okre�lenia przedzia�u czasowego zdarzenia
	typedef utils::shared_ptr<std::pair<float, float>> FloatPairPtr;
	//! Metoda wydziela przedzia�y czasowe, dla kt�rych realizowana jest analiza
	//! \param events zdarzenia, z kt�rych beda wyci�gane przedzia�y
	//! \param context kontekst kroku (lewy, prawy)
	C3DLIB_EXPORT std::vector<FloatPairPtr> getTimeSegments(EventsCollectionConstPtr events, C3DEventsCollection::Context context);

	//! Kolekcja dostarcza r�wnie� informacji o p�ytach GFR oraz u�atwia pobranie konkretnego kana�u
	class C3DLIB_EXPORT GRFCollection : public datachannel::AccessorsCollection<GRFChannel>
	{
	public:
		//! \return wszystkie wczytane p�yty pomiarowe zwi�zane z danymi GRF
		const IForcePlatformCollection& getPlatforms() const;
		//! Ustawienie p�yt pomiarowych
		//! \param val
		void setPlatforms(const IForcePlatformCollection& val);
		//! Metoda u�atwia pobranie kana�u GRF o odpowiednim typie
		//! \param type typ kana�u (np. M1 lub F2)
		GRFChannelConstPtr getGRFChannel(GRFChannel::Type type) const;

	private:
		//! Platformy pomiarowe zwi�zane z t� sam� pr�b� pomiarow� co kana�y z kolekcji
		IForcePlatformCollection platforms;
	};
	typedef utils::shared_ptr<GRFCollection> GRFCollectionPtr;
	typedef utils::shared_ptr<const GRFCollection> GRFCollectionConstPtr;

	//! Kontener wszystkich marker�w modelu, u�atwia obs�ug� danych, dodaje wsparcie o pliki VSK
	class C3DLIB_EXPORT MarkerCollection : public datachannel::AccessorsCollection<MarkerChannel>
	{
	public:
		//! Konstruktor
		//! \param vsk parser ze wczytanym plikiem vsk (dzi�ki niemu tworz� si� kolorowe po��czenia midzy markerami)
		MarkerCollection(vicon::VskPtr vsk = vicon::VskPtr());
		//! Zwraca nazw� markera na podstawie indeksu
		//! \param markerNo poprawny indeks, w przeciwnym razie poleci asercja
		virtual std::string getMarkerName(int markerNo) const;
		//! pr�buje pobra� kana� na podstawie nazwy
		//! \param name nazwa kana�u, wielko�� liter ma znaczenie
		//! \return kana� lub null je�li takiego nie ma
		MarkerChannelConstPtr tryGetChannelByName(const std::string& name);
		//! \return parser vsk zwi�zany z kolekcj�
		vicon::VskConstPtr getVsk() const;

	private:
		//! parser vsk zwi�zany z kolekcj�
		vicon::VskPtr vsk;
	};
	typedef utils::shared_ptr<MarkerCollection> MarkerCollectionPtr;
	typedef utils::shared_ptr<const MarkerCollection> MarkerCollectionConstPtr;


	typedef std::vector<double> MovieDelays;
	DEFINE_SMART_POINTERS(MovieDelays);

	//! makro u�atwia definicje dodatkowych kana��w, osobne typy u�atwiaja otrzymanie konkretnych danych z DM
#define DEFINE_CHANNEL_COLLECTION(name)                                         \
class C3DLIB_EXPORT name##Collection : public VectorChannelCollection {};				        \
    typedef utils::shared_ptr<name##Collection> name##CollectionPtr;				\
    typedef utils::shared_ptr<const name##Collection> name##CollectionConstPtr;


	DEFINE_CHANNEL_COLLECTION(Force);
	DEFINE_CHANNEL_COLLECTION(Moment);
	DEFINE_CHANNEL_COLLECTION(Angle);
	DEFINE_CHANNEL_COLLECTION(Power);
}

#endif // HEADER_GUARD 
