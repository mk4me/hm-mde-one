#include "NodeCreationTest.h"
#include <testnodefactory/TestNodeFactory.h>

CPPUNIT_TEST_SUITE_REGISTRATION( NodeCreationTest );

NodeCreationTest::NodeCreationTest(void) : TestFixture()
{
}


NodeCreationTest::~NodeCreationTest(void)
{
}

void NodeCreationTest::setUp(){
	ProcNodeA = TestNodeFactory::createProcessingNode("A", 0, 5);
	ProcNodeB = TestNodeFactory::createProcessingNode("B", 1, 2, 0);
	ProcNodeC = TestNodeFactory::createProcessingNode("C", 1, 2, 1);
	ProcNodeD = TestNodeFactory::createProcessingNode("D", 2, 1, 0);
	ProcNodeE = TestNodeFactory::createProcessingNode("E", 2, 1, 2, true);
	ProcNodeF = TestNodeFactory::createProcessingNode("F", 100, 500, 50);
	ProcNodeG = TestNodeFactory::createProcessingNode("G", 1000, 500, 2000);

	SrcNodeA = TestNodeFactory::createSourceNode("A", 0);
	SrcNodeB = TestNodeFactory::createSourceNode("B", 1);
	SrcNodeC = TestNodeFactory::createSourceNode("C", 2);
	SrcNodeD = TestNodeFactory::createSourceNode("D", 100);
	SrcNodeE = TestNodeFactory::createSourceNode("E", 1000);
}

void NodeCreationTest::testProcessingNodeCreation(){
	CPPUNIT_ASSERT( ProcNodeA == 0 );
	CPPUNIT_ASSERT( ProcNodeB != 0 );
	CPPUNIT_ASSERT( ProcNodeB->getInPins().size() == 1 );
	CPPUNIT_ASSERT( ProcNodeB->getOutPins().size() == 2 );
	//TODO
	//test for required pins
	CPPUNIT_ASSERT( ProcNodeC != 0 );
	CPPUNIT_ASSERT( ProcNodeC->getInPins().size() == 1 );
	CPPUNIT_ASSERT( ProcNodeC->getOutPins().size() == 2 );
	//TODO
	//test for required pins
	CPPUNIT_ASSERT( ProcNodeD != 0 );
	CPPUNIT_ASSERT( ProcNodeD->getInPins().size() == 2 );
	CPPUNIT_ASSERT( ProcNodeD->getOutPins().size() == 1 );
	//TODO
	//test for required pins
	CPPUNIT_ASSERT( ProcNodeE != 0 );
	CPPUNIT_ASSERT( ProcNodeE->getInPins().size() == 2 );
	CPPUNIT_ASSERT( ProcNodeE->getOutPins().size() == 1 );
	//TODO
	//test for required pins
	CPPUNIT_ASSERT( ProcNodeF != 0 );
	CPPUNIT_ASSERT( ProcNodeF->getInPins().size() == 100 );
	CPPUNIT_ASSERT( ProcNodeF->getOutPins().size() == 500 );
	//TODO
	//test for required pins
	CPPUNIT_ASSERT( ProcNodeG != 0 );
	CPPUNIT_ASSERT( ProcNodeG->getInPins().size() == 1000 );
	CPPUNIT_ASSERT( ProcNodeG->getOutPins().size() == 500 );
	//TODO
	//test for required pins
}

void NodeCreationTest::testSourceNodeCreation(){
	CPPUNIT_ASSERT( SrcNodeA == 0 );
	CPPUNIT_ASSERT( SrcNodeB != 0 );
	CPPUNIT_ASSERT( SrcNodeB->getInPins().size() == 0 );
	CPPUNIT_ASSERT( SrcNodeB->getOutPins().size() == 1 );
	CPPUNIT_ASSERT( SrcNodeC != 0 );
	CPPUNIT_ASSERT( SrcNodeC->getInPins().size() == 0 );
	CPPUNIT_ASSERT( SrcNodeC->getOutPins().size() == 2 );
	CPPUNIT_ASSERT( SrcNodeD != 0 );
	CPPUNIT_ASSERT( SrcNodeD->getInPins().size() == 0 );
	CPPUNIT_ASSERT( SrcNodeD->getOutPins().size() == 100 );
	CPPUNIT_ASSERT( SrcNodeE != 0 );
	CPPUNIT_ASSERT( SrcNodeE->getInPins().size() == 0 );
	CPPUNIT_ASSERT( SrcNodeE->getOutPins().size() == 1000 );
}
