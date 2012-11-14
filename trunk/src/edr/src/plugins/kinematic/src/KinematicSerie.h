/********************************************************************
	created:	2012/05/23
	created:	23:5:2012   21:12
	filename: 	KinematicSerie.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_KINEMATIC__KINEMATICSERIE_H__
#define HEADER_GUARD_KINEMATIC__KINEMATICSERIE_H__

#include <plugins/c3d/EventSerieBase.h>


/*! Podstawowa i abstrakcyjna seria danych w wizualizatorze 3D */
class KinematicSerie : public EventSerieBase
{
public: 
    typedef osg::ref_ptr<osg::MatrixTransform> MatrixTransformPtr;
    typedef osg::ref_ptr<osg::PositionAttitudeTransform> TransformPtr;

public:
    //! W konstruktorze tworzone są już węzły i zerowany czas
    KinematicSerie();
    virtual ~KinematicSerie() {}

public:
    //! \return macierz, na której operują manipulatory
    inline MatrixTransformPtr getMatrixTransformNode();
    //! \return punkt zaczepienia manipulatorów
    osg::Vec3 getPivot() const;
    //! KinematicSerie jest serią zawierającą eventy. Klasy pochodne maja możliwość ich obsługi
    virtual void setEvents(EventsCollectionConstPtr val);
    //! Zwraca czas 
    double getTime() const { return time; }
    //! Ustawia czas. Metoda celowo nie jest wirtualna, klasy dziedziczące powinny implementować setLocalTime
    //! \param val ustawiany czas
    void setTime(double val);
    //! Macierz serii z transformacją, która nie została zmieniona przez manipulatory
    virtual osg::Matrix getInitialMatrix() const { return osg::Matrix(); }
    //! Metoda niweluje efekty działania manipulatorów
    virtual void resetTransform();

protected:
    //! Abstrakcyjny setter do czasu, metoda z inną sygnaturą może uchronić przed błędami
    //! \param time ustawiany czas
    virtual void setLocalTime(double time) = 0;

protected:
    //! węzeł używany przez klasę pochodną
    TransformPtr transformNode;
    //! zdarzenia związane z próba pomiarowa
    EventsCollectionConstPtr events;
    //! macierz, na której działają manipulatory
    MatrixTransformPtr matrixTransform;
private:
    //! ustawiony czas dla serii
    double time;
};
#endif
