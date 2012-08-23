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
    //! W konstruktorze tworzone s� ju� w�z�y i zerowany czas
    KinematicSerie();
    virtual ~KinematicSerie() {}

public:
    //! \return macierz, na kt�rej operuj� manipulatory
    inline MatrixTransformPtr getMatrixTransformNode();
    //! \return punkt zaczepienia manipulator�w
    osg::Vec3 getPivot() const;
    //! KinematicSerie jest seri� zawieraj�c� eventy. Klasy pochodne maja mo�liwo�� ich obs�ugi
    virtual void setEvents(EventsCollectionConstPtr val);
    //! Zwraca czas 
    double getTime() const { return time; }
    //! Ustawia czas. Metoda celowo nie jest wirtualna, klasy dziedzicz�ce powinny implementowa� setLocalTime
    //! \param val ustawiany czas
    void setTime(double val);
    //! Macierz serii z transformacj�, kt�ra nie zosta�a zmieniona przez manipulatory
    virtual osg::Matrix getInitialMatrix() const { return osg::Matrix(); }
    //! Metoda niweluje efekty dzia�ania manipulator�w
    virtual void resetTransform();

protected:
    //! Abstrakcyjny setter do czasu, metoda z inn� sygnatur� mo�e uchroni� przed b��dami
    //! \param time ustawiany czas
    virtual void setLocalTime(double time) = 0;

protected:
    //! w�ze� u�ywany przez klas� pochodn�
    TransformPtr transformNode;
    //! zdarzenia zwi�zane z pr�ba pomiarowa
    EventsCollectionConstPtr events;
    //! macierz, na kt�rej dzia�aj� manipulatory
    MatrixTransformPtr matrixTransform;
private:
    //! ustawiony czas dla serii
    double time;
};
#endif
