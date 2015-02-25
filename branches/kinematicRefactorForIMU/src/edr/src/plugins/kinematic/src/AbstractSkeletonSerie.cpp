#include "PCH.h"
#include "AbstractSkeletonSerie.h"
#include <osgutils/OsgSchemeDrawer.h>
#include "TrajectoriesDrawer.h"
#include "SkeletalVisualizationSchemeHelper.h"
#include <threadingUtils/StreamData.h>
#include <kinematiclib/SkeletonState.h>
#include "osgManipulator/TranslateAxisDragger"

static const osg::Quat invQXYZ = osg::Quat(osg::PI_2, osg::Vec3(1.0f, 0.0f, 0.0f)) * osg::Quat(osg::PI_2, osg::Vec3(0.0f, 0.0f, 1.0f));

AbstractSkeletonSerie::AbstractSkeletonSerie(KinematicVisualizer * visualizer,
	const utils::TypeInfo & requestedType,
	const core::VariantConstPtr & data) :
	visualizer(visualizer),
	data(data), requestedType(requestedType),
	lastUpdateTime(std::numeric_limits<double>::min()),
	xyzAxis(false),
	pointsDrawer(new osgutils::PointsDrawer(15)),
	connectionsDrawer(new osgutils::ConnectionsDrawer(15)),
	localRootNode(new osg::PositionAttitudeTransform)
{
}

void AbstractSkeletonSerie::init(double ratio, int pointsCount,
			kinematic::SkeletonStatePtr state, const kinematic::SkeletonState::LinearizedNodesMapping& mapping)
{
	this->skeletonState = state;
	this->nodesMapping = mapping;
	joint2Index = kinematic::SkeletonState::createJoint2IndexMapping(*state, mapping);
	localRootNode->setScale(osg::Vec3(ratio, ratio, ratio));
	pointsDrawer->init(pointsCount);
	pointsDrawer->setSize(0.02 / ratio);
	pointsDrawer->setColor(osg::Vec4(1.0, 1.0, 0.0, 1.0));
	localRootNode->addChild(pointsDrawer->getNode());
	connections = kinematic::SkeletonState::createConnections(*state, joint2Index);
	connectionsDrawer->init(connections);
	connectionsDrawer->setColor(osg::Vec4(0.7, 0.7, 0.7, 0.5));
	connectionsDrawer->setSize(0.05 / ratio);
	localRootNode->addChild(connectionsDrawer->getNode());
	localRootNode->computeLocalToWorldMatrix(lToW, nullptr);

	matrixTransform->addChild(localRootNode);

	//setTime(0.0);

	rootPosition = osg::Vec3();// skeletonWithStates->getRootPosition(0.0);

	//setAxis(true);

	pointsAxesDrawer.setLength(0.1);
	pointsAxesDrawer.init(state->root());
	pointsAxesDrawer.setVisible(false);
	localRootNode->addChild(pointsAxesDrawer.getNode());
}



void AbstractSkeletonSerie::setAxis(bool xyz)
{
	//TODO - sprawdzic jak to dziala
	localRootNode->setAttitude(xyz == true ? invQXYZ : osg::Quat());

	localRootNode->computeLocalToWorldMatrix(lToW, nullptr);
}


const core::VariantConstPtr & AbstractSkeletonSerie::getData() const
{
	return data;
}


const osg::Vec3 AbstractSkeletonSerie::pivotPoint() const
{
	return rootPosition * lToW;
}

void AbstractSkeletonSerie::update()
{
	const kinematic::SkeletonState::NonRigidCompleteStateChange& frame = getStateChange();
	std::vector<osg::Vec3> pos(joint2Index.size());
	kinematic::SkeletonState::update(*skeletonState, frame, nodesMapping);
	
	kinematic::SkeletonState::JointConstPtr root = skeletonState->root();
	
	auto visitor = [&](kinematic::SkeletonState::JointConstPtr node, kinematic::SkeletonState::Joint::size_type level)
		{
			pos[joint2Index.at(node)] = node->value.globalPosition();
		};
	utils::TreeNode::visitLevelOrder(root, visitor);

	pointsDrawer->update(pos);
	connectionsDrawer->update(pos);
	pointsAxesDrawer.update();
}


const utils::TypeInfo & AbstractSkeletonSerie::getRequestedDataType() const
{
	return requestedType;
}



void AbstractSkeletonSerie::setJointsOrientationsVisible()
{
	pointsAxesDrawer.setVisible(!pointsAxesDrawer.isVisible());
}


void PointsOrientationsDrawer::init(kinematic::SkeletonState::JointConstPtr root)
{
	if (pointAxes.empty()) {
		auto visitor = [&](kinematic::SkeletonState::JointConstPtr node, kinematic::SkeletonState::Joint::size_type level)
		{
			osg::ref_ptr<osgManipulator::TranslateAxisDragger> ne = new osgManipulator::TranslateAxisDragger();
			ne->setupDefaultGeometry();
			pointAxes.push_back(ne);
			localNode->addChild(ne);
		};
		utils::TreeNode::visitLevelOrder(root, visitor);
		this->root = root;
		setVisible(visible);
	} else {
		throw core::runtime_error("Cannot call init method twice");
	}
}

osg::ref_ptr<osg::Switch> PointsOrientationsDrawer::getNode()
{
	return localNode;
}

void PointsOrientationsDrawer::update()
{
	auto root = this->root.lock();
	if (root) {
		auto it = pointAxes.begin();
		auto visitor = [&](kinematic::SkeletonState::JointConstPtr node, kinematic::SkeletonState::Joint::size_type level)
		{
			auto ne = *(it++);
			osg::Matrix mat;
			mat.set(node->value.globalOrientation());
			mat.setTrans(node->value.globalPosition());
			mat.preMultScale(osg::Vec3(scale, scale, scale));
			ne->setMatrix(mat);
		};
		utils::TreeNode::visitLevelOrder(root, visitor);
	}
}

void PointsOrientationsDrawer::setLength(double length)
{
	this->scale = length;
}

void PointsOrientationsDrawer::setVisible(bool visible)
{
	this->visible = visible;
	if (visible == true) {
		localNode->setAllChildrenOn();
	} else {
		localNode->setAllChildrenOff();
	}

}

bool PointsOrientationsDrawer::isVisible()
{
	return this->visible;
}
