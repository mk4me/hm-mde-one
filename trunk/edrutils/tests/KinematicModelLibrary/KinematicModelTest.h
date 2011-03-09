#ifndef HEADER_GUARD_KINEMATICMODEL_H__
#define HEADER_GUARD_KINEMATICMODEL_H__

#include <cppunit/extensions/HelperMacros.h>

class KinematicModelTest : public CPPUNIT_NS::TestCase
{
    CPPUNIT_TEST_SUITE(KinematicModelTest);
    CPPUNIT_TEST(test1);
    CPPUNIT_TEST(testHAnim);
    CPPUNIT_TEST_SUITE_END();

public:
    void test1();
    void testHAnim();
};
#endif