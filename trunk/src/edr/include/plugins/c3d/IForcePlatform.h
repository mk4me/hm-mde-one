/********************************************************************
	created:	2012/03/23
	created:	23:3:2012   10:37
	filename: 	ForcePlatform.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_C3DPLUGIN__FORCEPLATFORM_H__
#define HEADER_GUARD_C3DPLUGIN__FORCEPLATFORM_H__

#include <boost/range.hpp>
#include <osg/Vec3>
#include <core/SmartPtr.h>
#include <plugins/c3d/C3DChannels.h>

//! Interfejs platformy GRF, obiekty tego typu zasilaja system
class IForcePlatform
{
public:
    //! Interfejs wykrytego kroku na plycie pomiarowej.
    //! W tym przypadku jako krok, bierze sie moment, w ktorym cala stopa spoczywa na plycie pomiarowej
    class IStep {
        public:
            virtual ~IStep() {}
            //! \return czas rozpoczecia kroku
            virtual float getStartTime() const = 0;
            //! \return czas zakonczenia kroku
            virtual float getEndTime() const = 0;
            //! \return punkt, w ktorym znajduje sie koniec stopy (pieta)
            virtual osg::Vec3 getStartPoint() const = 0;
            //! \return punkt, w ktorym znajduje sie poczatek stopy (palce)
            virtual osg::Vec3 getEndPoint() const = 0;
            //! \return czy dane zawarte w klaasie sa poprawne
            virtual bool isValid() const = 0;
    };
    typedef core::shared_ptr<IStep> IStepPtr;
    typedef core::shared_ptr<const IStep> IStepConstPtr;

    typedef std::vector<IStepConstPtr> Steps;
    typedef boost::iterator_range<Steps::const_iterator> StepsConstRange;

    typedef std::vector<GRFChannelConstPtr> GRFChannels;
    typedef boost::iterator_range<GRFChannels::const_iterator> GRFChannelsConstRange;

public:
	virtual ~IForcePlatform() {}

public:
    //! \return wykryte kroki (w rozumieniu IStep)
    virtual StepsConstRange getSteps() const = 0;
    //! \return srodek plyty pomiarowej
    virtual osg::Vec3 getCenter() const = 0;
    //! \return szerokosc plyty pomiarowej
    virtual float getWidth() const = 0;
    //! \return dlugosc plyty pomiarowej
    virtual float getLength() const = 0;
    //! Metoda pozwala okreslic orientacje plyty pomiarowej
    //! \return kolejnosc wystapienia wiercholkow (lewy prawy = 1, prawy lewy = -1)
    virtual float getSignX() const = 0;
    //! Metoda pozwala okreslic orientacje plyty pomiarowej
    //! \return kolejnosc wystapienia wiercholkow (gora dol = 1, dol gora = -1)
    virtual float getSignY() const = 0;
    //! \return kanal z odpowiadajaca plycie sila
    virtual GRFChannelConstPtr getForceChannel() const = 0;
    //! \return kanal z odpowiadajaca plycie momentem sily
    virtual GRFChannelConstPtr getMomentChannel() const = 0;
    //! Metoda liczy odleglosc danego punktu od srodka plyty pomiarowej
    //! \param vec dany punkt
    //! \return zmierzona odleglosc
    float getDistanceToCenter(const osg::Vec3& vec) const {
        return (getCenter() - vec).length();
    }
};
typedef core::shared_ptr<IForcePlatform> IForcePlatformPtr;
typedef core::shared_ptr<const IForcePlatform> IForcePlatformConstPtr;
typedef std::vector<IForcePlatformPtr> IForcePlatformCollection;


#endif
