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
    //! typ zdarzenia wczytanego z pliku C3D
    typedef c3dlib::C3DParser::IEvent IEvent;
    //! kontekst zdarzenia (strona lewa, prawa, ... )
    typedef c3dlib::C3DParser::IEvent::Context Context;
    //! akcesor do danych zapisanych w DataChannel z markerem
	typedef utils::DataChannelTimeAccessor<osg::Vec3f, float> TimeAccessor;

    //! Reprezentacja wykrytego kroku na p³ycie pomiarowej.
    //! W tym przypadku jako krok, bierze siê moment, w którym ca³a stopa spoczywa na p³ycie pomiarowej
    class Step : public IStep
    {
    public:
        //! domyœlny konstruktor, krok bêdzie nie zainicjalizowany
        Step() : startTime(-1.0f), endTime(-1.0f), context(ForcePlatform::IEvent::General) {}
        //! Kontruktor, który tworzy zainicjalizowana instancje kroku
        //! \param t1 czas rozpoczêcia kroku
        //! \param t2 czas zakoñczenia kroku
        //! \param c konkekst kroku (lewy , prawy)
        Step(float t1, float t2, ForcePlatform::Context c) : startTime(t1), endTime(t2), context(c) {}

    public:
        //! \return czas rozpoczêcia kroku
        virtual float getStartTime() const { return startTime; }
        //! \param val czas rozpoczêcia kroku
        virtual void setStartTime(float val) { startTime = val; }
        //! \return czas zakoñczenia kroku
        virtual float getEndTime() const { return endTime; }
        //! \param val czas zakoñczenia kroku
        virtual void setEndTime(float val) { endTime = val; }
        //! \return punkt, w którym znajduje siê koniec stopy (piêta)
        virtual osg::Vec3 getStartPoint() const { return startPoint; }
        //! \param val punkt, w którym znajduje siê koniec stopy (piêta)
        virtual void setStartPoint(const osg::Vec3& val) { startPoint = val; }
        //! \return punkt, w którym znajduje siê pocz¹tek stopy (palce)
        virtual osg::Vec3 getEndPoint() const { return endPoint; }
        //! \param val punkt, w którym znajduje siê pocz¹tek stopy (palce)
        virtual void setEndPoint(const osg::Vec3& val) { endPoint = val; }
        //! \return czy dane zawarte w klasie s¹ poprawne
        virtual bool isValid() const { return startTime >= 0.0f && endTime >= 0.0f; }
        //! \return konkekst kroku (lewy , prawy)
        ForcePlatform::Context getContext() const { return context; }
        //! \param val konkekst kroku (lewy , prawy)
        void setContext(ForcePlatform::Context val) { context = val; }

    private:
        //! czas rozpoczêcia kroku
        float startTime;
        //! czas zakoñczenia kroku
        float endTime;
        //! punkt, w którym znajduje siê koniec stopy (piêta)
        osg::Vec3 startPoint;
        //! punkt, w którym znajduje siê pocz¹tek stopy (palce)
        osg::Vec3 endPoint;
        //! konkekst kroku (lewy , prawy)
        Context context;
    };

public:
    //! Konstruktor, inicjalizuje klasê na podstawie surowych danych platformy
    //! \param platform platforma dostarczona przez parser c3d
    ForcePlatform(c3dlib::ForcePlatformPtr platform);

public:
    //! \return wykryte kroki (w rozumieniu IStep)
    virtual StepsConstRange getSteps() const;
    //! \return œrodek p³yty pomiarowej
    virtual osg::Vec3 getCenter() const;
    //! \return szerokoœæ p³yty pomiarowej
    virtual float getWidth() const;
    //! \return d³ugoœæ p³yty pomiarowej
    virtual float getLength() const;
    //! Metoda pozwala okreœliæ orientacjê p³yty pomiarowej
    //! \return kolejnoœæ wyst¹pienia wierzcho³ków (lewy prawy = 1, prawy lewy = -1)
    virtual float getSignX() const;
    //! Metoda pozwala okreœliæ orientacjê p³yty pomiarowej
    //! \return kolejnoœæ wyst¹pienia wierzcho³ków (góra dol = 1, dol góra = -1)
    virtual float getSignY() const;
    //! Algorytm stara siê wykryæ wszystkie kroki(w rozumieniu Step) zwiazene z p³yta GRF
    //! \param markers kolekcja z markerami, potrzebna aby pobraæ markery zwi¹zane ze stop¹
    //! \param events kolekcja ze zdarzeniami, pomocne przy wykrywaniu kroków 
    void computeSteps( MarkerCollectionPtr markers, EventsCollectionConstPtr events );
    //! \return kana³ z odpowiadaj¹ca p³ycie si³¹
    virtual GRFChannelConstPtr getForceChannel() const;
    //! \return kana³ z odpowiadaj¹ca p³ycie momentem si³y
    virtual GRFChannelConstPtr getMomentChannel() const;
    //! ustawia kana³ z odpowiadaj¹ca p³ycie si³¹
    virtual void setForceChannel(GRFChannelConstPtr val)  { force = val; }
    //! ustawia kana³ z odpowiadaj¹ca p³ycie momentem si³y
    virtual void setMomentChannel(GRFChannelConstPtr val) { moment = val; }

private:
    //! Sprawdza czy punkt znajduje siê w obrêbie p³yty (wysokoœæ nie ma znaczenia)
    //! \param v sprawdzany punkt
    bool isInsideXY( const osg::Vec3& v) const;

private:
    //! obiekt pochodz¹cy wprost z parsera c3d, surowe dane platformy
    c3dlib::ForcePlatform platform;
    //! kolekcja z krokami (w rozumieniu klasy Step)
    Steps steps;
    //! kana³ z odpowiadaj¹ca p³ycie si³a
    GRFChannelConstPtr force;
    //! kana³ z odpowiadaj¹ca p³ycie momentem si³y
    GRFChannelConstPtr moment;
};
typedef core::shared_ptr<ForcePlatform> ForcePlatformPtr;
typedef core::shared_ptr<const ForcePlatform> ForcePlatformConstPtr;


#endif
