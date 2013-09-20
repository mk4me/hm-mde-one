#include "FileDataManagerTest.h"
#include "../src/core/src/FileDataManager.h"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( FileDataManagerTest);

void FileDataManagerTest::setUp()
{
}

void FileDataManagerTest::testRemoveFile()
{
    core::FileDataManager manager;
    manager.addFile("test.c3d");
}

void FileDataManagerTest::testAddFile()
{

}
