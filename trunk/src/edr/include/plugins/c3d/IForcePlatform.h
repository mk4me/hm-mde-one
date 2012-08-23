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

//! Interfejs platformy GRF, obiekty tego typu zasilaj� system
class IForcePlatform
{
public:
    //! Interfejs wykrytego kroku na p�ycie pomiarowej.
    //! W tym przypadku jako krok, bierze si� moment, w kt�rym ca�a stopa spoczywa na p�ycie pomiarowej
    class IStep {
        public:
            virtual ~IStep() {}
            //! \return czas rozpocz�cia kroku
            virtual float getStartTime() const = 0;
            //! \return czas zako�czenia kroku
            virtual float getEndTime() const = 0;
            //! \return punkt, w kt�rym znajduje si� koniec stopy (pi�ta)
            virtual osg::Vec3 getStartPoint() const = 0;
            //! \return punkt, w kt�rym znajduje si� pocz�tek stopy (palce)
            virtual osg::Vec3 getEndPoint() const = 0;
            //! \return czy dane zawarte w klasie s� poprawne
            virtual bool isValid() const = 0;
    };
    typedef core::shared_ptr<IStep> IStepPtr;
    typedef core::shared_ptr<const IStep> IStepConstPtr;

    //! wektor z krokami
    typedef std::vector<IStepConstPtr> Steps;
    //! niemodyfikowalny zakres krok�w z kolekcji
    typedef boost::iterator_range<Steps::const_iterator> StepsConstRange;
    //! wektor z kana�ami GRF
    typedef std::vector<GRFChannelConstPtr> GRFChannels;
    //! niemodyfikowalny zakres kana��w GRF
    typedef boost::iterator_range<GRFChannels::const_iterator> GRFChannelsConstRange;

public:
	virtual ~IForcePlatform() {}

public:
    //! \return wykryte kroki (w rozumieniu IStep)
    virtual StepsConstRange getSteps() const = 0;
    //! \return �rodek p�yty pomiarowej
    virtual osg::Vec3 getCenter() const = 0;
    //! \return szeroko�� p�yty pomiarowej
    virtual float getWidth() const = 0;
    //! \return d�ugo�� p�yty pomiarowej
    virtual float getLength() const = 0;
    //! Metoda pozwala okre�li� orientacje p�yty pomiarowej
    //! \return kolejno�� wyst�pienia wierzcho�k�w (lewy prawy = 1, prawy lewy = -1)
    virtual float getSignX() const = 0;
    //! Metoda pozwala okre�li� orientacje p�yty pomiarowej
    //! \return kolejno�� wyst�pienia wierzcho�k�w (gora dol = 1, dol gora = -1)
    virtual float getSignY() const = 0;
    //! \return kana� z odpowiadaj�ca p�ycie si�a
    virtual GRFChannelConstPtr getForceChannel() const = 0;
    //! \return kana� z odpowiadaj�ca p�ycie momentem si�y
    virtual GRFChannelConstPtr getMomentChannel() const = 0;
    //! Metoda liczy odleg�o�� danego punktu od srodka p�yty pomiarowej
    //! \param vec dany punkt
    //! \return zmierzona odleg�o��
    float getDistanceToCenter(const osg::Vec3& vec) const {
        return (getCenter() - vec).length();
    }
};
typedef core::shared_ptr<IForcePlatform> IForcePlatformPtr;
typedef core::shared_ptr<const IForcePlatform> IForcePlatformConstPtr;
typedef std::vector<IForcePlatformPtr> IForcePlatformCollection;


#endif
