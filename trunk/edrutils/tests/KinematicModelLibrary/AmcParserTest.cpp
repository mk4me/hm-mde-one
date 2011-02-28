#include <KinematicModelLibrary/stdafx.h>
#include <KinematicModelLibrary/SkeletalParsers.h>
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
    hmAnimation::AmcParser amc;
    hmAnimation::SkeletalModel::Ptr model(new hmAnimation::SkeletalModel);
    amc.parse(model, "kinematic/acclaim/cmu/01/01_14.amc");

}

void AmcParserTest::testLoadNotExisting() {
    hmAnimation::AsfParser asf;
    hmAnimation::SkeletalModel::Ptr model(new hmAnimation::SkeletalModel);
    asf.parse(model, "NotExist.amc");
}
