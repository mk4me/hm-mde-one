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
#include "MiscTest.h"

using namespace std;
using namespace hmAnimation;
CPPUNIT_TEST_SUITE_REGISTRATION( MiscTest );

MiscTest::MiscTest(void)
{
}

MiscTest::~MiscTest(void)
{

}


void MiscTest::testOK()
{
    Logger::getInstance().log(Logger::Info, "Test ktory powinien sie udac");
    AsfParser asf;
    SkeletalModel::Ptr model(new SkeletalModel);
    system("dir");
    asf.parse(model, "kinematic/acclaim/cmu/01/01.asf");
    CPPUNIT_ASSERT(true);

}

void MiscTest::testFail()
{
    Logger::getInstance().log(Logger::Info, "Test ktory NIE powinien sie udac");
    CPPUNIT_ASSERT(false);
}
