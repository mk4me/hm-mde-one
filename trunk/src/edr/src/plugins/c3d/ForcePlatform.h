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

    //! Reprezentacja wykrytego kroku na p�ycie pomiarowej.
    //! W tym przypadku jako krok, bierze si� moment, w kt�rym ca�a stopa spoczywa na p�ycie pomiarowej
    class Step : public IStep
    {
    public:
        //! domy�lny konstruktor, krok b�dzie nie zainicjalizowany
        Step() : startTime(-1.0f), endTime(-1.0f), context(ForcePlatform::IEvent::General) {}
        //! Kontruktor, kt�ry tworzy zainicjalizowana instancje kroku
        //! \param t1 czas rozpocz�cia kroku
        //! \param t2 czas zako�czenia kroku
        //! \param c konkekst kroku (lewy , prawy)
        Step(float t1, float t2, ForcePlatform::Context c) : startTime(t1), endTime(t2), context(c) {}

    public:
        //! \return czas rozpocz�cia kroku
        virtual float getStartTime() const { return startTime; }
        //! \param val czas rozpocz�cia kroku
        virtual void setStartTime(float val) { startTime = val; }
        //! \return czas zako�czenia kroku
        virtual float getEndTime() const { return endTime; }
        //! \param val czas zako�czenia kroku
        virtual void setEndTime(float val) { endTime = val; }
        //! \return punkt, w kt�rym znajduje si� koniec stopy (pi�ta)
        virtual osg::Vec3 getStartPoint() const { return startPoint; }
        //! \param val punkt, w kt�rym znajduje si� koniec stopy (pi�ta)
        virtual void setStartPoint(const osg::Vec3& val) { startPoint = val; }
        //! \return punkt, w kt�rym znajduje si� pocz�tek stopy (palce)
        virtual osg::Vec3 getEndPoint() const { return endPoint; }
        //! \param val punkt, w kt�rym znajduje si� pocz�tek stopy (palce)
        virtual void setEndPoint(const osg::Vec3& val) { endPoint = val; }
        //! \return czy dane zawarte w klasie s� poprawne
        virtual bool isValid() const { return startTime >= 0.0f && endTime >= 0.0f; }
        //! \return konkekst kroku (lewy , prawy)
        ForcePlatform::Context getContext() const { return context; }
        //! \param val konkekst kroku (lewy , prawy)
        void setContext(ForcePlatform::Context val) { context = val; }

    private:
        //! czas rozpocz�cia kroku
        float startTime;
        //! czas zako�czenia kroku
        float endTime;
        //! punkt, w kt�rym znajduje si� koniec stopy (pi�ta)
        osg::Vec3 startPoint;
        //! punkt, w kt�rym znajduje si� pocz�tek stopy (palce)
        osg::Vec3 endPoint;
        //! konkekst kroku (lewy , prawy)
        Context context;
    };

public:
    //! Konstruktor, inicjalizuje klas� na podstawie surowych danych platformy
    //! \param platform platforma dostarczona przez parser c3d
    ForcePlatform(c3dlib::ForcePlatformPtr platform);

public:
    //! \return wykryte kroki (w rozumieniu IStep)
    virtual StepsConstRange getSteps() const;
    //! \return �rodek p�yty pomiarowej
    virtual osg::Vec3 getCenter() const;
    //! \return szeroko�� p�yty pomiarowej
    virtual float getWidth() const;
    //! \return d�ugo�� p�yty pomiarowej
    virtual float getLength() const;
    //! Metoda pozwala okre�li� orientacj� p�yty pomiarowej
    //! \return kolejno�� wyst�pienia wierzcho�k�w (lewy prawy = 1, prawy lewy = -1)
    virtual float getSignX() const;
    //! Metoda pozwala okre�li� orientacj� p�yty pomiarowej
    //! \return kolejno�� wyst�pienia wierzcho�k�w (g�ra dol = 1, dol g�ra = -1)
    virtual float getSignY() const;
    //! Algorytm stara si� wykry� wszystkie kroki(w rozumieniu Step) zwiazene z p�yta GRF
    //! \param markers kolekcja z markerami, potrzebna aby pobra� markery zwi�zane ze stop�
    //! \param events kolekcja ze zdarzeniami, pomocne przy wykrywaniu krok�w 
    void computeSteps( MarkerCollectionPtr markers, EventsCollectionConstPtr events );
    //! \return kana� z odpowiadaj�ca p�ycie si��
    virtual GRFChannelConstPtr getForceChannel() const;
    //! \return kana� z odpowiadaj�ca p�ycie momentem si�y
    virtual GRFChannelConstPtr getMomentChannel() const;
    //! ustawia kana� z odpowiadaj�ca p�ycie si��
    virtual void setForceChannel(GRFChannelConstPtr val)  { force = val; }
    //! ustawia kana� z odpowiadaj�ca p�ycie momentem si�y
    virtual void setMomentChannel(GRFChannelConstPtr val) { moment = val; }

private:
    //! Sprawdza czy punkt znajduje si� w obr�bie p�yty (wysoko�� nie ma znaczenia)
    //! \param v sprawdzany punkt
    bool isInsideXY( const osg::Vec3& v) const;

private:
    //! obiekt pochodz�cy wprost z parsera c3d, surowe dane platformy
    c3dlib::ForcePlatform platform;
    //! kolekcja z krokami (w rozumieniu klasy Step)
    Steps steps;
    //! kana� z odpowiadaj�ca p�ycie si�a
    GRFChannelConstPtr force;
    //! kana� z odpowiadaj�ca p�ycie momentem si�y
    GRFChannelConstPtr moment;
};
typedef core::shared_ptr<ForcePlatform> ForcePlatformPtr;
typedef core::shared_ptr<const ForcePlatform> ForcePlatformConstPtr;


#endif
