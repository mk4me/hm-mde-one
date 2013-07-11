#include "UniversalSourceTest.h"
#include <plugins/dfElements/DFSources.h>
#include <plugins/dfElements/DFPins.h>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( UniversalSourceTest );


void UniversalSourceTest::setUp()
{
    char** argv = nullptr; int arc = 0;
    app = new QApplication(arc, argv);
}

void UniversalSourceTest::testCreate()
{
    UniversalSource<VectorChannelReaderInterface> us;
    CPPUNIT_ASSERT(!us.isNodeValid());
    us.produce();
}

void UniversalSourceTest::tearDown()
{
    delete app;
}
