#include "PCH.h"
#include "SkeletonSerie.h"
#include <osgutils/OsgSchemeDrawer.h>
#include "TrajectoriesDrawer.h"
#include "SkeletalVisualizationSchemeHelper.h"
#include <threadingUtils/StreamData.h>
#include <kinematiclib/SkeletonState.h>
#include "osgManipulator/TranslateAxisDragger"


SkeletonSerie::SkeletonSerie(KinematicVisualizer * visualizer, const utils::TypeInfo & requestedType, const core::VariantConstPtr & data) :
		AbstractSkeletonTimeSerie(visualizer, requestedType, data),
		name("Skeleton")
{
	data->getMetadata("core/name", name);
	skeletonWithStates = data->get();
	skeletonState.reset(new kinematic::SkeletonState(kinematic::SkeletonState::create(*skeletonWithStates->skeleton)));
	auto ratio = skeletonWithStates->scale;
	//int pointsCount = skeletonWithStates->states->frames.front().size();
	int pointsCount =  skeletonWithStates->nodesMapping.left.size();
	AbstractSkeletonSerie::init(ratio, pointsCount, skeletonState, skeletonWithStates->nodesMapping);
	setTime(0.0);
	setAxis(true);

}



void SkeletonSerie::update()
{
	auto t = std::max(lastUpdateTime, 0.0);
	int frameNo = lastUpdateTime / skeletonWithStates->states->frameTime;
	int maxSize = skeletonWithStates->states->frames.size();
	frameNo = frameNo >= maxSize ? maxSize - 1 : frameNo;
	kinematic::SkeletonState::setLocal(*getSkeletonState(), skeletonWithStates->states->frames[frameNo], nodesMapping);
	AbstractSkeletonSerie::update();
}

const std::vector<std::vector<osg::Vec3>> SkeletonSerie::createPointsPositions(const unsigned int density) const
{
	std::vector<std::vector<osg::Vec3>> ret(density);

	const double delta = this->getLength() / (double)density;

	double time = 0.0;
	kinematic::SkeletonState sstate = kinematic::SkeletonState::create(*this->skeletonWithStates->skeleton);
	auto j2i = kinematic::SkeletonState::createJoint2IndexMapping(sstate, skeletonWithStates->nodesMapping);
	for (unsigned int i = 0; i < density; ++i, time = delta * i) {
		int frameNo = time / this->skeletonWithStates->states->frameTime;
		auto& frame = skeletonWithStates->states->frames[frameNo];
		kinematic::SkeletonState::setLocal(sstate, frame, skeletonWithStates->nodesMapping);
		std::vector<osg::Vec3> position(j2i.size());
		for (auto& j : j2i) {
			position[j.second] = (j.first->value.globalPosition()) * static_cast<float>(this->skeletonWithStates->scale);
		}
		ret[i] = position ;
	}
		
	return ret;
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
	ghostDrawer.reset(new osgutils::GhostSchemeDrawer(3, 3));

	//punkty dla ducha i trajektorii
	auto allPointsPositions = createPointsPositions(300);

	std::vector<std::vector<osg::Vec3>> pointsPositions(10);

	for (unsigned int i = 0; i < 10; ++i){
		pointsPositions[i] = allPointsPositions[i * 30];
	}

	ghostDrawer->init(pointsPositions,connections);
	ghostDrawer->pointsDrawer()->setColor(osg::Vec4(1.0f, 1.0f, 0.9f, 0.25f));
	ghostDrawer->connectionsDrawer()->setColor(osg::Vec4(1.0f, 1.0f, 0.9f, 0.25f));
	ghostDrawer->pointsDrawer()->setSize(0.02);
	ghostDrawer->connectionsDrawer()->setSize(0.005);
	ghostDrawer->getNode()->setNodeMask(false);

	localRootNode->addChild(ghostDrawer->getNode());

	// teraz punkty dla ducha przerabiam na punkty dla trajektorii
	// przechodzę z klatek po czasie do klatek po stawach - generalnie transpozycja

	int count = pointsPositions.begin()->size();
	std::vector<std::vector<osg::Vec3>> trajectories(count);

	for (auto it = allPointsPositions.begin(); it != allPointsPositions.end(); ++it){
		for (unsigned int i = 0; i < count; ++i){
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
