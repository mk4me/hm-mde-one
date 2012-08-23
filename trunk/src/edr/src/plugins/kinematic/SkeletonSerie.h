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

//! Seria danych wizualizatora 3D wizualizuj¹ca animacje szkieletowa
class SkeletonSerie :  public QObject, public KinematicSerie
{
	Q_OBJECT;
public:
    //! wskaŸnik do wêz³a przechowywuj¹cego geometriê
	typedef osg::ref_ptr<osg::Geode> GeodePtr;
    //! wskaŸnik do wêz³a przechowywuj¹cego grupê wêz³ów
	typedef osg::ref_ptr<osg::Group> GroupPtr;

public:
	//! Konstuktor
	//! \param visualizer wizualizator, który stworzy³ serie danych
	SkeletonSerie(KinematicVisualizer * visualizer);
	virtual ~SkeletonSerie() {}

private slots:
	//! zmiana osi, gdy otrzymamy obiekt w nietypowym uk³. wspó³rzêdnych
	//! \param xyz 
	void setAxis(bool xyz);

public:
    //! Ustawia now¹ nazwê serii
    //! \param name nowa nazwa
    virtual void setName(const std::string & name);
    //! \return nazwa serii
    virtual const std::string & getName() const;
    //! Ustawienie danych, inicjalizacja 
    //! \param data dane typu JointAnglesCollection
	virtual void setData(const core::ObjectWrapperConstPtr & data);
    //! \return ustawione dane
    virtual const core::ObjectWrapperConstPtr & getData() const;
	//! \return d³ugoœæ kana³u w sekundach
	virtual double getLength() const;
	//! Czas zawiera siê miêdzy 0 a getLength()
	//! \param time Aktualny, lokalny czas kana³u w sekundach
	virtual void setLocalTime(double time);
    //! \return macierz serii z transformacja, która nie zosta³a zmieniona przez manipulatory
    osg::Matrix getInitialMatrix() const;

private:
    //! Dialog z trajektoriami dzia³a dla MarkersCollection, dlatego nale¿y dokonaæ konwersji
    //! \param joints struktura i dane animacji szkieletowej
    //! \return kolekcja z markerami, dla której mo¿na ju¿ u¿ywaæ dialogu TrajectoriesDialog
    MarkerCollectionConstPtr createTrajectories(kinematic::JointAnglesCollectionConstPtr joints);
    //! \return macierz potrzebna przy operacjach z ró¿nymi ukl. wspó³rzêdnych
    osg::Matrix getXYZMatrix() const;
    //! niweluje dzia³anie manipulatorów
    virtual void resetTransform();

private:
    //! Wizualizator, który utworzy³ serie
	KinematicVisualizer * visualizer;
    //! po³¹czenie miêdzy aniamcja szieletowa (struktura + dane) a ich wizualizacja
	SkeletalVisualizationSchemePtr scheme;
    //! drawer schematu
	SchemeDrawerContainerPtr skeletonDrawers;
    //! wrapper przekazany serii
    core::ObjectWrapperConstPtr data;
    //! nazwa serii
    std::string name;
    //! g³ówny wezej animacji
    TransformPtr skeletonNode;
    //! czy operujemy na uk³adzie XYZ czy innym
    bool xyzAxis;
    //! Drawer rysujacy trajektorie, powsta³y one dziêki konwersji JoinAnglesCollection - > MarkersCollection
    TrajectoryDrawerPtr trajectoryDrawer;
};
typedef boost::shared_ptr<SkeletonSerie> SkeletonSeriePtr;
typedef boost::shared_ptr<const SkeletonSerie> SkeletonSerieConstPtr;

#endif

