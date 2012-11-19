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
#include <kinematiclib/SkeletalParsers.h>
#include <list>
#include "AmcParserTest.h"



AmcParserTest::AmcParserTest(void)
{
}

AmcParserTest::~AmcParserTest(void)
{

}

void AmcParserTest::testLoad()
{
    kinematic::AmcParser amc;
   // kinematic::SkeletalModelPtr model(new kinematic::SkeletalModel);
	kinematic::SkeletalDataPtr data(new kinematic::SkeletalData);
    amc.parse(data, "kinematic/acclaim/cmu/01/01_14.amc");

}

void AmcParserTest::testLoadNotExisting() {
    kinematic::AsfParser asf;
    kinematic::SkeletalModelPtr model(new kinematic::SkeletalModel);
    asf.parse(model, "NotExist.amc");
}
