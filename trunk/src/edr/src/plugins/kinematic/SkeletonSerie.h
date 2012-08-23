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

//! Seria danych wizualizatora 3D wizualizuj�ca animacje szkieletowa
class SkeletonSerie :  public QObject, public KinematicSerie
{
	Q_OBJECT;
public:
    //! wska�nik do w�z�a przechowywuj�cego geometri�
	typedef osg::ref_ptr<osg::Geode> GeodePtr;
    //! wska�nik do w�z�a przechowywuj�cego grup� w�z��w
	typedef osg::ref_ptr<osg::Group> GroupPtr;

public:
	//! Konstuktor
	//! \param visualizer wizualizator, kt�ry stworzy� serie danych
	SkeletonSerie(KinematicVisualizer * visualizer);
	virtual ~SkeletonSerie() {}

private slots:
	//! zmiana osi, gdy otrzymamy obiekt w nietypowym uk�. wsp�rz�dnych
	//! \param xyz 
	void setAxis(bool xyz);

public:
    //! Ustawia now� nazw� serii
    //! \param name nowa nazwa
    virtual void setName(const std::string & name);
    //! \return nazwa serii
    virtual const std::string & getName() const;
    //! Ustawienie danych, inicjalizacja 
    //! \param data dane typu JointAnglesCollection
	virtual void setData(const core::ObjectWrapperConstPtr & data);
    //! \return ustawione dane
    virtual const core::ObjectWrapperConstPtr & getData() const;
	//! \return d�ugo�� kana�u w sekundach
	virtual double getLength() const;
	//! Czas zawiera si� mi�dzy 0 a getLength()
	//! \param time Aktualny, lokalny czas kana�u w sekundach
	virtual void setLocalTime(double time);
    //! \return macierz serii z transformacja, kt�ra nie zosta�a zmieniona przez manipulatory
    osg::Matrix getInitialMatrix() const;

private:
    //! Dialog z trajektoriami dzia�a dla MarkersCollection, dlatego nale�y dokona� konwersji
    //! \param joints struktura i dane animacji szkieletowej
    //! \return kolekcja z markerami, dla kt�rej mo�na ju� u�ywa� dialogu TrajectoriesDialog
    MarkerCollectionConstPtr createTrajectories(kinematic::JointAnglesCollectionConstPtr joints);
    //! \return macierz potrzebna przy operacjach z r�nymi ukl. wsp�rz�dnych
    osg::Matrix getXYZMatrix() const;
    //! niweluje dzia�anie manipulator�w
    virtual void resetTransform();

private:
    //! Wizualizator, kt�ry utworzy� serie
	KinematicVisualizer * visualizer;
    //! po��czenie mi�dzy aniamcja szieletowa (struktura + dane) a ich wizualizacja
	SkeletalVisualizationSchemePtr scheme;
    //! drawer schematu
	SchemeDrawerContainerPtr skeletonDrawers;
    //! wrapper przekazany serii
    core::ObjectWrapperConstPtr data;
    //! nazwa serii
    std::string name;
    //! g��wny wezej animacji
    TransformPtr skeletonNode;
    //! czy operujemy na uk�adzie XYZ czy innym
    bool xyzAxis;
    //! Drawer rysujacy trajektorie, powsta�y one dzi�ki konwersji JoinAnglesCollection - > MarkersCollection
    TrajectoryDrawerPtr trajectoryDrawer;
};
typedef boost::shared_ptr<SkeletonSerie> SkeletonSeriePtr;
typedef boost::shared_ptr<const SkeletonSerie> SkeletonSerieConstPtr;

#endif

