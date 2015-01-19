#include "PCH.h"
#include "SkeletonSerie2.h"
#include "OsgSchemeDrawer.h"
#include "TrajectoriesDrawer.h"
#include "SkeletalVisualizationSchemeHelper.h"
#include <kinematiclib/JointAnglesCollection.h>
#include <threadingUtils/StreamData.h>

static const osg::Quat invQXYZ = osg::Quat(osg::PI_2, osg::Vec3(1.0f, 0.0f, 0.0f)) * osg::Quat(osg::PI_2, osg::Vec3(0.0f, 0.0f, 1.0f));

SkeletonSerie2::SkeletonSerie2(KinematicVisualizer * visualizer,
	const utils::TypeInfo & requestedType,
	const core::VariantConstPtr & data) :
	visualizer(visualizer),
	data(data), requestedType(requestedType),
	lastUpdateTime(std::numeric_limits<double>::min()),
	xyzAxis(false),
	name("Skeleton"),
	pointsDrawer(new PointsDrawer(3)),
	connectionsDrawer(new ConnectionsDrawer(3)),
	localRootNode(new osg::PositionAttitudeTransform)
{
	data->getMetadata("core/name", name);
	skeletonWithStates = data->get();
		
	auto ratio = 1.0;
	//pointsDrawer->init(skeletonWithStates->states->jointNames.size());
	pointsDrawer->init(skeletonWithStates->states->frames.front().size());
	pointsDrawer->setSize(0.02 / ratio);
	//connectionsDrawer->setSize(0.05 / ratio);
	localRootNode->setScale(osg::Vec3(ratio, ratio, ratio));
	pointsDrawer->setColor(osg::Vec4(1.0, 1.0, 0.0, 1.0));
	//connectionsDrawer->setColor(osg::Vec4(0.7, 0.7, 0.7, 0.5));

	localRootNode->addChild(pointsDrawer->getNode());
	//localRootNode->addChild(connectionsDrawer->getNode());
	localRootNode->computeLocalToWorldMatrix(lToW, nullptr);

	matrixTransform->addChild(localRootNode);

	setTime(0.0);

	rootPosition = osg::Vec3();// skeletonWithStates->getRootPosition(0.0);

	setAxis(true);
}

const std::vector<std::vector<osg::Vec3>> SkeletonSerie2::createPointsPositions(const unsigned int density) const
{
	std::vector<std::vector<osg::Vec3>> ret(density);

	
	return ret;
}

void SkeletonSerie2::setAxis(bool xyz)
{
	//TODO - sprawdzic jak to dziala
	localRootNode->setAttitude(xyz == true ? invQXYZ : osg::Quat());

	localRootNode->computeLocalToWorldMatrix(lToW, nullptr);
}

void SkeletonSerie2::setName(const std::string & name)
{
	this->name = name;
}

const std::string SkeletonSerie2::getName() const
{
	return name;
}

const core::VariantConstPtr & SkeletonSerie2::getData() const
{
	return data;
}

double SkeletonSerie2::getLength() const
{
	return skeletonWithStates->states->getLength();
}

double SkeletonSerie2::getBegin() const
{
	return 0.0;
}

double SkeletonSerie2::getEnd() const
{
	return getLength();
}

const osg::Vec3 SkeletonSerie2::pivotPoint() const
{
	return rootPosition * lToW;
}

void SkeletonSerie2::update()
{
	auto t = std::max(lastUpdateTime, 0.0);
	std::vector<osg::Vec3> pos;
	const kinematic::Skeleton& skeleton = *skeletonWithStates->skeleton;
	kinematic::SkeletonState ss = kinematic::SkeletonState::create(skeleton);
	int frameNo = lastUpdateTime / skeletonWithStates->states->frameTime;
	auto frame = skeletonWithStates->states->frames[frameNo];
	kinematic::SkeletonState::update(ss,frame);
	kinematic::SkeletonState::Joint::visitLevelOrder(ss.root(), [&pos](kinematic::SkeletonState::JointConstPtr node, kinematic::SkeletonState::Joint::size_type level)
	{				
		pos.push_back(node->value.globalPosition());		
	});

	pointsDrawer->update(pos);
//	auto rotations = skeletonWithStates->getValues(lastUpdateTime);
//	rootPosition = skeletonWithStates->getRootPosition(lastUpdateTime / skeletonWithStates->getLength());
//
//	skeletonSchemeHelper->updateJointTransforms(rootPosition, rotations);
}

void SkeletonSerie2::setLocalTime(double time)
{
	if ((lastUpdateTime == std::numeric_limits<double>::min()) ||
		(std::abs(time - lastUpdateTime) >= skeletonWithStates->states->frameTime)){
		lastUpdateTime = time;
		requestUpdate();
	}
}

const utils::TypeInfo & SkeletonSerie2::getRequestedDataType() const
{
	return requestedType;
}

const bool SkeletonSerie2::ghostVisible() const
{
	if (ghostDrawer == nullptr){
		return false;
	}

	return !(ghostDrawer->getNode()->getNodeMask() == 0);
}

void SkeletonSerie2::createGhostAndTrajectories()
{
	trajectoriesManager.reset(new TrajectoryDrawerManager);
	ghostDrawer.reset(new GhostSchemeDrawer(3, 10));

	//punkty dla ducha i trajektorii
	auto allPointsPositions = createPointsPositions(300);

	std::vector<std::vector<osg::Vec3>> pointsPositions(10);

	for (unsigned int i = 0; i < 10; ++i){
		pointsPositions[i] = allPointsPositions[i * 30];
	}

	//ghostDrawer->init(pointsPositions,
	//jointsMapping->generateMappedConnectionsDescription());
	//ghostDrawer->pointsDrawer()->setColor(osg::Vec4(1.0f, 1.0f, 0.9f, 0.25f));
	//ghostDrawer->connectionsDrawer()->setColor(osg::Vec4(1.0f, 1.0f, 0.9f, 0.25f));
	//ghostDrawer->pointsDrawer()->setSize(0.02);
	//ghostDrawer->connectionsDrawer()->setSize(0.005);
	//ghostDrawer->getNode()->setNodeMask(false);
	//
	//localRootNode->addChild(ghostDrawer->getNode());

	// teraz punkty dla ducha przerabiam na punkty dla trajektorii
	// przechodzę z klatek po czasie do klatek po stawach - generalnie transpozycja

	/*std::vector<std::vector<osg::Vec3>> trajectories(jointsMapping->mappedJointsNumber());
	
	for (auto it = allPointsPositions.begin(); it != allPointsPositions.end(); ++it){
		for (unsigned int i = 0; i < jointsMapping->mappedJointsNumber(); ++i){
			trajectories[i].push_back((*it)[i]);
		}
	}
	
	trajectoriesManager->initialize(trajectories);
	trajectoriesManager->setVisible(false);
	trajectoriesManager->setColor(osg::Vec4(1.0, 0.0, 0.0, 0.5));
	localRootNode->addChild(trajectoriesManager->getNode());*/
}

void SkeletonSerie2::setGhostVisible(const bool visible)
{
	if (visible == true && ghostDrawer == nullptr){
		createGhostAndTrajectories();
	}

	ghostDrawer->getNode()->setNodeMask(visible == true ? 1 : 0);
}

utils::shared_ptr<TrajectoryDrawerManager> SkeletonSerie2::getTrajectoriesManager()
{
	if (!trajectoriesManager) {
		createGhostAndTrajectories();
	}
	return trajectoriesManager;
}
