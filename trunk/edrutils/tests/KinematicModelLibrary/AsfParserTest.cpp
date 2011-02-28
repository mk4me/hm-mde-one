#include <KinematicModelLibrary/stdafx.h>
#include <KinematicModelLibrary/SkeletalParsers.h>
#include "AsfParserTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION( AsfParserTest );

AsfParserTest::AsfParserTest(void)
{
}

AsfParserTest::~AsfParserTest(void)
{

}

void AsfParserTest::testLoad()
{
    hmAnimation::AsfParser asf;
    hmAnimation::SkeletalModel::Ptr model(new hmAnimation::SkeletalModel);
    
    asf.parse(model, "kinematic/acclaim/cmu/01/01.asf");
    asf.save(model, "asftest.asf");
}

void AsfParserTest::testLoadNotExisting() {
    hmAnimation::AsfParser asf;
    hmAnimation::SkeletalModel::Ptr model(new hmAnimation::SkeletalModel);
    asf.parse(model, "NotExist.asf");
}
