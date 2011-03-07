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
#include "AmcParserTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION( AmcParserTest );

AmcParserTest::AmcParserTest(void)
{
}

AmcParserTest::~AmcParserTest(void)
{

}

void AmcParserTest::testLoad()
{
    kinematic::AmcParser amc;
    kinematic::SkeletalModel::Ptr model(new kinematic::SkeletalModel);
    amc.parse(model, "kinematic/acclaim/cmu/01/01_14.amc");

}

void AmcParserTest::testLoadNotExisting() {
    kinematic::AsfParser asf;
    kinematic::SkeletalModel::Ptr model(new kinematic::SkeletalModel);
    asf.parse(model, "NotExist.amc");
}
