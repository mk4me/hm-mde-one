#include "JointAnglesCollectionTest.h"
#include <kinematiclib/JointAnglesCollection.h>
#include <vector>
#include <utils/SmartPtr.h>

using namespace utils;
using namespace kinematic;

#ifdef UTILS_ENABLE_PRIVATE_TESTS

struct cloneTest
{
    JointAnglesCollectionPtr origin;
    JointAnglesCollectionPtr clone;
};

template<>
void JointAnglesCollection::_privateTestHook<cloneTest>(cloneTest& cs)
{
    CPPUNIT_ASSERT_EQUAL(cs.origin->configurationID, cs.clone->configurationID);
    CPPUNIT_ASSERT_EQUAL(cs.origin->lengthRatio, cs.clone->lengthRatio);
    CPPUNIT_ASSERT_EQUAL(cs.origin->channels.size(), cs.clone->channels.size());
    CPPUNIT_ASSERT_EQUAL(cs.origin->rootPositions.size(), cs.clone->rootPositions.size());
    CPPUNIT_ASSERT_EQUAL(cs.origin->initialized, cs.clone->initialized);
    for (int i = cs.origin->rootPositions.size() - 1; i >= 0; --i) {
        CPPUNIT_ASSERT_DOUBLES_EQUAL(cs.origin->rootPositions[i][0], cs.clone->rootPositions[i][0], 0.000001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(cs.origin->rootPositions[i][1], cs.clone->rootPositions[i][1], 0.000001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(cs.origin->rootPositions[i][2], cs.clone->rootPositions[i][2], 0.000001);
    }

    for (int i = cs.origin->channels.size() - 1; i >= 0; --i) {
        auto originC = cs.origin->channels[i];
        auto cloneC = cs.clone->channels[i];
        CPPUNIT_ASSERT_EQUAL(originC->size(), cloneC->size());
        CPPUNIT_ASSERT_EQUAL(originC->getName(), cloneC->getName());
        CPPUNIT_ASSERT_DOUBLES_EQUAL(originC->getLength(), cloneC->getLength(), 0.000001);

        for (int j = originC->size() - 1; j >= 0; --j) {
            CPPUNIT_ASSERT_DOUBLES_EQUAL(originC->argument(j), cloneC->argument(j), 0.000001);
            
            CPPUNIT_ASSERT_DOUBLES_EQUAL(originC->value(j).x(), cloneC->value(j).x(), 0.000001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(originC->value(j).y(), cloneC->value(j).y(), 0.000001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(originC->value(j).z(), cloneC->value(j).z(), 0.000001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(originC->value(j).w(), cloneC->value(j).w(), 0.000001);
        }
    }
}

void JointAnglesCollectionTest::testClone()
{
    {
        JointAnglesCollectionPtr joints = utils::make_shared<JointAnglesCollection>();
        cloneTest ct = { joints, JointAnglesCollectionPtr(joints->clone()) };
        JointAnglesCollection::_privateTestHook<cloneTest>(ct);
    }
    {
        AsfParser asf;
        AmcParser amc;
        SkeletalModelPtr model(new SkeletalModel);
        SkeletalDataPtr data(new SkeletalData);
        asf.parse(model, "test.asf");
        amc.parse(data, "test.amc");
        JointAnglesCollectionPtr joints = utils::make_shared<JointAnglesCollection>();
        joints->setSkeletal(model, data);
        cloneTest ct = { joints, JointAnglesCollectionPtr(joints->clone()) };
        JointAnglesCollection::_privateTestHook<cloneTest>(ct);
    }
    
    testClonePublic();
}

#else

void JointAnglesCollectionTest::testClone()
{
    testClonePublic();
}

#endif


void JointAnglesCollectionTest::testClonePublic()
{

}