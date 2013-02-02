/********************************************************************
	created:	2011/09/04
	created:	4:9:2011   14:24
	filename: 	SkeletonSerie.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_KINEMATIC__SKELETONSERIE_H__
#define HEADER_GUARD_KINEMATIC__SKELETONSERIE_H__

#include <corelib/IVisualizer.h>
#include <osg/Geode>
#include "KinematicVisualizer.h"

//! Seria danych wizualizatora 3D wizualizująca animacje szkieletowa
class SkeletonSerie :  public QObject, public KinematicSerie
{
	Q_OBJECT;
public:
    //! wskaźnik do węzła przechowywującego geometrię
	typedef osg::ref_ptr<osg::Geode> GeodePtr;
    //! wskaźnik do węzła przechowywującego grupę węzłów
	typedef osg::ref_ptr<osg::Group> GroupPtr;

public:
	//! Konstuktor
	//! \param visualizer wizualizator, który stworzył serie danych
	SkeletonSerie(KinematicVisualizer * visualizer);
	virtual ~SkeletonSerie() {}

private slots:
	//! zmiana osi, gdy otrzymamy obiekt w nietypowym ukł. współrzędnych
	//! \param xyz 
	void setAxis(bool xyz);

public:
    //! Ustawia nową nazwę serii
    //! \param name nowa nazwa
    virtual void setName(const std::string & name);
    //! \return nazwa serii
    virtual const std::string & getName() const;
    //! Ustawienie danych, inicjalizacja 
    //! \param data dane typu JointAnglesCollection
	virtual void setData(const core::ObjectWrapperConstPtr & data);
    //! \return ustawione dane
    virtual const core::ObjectWrapperConstPtr & getData() const;
	//! \return długość kanału w sekundach
	virtual double getLength() const;
	//! Czas zawiera się między 0 a getLength()
	//! \param time Aktualny, lokalny czas kanału w sekundach
	virtual void setLocalTime(double time);
    //! \return macierz serii z transformacja, która nie została zmieniona przez manipulatory
    osg::Matrix getInitialMatrix() const;

private:
    //! Dialog z trajektoriami działa dla MarkersCollection, dlatego należy dokonać konwersji
    //! \param joints struktura i dane animacji szkieletowej
    //! \return kolekcja z markerami, dla której można już używać dialogu TrajectoriesDialog
    MarkerCollectionConstPtr createTrajectories(kinematic::JointAnglesCollectionConstPtr joints);
    //! \return macierz potrzebna przy operacjach z różnymi ukl. współrzędnych
    osg::Matrix getXYZMatrix() const;
    //! niweluje działanie manipulatorów
    virtual void resetTransform();

private:
    //! Wizualizator, który utworzył serie
	KinematicVisualizer * visualizer;
    //! połączenie między aniamcja szieletowa (struktura + dane) a ich wizualizacja
	SkeletalVisualizationSchemePtr scheme;
    //! drawer schematu
	SchemeDrawerContainerPtr skeletonDrawers;
    //! wrapper przekazany serii
    core::ObjectWrapperConstPtr data;
    //! nazwa serii
    std::string name;
    //! główny wezej animacji
    TransformPtr skeletonNode;
    //! czy operujemy na układzie XYZ czy innym
    bool xyzAxis;
    //! Drawer rysujący trajektorie, powstały one dzięki konwersji JoinAnglesCollection - > MarkersCollection
    TrajectoryDrawerPtr trajectoryDrawer;
};
typedef boost::shared_ptr<SkeletonSerie> SkeletonSeriePtr;
typedef boost::shared_ptr<const SkeletonSerie> SkeletonSerieConstPtr;

#endif

