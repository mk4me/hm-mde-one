#ifndef HEADER_GUARD__DFMTEST_MODEL_H__
#define HEADER_GUARD__DFMTEST_MODEL_H__

#include <testnodefactory/TestDFNodeProcessing.h>
#include <testnodefactory/TestDFNodeSource.h>

#include <cppunit/extensions/HelperMacros.h>
#include <string>


class ModelOperationsTest : public CppUnit::TestFixture
{
public:
	ModelOperationsTest(void);
	virtual ~ModelOperationsTest(void);

	void testBasicNodeAddition();
	void testDFSourceNodeAddition();
	void testDFProcessingNodeAddition();
	void testConnectingNodes();
	void testDisconnectingNodes();

	CPPUNIT_TEST_SUITE( ModelOperationsTest );
	CPPUNIT_TEST( testBasicNodeAddition );
	CPPUNIT_TEST( testDFSourceNodeAddition );
	CPPUNIT_TEST( testDFProcessingNodeAddition );
	CPPUNIT_TEST( testConnectingNodes );
	CPPUNIT_TEST_SUITE_END();

protected:

	dflm::DFMPtr DFModelA;
	dflm::DFMPtr DFModelB;
	dflm::DFMPtr DFModelC;
	dflm::DFMPtr DFModelD;
	dflm::DFMPtr DFModelE;
	dflm::DFMPtr DFModelF;
	dflm::DFMPtr DFModelG;

};

#endif