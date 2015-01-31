#include <limits>
#include <iostream>
#include <vector>
#include <fstream>
#include <istream>
#include <string>
#include <map>
#include <boost/smart_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/smart_ptr.hpp>
#include <osg/Vec3d>
#include <osg/Quat>
#include <boost/shared_ptr.hpp>
#include <list>
#include <boost/filesystem.hpp>
#include "SkeletonStateTest.h"
#include "acclaimformatslib/AsfParser.h"
#include "acclaimformatslib/Skeleton.h"
#include "utils/Debug.h"
#include "acclaimformatslib/AmcParser.h"
#include "kinematicUtils/RotationConverter.h"

const double epsilon = 0.00001;



SkeletonStateTest::SkeletonStateTest(void) : 
	acclaimSkeleton(nullptr),
	acclaimData(new acclaim::MotionData())
{

}

SkeletonStateTest::~SkeletonStateTest(void)
{

}

void SkeletonStateTest::setUp()
{
	acclaim::AsfParser asf;
	acclaimSkeleton.reset(new acclaim::Skeleton(asf.parse("./testFiles/test.asf")));

	acclaim::AmcParser amc;
	amc.parse(*acclaimData, "./testFiles/test.amc");
}

void SkeletonStateTest::tearDown()
{

}


void SkeletonStateTest::testMapping()
{
	kinematic::Skeleton skeleton;
	kinematic::Skeleton::convert(*this->acclaimSkeleton, skeleton);
	kinematic::SkeletonState::LinearizedNodesMapping mapping = kinematic::SkeletonState::createMapping(skeleton);
	for (auto& p : mapping.left) {
		UTILS_DEBUG_PRINT("%d - > %s", p.first, p.second.c_str());
	}
	CPPUNIT_ASSERT_EQUAL(27, (int)mapping.left.size());
	CPPUNIT_ASSERT_EQUAL(27, (int)mapping.right.size());
	CPPUNIT_ASSERT_EQUAL(std::string("root"), mapping.left.at(0));
	CPPUNIT_ASSERT_EQUAL(0u, mapping.right.at("root"));
}

// tworzone mapowanie w const auto mapping = kinematic::SkeletonState::createMapping(skeleton);
// 0 - > root
// 1 - > lhipjoint
// 2 - > rhipjoint
// 3 - > lowerback
// 4 - > lfemur
// 5 - > rfemur
// 6 - > upperback
// 7 - > ltibia
// 8 - > rtibia
// 9 - > thorax
// 10 - > lfoot
// 11 - > rfoot
// 12 - > lowerneck
// 13 - > lshoulderjoint
// 14 - > rshoulderjoint
// 15 - > ltoes
// 16 - > rtoes
// 17 - > upperneck
// 18 - > lhumerus
// 19 - > rhumerus
// 20 - > head
// 21 - > lradius
// 22 - > rradius
// 23 - > lwrist
// 24 - > rwrist
// 25 - > lhand
// 26 - > rhand

void SkeletonStateTest::testFrameConvert()
{
	kinematic::Skeleton skeleton;
	kinematic::Skeleton::convert(*this->acclaimSkeleton, skeleton);
	const auto mapping = kinematic::SkeletonState::createMapping(skeleton);
	kinematic::SkeletonState::RigidPartialStateChange sChange = kinematic::SkeletonState::convert(*acclaimSkeleton, acclaimData->frames[0], mapping);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(9.60681, sChange.translation.x(), epsilon);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(4.21614, sChange.translation.y(), epsilon);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.253612, sChange.translation.z(), epsilon);

	// lowerback -6.90483 0.869314 0.0870155
	// lowerback ma index 3 w mapping...
	osg::Vec3d rad = kinematicUtils::toRadians(osg::Vec3d(-6.90483, 0.869314, 0.0870155));
	osg::Quat q = kinematicUtils::convertXYZ(rad);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(q.x(), sChange.rotations[3].x(), epsilon);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(q.y(), sChange.rotations[3].y(), epsilon);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(q.z(), sChange.rotations[3].z(), epsilon);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(q.w(), sChange.rotations[3].w(), epsilon);


	sChange = kinematic::SkeletonState::convert(*acclaimSkeleton, *acclaimData->frames.rbegin(), mapping);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(-7.58606, sChange.translation.x(), epsilon);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(4.3173, sChange.translation.y(), epsilon);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.476158, sChange.translation.z(), epsilon);

	osg::Vec3d axis{ -180.00000000000000, 5.1447700000000002e-009, -151.00000000000000 };
	osg::Vec3d shift{ 0.0418060236, -0.0754254982, -0.0142621007 };
	osg::Vec3d after{ 2.63005631e-006, -0.0862365887, 0.0142621007 };

	osg::Vec3d v = shift;
	osg::Vec3d r = kinematicUtils::toRadians(-axis);
	osg::Quat rot; rot.makeRotate(r.z(), osg::Vec3(0, 0, 1),
								  r.y(), osg::Vec3(0, 1, 0),
								  r.x(), osg::Vec3(1, 0, 0));
	v = rot * v;
	after = after;

}
