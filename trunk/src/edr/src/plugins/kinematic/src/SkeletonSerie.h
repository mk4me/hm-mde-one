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
#include "KinematicSerie.h"
#include <kinematiclib/JointAnglesCollection.h>
#include <plugins/kinematic/Wrappers.h>

class KinematicVisualizer;
class SkeletalVisualizationSchemeHelper;
class IPointsSchemeDrawer;
class IConnectionsSchemeDrawer;
class GhostSchemeDrawer;
class SkeletonJointsMapping;
class TrajectoryDrawerManager;

//! Seria danych wizualizatora 3D wizualizująca animacje szkieletowa
class SkeletonSerie :  public QObject, public KinematicTimeSerie, public IGhostSerie
{
	Q_OBJECT;

public:
	//! Konstuktor
	//! \param visualizer wizualizator, który stworzył serie danych
	SkeletonSerie(KinematicVisualizer * visualizer,
		const core::TypeInfo & requestedType, const core::ObjectWrapperConstPtr & data);

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
    virtual const std::string getName() const;
    //! \return ustawione dane
    virtual const core::ObjectWrapperConstPtr & getData() const;

	virtual void update();

	virtual const utils::TypeInfo & getRequestedDataType() const;
	//! \return długość kanału w sekundach
	virtual double getLength() const;
	virtual double getBegin() const;
	virtual double getEnd() const;
    //! \return macierz serii z transformacja, która nie została zmieniona przez manipulatory
    virtual osg::Matrix getInitialMatrix() const;

	virtual const osg::Quat orientation() const;

	virtual void setOrientation(const osg::Quat & orientation);

	virtual const osg::Vec3 position() const;

	virtual void setPosition(const osg::Vec3 & position);

	virtual const bool ghostVisible() const;
	virtual void setGhostVisible(const bool visible);

protected:
	//! Abstrakcyjny setter do czasu, metoda z inną sygnaturą może uchronić przed błędami
	//! \param time ustawiany czas
	virtual void setLocalTime(double time);

private:

	const std::vector<std::vector<osg::Vec3>> createPointsPositions(const unsigned int density) const;

    //! \return macierz potrzebna przy operacjach z różnymi ukl. współrzędnych
    static osg::Matrix getXYZMatrix();
    //! niweluje działanie manipulatorów
    virtual void resetTransform();

private:
	//! Aktualna dodatkowa rotacja wynikająca ze zmiany osi
	osg::Quat preRot;
	//! Pozycja wynikająca z położenia roota szkieletu
	osg::Vec3 rootPosition;
	//! Czas faktycznej aktualizacji
	double lastUpdateTime;
    //! Wizualizator, który utworzył serie
	KinematicVisualizer * visualizer;
    //! wrapper przekazany serii
    core::ObjectWrapperConstPtr data;
	//! Dane rozpakowane
	kinematic::JointAnglesCollectionConstPtr jointAngles;
	//! Typ danych
	utils::TypeInfo requestedType;
    //! nazwa serii
    std::string name;
    //! czy operujemy na układzie XYZ czy innym
    bool xyzAxis;
	//! Obiekt rysujący punkty
	core::shared_ptr<IPointsSchemeDrawer> pointsDrawer;
	//! Obiekt rysujący połączenia
	core::shared_ptr<IConnectionsSchemeDrawer> connectionsDrawer;
	//! Obiekt pomocniczny do mapowania jointów z danych do wizualizacji
	core::shared_ptr<SkeletonJointsMapping> jointsMapping;
	//! Klasa pomocnicza przy rysowaniu szkieletu
	core::shared_ptr<SkeletalVisualizationSchemeHelper> skeletonSchemeHelper;
	//! Klasa pomocnicza przy rysowaniu ducha
	core::shared_ptr<GhostSchemeDrawer> ghostDrawer;
	//! Klasa pomocnicza przy rysowaniu trajektorii
	core::shared_ptr<TrajectoryDrawerManager> trajectoriesManager;
};

//! Seria danych wizualizatora 3D wizualizująca animacje szkieletowa
class SkeletonStreamSerie :  public QObject, public KinematicSerie
{
	Q_OBJECT;

public:
	//! Konstuktor
	//! \param visualizer wizualizator, który stworzył serie danych
	SkeletonStreamSerie(KinematicVisualizer * visualizer,
		const core::TypeInfo & requestedType, const core::ObjectWrapperConstPtr & data);

	virtual ~SkeletonStreamSerie();

	private slots:
		//! zmiana osi, gdy otrzymamy obiekt w nietypowym ukł. współrzędnych
		//! \param xyz 
		void setAxis(bool xyz);

public:
	//! Ustawia nową nazwę serii
	//! \param name nowa nazwa
	virtual void setName(const std::string & name);
	//! \return nazwa serii
	virtual const std::string getName() const;
	//! \return ustawione dane
	virtual const core::ObjectWrapperConstPtr & getData() const;

	virtual void update();

	virtual const utils::TypeInfo & getRequestedDataType() const;
	//! \return macierz serii z transformacja, która nie została zmieniona przez manipulatory
	virtual osg::Matrix getInitialMatrix() const;

	virtual const osg::Quat orientation() const;

	virtual void setOrientation(const osg::Quat & orientation);

	virtual const osg::Vec3 position() const;

	virtual void setPosition(const osg::Vec3 & position);

	//! \return macierz potrzebna przy operacjach z różnymi ukl. współrzędnych
	static osg::Matrix getXYZMatrix();
	//! niweluje działanie manipulatorów
	virtual void resetTransform();

private:
	utils::StreamStatusObserverPtr updater;
	//! Aktualna dodatkowa rotacja wynikająca ze zmiany osi
	osg::Quat preRot;
	//! Pozycja wynikająca z położenia roota szkieletu
	osg::Vec3 rootPosition;
	//! Wizualizator, który utworzył serie
	KinematicVisualizer * visualizer;
	//! wrapper przekazany serii
	core::ObjectWrapperConstPtr data;
	//! Dane rozpakowane
	SkeletonDataStreamConstPtr skeletalData;
	//! Typ danych
	utils::TypeInfo requestedType;
	//! nazwa serii
	std::string name;
	//! czy operujemy na układzie XYZ czy innym
	bool xyzAxis;
	//! Obiekt rysujący punkty
	core::shared_ptr<IPointsSchemeDrawer> pointsDrawer;
	//! Obiekt rysujący połączenia
	core::shared_ptr<IConnectionsSchemeDrawer> connectionsDrawer;
};

typedef boost::shared_ptr<SkeletonSerie> SkeletonSeriePtr;
typedef boost::shared_ptr<const SkeletonSerie> SkeletonSerieConstPtr;

#endif

