#ifndef HEADER_GUARD_ASFPARSERTEST_H__
#define HEADER_GUARD_ASFPARSERTEST_H__

#include <cppunit/extensions/HelperMacros.h>

class AsfParserTest : public CPPUNIT_NS::TestCase
{
    CPPUNIT_TEST_SUITE(AsfParserTest);
    CPPUNIT_TEST(testLoad);
    CPPUNIT_TEST_EXCEPTION(testLoadNotExisting, kinematic::UnableToOpenFileException);
    CPPUNIT_TEST_SUITE_END();
public:
    AsfParserTest(void);
    virtual ~AsfParserTest(void);

public:
    void testLoad();
    void testLoadNotExisting();
};
#endif
