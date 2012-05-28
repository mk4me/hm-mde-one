/********************************************************************
	created:	2012/03/24
	created:	24:3:2012   13:12
	filename: 	ForcePlatform.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_ForcePlaform__FORCEPLATFORM_H__
#define HEADER_GUARD_ForcePlaform__FORCEPLATFORM_H__

#include "C3DPCH.h"
#include <c3dlib/C3DParser.h>
#include <plugins/c3d/IForcePlatform.h>
#include <plugins/c3d/C3DChannels.h>
#include <plugins/c3d/C3DCollections.h>

// Klasa reprezentuje platforme GRF
class ForcePlatform : public IForcePlatform
{
public:
    typedef c3dlib::C3DParser::IEvent IEvent;
    typedef c3dlib::C3DParser::IEvent::Context Context;
	typedef utils::DataChannelTimeAccessor<osg::Vec3f, float> TimeAccessor;

    //! Reprezentacja wykrytego kroku na plycie pomiarowej.
    //! W tym przypadku jako krok, bierze sie moment, w ktorym cala stopa spoczywa na plycie pomiarowej
    class Step : public IStep
    {
    public:
        //! Domyslny konstruktor, krok bedzie nie zainicjalizowany
        Step() : startTime(-1.0f), endTime(-1.0f), context(ForcePlatform::IEvent::General) {}
        //! Kontruktor, ktory tworzy zainicjalizowana instancje kroku
        //! \param t1 czas rozpoczecia kroku
        //! \param t2 czas zakonczenia kroku
        //! \param c konkekst kroku (lewy , prawy)
        Step(float t1, float t2, ForcePlatform::Context c) : startTime(t1), endTime(t2), context(c) {}

    public:
        //! \return czas rozpoczecia kroku
        virtual float getStartTime() const { return startTime; }
        //! \param val czas rozpoczecia kroku
        virtual void setStartTime(float val) { startTime = val; }
        //! \return czas zakonczenia kroku
        virtual float getEndTime() const { return endTime; }
        //! \param val czas zakonczenia kroku
        virtual void setEndTime(float val) { endTime = val; }
        //! \return punkt, w ktorym znajduje sie koniec stopy (pieta)
        virtual osg::Vec3 getStartPoint() const { return startPoint; }
        //! \param val punkt, w ktorym znajduje sie koniec stopy (pieta)
        virtual void setStartPoint(const osg::Vec3& val) { startPoint = val; }
        //! \return punkt, w ktorym znajduje sie poczatek stopy (palce)
        virtual osg::Vec3 getEndPoint() const { return endPoint; }
        //! \param val punkt, w ktorym znajduje sie poczatek stopy (palce)
        virtual void setEndPoint(const osg::Vec3& val) { endPoint = val; }
        //! \return czy dane zawarte w klaasie sa poprawne
        virtual bool isValid() const { return startTime >= 0.0f && endTime >= 0.0f; }
        //! \return konkekst kroku (lewy , prawy)
        ForcePlatform::Context getContext() const { return context; }
        //! \param val konkekst kroku (lewy , prawy)
        void setContext(ForcePlatform::Context val) { context = val; }

    private:
        //! czas rozpoczecia kroku
        float startTime;
        //! czas zakonczenia kroku
        float endTime;
        //! punkt, w ktorym znajduje sie koniec stopy (pieta)
        osg::Vec3 startPoint;
        //! punkt, w ktorym znajduje sie poczatek stopy (palce)
        osg::Vec3 endPoint;
        //! konkekst kroku (lewy , prawy)
        Context context;
    };

public:
    //! Konstruktor, inicjalizuje klase na podstawie surowych danych platformy
    //! \param platform platforma dostarczona przez parser c3d
    ForcePlatform(c3dlib::ForcePlatformPtr platform);

public:
    //! \return wykryte kroki (w rozumieniu IStep)
    virtual StepsConstRange getSteps() const;
    //! \return srodek plyty pomiarowej
    virtual osg::Vec3 getCenter() const;
    //! \return szerokosc plyty pomiarowej
    virtual float getWidth() const;
    //! \return dlugosc plyty pomiarowej
    virtual float getLength() const;
    //! Metoda pozwala okreslic orientacje plyty pomiarowej
    //! \return kolejnosc wystapienia wiercholkow (lewy prawy = 1, prawy lewy = -1)
    virtual float getSignX() const;
    //! Metoda pozwala okreslic orientacje plyty pomiarowej
    //! \return kolejnosc wystapienia wiercholkow (gora dol = 1, dol gora = -1)
    virtual float getSignY() const;
    //! Algorytm stara sie wykryc wszystkie kroki(w rozumieniu Step) zwiazene z plyta GRF
    //! \param markers kolekcja z markerami, potrzebna aby pobrac markery zwiazane ze stopa
    //! \param events kolekcja ze zdarzeniami, pomocne przy wykrywaniu krokow 
    void computeSteps( MarkerCollectionPtr markers, EventsCollectionConstPtr events );
    //! \return kanal z odpowiadajaca plycie sila
    virtual GRFChannelConstPtr getForceChannel() const;
    //! \return kanal z odpowiadajaca plycie momentem sily
    virtual GRFChannelConstPtr getMomentChannel() const;
    virtual void setForceChannel(GRFChannelConstPtr val)  { force = val; }
    virtual void setMomentChannel(GRFChannelConstPtr val) { moment = val; }

private:
    //! Sprawdza czy punkt znajduje sie w obrebie plyty (wysokosc nie ma znaczenia)
    //! \param v sprawdzany punkt
    bool isInsideXY( const osg::Vec3& v) const;

private:
    //! obiekt pochodzacy wprost z parsera c3d, surowe dane platformy
    c3dlib::ForcePlatform platform;
    //! kolekcja z krokami (w rozumieniu klasy Step)
    Steps steps;
    //! kanal z odpowiadajaca plycie sila
    GRFChannelConstPtr force;
    //! kanal z odpowiadajaca plycie momentem sily
    GRFChannelConstPtr moment;
};
typedef core::shared_ptr<ForcePlatform> ForcePlatformPtr;
typedef core::shared_ptr<const ForcePlatform> ForcePlatformConstPtr;


#endif
