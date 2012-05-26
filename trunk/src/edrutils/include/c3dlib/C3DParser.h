/********************************************************************
	created:	2011/05/30
	created:	30:5:2011   8:59
	filename: 	C3DParser.h
	author:		Wojciech Knie�
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_C3DLIB__C3DPARSER_H__
#define HEADER_GUARD_C3DLIB__C3DPARSER_H__

#include <string>
#include <vector>
#include <map>
#include <set>
#include <boost/noncopyable.hpp>
#include <osg/Vec3>
#include <c3dlib/Export.h>

namespace c3dlib {


//! Opis platformy GRF
struct C3DLIB_EXPORT ForcePlatform
{
	//! wierzcholki pojedynczej plyty
	osg::Vec3 corners[4];
	//! przesuniecie plyty w ukladzie odniesienia zgodnym z markerami
	osg::Vec3 origin;
    //! typ plyty, w aplikacji obslugiwany jest 2
    int type;
    //! lista indeksow kanalow analogowych podpietych do plyty (np. F1 i M1 to kanaly 1-6)
    std::set<std::string> channelLabels;
};
typedef ForcePlatform* ForcePlatformPtr;
typedef const ForcePlatform* ForcePlatformConstPtr;
typedef const std::vector<ForcePlatformConstPtr>& ForcePlatformConstCollection;
typedef std::vector<ForcePlatformPtr> ForcePlatformCollection;

//! Klasa sluzy do pobierania informacji z pliku C3D, przykrywa biblioteke btk
class C3DLIB_EXPORT C3DParser
{
public:
	//! Interfejs dostarcza opisu dla danych akwizycji (jest to czesc wspolna dla danych analogowych, punktow i zdarzen)
	class IAquisitionEntry 
	{
	public:
		virtual ~IAquisitionEntry() {}
	public:
		//! \return Etykieta danych akwizycji
		virtual const std::string& getLabel() const = 0;

		//! \return Opis danych akwizycji
		virtual const std::string& getDescription() const = 0;
	};
    typedef IAquisitionEntry* IAqusitionEntryPtr;
    typedef const IAquisitionEntry* IAqusitionEntryConstPtr;

	//! Interfejs dostarcza potrzebnych danych nt. punktu c3d
    class IPoint : public IAquisitionEntry
    {
	public:
        typedef enum {Marker, VirtualMarker, VirtualMarkerForFrame, Angle, Force, Moment, Power, Scalar} Type;
	public:
        //! \return Typ punktu c3d 
        virtual Type getType() const = 0;
        //! \return jednostka dla danych analogowych (np. mm)
		virtual const std::string& getUnit() const = 0;
		//! \param index nr probki
        //! \return warto�� probki
		virtual osg::Vec3 getValue(int index) const = 0;
    };
	typedef IPoint* IPointPtr;
	typedef const IPoint* IPointConstPtr;
		
	//! Dostarcza danych dla kanalu analogowego
    class IAnalog : public IAquisitionEntry
    {
	public:
        //! \return jednostka dla danych analogowych (np. mm)
		virtual const std::string& getUnit() const = 0;
        //! \return uzyta skala w pliku C3D
		virtual double getScale() const = 0;
		//! \param index nr probki
		//! pobiera wartosc probki
		virtual double getValue(int index) const = 0;
	};
	typedef IAnalog* IAnalogPtr;
	typedef const IAnalog* IAnalogConstPtr;

	
	
	//! dostarcza danych o zdarzeniu zapisanym w c3d (np. dotkniecie stopa podlogi)
    class IEvent : public IAquisitionEntry
    {
    public:
        enum Context
        {
            General,
            Left,
            Right
        };

	public:
        //! return kontekst zdarzenia (lewy, prawy)
        virtual Context getContext() const = 0;
        //! return dodatkowa informacja - nazwa badanego
        virtual std::string getSubject() const = 0;
        //! return czas, dla ktorego wystapilo zdarzenie
        virtual double getTime() const = 0;
		//! zwraca kopie obiektu, kopia musi byc pozniej zwolniona !
		virtual IEvent* clone() const = 0;
    };
	typedef IEvent* IEventPtr;
	typedef const IEvent* IEventConstPtr;

public:
    C3DParser();
    virtual ~C3DParser();

public:
    //! Wczytanie pliku
    //! \param filename
    void load(const std::string& filename);
    //! Zapis pliku
    //! \param filename
    void save(const std::string& filename);

    //! Wczytanie plikow
    //! \param filenames kolekcja z nazwami plikow
    //! \param importWarnings zwraca warningi podczas wczytywania plikow
    void importFrom(const std::vector<const std::string>& filenames, std::string& importWarnings);

public:
	//! \return czestotliwosc dla wszystkich danych analogowych
	double getAnalogFrequency() const;
	//! \param indeks w kolekcji pobieranego kanalu analogowego
	//! \return kanal analogowy
	IAnalogConstPtr getAnalog(int index) const;
	//! \return liczba kanalow analogowych
	int getNumAnalogs() const;
	//! \return liczba pojedynczych probek w kanalach analogowych
	int getNumAnalogFrames() const;
	//! \return liczba probek analogowych przypadajacych na probe punktu
	int getNumberAnalogSamplePerFrame() const;
	//! \param index indeks w kolekcji pobieranego punktu
	//! \return punkt z c3d
	IPointConstPtr getPoint(int index) const;
	//! \return liczba punktow w kolekcji
	int getNumPoints() const;
	//! \return liczba zapisanych probek dla punktow
	int getNumPointFrames() const;
	//! \return czestotliwosc zapisu danych dla punktow
    double getPointFrequency() const;
    //! \return liczba eventow wczytanych z pliku c3d
	int getNumEvents() const;
    //! \return event o konkrentym indeksie (zgodnie z kolejnoscia w c3d)
	IEventPtr getEvent(int index) const;

	ForcePlatformCollection getForcePlatforms() { return forcePlatforms; }

private:
    //! realizacja idiomu "Cashire cat"
    //! http://en.wikipedia.org/wiki/Opaque_pointer
    class __data;
    //! Wskaznik do danych, ktore zostana calkowicie ukryte
    //! przed uzytkownikami biblioteki.
    __data* data;
	//! kolekcja punktow c3d
    std::vector<IPointPtr> points;
	//! kolekcja kanalow analogowych
    std::vector<IAnalogPtr> analogs;
	//! kolekcja zdarzen
    std::vector<IEventPtr> events;
	//! plyty GRF
	ForcePlatformCollection forcePlatforms;

private:
	//! Zapewnia wlasciwe wczytywanie danych akwizycji
    void loadAcquisition();
	//! zwraca mno�nik, dzi�ki kt�remu mo�na przekonwertowa� warto�� do SI
	float getUnitScale(const std::string& unit) const;
};

}

#endif