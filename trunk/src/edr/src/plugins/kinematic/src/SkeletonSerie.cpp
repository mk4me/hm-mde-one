#include "PCH.h"
#include "SkeletonSerie.h"
#include "OsgSchemeDrawer.h"
#include "TrajectoriesDrawer.h"
#include "SkeletalVisualizationSchemeHelper.h"
#include <kinematiclib/JointAnglesCollection.h>

static const osg::Quat invQXYZ = osg::Quat(osg::PI_2, osg::Vec3(1.0f, 0.0f, 0.0f)) * osg::Quat(osg::PI_2, osg::Vec3(0.0f, 0.0f, 1.0f));

SkeletonSerie::SkeletonSerie( KinematicVisualizer * visualizer,
	const core::TypeInfo & requestedType,
	const core::ObjectWrapperConstPtr & data ) : 
	visualizer(visualizer),
	data(data), requestedType(requestedType),
	lastUpdateTime(std::numeric_limits<double>::min()),
    xyzAxis(false),
	name("Skeleton"),
	pointsDrawer(new PointsDrawer(3)),
	connectionsDrawer(new ConnectionsDrawer(10)),
	ghostDrawer(new GhostSchemeDrawer(3, 10)),
	jointsMapping(new SkeletonJointsMapping),
	trajectoriesManager(new TrajectoryDrawerManager)
{	
	UTILS_ASSERT(data->getTypeInfo() == typeid(kinematic::JointAnglesCollection));
	data->tryGetMeta("core/name", name);	
	jointAngles = data->get();

	std::vector<std::string> mapping;
	mapping.reserve(jointAngles->getNumChannels());

	for(unsigned int i = 0; i < jointAngles->getNumChannels(); ++i){
		mapping.push_back(jointAngles->getChannel(i)->getName());
	}

	jointsMapping->init(jointAngles->getHAnimSkeleton(), mapping);

	skeletonSchemeHelper.reset(new SkeletalVisualizationSchemeHelper(pointsDrawer.get(),
		connectionsDrawer.get()));

	auto connections = jointsMapping->generateMappedConnectionsDescription();

	skeletonSchemeHelper->init(jointsMapping.get(), connections);

	pointsDrawer->setSize(0.02);
	connectionsDrawer->setSize(0.005);

	pointsDrawer->setColor(osg::Vec4(1.0, 0.0, 0.0, 0.5));
	connectionsDrawer->setColor(osg::Vec4(0.0, 1.0, 0.0, 0.5));

	matrixTransform->addChild(pointsDrawer->getNode());
	matrixTransform->addChild(connectionsDrawer->getNode());

	//punkty dla ducha i trajektorii
	auto allPointsPositions = createPointsPositions(300);

	std::vector<std::vector<osg::Vec3>> pointsPositions(10);

	for(unsigned int i = 0; i < 10; ++i){
		pointsPositions[i] = allPointsPositions[i * 30];
	}

	ghostDrawer->init(pointsPositions, connections);
	ghostDrawer->pointsDrawer()->setColor(osg::Vec4(0.0f, 1.0f, 0.0f, 0.5f));
	ghostDrawer->connectionsDrawer()->setColor(osg::Vec4(1.0f, 0.0f, 0.0f, 0.5f));
	ghostDrawer->pointsDrawer()->setSize(0.02);
	ghostDrawer->connectionsDrawer()->setSize(0.005);

	matrixTransform->addChild(ghostDrawer->getNode());

	setGhostVisible(false);

	// teraz punkty dla ducha przerabiam na punkty dla trajektorii
	// przechodzę z klatek po czasie do klatek po stawach - generalnie transpozycja
	
	std::vector<std::vector<osg::Vec3>> trajectories(jointsMapping->mappedJointsNumber());

	for(auto it = allPointsPositions.begin(); it != allPointsPositions.end(); ++it){
		for(unsigned int i = 0; i < jointsMapping->mappedJointsNumber(); ++i){
			trajectories[i].push_back((*it)[i]);
		}
	}

	trajectoriesManager->initialize(trajectories);	
	trajectoriesManager->setVisible(false);
	trajectoriesManager->setColor(osg::Vec4(1.0, 0.0, 0.0, 0.5));
	matrixTransform->addChild(trajectoriesManager->getNode());

	setTime(0.0);

	rootPosition = jointAngles->getRootPosition(0.0);

	setAxis(true);
}

const std::vector<std::vector<osg::Vec3>> SkeletonSerie::createPointsPositions(const unsigned int density) const
{
	std::vector<std::vector<osg::Vec3>> ret(density);

	const double delta = jointAngles->getLength() / (double)density;

	double time = 0.0;

	for(unsigned int i = 0; i < density; ++i, time = delta * i){
		auto rotations = jointAngles->getValues(time);
		auto rootPosition = jointAngles->getRootPosition(time / jointAngles->getLength());
		std::vector<osg::Vec3> positions;
		skeletonSchemeHelper->calculatePointsPositions(positions,
			rootPosition,
			rotations);

		ret[i] = positions;
	}

	return ret;
}

const osg::Quat SkeletonSerie::orientation() const
{
	return preRot.inverse() * matrixTransform->getMatrix().getRotate();
}

void SkeletonSerie::setOrientation(const osg::Quat & orientation)
{
	auto q = preRot * orientation;
	auto m = matrixTransform->getMatrix();
	m.setRotate(q);
	matrixTransform->setMatrix(m);
}

const osg::Vec3 SkeletonSerie::position() const
{
	return rootPosition * matrixTransform->getMatrix();
}

void SkeletonSerie::setPosition(const osg::Vec3 & position)
{
	auto p = position - rootPosition;
	auto m = matrixTransform->getMatrix();

	m.setTrans(p * osg::Matrix::inverse(m));
	matrixTransform->setMatrix(m);
}

void SkeletonSerie::setAxis( bool xyz)
{
	//TODO - sprawdzic jak to dziala
    xyzAxis = xyz;
	auto o = orientation();
	if(xyz == true) {				
		preRot = invQXYZ;
	}else{
		preRot = invQXYZ.inverse();
	}

	setOrientation(o);
}

osg::Matrix SkeletonSerie::getXYZMatrix()
{
    return osg::Matrix(invQXYZ);
}

osg::Matrix SkeletonSerie::getInitialMatrix() const
{
  return xyzAxis ? getXYZMatrix() : osg::Matrix();
}

void SkeletonSerie::resetTransform()
{
    MatrixTransformPtr transform = getMatrixTransformNode();
    transform->setMatrix(getInitialMatrix());
}

void SkeletonSerie::setName( const std::string & name )
{
    this->name = name;
}

const std::string SkeletonSerie::getName() const
{
    return name;
}

const core::ObjectWrapperConstPtr & SkeletonSerie::getData() const
{
    return data;
}

double SkeletonSerie::getLength() const
{
    return jointAngles->getLength();
}

double SkeletonSerie::getBegin() const
{
	return 0.0;
}

double SkeletonSerie::getEnd() const
{
	return getLength();
}

void SkeletonSerie::update()
{
	auto t = std::max(lastUpdateTime, 0.0);

	auto rotations = jointAngles->getValues(lastUpdateTime);
	rootPosition = jointAngles->getRootPosition(lastUpdateTime / jointAngles->getLength());

	skeletonSchemeHelper->updateJointTransforms(rootPosition, rotations);
}

void SkeletonSerie::setLocalTime(double time)
{
	if( (lastUpdateTime == std::numeric_limits<double>::min()) ||
		(std::abs(time - lastUpdateTime) >= jointAngles->getChannel(0)->getSampleDuration())){
		lastUpdateTime = time;
		update();
	}
}

const utils::TypeInfo & SkeletonSerie::getRequestedDataType() const
{
	return requestedType;
}

const bool SkeletonSerie::ghostVisible() const
{
	return !(ghostDrawer->getNode()->getNodeMask() == 0);
}

void SkeletonSerie::setGhostVisible(const bool visible)
{
	ghostDrawer->getNode()->setNodeMask( visible == true ? 1 : 0);
}


