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
#include "AsfParserTest.h"



AsfParserTest::AsfParserTest(void)
{
}

AsfParserTest::~AsfParserTest(void)
{

}

void AsfParserTest::testLoad()
{
    kinematic::AsfParser asf;
    kinematic::SkeletalModelPtr model(new kinematic::SkeletalModel);
    
    asf.parse(model, "kinematic/acclaim/cmu/01/01.asf");
    asf.save(model, "asftest.asf");
}

void AsfParserTest::testLoadNotExisting() {
    kinematic::AsfParser asf;
    kinematic::SkeletalModelPtr model(new kinematic::SkeletalModel);
    asf.parse(model, "NotExist.asf");
}