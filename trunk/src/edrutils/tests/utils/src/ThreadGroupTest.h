/********************************************************************
    created:  2013/03/29
    created:  29:3:2013   19:13
    filename: ThreadGroupTest.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___THREADGROUPTEST_H__
#define HEADER_GUARD___THREADGROUPTEST_H__

#include <cppunit/extensions/HelperMacros.h>

class ThreadGroupTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(ThreadGroupTest);
	CPPUNIT_TEST(testJoin);
	CPPUNIT_TEST(testCancel);
	CPPUNIT_TEST(testNotify);	
	CPPUNIT_TEST(testExceptionPersistence);
	CPPUNIT_TEST(testCancelPersistence);
	CPPUNIT_TEST_SUITE_END();

public:
	void testJoin();
	void testCancel();
	void testNotify();
	void testExceptionPersistence();
	void testCancelPersistence();
};

#endif	//	HEADER_GUARD___THREADGROUPTEST_H__
