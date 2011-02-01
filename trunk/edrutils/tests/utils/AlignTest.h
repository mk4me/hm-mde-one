/********************************************************************
	created:  2010/07/16
	created:  16:7:2010   16:28
	filename: AlignTest.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__ALIGNTEST_H__
#define __HEADER_GUARD__ALIGNTEST_H__

#include <cppunit/extensions/HelperMacros.h>

class AlignTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE(AlignTest);
  CPPUNIT_TEST(testAlign);
  CPPUNIT_TEST(testAlignTo);
  CPPUNIT_TEST_SUITE_END();

public:
  void testAlign();
  void testAlignTo();
};




#endif  // __HEADER_GUARD__ALIGNTEST_H__