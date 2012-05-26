/********************************************************************
	created:	2011/09/04
	created:	4:9:2011   14:24
	filename: 	SkeletonSerie.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_KINEMATIC__SKELETONSERIE_H__
#define HEADER_GUARD_KINEMATIC__SKELETONSERIE_H__

#include <core/IVisualizer.h>
#include <osg/Geode>
#include "KinematicVisualizer.h"

//! Seria danych wizualizatora 3D wizualizujaca animacje szkieletowa
class SkeletonSerie :  public QObject, public KinematicSerie
{
	Q_OBJECT;
public:
	typedef osg::ref_ptr<osg::Geode> GeodePtr;
	typedef osg::ref_ptr<osg::Group> GroupPtr;

public:
	//! Konstuktor
	//! \param visualizer wizualizator, ktory stworzyl serie danych
	SkeletonSerie(KinematicVisualizer * visualizer);
	virtual ~SkeletonSerie() {}

private slots:
	//! zmiana osi, gdy otrzymamy obiekt w nietypowym ukl. wspolrzednych
	//! \param xyz 
	void setAxis(bool xyz);

public:
    //! Ustawia nowa nazwe seri
    //! \param name 
    virtual void setName(const std::string & name);
    //! \return nazwa serii
    virtual const std::string & getName() const;
    //! Ustawienie danych, inicjalizacja 
    //! \param data dane typu JointAnglesCollection
	virtual void setData(const core::ObjectWrapperConstPtr & data);
    //! \return ustawione dane
    virtual const core::ObjectWrapperConstPtr & getData() const;
	//! \return Dlugosc kanalu w sekundach
	virtual double getLength() const;
	//! Czas zawiera siê miêdzy 0 a getLength()
	//! \param time Aktualny, lokalny czas kanalu w sekundach
	virtual void setLocalTime(double time);
    //! \return macierz serii z transformacja, ktora nie zostala zmieniona przez manipulatory
    osg::Matrix getInitialMatrix() const;

private:
    //! Dialog z trajektoriami dziala dla MarkersCollection, dlatego nalezy dokonac konwersji
    //! \param joints struktura i dane animacji szkieletowej
    //! \return kolekcja z markerami, dla ktorej mozna juz uzywac dialogu TrajectoriesDialog
    MarkerCollectionConstPtr createTrajectories(kinematic::JointAnglesCollectionConstPtr joints);
    //! \return macierz potrzebna przy operacjach z roznymi ukl. wspolrzednych
    osg::Matrix getXYZMatrix() const;
    //! niweluje dzialanie manipulatorow
    virtual void resetTransform();

private:
    //! Wizualizator, ktory utworzyl serie
	KinematicVisualizer * visualizer;
    //! Polacznie miedzy aniamcja szieletowa (struktura + dane) a ich wizualizacja
	SkeletalVisualizationSchemePtr scheme;
    //! drawer schematu
	SchemeDrawerContainerPtr skeletonDrawers;
    //! wrapper przekazany serii
    core::ObjectWrapperConstPtr data;
    //! nazwa serii
    std::string name;
    //! glowny wezej animacji
    TransformPtr skeletonNode;
    //! czy operujemy na ukladzie XYZ czy innym
    bool xyzAxis;
    //! Drawer rysujacy trajektorie, powstaly one dzieki konwersji JoinAnglesCollection - > MarkersCollection
    TrajectoryDrawerPtr trajectoryDrawer;
};
typedef boost::shared_ptr<SkeletonSerie> SkeletonSeriePtr;
typedef boost::shared_ptr<const SkeletonSerie> SkeletonSerieConstPtr;

#endif

