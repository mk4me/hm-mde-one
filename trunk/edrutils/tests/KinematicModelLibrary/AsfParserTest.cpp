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
    // TODO zmienic ktorys z plikow CMakeLists bo na razie ten test nie ma sensu
    //asf.parse(model, "C:\\Programming\\Projects\\HDEdr\\TestsOfKinematicModelLibrary\\11.asf");

}

void AsfParserTest::testLoadNotExisting() {
    hmAnimation::AsfParser asf;
    hmAnimation::SkeletalModel::Ptr model(new hmAnimation::SkeletalModel);
    asf.parse(model, "NULLER.asf");
}
