#include <KinematicModelLibrary/stdafx.h>
#include <KinematicModelLibrary/SkeletalParsers.h>
#include <KinematicModelLibrary/KinematicModel.h>
#include "KinematicModelTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION( KinematicModelTest );

using namespace hmAnimation;
void KinematicModelTest::test1()
{
   AsfParser asf;
   AmcParser amc;
   SkeletalModel::Ptr model(new SkeletalModel);

    Logger::getInstance().log(Logger::Info, "test kinematic model");
    asf.parse(model, "kinematic/acclaim/cmu/01/01.asf");
    amc.parse(model, "kinematic/acclaim/cmu/01/01_01.amc");
    
    KinematicModel kinematic;
    kinematic.setSkeletalData(model);
    hAnimJoint::Ptr vt6 = kinematic.getJointByName("vt6");
    kinematic.activateJoint(vt6, true);
    hAnimJoint::Ptr l_index3 = kinematic.getJointByName("l_index3");
    kinematic.activateJoint(l_index3, true);
    kinematic.activateJoint("r_index0", true);

    model = kinematic.createSkeletalData();
    CPPUNIT_ASSERT(model->getJointByName("vt6") );
    CPPUNIT_ASSERT(model->getJointByName("l_index3"));
    CPPUNIT_ASSERT(model->getJointByName("r_index0"));
    asf.save(model, "kinematic/acclaim/cmu/01/01_TEST.asf");
    amc.save(model, "kinematic/acclaim/cmu/01/01_01_TEST.amc");
}

void KinematicModelTest::testHAnim()
{

}

