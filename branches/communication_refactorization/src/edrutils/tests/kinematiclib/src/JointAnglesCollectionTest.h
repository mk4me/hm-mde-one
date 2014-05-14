/********************************************************************
	created:	2013/08/24
	created:	24:8:2013   17:44
	filename: 	JointAnglesCollectionTest.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_KINEMATIC_TEST__JOINTANGLESCOLLECTIONTEST_H__
#define HEADER_GUARD_KINEMATIC_TEST__JOINTANGLESCOLLECTIONTEST_H__

#include <cppunit/extensions/HelperMacros.h>

class JointAnglesCollectionTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(JointAnglesCollectionTest);
    CPPUNIT_TEST(testClone);
    CPPUNIT_TEST_SUITE_END();

public:
    virtual ~JointAnglesCollectionTest() {}

public:
    void testClone();	

private:
    void testClonePublic();
};

#endif
