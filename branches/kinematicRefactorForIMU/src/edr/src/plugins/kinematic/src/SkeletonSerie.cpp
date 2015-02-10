#include "PCH.h"
#include "SkeletonSerie.h"
#include <osgutils/OsgSchemeDrawer.h>
#include "TrajectoriesDrawer.h"
#include "SkeletalVisualizationSchemeHelper.h"
#include <threadingUtils/StreamData.h>
#include <kinematiclib/SkeletonState.h>

static const osg::Quat invQXYZ = osg::Quat(osg::PI_2, osg::Vec3(1.0f, 0.0f, 0.0f)) * osg::Quat(osg::PI_2, osg::Vec3(0.0f, 0.0f, 1.0f));

SkeletonSerie::SkeletonSerie(KinematicVisualizer * visualizer,
	const utils::TypeInfo & requestedType,
	const core::VariantConstPtr & data) :
	visualizer(visualizer),
	data(data), requestedType(requestedType),
	lastUpdateTime(std::numeric_limits<double>::min()),
	xyzAxis(false),
	name("Skeleton"),
	pointsDrawer(new osgutils::PointsDrawer(3)),
	connectionsDrawer(new osgutils::ConnectionsDrawer(3)),
	localRootNode(new osg::PositionAttitudeTransform)
{
	data->getMetadata("core/name", name);
	skeletonWithStates = data->get();
	skeletonState.reset(new kinematic::SkeletonState(kinematic::SkeletonState::create(*skeletonWithStates->skeleton)));
	joint2Index = kinematic::SkeletonState::createJoint2IndexMapping(*skeletonState, skeletonWithStates->nodesMapping);
	auto ratio = skeletonWithStates->scale;
	localRootNode->setScale(osg::Vec3(ratio, ratio, ratio));
	pointsDrawer->init(skeletonWithStates->states->frames.front().size());
	pointsDrawer->setSize(0.02 / ratio);
	pointsDrawer->setColor(osg::Vec4(1.0, 1.0, 0.0, 1.0));
	localRootNode->addChild(pointsDrawer->getNode());
	connections = kinematic::SkeletonState::createConnections(*skeletonState, joint2Index);
	connectionsDrawer->init(connections);
	connectionsDrawer->setColor(osg::Vec4(0.7, 0.7, 0.7, 0.5));
	connectionsDrawer->setSize(0.05 / ratio);
	localRootNode->addChild(connectionsDrawer->getNode());
	localRootNode->computeLocalToWorldMatrix(lToW, nullptr);

	matrixTransform->addChild(localRootNode);

	setTime(0.0);

	rootPosition = osg::Vec3();// skeletonWithStates->getRootPosition(0.0);

	setAxis(true);
}

const std::vector<std::vector<osg::Vec3>> SkeletonSerie::createPointsPositions(const unsigned int density) const
{
	std::vector<std::vector<osg::Vec3>> ret(density);

	const double delta = this->getLength() / (double)density;

	double time = 0.0;
	kinematic::SkeletonState sstate = kinematic::SkeletonState::create(*this->skeletonWithStates->skeleton);
	auto j2i = kinematic::SkeletonState::createJoint2IndexMapping(sstate, skeletonWithStates->nodesMapping);
	for (unsigned int i = 0; i < density; ++i, time = delta * i) {
		int frameNo = time * this->skeletonWithStates->states->frameTime;
		auto& frame = skeletonWithStates->states->frames[frameNo];
		kinematic::SkeletonState::update(sstate, frame, skeletonWithStates->nodesMapping);
		std::vector<osg::Vec3> position(j2i.size());
		for (auto& j : j2i) {
			position[j.second] = j.first->value.globalPosition() + frame[0].translation;
		}
		ret[i] = position;
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
	return skeletonWithStates->states->getLength();
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
	std::vector<osg::Vec3> pos(joint2Index.size());
	int frameNo = lastUpdateTime / skeletonWithStates->states->frameTime;
	int maxSize = skeletonWithStates->states->frames.size();
	frameNo = frameNo >= maxSize ? maxSize - 1 : frameNo;
	auto& frame = skeletonWithStates->states->frames[frameNo];
	kinematic::SkeletonState::update(*skeletonState, frame, skeletonWithStates->nodesMapping);
	
	kinematic::SkeletonState::JointConstPtr root = skeletonState->root();
	auto visitor = [&](kinematic::SkeletonState::JointConstPtr node, kinematic::SkeletonState::Joint::size_type level)
		{
			pos[joint2Index.at(node)] = node->value.globalPosition();
		};
	kinematic::SkeletonState::Joint::visitLevelOrder(root, visitor);

	pointsDrawer->update(pos);
	connectionsDrawer->update(pos);
}

void SkeletonSerie::setLocalTime(double time)
{
	if ((lastUpdateTime == std::numeric_limits<double>::min()) ||
		(std::abs(time - lastUpdateTime) >= skeletonWithStates->states->frameTime)){
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
	ghostDrawer.reset(new osgutils::GhostSchemeDrawer(3, 10));

	//punkty dla ducha i trajektorii
	auto allPointsPositions = createPointsPositions(300);

	std::vector<std::vector<osg::Vec3>> pointsPositions(10);

	for (unsigned int i = 0; i < 10; ++i){
		pointsPositions[i] = allPointsPositions[i * 30];
	}

	ghostDrawer->init(pointsPositions, connections);
	ghostDrawer->pointsDrawer()->setColor(osg::Vec4(1.0f, 1.0f, 0.9f, 0.25f));
	ghostDrawer->connectionsDrawer()->setColor(osg::Vec4(1.0f, 1.0f, 0.9f, 0.25f));
	ghostDrawer->pointsDrawer()->setSize(0.05);
	ghostDrawer->connectionsDrawer()->setSize(0.01);
	ghostDrawer->getNode()->setNodeMask(false);
	
	localRootNode->addChild(ghostDrawer->getNode());

	// teraz punkty dla ducha przerabiam na punkty dla trajektorii
	// przechodzę z klatek po czasie do klatek po stawach - generalnie transpozycja
	auto jointCount = skeletonWithStates->nodesMapping.size();
	std::vector<std::vector<osg::Vec3>> trajectories(jointCount);
	
	for (auto& positions : allPointsPositions) {
		for (unsigned int i = 0; i < jointCount; ++i){
			trajectories[i].push_back(positions[i]);
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





