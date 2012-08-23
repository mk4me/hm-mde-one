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

//! Interfejs platformy GRF, obiekty tego typu zasilaj¹ system
class IForcePlatform
{
public:
    //! Interfejs wykrytego kroku na p³ycie pomiarowej.
    //! W tym przypadku jako krok, bierze siê moment, w którym ca³a stopa spoczywa na p³ycie pomiarowej
    class IStep {
        public:
            virtual ~IStep() {}
            //! \return czas rozpoczêcia kroku
            virtual float getStartTime() const = 0;
            //! \return czas zakoñczenia kroku
            virtual float getEndTime() const = 0;
            //! \return punkt, w którym znajduje siê koniec stopy (piêta)
            virtual osg::Vec3 getStartPoint() const = 0;
            //! \return punkt, w którym znajduje siê pocz¹tek stopy (palce)
            virtual osg::Vec3 getEndPoint() const = 0;
            //! \return czy dane zawarte w klasie s¹ poprawne
            virtual bool isValid() const = 0;
    };
    typedef core::shared_ptr<IStep> IStepPtr;
    typedef core::shared_ptr<const IStep> IStepConstPtr;

    //! wektor z krokami
    typedef std::vector<IStepConstPtr> Steps;
    //! niemodyfikowalny zakres kroków z kolekcji
    typedef boost::iterator_range<Steps::const_iterator> StepsConstRange;
    //! wektor z kana³ami GRF
    typedef std::vector<GRFChannelConstPtr> GRFChannels;
    //! niemodyfikowalny zakres kana³ów GRF
    typedef boost::iterator_range<GRFChannels::const_iterator> GRFChannelsConstRange;

public:
	virtual ~IForcePlatform() {}

public:
    //! \return wykryte kroki (w rozumieniu IStep)
    virtual StepsConstRange getSteps() const = 0;
    //! \return œrodek p³yty pomiarowej
    virtual osg::Vec3 getCenter() const = 0;
    //! \return szerokoœæ p³yty pomiarowej
    virtual float getWidth() const = 0;
    //! \return d³ugoœæ p³yty pomiarowej
    virtual float getLength() const = 0;
    //! Metoda pozwala okreœliæ orientacje p³yty pomiarowej
    //! \return kolejnoœæ wyst¹pienia wierzcho³ków (lewy prawy = 1, prawy lewy = -1)
    virtual float getSignX() const = 0;
    //! Metoda pozwala okreœliæ orientacje p³yty pomiarowej
    //! \return kolejnoœæ wyst¹pienia wierzcho³ków (gora dol = 1, dol gora = -1)
    virtual float getSignY() const = 0;
    //! \return kana³ z odpowiadaj¹ca p³ycie si³a
    virtual GRFChannelConstPtr getForceChannel() const = 0;
    //! \return kana³ z odpowiadaj¹ca p³ycie momentem si³y
    virtual GRFChannelConstPtr getMomentChannel() const = 0;
    //! Metoda liczy odleg³oœæ danego punktu od srodka p³yty pomiarowej
    //! \param vec dany punkt
    //! \return zmierzona odleg³oœæ
    float getDistanceToCenter(const osg::Vec3& vec) const {
        return (getCenter() - vec).length();
    }
};
typedef core::shared_ptr<IForcePlatform> IForcePlatformPtr;
typedef core::shared_ptr<const IForcePlatform> IForcePlatformConstPtr;
typedef std::vector<IForcePlatformPtr> IForcePlatformCollection;


#endif
