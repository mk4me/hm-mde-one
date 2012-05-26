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
    //! W konstruktorze tworzone sa juz wezly i zerowany czas
    KinematicSerie();
    virtual ~KinematicSerie() {}

public:
    //! \return macierz, na ktorej operuja manipulatory
    inline MatrixTransformPtr getMatrixTransformNode();
    //! \return punkt zaczepienia manipulatorow
    osg::Vec3 getPivot() const;
    //! KinematicSerie jest seria zawierajaca eventy. Klasy pochodne maja mozliwosc ich obslugi
    virtual void setEvents(EventsCollectionConstPtr val);
    //! Zwraca czas 
    double getTime() const { return time; }
    //! Ustawia czas. Metoda celowo nie jest wirtualna, klasy dziedziczace powinny implementowac setLocalTime
    //! \param val ustawiany czas
    void setTime(double val);
    //! Macierz serii z transformacja, ktora nie zostala zmieniona przez manipulatory
    virtual osg::Matrix getInitialMatrix() const { return osg::Matrix(); }
    //! Metoda niweluje efekty dzialania manipulatorow
    virtual void resetTransform();

protected:
    //! Abstrakcyjny setter do czasu, metoda z inna sygnatura moze uchronic przed bledami
    //! \param time ustawiany czas
    virtual void setLocalTime(double time) = 0;

protected:
    //! wezel uzywany przez klase pochodna
    TransformPtr transformNode;
    //! zdarzenia zwiazane z proba pomiarowa
    EventsCollectionConstPtr events;
    //! macierz, na ktorej dzialaja manipulatory
    MatrixTransformPtr matrixTransform;
private:
    //! ustawiony czas dla serii
    double time;
};
#endif
