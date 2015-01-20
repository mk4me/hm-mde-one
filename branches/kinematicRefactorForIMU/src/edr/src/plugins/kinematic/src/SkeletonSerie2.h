/********************************************************************
	created:	2011/09/04
	created:	4:9:2011   14:24
	filename: 	SkeletonSerie2.h
	author:		Wojciech Kniec

	purpose:
	*********************************************************************/

#ifndef HEADER_GUARD_KINEMATIC__SkeletonSerie2_H__
#define HEADER_GUARD_KINEMATIC__SkeletonSerie2_H__

#include <corelib/IVisualizer.h>
#include "KinematicSerie.h"
#include <kinematiclib/JointAnglesCollection.h>
#include <plugins/kinematic/Wrappers.h>
#include <osg/PositionAttitudeTransform>
#include "TrajectoriesDrawer.h"
#include <osgAnimation/Bone>

class KinematicVisualizer;
class SkeletalVisualizationSchemeHelper;
class IPointsSchemeDrawer;
class IConnectionsSchemeDrawer;
class GhostSchemeDrawer;
class SkeletonJointsMapping;
class TrajectoryDrawerManager;

//! Seria danych wizualizatora 3D wizualizująca animacje szkieletowa
class SkeletonSerie2 : public QObject, public KinematicTimeSerie, public IGhostSerie
{
	Q_OBJECT;

public:
	//! Konstuktor
	//! \param visualizer wizualizator, który stworzył serie danych
	SkeletonSerie2(KinematicVisualizer * visualizer,
		const utils::TypeInfo & requestedType, const core::VariantConstPtr & data);

	virtual ~SkeletonSerie2() {}

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
	virtual const core::VariantConstPtr & getData() const;

	virtual void update();

	virtual const osg::Vec3 pivotPoint() const;

	virtual const utils::TypeInfo & getRequestedDataType() const;
	//! \return długość kanału w sekundach
	virtual double getLength() const;
	virtual double getBegin() const;
	virtual double getEnd() const;

	virtual const bool ghostVisible() const;
	virtual void setGhostVisible(const bool visible);

	utils::shared_ptr<TrajectoryDrawerManager> getTrajectoriesManager();


protected:
	//! Abstrakcyjny setter do czasu, metoda z inną sygnaturą może uchronić przed błędami
	//! \param time ustawiany czas
	virtual void setLocalTime(double time);

private:

	const std::vector<std::vector<osg::Vec3>> createPointsPositions(const unsigned int density) const;

	void createGhostAndTrajectories();
	SegmentsDescriptors createConnections(const kinematic::SkeletonState& skeleton);

	std::map<kinematic::SkeletonState::JointConstPtr, unsigned int> createJoint2IndexMapping(const kinematic::SkeletonState &skeleton) const;

private:

	osg::Matrix lToW;

	osg::ref_ptr<osg::PositionAttitudeTransform> localRootNode;
	//! Aktualna dodatkowa rotacja wynikająca ze zmiany osi
	osg::Quat preRot;
	//! Pozycja wynikająca z położenia roota szkieletu
	osg::Vec3 rootPosition;
	//! Czas faktycznej aktualizacji
	double lastUpdateTime;
	//! Wizualizator, który utworzył serie
	KinematicVisualizer * visualizer;
	//! wrapper przekazany serii
	core::VariantConstPtr data;
	//! Dane rozpakowane
	SkeletonWithStatesConstPtr skeletonWithStates;
	//! Stan szieletu na podstawie danych z OW
	utils::scoped_ptr<kinematic::SkeletonState> skeletonState;
	//! Typ danych
	utils::TypeInfo requestedType;
	//! nazwa serii
	std::string name;
	//! czy operujemy na układzie XYZ czy innym
	bool xyzAxis;
	//! Obiekt rysujący punkty
	utils::shared_ptr<IPointsSchemeDrawer> pointsDrawer;
	//! Obiekt rysujący połączenia
	utils::shared_ptr<IConnectionsSchemeDrawer> connectionsDrawer;
	//! Klasa pomocnicza przy rysowaniu ducha
	utils::shared_ptr<GhostSchemeDrawer> ghostDrawer;
	//! Klasa pomocnicza przy rysowaniu trajektorii
	TrajectoryDrawerManagerPtr trajectoriesManager;
	//! stworzone połączenia między punktami
	SegmentsDescriptors connections;
	//! mapowanie joint -> index
	std::map<kinematic::SkeletonState::JointConstPtr, unsigned int> joint2Index;
};



#endif
