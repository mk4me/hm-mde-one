#include "Viewer.h"
#include "osgViewer/Viewer"
#include "acclaimformatslib/AsfParser.h"
#include "acclaimformatslib/AmcParser.h"
#include "acclaimformatslib/MotionData.h"
#include "kinematiclib/Skeleton.h"
#include "kinematiclib/SkeletonState.h"
#include "osg/Vec3"
#include <vector>
#include "osgutils/OsgSchemeDrawer.h"
#include "osgGA/TrackballManipulator"
#include "osg/PositionAttitudeTransform"

void kinematicTest::Viewer::start()
{
	acclaim::AsfParser asf;
	auto acclaimSkeleton = asf.parse("./testFiles/test.asf");

	acclaim::AmcParser amc;
	acclaim::MotionData acclaimData;
	amc.parse(acclaimData, "./testFiles/test.amc");

	kinematic::Skeleton skeleton;
	kinematic::Skeleton::convert(acclaimSkeleton, skeleton);
	kinematic::SkeletonState skeletonState = kinematic::SkeletonState::create(skeleton);
	

	const auto mapping = kinematic::SkeletonState::createMapping(skeleton);
	for (auto& p : mapping.right) {
		std::cout << p.first << std::string(", ") << p.second << std::endl;
	}
	auto joint2index = kinematic::SkeletonState::createJoint2IndexMapping(skeletonState, mapping);


	int framesCount = acclaimData.frames.size();
	osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform();
	osgViewer::Viewer viewer;
	osg::Quat att; att.makeRotate(90, 1, 0, 0);
	pat->setAttitude(att);
	float x = 0, y = 20, z = -2.5;
	float a = 0, b = 0, c = 0;

	viewer.setSceneData(pat);
	int frameIdx = 0;
	viewer.setUpViewInWindow(1090, 100, 500, 500);
	//viewer.run();
	osg::ref_ptr<osgGA::TrackballManipulator> tm = new osgGA::TrackballManipulator;
	osgutils::PointsDrawer pointsDrawer(3);
	pointsDrawer.init(mapping.size());
	pointsDrawer.setSize(0.1);
	pointsDrawer.setColor(osg::Vec4(1.0, 1.0, 0.0, 1.0));
	pat->addChild(pointsDrawer.getNode());

	auto sd = kinematic::SkeletonState::createConnections(skeletonState, joint2index);
	osgutils::ConnectionsSphereDrawer	connectionsDrawer(3);
	connectionsDrawer.init(sd);
	connectionsDrawer.setSize(0.04);
	connectionsDrawer.setColor(osg::Vec4(1.0, 1.0, 0.0, 1.0));
	pat->addChild(connectionsDrawer.getNode());

	viewer.setCameraManipulator(tm);
	viewer.realize();
	while (!viewer.done()) {
		viewer.frame();
		frameIdx = frameIdx >= (framesCount - 1) ? 0 : ++frameIdx;
		kinematic::SkeletonState::RigidPartialStateChange sChange = kinematic::SkeletonState::convert(acclaimSkeleton, acclaimData.frames[frameIdx], mapping);
		
		//auto frame = kinematic::SkeletonState::convertStateChange(mapping, sChange);
		
		kinematic::SkeletonState::setLocal(skeletonState, sChange, mapping);
		auto pos = getPos(skeletonState);
		std::transform(pos.begin(), pos.end(), pos.begin(), [&](const osg::Vec3& p) { return p + osg::Vec3(a, b, c); });
		pat->setPosition(osg::Vec3(x,y,z));
		pointsDrawer.update(pos);
		connectionsDrawer.update(pos);
	}
}

std::vector<osg::Vec3> kinematicTest::Viewer::getPos(kinematic::SkeletonState &skeletonState)
{
	kinematic::SkeletonState::JointConstPtr root = skeletonState.root();
	std::vector<osg::Vec3> pos;

	auto visitor = [&](kinematic::SkeletonState::JointConstPtr node, kinematic::SkeletonState::Joint::size_type level)
	{
		pos.push_back(node->value.globalPosition());
	};
	utils::TreeNode::visitLevelOrder(root, visitor);
	return pos;
}
