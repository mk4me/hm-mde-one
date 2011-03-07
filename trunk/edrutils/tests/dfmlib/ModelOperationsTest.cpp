#include "ModelOperationsTest.h"
#include <testnodefactory/TestNode.h>
#include <testnodefactory/TestNodeFactory.h>

#include <dfmlib/DFModel.h>


CPPUNIT_TEST_SUITE_REGISTRATION( ModelOperationsTest );

ModelOperationsTest::ModelOperationsTest(void) : TestFixture()
{
}


ModelOperationsTest::~ModelOperationsTest(void)
{
}

void ModelOperationsTest::testBasicNodeAddition(){
	dflm::DFMPtr model(new dflm::DFModel());

	CPPUNIT_ASSERT( model->addNode(dflm::NPtr(new TestNode("TestNode"))) == false );
	CPPUNIT_ASSERT( model->getNodes().size() == 0 );
	CPPUNIT_ASSERT( model->getSourceNodes().size() == 0 );
	CPPUNIT_ASSERT( model->getLeafNodes().size() == 0 );
	CPPUNIT_ASSERT( model->getConnections().size() == 0 );
	CPPUNIT_ASSERT( model->getRequiringConnections().size() == 0 );
}

void ModelOperationsTest::testDFSourceNodeAddition(){
	dflm::DFMPtr model(new dflm::DFModel());

	CPPUNIT_ASSERT( model->addNode(TestNodeFactory::createSourceNode("A",1)) == true );
	CPPUNIT_ASSERT( model->getNodes().size() == 1 );
	CPPUNIT_ASSERT( model->getSourceNodes().size() == 1 );
	CPPUNIT_ASSERT( model->getLeafNodes().size() == 0 );
	CPPUNIT_ASSERT( model->getConnections().size() == 0 );
	CPPUNIT_ASSERT( model->getRequiringConnections().size() == 0 );

	CPPUNIT_ASSERT( model->addNode(TestNodeFactory::createSourceNode("B",2)) == true );
	CPPUNIT_ASSERT( model->getNodes().size() == 2 );
	CPPUNIT_ASSERT( model->getSourceNodes().size() == 2 );
	CPPUNIT_ASSERT( model->getLeafNodes().size() == 0 );
	CPPUNIT_ASSERT( model->getConnections().size() == 0 );
	CPPUNIT_ASSERT( model->getRequiringConnections().size() == 0 );

	CPPUNIT_ASSERT( model->addNode(TestNodeFactory::createSourceNode("C",3)) == true );
	CPPUNIT_ASSERT( model->getNodes().size() == 3 );
	CPPUNIT_ASSERT( model->getSourceNodes().size() == 3 );
	CPPUNIT_ASSERT( model->getLeafNodes().size() == 0 );
	CPPUNIT_ASSERT( model->getConnections().size() == 0 );
	CPPUNIT_ASSERT( model->getRequiringConnections().size() == 0 );
}

void ModelOperationsTest::testDFProcessingNodeAddition(){
	dflm::DFMPtr model(new dflm::DFModel());

	CPPUNIT_ASSERT( model->addNode(TestNodeFactory::createProcessingNode("A",1,1)) == true );
	CPPUNIT_ASSERT( model->getNodes().size() == 1 );
	CPPUNIT_ASSERT( model->getSourceNodes().size() == 0 );
	CPPUNIT_ASSERT( model->getLeafNodes().size() == 0 );
	CPPUNIT_ASSERT( model->getConnections().size() == 0 );
	CPPUNIT_ASSERT( model->getRequiringConnections().size() == 1 );

	CPPUNIT_ASSERT( model->addNode(TestNodeFactory::createProcessingNode("B",1,1)) == true );
	CPPUNIT_ASSERT( model->getNodes().size() == 2 );
	CPPUNIT_ASSERT( model->getSourceNodes().size() == 0 );
	CPPUNIT_ASSERT( model->getLeafNodes().size() == 0 );
	CPPUNIT_ASSERT( model->getConnections().size() == 0 );
	CPPUNIT_ASSERT( model->getRequiringConnections().size() == 2 );

	CPPUNIT_ASSERT( model->addNode(TestNodeFactory::createProcessingNode("C",1,1)) == true );
	CPPUNIT_ASSERT( model->getNodes().size() == 3 );
	CPPUNIT_ASSERT( model->getSourceNodes().size() == 0 );
	CPPUNIT_ASSERT( model->getLeafNodes().size() == 0 );
	CPPUNIT_ASSERT( model->getConnections().size() == 0 );
	CPPUNIT_ASSERT( model->getRequiringConnections().size() == 3 );
}

void ModelOperationsTest::testConnectingNodes(){
	dflm::DFMPtr model(new dflm::DFModel());

	dflm::DFNPtr srcA = TestNodeFactory::createSourceNode("A",1);
	model->addNode(srcA);
	dflm::DFNPtr srcB = TestNodeFactory::createSourceNode("B",2);
	model->addNode(srcB);
	dflm::DFNPtr srcC = TestNodeFactory::createSourceNode("C",3);
	model->addNode(srcC);

	dflm::DFNPtr procA = TestNodeFactory::createProcessingNode("A",1,1);
	model->addNode(procA);
	dflm::DFNPtr procB = TestNodeFactory::createProcessingNode("B",2,1);
	model->addNode(procB);
	dflm::DFNPtr procC = TestNodeFactory::createProcessingNode("C",2,1,1);
	model->addNode(procC);
	dflm::DFNPtr procD = TestNodeFactory::createProcessingNode("D",1,2);
	model->addNode(procD);
	dflm::DFNPtr procE = TestNodeFactory::createProcessingNode("E",2,2);
	model->addNode(procE);
	dflm::DFNPtr procF = TestNodeFactory::createProcessingNode("F",2,2,1);
	model->addNode(procF);
	dflm::DFNPtr procG = TestNodeFactory::createProcessingNode("G",2,2,2);
	model->addNode(procG);

	//try connect output src pin to itself
	CPPUNIT_ASSERT(model->connect(*srcA->getOutPins().begin(), *srcA->getOutPins().begin()) == 0);
	CPPUNIT_ASSERT((*srcA->getOutPins().begin())->getConnections().empty() == true);
	CPPUNIT_ASSERT(model->getConnections().empty() == true);
	CPPUNIT_ASSERT( model->getLeafNodes().empty() == true );
	CPPUNIT_ASSERT( model->getRequiringConnections().size() == 7 );

	//try connect output proc pin to itself
	CPPUNIT_ASSERT(model->connect(*procA->getOutPins().begin(), *procA->getOutPins().begin()) == 0);
	CPPUNIT_ASSERT((*procA->getOutPins().begin())->getConnections().empty() == true);
	CPPUNIT_ASSERT(model->getConnections().empty() == true);
	CPPUNIT_ASSERT( model->getLeafNodes().empty() == true );
	CPPUNIT_ASSERT( model->getRequiringConnections().size() == 7 );

	//try connect input proc pin to itself
	CPPUNIT_ASSERT(model->connect(*procA->getInPins().begin(), *procA->getInPins().begin()) == 0);
	CPPUNIT_ASSERT((*procA->getInPins().begin())->getConnections().empty() == true);
	CPPUNIT_ASSERT(model->getConnections().empty() == true);
	CPPUNIT_ASSERT( model->getLeafNodes().empty() == true );
	CPPUNIT_ASSERT( model->getRequiringConnections().size() == 7 );

	//try connect pins of the same node
	CPPUNIT_ASSERT(model->connect(*procA->getOutPins().begin(), *procA->getInPins().begin()) == 0);
	CPPUNIT_ASSERT((*procA->getInPins().begin())->getConnections().empty() == true);
	CPPUNIT_ASSERT((*procA->getOutPins().begin())->getConnections().empty() == true);
	CPPUNIT_ASSERT(model->getConnections().empty() == true);
	CPPUNIT_ASSERT( model->getLeafNodes().empty() == true );
	CPPUNIT_ASSERT( model->getRequiringConnections().size() == 7 );

	//try connect A and B nodes
	CPPUNIT_ASSERT(model->connect(*procA->getOutPins().begin(), *procB->getInPins().begin()) != 0);
	CPPUNIT_ASSERT((*procA->getOutPins().begin())->getConnections().size() == 1);
	CPPUNIT_ASSERT((*procB->getInPins().begin())->getConnections().size() == 1);
	CPPUNIT_ASSERT(model->getConnections().size() == 1);
	CPPUNIT_ASSERT( model->getLeafNodes().size() == 1 );
	CPPUNIT_ASSERT( model->getRequiringConnections().size() == 6 );

	//try connect once again already connected pins
	CPPUNIT_ASSERT(model->connect(*procA->getOutPins().begin(), *procB->getInPins().begin()) == 0);
	CPPUNIT_ASSERT((*procA->getOutPins().begin())->getConnections().size() == 1);
	CPPUNIT_ASSERT((*procB->getInPins().begin())->getConnections().size() == 1);
	CPPUNIT_ASSERT(model->getConnections().size() == 1);
	CPPUNIT_ASSERT( model->getLeafNodes().size() == 1 );
	CPPUNIT_ASSERT( model->getRequiringConnections().size() == 6 );

	//try connect B with C - B is no more a leaf, but C is a leaf!!
	CPPUNIT_ASSERT(model->connect(*procB->getOutPins().begin(), *procC->getInPins().begin()) != 0);
	CPPUNIT_ASSERT((*procB->getOutPins().begin())->getConnections().size() == 1);
	CPPUNIT_ASSERT((*procC->getInPins().begin())->getConnections().size() == 1);
	CPPUNIT_ASSERT(model->getConnections().size() == 2);
	CPPUNIT_ASSERT( model->getLeafNodes().size() == 1 );
	CPPUNIT_ASSERT( model->getRequiringConnections().size() == 5 );

	//try connect B with D - D is a leaf!!
	CPPUNIT_ASSERT(model->connect(*procB->getOutPins().begin(), *procD->getInPins().begin()) != 0);
	CPPUNIT_ASSERT((*procB->getOutPins().begin())->getConnections().size() == 2);
	CPPUNIT_ASSERT((*procD->getInPins().begin())->getConnections().size() == 1);
	CPPUNIT_ASSERT(model->getConnections().size() == 3);
	CPPUNIT_ASSERT( model->getLeafNodes().size() == 2 );
	CPPUNIT_ASSERT( model->getRequiringConnections().size() == 4 );

	//try connect input pin already connected B and E
	CPPUNIT_ASSERT(model->connect(*procE->getOutPins().begin(), *procB->getInPins().begin()) == 0);
	CPPUNIT_ASSERT((*procE->getOutPins().begin())->getConnections().size() == 0);
	CPPUNIT_ASSERT((*procB->getInPins().begin())->getConnections().size() == 1);
	CPPUNIT_ASSERT(model->getConnections().size() == 3);
	CPPUNIT_ASSERT( model->getLeafNodes().size() == 2 );
	CPPUNIT_ASSERT( model->getRequiringConnections().size() == 4 );

	//try connect D and A - loop!!
	CPPUNIT_ASSERT(model->connect(*procD->getOutPins().begin(), *procA->getInPins().begin()) == 0);
	CPPUNIT_ASSERT((*procD->getOutPins().begin())->getConnections().size() == 0);
	CPPUNIT_ASSERT((*procA->getInPins().begin())->getConnections().size() == 0);
	CPPUNIT_ASSERT(model->getConnections().size() == 3);
	CPPUNIT_ASSERT( model->getLeafNodes().size() == 2 );
	CPPUNIT_ASSERT( model->getRequiringConnections().size() == 4 );

	//try connect C and B - loop!!
	CPPUNIT_ASSERT(model->connect(*procC->getOutPins().begin(), *(++procB->getInPins().begin())) == 0);
	CPPUNIT_ASSERT((*procC->getOutPins().begin())->getConnections().size() == 0);
	CPPUNIT_ASSERT((*(++procB->getInPins().begin()))->getConnections().size() == 0);
	CPPUNIT_ASSERT(model->getConnections().size() == 3);
	CPPUNIT_ASSERT( model->getLeafNodes().size() == 2 );
	CPPUNIT_ASSERT( model->getRequiringConnections().size() == 4 );
}

void ModelOperationsTest::testDisconnectingNodes(){
	dflm::DFMPtr model(new dflm::DFModel());

	dflm::DFNPtr srcA = TestNodeFactory::createSourceNode("A",1);
	model->addNode(srcA);
	dflm::DFNPtr srcB = TestNodeFactory::createSourceNode("B",2);
	model->addNode(srcB);
	dflm::DFNPtr srcC = TestNodeFactory::createSourceNode("C",3);
	model->addNode(srcC);

	dflm::DFNPtr procA = TestNodeFactory::createProcessingNode("A",1,1);
	model->addNode(procA);
	dflm::DFNPtr procB = TestNodeFactory::createProcessingNode("B",2,1);
	model->addNode(procB);
	dflm::DFNPtr procC = TestNodeFactory::createProcessingNode("C",2,1,1);
	model->addNode(procC);
	dflm::DFNPtr procD = TestNodeFactory::createProcessingNode("D",1,2);
	model->addNode(procD);
	dflm::DFNPtr procE = TestNodeFactory::createProcessingNode("E",2,2);
	model->addNode(procE);
	dflm::DFNPtr procF = TestNodeFactory::createProcessingNode("F",2,2,1);
	model->addNode(procF);
	dflm::DFNPtr procG = TestNodeFactory::createProcessingNode("G",2,2,2);
	model->addNode(procG);

	//connect A and B nodes
	CPPUNIT_ASSERT(model->connect(*procA->getOutPins().begin(), *procB->getInPins().begin()) != 0);

	//connect B with C - B is no more a leaf, but C is a leaf!!
	CPPUNIT_ASSERT(model->connect(*procB->getOutPins().begin(), *procC->getInPins().begin()) != 0);

	//connect B with D - D is a leaf!!
	CPPUNIT_ASSERT(model->connect(*procB->getOutPins().begin(), *procD->getInPins().begin()) != 0);

	//connect D and E
	CPPUNIT_ASSERT(model->connect(*procD->getOutPins().begin(), *procE->getInPins().begin()) == 0);

	//connect B and E
	CPPUNIT_ASSERT(model->connect(*procB->getOutPins().begin(), *(++procE->getInPins().begin())) == 0);

	/*CPPUNIT_ASSERT((*(++procB->getInPins().begin()))->getConnections().size() == 0);
	CPPUNIT_ASSERT(model->getConnections().size() == 3);
	CPPUNIT_ASSERT( model->getLeafNodes().size() == 2 );
	CPPUNIT_ASSERT( model->getRequiringConnections().size() == 4 );*/
}
