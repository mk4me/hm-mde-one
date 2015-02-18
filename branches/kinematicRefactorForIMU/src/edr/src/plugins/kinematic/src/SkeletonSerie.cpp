#include "PCH.h"
#include "SkeletonSerie.h"
#include <osgutils/OsgSchemeDrawer.h>
#include "TrajectoriesDrawer.h"
#include "SkeletalVisualizationSchemeHelper.h"
#include <threadingUtils/StreamData.h>
#include <kinematiclib/SkeletonState.h>
#include "osgManipulator/TranslateAxisDragger"

//static const osg::Quat invQXYZ = osg::Quat(osg::PI_2, osg::Vec3(1.0f, 0.0f, 0.0f)) * osg::Quat(osg::PI_2, osg::Vec3(0.0f, 0.0f, 1.0f));

SkeletonSerie::SkeletonSerie(KinematicVisualizer * visualizer, const utils::TypeInfo & requestedType, const core::VariantConstPtr & data) :
		AbstractSkeletonTimeSerie(visualizer, requestedType, data),
		name("Skeleton")
{
	data->getMetadata("core/name", name);
	skeletonWithStates = data->get();
	skeletonState.reset(new kinematic::SkeletonState(kinematic::SkeletonState::create(*skeletonWithStates->skeleton)));
	auto ratio = skeletonWithStates->scale;
	int pointsCount = skeletonWithStates->states->frames.front().size();
	AbstractSkeletonSerie::init(ratio, pointsCount, skeletonState, skeletonWithStates->nodesMapping);

//	joint2Index = kinematic::SkeletonState::createJoint2IndexMapping(*skeletonState, skeletonWithStates->nodesMapping);
//
//	localRootNode->setScale(osg::Vec3(ratio, ratio, ratio));
//	pointsDrawer->init(pointsCount);
//	pointsDrawer->setSize(0.02 / ratio);
//	pointsDrawer->setColor(osg::Vec4(1.0, 1.0, 0.0, 1.0));
//	localRootNode->addChild(pointsDrawer->getNode());
//	connections = kinematic::SkeletonState::createConnections(*skeletonState, joint2Index);
//	connectionsDrawer->init(connections);
//	connectionsDrawer->setColor(osg::Vec4(0.7, 0.7, 0.7, 0.5));
//	connectionsDrawer->setSize(0.05 / ratio);
//	localRootNode->addChild(connectionsDrawer->getNode());
//	localRootNode->computeLocalToWorldMatrix(lToW, nullptr);
//
//
//	rootPosition = osg::Vec3();// skeletonWithStates->getRootPosition(0.0);
//	matrixTransform->addChild(localRootNode);
//
//	pointsAxesDrawer.setLength(0.1);
//	pointsAxesDrawer.init(skeletonState->root());
//	pointsAxesDrawer.setVisible(false);
//	localRootNode->addChild(pointsAxesDrawer.getNode());

	setTime(0.0);

	//setAxis(true);

}



const kinematic::SkeletonState::NonRigidCompleteStateChange& SkeletonSerie::getStateChange()
{
	auto t = std::max(lastUpdateTime, 0.0);
	int frameNo = lastUpdateTime / skeletonWithStates->states->frameTime;
	int maxSize = skeletonWithStates->states->frames.size();
	frameNo = frameNo >= maxSize ? maxSize - 1 : frameNo;
	return skeletonWithStates->states->frames[frameNo];
}

void SkeletonSerie::update()
{
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

//void SkeletonSerie::setAxis(bool xyz)
//{
//	//TODO - sprawdzic jak to dziala
//	localRootNode->setAttitude(xyz == true ? invQXYZ : osg::Quat());
//
//	localRootNode->computeLocalToWorldMatrix(lToW, nullptr);
//}


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





