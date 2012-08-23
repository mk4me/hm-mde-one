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
    //! W konstruktorze tworzone s¹ ju¿ wêz³y i zerowany czas
    KinematicSerie();
    virtual ~KinematicSerie() {}

public:
    //! \return macierz, na której operuj¹ manipulatory
    inline MatrixTransformPtr getMatrixTransformNode();
    //! \return punkt zaczepienia manipulatorów
    osg::Vec3 getPivot() const;
    //! KinematicSerie jest seri¹ zawieraj¹c¹ eventy. Klasy pochodne maja mo¿liwoœæ ich obs³ugi
    virtual void setEvents(EventsCollectionConstPtr val);
    //! Zwraca czas 
    double getTime() const { return time; }
    //! Ustawia czas. Metoda celowo nie jest wirtualna, klasy dziedzicz¹ce powinny implementowaæ setLocalTime
    //! \param val ustawiany czas
    void setTime(double val);
    //! Macierz serii z transformacj¹, która nie zosta³a zmieniona przez manipulatory
    virtual osg::Matrix getInitialMatrix() const { return osg::Matrix(); }
    //! Metoda niweluje efekty dzia³ania manipulatorów
    virtual void resetTransform();

protected:
    //! Abstrakcyjny setter do czasu, metoda z inn¹ sygnatur¹ mo¿e uchroniæ przed b³êdami
    //! \param time ustawiany czas
    virtual void setLocalTime(double time) = 0;

protected:
    //! wêze³ u¿ywany przez klasê pochodn¹
    TransformPtr transformNode;
    //! zdarzenia zwi¹zane z próba pomiarowa
    EventsCollectionConstPtr events;
    //! macierz, na której dzia³aj¹ manipulatory
    MatrixTransformPtr matrixTransform;
private:
    //! ustawiony czas dla serii
    double time;
};
#endif
