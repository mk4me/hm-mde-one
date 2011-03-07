#include <tinyxml.h>
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
#include <KinematicModelLibrary/SkeletalParsers.h>
#include <list>
#include <KinematicModelLibrary/SkeletalParsers.h>
#include <KinematicModelLibrary/KinematicModel.h>
#include "KinematicModelTest.h"


//CPPUNIT_TEST_SUITE_REGISTRATION( KinematicModelTest );

using namespace kinematic;
void KinematicModelTest::test1()
{
   KinematicModel kinematic;
   kinematic.loadMappingDictionary("kinematic/dictionary.xml");
   AsfParser asf;
   AmcParser amc;
   SkeletalModel::Ptr model(new SkeletalModel);

    Logger::getInstance().log(Logger::Info, "test kinematic model");
    asf.parse(model, "kinematic/acclaim/cmu/01/01.asf");
    amc.parse(model, "kinematic/acclaim/cmu/01/01_01.amc");
    
    
    kinematic.setSkeletalData(model);
    hAnimJointPtr vt6 = kinematic.getJointByName("vt6");
    kinematic.activateJoint(vt6, true);
    hAnimJointPtr l_index3 = kinematic.getJointByName("l_index3");
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

