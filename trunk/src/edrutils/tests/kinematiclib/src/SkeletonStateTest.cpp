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
	acclaimData(new acclaim::MotionData()),
	acclaimhelperMotionData(new acclaim::Skeleton::HelperMotionData),
	acclaimActiveBones(0)
{

}

SkeletonStateTest::~SkeletonStateTest(void)
{

}

void SkeletonStateTest::setUp()
{
	std::ifstream fileAsf("./testFiles/test.asf");
	acclaimSkeleton.reset(new acclaim::Skeleton(acclaim::AsfParser::parse(fileAsf, true)));

	std::ifstream fileAmc("./testFiles/test.amc");
	acclaim::AmcParser::parse(*acclaimData, fileAmc);

	*acclaimhelperMotionData = acclaim::Skeleton::helperMotionData(*acclaimSkeleton);
	acclaimActiveBones = acclaim::Skeleton::activeBones(*acclaimSkeleton);
}

void SkeletonStateTest::tearDown()
{

}


void SkeletonStateTest::testMapping()
{
	kinematic::Skeleton skeleton;
	kinematic::Skeleton::convert(*this->acclaimSkeleton, skeleton);
	kinematic::LinearizedSkeleton::Mapping mapping = kinematic::LinearizedSkeleton::createNonLeafMapping(skeleton);
	for (auto& p : mapping.left) {
		UTILS_DEBUG_PRINT("%d - > %s", p.first, p.second.c_str());
	}
	CPPUNIT_ASSERT_EQUAL((int)acclaimSkeleton->bones.size(), (int)mapping.left.size());
	CPPUNIT_ASSERT_EQUAL((int)acclaimSkeleton->bones.size(), (int)mapping.right.size());
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
	const auto mapping = kinematic::LinearizedSkeleton::createCompleteMapping(skeleton);
	kinematic::SkeletonState::RigidPartialState sChange = kinematic::SkeletonState::convert(*acclaimSkeleton, acclaimData->frames[0].bonesData, mapping, *acclaimhelperMotionData);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(9.60681, sChange.position.x(), epsilon);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(4.21614, sChange.position.y(), epsilon);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.253612, sChange.position.z(), epsilon);

	// lowerback -6.90483 0.869314 0.0870155
	// lowerback ma index 3 w mapping...
	osg::Vec3d rad = kinematicUtils::toRadians(osg::Vec3d(-6.90483, 0.869314, 0.0870155));
	osg::Quat q = kinematicUtils::convertXYZ(rad);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(q.x(), sChange.orientations[3].x(), epsilon);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(q.y(), sChange.orientations[3].y(), epsilon);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(q.z(), sChange.orientations[3].z(), epsilon);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(q.w(), sChange.orientations[3].w(), epsilon);

	sChange = kinematic::SkeletonState::convert(*acclaimSkeleton, acclaimData->frames.rbegin()->bonesData, mapping, *acclaimhelperMotionData);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(-7.58606, sChange.position.x(), epsilon);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(4.3173, sChange.position.y(), epsilon);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.476158, sChange.position.z(), epsilon);
}

void SkeletonStateTest::testConvertStateChange()
{
	kinematic::Skeleton skeleton;
	kinematic::Skeleton::convert(*this->acclaimSkeleton, skeleton);
	auto mapping = kinematic::LinearizedSkeleton::createCompleteMapping(skeleton);

	kinematic::SkeletonState::RigidPartialState frame = kinematic::SkeletonState::convert(*acclaimSkeleton, acclaimData->frames[0].bonesData, mapping, *acclaimhelperMotionData);
	//kinematic::SkeletonState::NonRigidCompleteStateChange frame = kinematic::SkeletonState::convertStateChange(mapping, sChange);
	CPPUNIT_ASSERT_EQUAL((int)acclaimActiveBones, (int)frame.orientations.size());
	auto id1 = mapping.right.find("lhipjoint")->get_left();
	auto id2 = mapping.right.find("rhipjoint")->get_left();


	//auto lhipjointData = frame[mapping.right.at("lhipjoint")];
	//auto rhipjointData = frame[mapping.right.at("rhipjoint")];
	CPPUNIT_ASSERT(frame.orientations.find(id1) == frame.orientations.end());
	CPPUNIT_ASSERT(frame.orientations.find(id2) == frame.orientations.end());
	//CPPUNIT_ASSERT(rhipjointData.rotation == osg::Quat());
	//CPPUNIT_ASSERT(rhipjointData.rotation == lhipjointData.rotation);

	//rhand 9.42497 - 7.79745 32.1625
	auto rhand = frame.orientations[mapping.right.at("rhand")];
	//CPPUNIT_ASSERT(rhand.translation == osg::Vec3());
	osg::Vec3d rad = kinematicUtils::toRadians(osg::Vec3d(9.42497, -7.79745, 32.1625));
	osg::Quat q = kinematicUtils::convertXYZ(rad);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(q.x(), rhand.x(), epsilon);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(q.y(), rhand.y(), epsilon);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(q.z(), rhand.z(), epsilon);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(q.w(), rhand.w(), epsilon);
}
