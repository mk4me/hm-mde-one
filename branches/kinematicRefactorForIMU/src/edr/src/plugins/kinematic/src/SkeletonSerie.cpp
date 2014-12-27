#include "PCH.h"
#include "SkeletonSerie.h"
#include "OsgSchemeDrawer.h"
#include "TrajectoriesDrawer.h"
#include "SkeletalVisualizationSchemeHelper.h"
#include <kinematiclib/JointAnglesCollection.h>
#include <threadingUtils/StreamData.h>

static const osg::Quat invQXYZ = osg::Quat(osg::PI_2, osg::Vec3(1.0f, 0.0f, 0.0f)) * osg::Quat(osg::PI_2, osg::Vec3(0.0f, 0.0f, 1.0f));

SkeletonSerie::SkeletonSerie(KinematicVisualizer * visualizer,
	const utils::TypeInfo & requestedType,
	const core::VariantConstPtr & data) :
	visualizer(visualizer),
	data(data), requestedType(requestedType),
	lastUpdateTime(std::numeric_limits<double>::min()),
	xyzAxis(false),
	name("Skeleton"),
	pointsDrawer(new PointsDrawer(3)),
	connectionsDrawer(new ConnectionsSphereDrawer(3)),
	jointsMapping(new SkeletonJointsMapping),
	localRootNode(new osg::PositionAttitudeTransform)
{
	UTILS_ASSERT(data->data()->getTypeInfo() == typeid(kinematic::JointAnglesCollection));
	data->getMetadata("core/name", name);
	jointAngles = data->get();

	std::vector<std::string> mapping;
	mapping.reserve(jointAngles->getNumChannels());

	for (unsigned int i = 0; i < jointAngles->getNumChannels(); ++i){
		mapping.push_back(jointAngles->getChannel(i)->getName());
	}

	jointsMapping->init(jointAngles->getSkeleton(), mapping);

	skeletonSchemeHelper.reset(new SkeletalVisualizationSchemeHelper(pointsDrawer.get(),
		connectionsDrawer.get()));

	auto connections = jointsMapping->generateMappedConnectionsDescription();

	skeletonSchemeHelper->init(jointsMapping.get(), connections);

	float ratio = jointAngles->getLengthRatio();
	if (ratio > 0) {
		pointsDrawer->setSize(0.02 / ratio);
		connectionsDrawer->setSize(0.005 / ratio);
		localRootNode->setScale(osg::Vec3(ratio, ratio, ratio));
	}
	pointsDrawer->setColor(osg::Vec4(1.0, 1.0, 0.0, 1.0));
	connectionsDrawer->setColor(osg::Vec4(0.7, 0.7, 0.7, 0.5));

	//localRootNode->addChild(pointsDrawer->getNode());
	localRootNode->addChild(connectionsDrawer->getNode());
	localRootNode->computeLocalToWorldMatrix(lToW, nullptr);

	matrixTransform->addChild(localRootNode);

	setTime(0.0);

	rootPosition = jointAngles->getRootPosition(0.0);

	setAxis(true);
}

const std::vector<std::vector<osg::Vec3>> SkeletonSerie::createPointsPositions(const unsigned int density) const
{
	std::vector<std::vector<osg::Vec3>> ret(density);

	const double delta = jointAngles->getLength() / (double)density;

	double time = 0.0;

	for (unsigned int i = 0; i < density; ++i, time = delta * i){
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

void SkeletonSerie::setAxis(bool xyz)
{
	//TODO - sprawdzic jak to dziala
	localRootNode->setAttitude(xyz == true ? invQXYZ : osg::Quat());

	localRootNode->computeLocalToWorldMatrix(lToW, nullptr);
}

void SkeletonSerie::setName(const std::string & name)
{
	this->name = name;
}

const std::string SkeletonSerie::getName() const
{
	return name;
}

const core::VariantConstPtr & SkeletonSerie::getData() const
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

const osg::Vec3 SkeletonSerie::pivotPoint() const
{
	return rootPosition * lToW;
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
	if ((lastUpdateTime == std::numeric_limits<double>::min()) ||
		(std::abs(time - lastUpdateTime) >= jointAngles->getChannel(0)->getSampleDuration())){
		lastUpdateTime = time;
		requestUpdate();
	}
}

const utils::TypeInfo & SkeletonSerie::getRequestedDataType() const
{
	return requestedType;
}

const bool SkeletonSerie::ghostVisible() const
{
	if (ghostDrawer == nullptr){
		return false;
	}

	return !(ghostDrawer->getNode()->getNodeMask() == 0);
}

void SkeletonSerie::createGhostAndTrajectories()
{
	trajectoriesManager.reset(new TrajectoryDrawerManager);
	ghostDrawer.reset(new GhostSchemeDrawer(3, 10));

	//punkty dla ducha i trajektorii
	auto allPointsPositions = createPointsPositions(300);

	std::vector<std::vector<osg::Vec3>> pointsPositions(10);

	for (unsigned int i = 0; i < 10; ++i){
		pointsPositions[i] = allPointsPositions[i * 30];
	}

	ghostDrawer->init(pointsPositions,
		jointsMapping->generateMappedConnectionsDescription());
	ghostDrawer->pointsDrawer()->setColor(osg::Vec4(1.0f, 1.0f, 0.9f, 0.25f));
	ghostDrawer->connectionsDrawer()->setColor(osg::Vec4(1.0f, 1.0f, 0.9f, 0.25f));
	ghostDrawer->pointsDrawer()->setSize(0.02);
	ghostDrawer->connectionsDrawer()->setSize(0.005);
	ghostDrawer->getNode()->setNodeMask(false);

	localRootNode->addChild(ghostDrawer->getNode());

	// teraz punkty dla ducha przerabiam na punkty dla trajektorii
	// przechodzę z klatek po czasie do klatek po stawach - generalnie transpozycja

	std::vector<std::vector<osg::Vec3>> trajectories(jointsMapping->mappedJointsNumber());

	for (auto it = allPointsPositions.begin(); it != allPointsPositions.end(); ++it){
		for (unsigned int i = 0; i < jointsMapping->mappedJointsNumber(); ++i){
			trajectories[i].push_back((*it)[i]);
		}
	}

	trajectoriesManager->initialize(trajectories);
	trajectoriesManager->setVisible(false);
	trajectoriesManager->setColor(osg::Vec4(1.0, 0.0, 0.0, 0.5));
	localRootNode->addChild(trajectoriesManager->getNode());
}

void SkeletonSerie::setGhostVisible(const bool visible)
{
	if (visible == true && ghostDrawer == nullptr){
		createGhostAndTrajectories();
	}

	ghostDrawer->getNode()->setNodeMask(visible == true ? 1 : 0);
}

utils::shared_ptr<TrajectoryDrawerManager> SkeletonSerie::getTrajectoriesManager()
{
	if (!trajectoriesManager) {
		createGhostAndTrajectories();
	}
	return trajectoriesManager;
}

//---------------------------------------------------------------------

class SkeletonStreamSerieUpdater : public threadingUtils::IStreamStatusObserver
{
public:
	SkeletonStreamSerieUpdater(SkeletonStreamSerie * serie)
		: serie(serie)
	{
	}

	virtual ~SkeletonStreamSerieUpdater() {}

	//! Metoda wołana kiedy faktycznie stan strumienia się zmieni
	virtual void update()
	{
		//serie->requestUpdate();
		serie->myUpdate();
	}

private:

	SkeletonStreamSerie * serie;
};

SkeletonStreamSerie::SkeletonStreamSerie(KinematicVisualizer * visualizer,
	const utils::TypeInfo & requestedType,
	const core::VariantConstPtr & data) :
	visualizer(visualizer),
	data(data), requestedType(requestedType),
	xyzAxis(false),
	name("SkeletonData"),
	pointsDrawer(new PointsDrawer(3)),
	connectionsDrawer(new ConnectionsSphereDrawer(10)),
	heightCompensation(false),
	localRootNode(new osg::PositionAttitudeTransform)
{
	UTILS_ASSERT(data->data()->getTypeInfo() == typeid(SkeletonDataStream));
	data->getMetadata("core/name", name);
	skeletalData = data->get();

	pointsDrawer->init(skeletalData->jointsCount);
	pointsDrawer->setSize(0.02);

	connectionsDrawer->init(skeletalData->connections);
	connectionsDrawer->setSize(0.005);

	pointsDrawer->setColor(osg::Vec4(1.0, 0.0, 0.0, 0.5));
	connectionsDrawer->setColor(osg::Vec4(0.0, 1.0, 0.0, 0.5));

	localRootNode->addChild(pointsDrawer->getNode());
	localRootNode->addChild(connectionsDrawer->getNode());
	localRootNode->computeLocalToWorldMatrix(lToW, nullptr);

	matrixTransform->addChild(localRootNode);

	//setAxis(true);

	updater.reset(new SkeletonStreamSerieUpdater(this));

	skeletalData->jointsStream->attachObserver(updater);
}

SkeletonStreamSerie::~SkeletonStreamSerie()
{
	if (skeletalData != nullptr){
		skeletalData->jointsStream->detachObserver(updater);
	}
}

void SkeletonStreamSerie::setAxis(bool xyz)
{
	//TODO - sprawdzic jak to dziala
	xyzAxis = xyz;
	localRootNode->setAttitude(xyz == true ? invQXYZ : osg::Quat());
	localRootNode->computeLocalToWorldMatrix(lToW, nullptr);
}

void SkeletonStreamSerie::setName(const std::string & name)
{
	this->name = name;
}

const std::string SkeletonStreamSerie::getName() const
{
	return name;
}

const core::VariantConstPtr & SkeletonStreamSerie::getData() const
{
	return data;
}

void SkeletonStreamSerie::update()
{
	std::vector<osg::Vec3> points;
	skeletalData->jointsStream->data(points);

	if (heightCompensation == false){
		heightCompensation = true;
		float minZ = std::numeric_limits<float>::max();
		for (auto it = points.begin(); it != points.end(); ++it){
			minZ = std::min(minZ, (*it).z());
		}

		minZ -= pointsDrawer->size(0);

		localRootNode->setPosition(osg::Vec3(0, 0, -minZ));
		localRootNode->computeLocalToWorldMatrix(lToW, nullptr);
	}

	rootPosition = osg::Vec3(0, 0, 0);

	for (auto it = points.begin(); it != points.end(); ++it){
		rootPosition += *it;
	}

	rootPosition /= points.size();

	pointsDrawer->update(points);
	connectionsDrawer->update(points);
}

const utils::TypeInfo & SkeletonStreamSerie::getRequestedDataType() const
{
	return requestedType;
}

const osg::Vec3 SkeletonStreamSerie::pivotPoint() const
{
	return rootPosition * lToW;
}