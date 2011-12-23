/********************************************************************
	created:	2011/05/30
	created:	30:5:2011   8:59
	filename: 	C3DParser.h
	author:		Wojciech Knieæ
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_C3DLIB__C3DPARSER_H__
#define HEADER_GUARD_C3DLIB__C3DPARSER_H__

#include <string>
#include <vector>
#include <map>
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
	//! zwraca srodek plyty  w ukladzie odniesienia zgodnym z markerami
	osg::Vec3 getCenter() const 
	{
		osg::Vec3 c;
		c += corners[0];
		c += corners[1];
		c += corners[2];
		c += corners[3];
		c *= 0.25f;
		c += origin;
		return c;
	}

	float getWidth() const {
		return (corners[0] - corners[1]).length();
	}

	float getLength() const {
		return (corners[1] - corners[2]).length();
	}

	float getDistanceToCenter(const osg::Vec3& vec) const {
		return (getCenter() - vec).length();
	}
};
//typedef boost::shared_ptr<ForcePlatform> ForcePlatformPtr;
//typedef boost::shared_ptr<const ForcePlatform> ForcePlatformConstPtr;
//typedef const std::vector<ForcePlatformConstPtr>& ForcePlatformConstCollection;
//typedef std::vector<ForcePlatformConstPtr> ForcePlatformCollection;
typedef ForcePlatform* ForcePlatformPtr;
typedef const ForcePlatform* ForcePlatformConstPtr;
typedef const std::vector<ForcePlatformConstPtr>& ForcePlatformConstCollection;
typedef std::vector<ForcePlatformConstPtr> ForcePlatformCollection;

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
	//typedef boost::shared_ptr<IAquisitionEntry> IAqusitionEntryPtr;
	//typedef boost::shared_ptr<const IAquisitionEntry> IAqusitionEntryConstPtr;
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
		virtual const std::string& getUnit() const = 0;
		virtual osg::Vec3 getValue(int index) const = 0;
    };
	typedef IPoint* IPointPtr;
	typedef const IPoint* IPointConstPtr;
		
	//! Dostarcza danych dla kanalu analogowego
    class IAnalog : public IAquisitionEntry
    {
	public:
		virtual const std::string& getUnit() const = 0;
		virtual double getScale() const = 0;
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
        virtual Context getContext() const = 0;
        virtual std::string getSubject() const = 0;
        virtual double getTime() const = 0;
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

	int getNumEvents() const;
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
	std::vector<ForcePlatformConstPtr> forcePlatforms;

private:
	//! Zapewnia wlasciwe wczytywanie danych akwizycji
    void loadAcquisition();
	//! zwraca mno¿nik, dziêki któremu mo¿na przekonwertowaæ wartoœæ do SI
	float getUnitScale(const std::string& unit) const;
};

}

#endif