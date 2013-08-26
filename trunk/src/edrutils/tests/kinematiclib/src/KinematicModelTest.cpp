#include <tinyxml.h>
#include <limits>
#include <iostream>
#include <vector>
#include <fstream>
#include <istream>
#include <string>
#include <map>
#include <boost/smart_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/smart_ptr.hpp>
#include <osg/Vec3d>
#include <osg/Quat>
#include <boost/shared_ptr.hpp>
#include <kinematiclib/SkeletalParsers.h>
#include <list>
#include <kinematiclib/SkeletalParsers.h>
#include <kinematiclib/JointAnglesCollection.h>
#include "KinematicModelTest.h"

using namespace kinematic;


void KinematicModelTest::testHAnim()
{

}


#ifdef UTILS_ENABLE_PRIVATE_TESTS

struct cloneTest
{
    hAnimSkeletonPtr origin;
    hAnimSkeletonPtr clone;
};

struct jointCloneTest
{
    hAnimJointPtr origin;
    hAnimJointPtr clone;
};

struct boneCloneTest
{
    hAnimBonePtr origin;
    hAnimBonePtr clone;
};

#define TEST_EQ(prop) CPPUNIT_ASSERT_EQUAL(cs.origin->prop, cs.clone->prop);
#define TEST_DBL(prop)  CPPUNIT_ASSERT_DOUBLES_EQUAL(cs.origin->prop, cs.clone->prop, 0.00001);

template<>
void hAnimBone::_privateTestHook(boneCloneTest& cs)
{
    TEST_EQ(name); 
    TEST_EQ(childrenJoints.size());
    for (int i = cs.origin->childrenJoints.size() - 1; i >= 0; --i) {
        auto originJoint = cs.origin->childrenJoints[i];
        auto cloneJoint = cs.clone->childrenJoints[i];
        
        if (originJoint && cloneJoint) {
            CPPUNIT_ASSERT(originJoint.get() != cloneJoint.get());
            CPPUNIT_ASSERT_EQUAL(originJoint->getName(), cloneJoint->getName());
        } else {
            CPPUNIT_ASSERT(!originJoint);
            CPPUNIT_ASSERT(!cloneJoint);
        }

    }

    auto originParent = cs.origin->parentJoint.lock();
    auto cloneParent = cs.clone->parentJoint.lock();
    
    if (originParent && cloneParent) {
        CPPUNIT_ASSERT(originParent.get() != cloneParent.get());
    } else {
        CPPUNIT_ASSERT(!originParent);
        CPPUNIT_ASSERT(!cloneParent);
    }
}

template<>
void hAnimJoint::_privateTestHook(jointCloneTest& cs)
{
    TEST_DBL(localShift[0]);
    TEST_DBL(localShift[1]);
    TEST_DBL(localShift[2]);
    TEST_EQ(joint.id);                              
    TEST_EQ(joint.name);                    
    TEST_DBL(joint.direction[0]);                                    
    TEST_DBL(joint.direction[1]);                     
    TEST_DBL(joint.direction[2]); 
    TEST_DBL(joint.length);                       
    TEST_DBL(joint.axis[0]);                     
    TEST_DBL(joint.axis[1]);
    TEST_DBL(joint.axis[2]);
    TEST_EQ(joint.order);                   
    TEST_EQ(joint.bodymass);                     
    TEST_EQ(joint.cofmass); 
    TEST_EQ(joint.dofs.size());
    for (int i = cs.origin->joint.dofs.size() - 1; i >= 0; --i) {
        TEST_EQ(joint.dofs[i].channel);
        TEST_EQ(joint.dofs[i].minLimit);
        TEST_EQ(joint.dofs[i].maxLimit);
    }
    TEST_EQ(active);

    TEST_DBL(childParentRotation[0]);
    TEST_DBL(childParentRotation[1]);
    TEST_DBL(childParentRotation[2]);
    TEST_DBL(childParentRotation[3]);

    auto parentOrigin = cs.origin->joint.parent.lock();
    auto parentClone = cs.clone->joint.parent.lock();
    if (parentOrigin && parentClone) {
        CPPUNIT_ASSERT(parentOrigin.get() != parentClone.get());
        CPPUNIT_ASSERT_EQUAL(parentOrigin->id, parentClone->id);
        CPPUNIT_ASSERT_EQUAL(parentOrigin->name, parentClone->name);
    } else {
        CPPUNIT_ASSERT(!parentOrigin && !parentClone);
    }

    auto aparentOrigin = cs.origin->activeParent.lock();
    auto aparentClone = cs.clone->activeParent.lock();
    if (aparentOrigin && aparentClone) {
        CPPUNIT_ASSERT(aparentOrigin.get() != aparentClone.get());
        CPPUNIT_ASSERT_EQUAL(aparentOrigin->joint.id, aparentClone->joint.id);
        CPPUNIT_ASSERT_EQUAL(aparentOrigin->joint.name, aparentClone->joint.name);
    } else {
        CPPUNIT_ASSERT(!aparentOrigin && !aparentClone);
    }


    //std::vector<hAnimBonePtr> childrenBones;
    TEST_EQ(childrenBones.size());
    for (int i = cs.origin->childrenBones.size() - 1; i >= 0; --i) {
        auto boneOrigin = cs.origin->childrenBones[i];
        auto boneClone = cs.clone->childrenBones[i];
        if (boneOrigin && boneClone) {

            CPPUNIT_ASSERT(boneOrigin.get() != boneClone.get());
            boneCloneTest bct = {boneOrigin, boneClone};
            hAnimBone::_privateTestHook(bct);
        } else {
            CPPUNIT_ASSERT(!boneOrigin && !boneClone);
        }
    }


    TEST_EQ(activeJointChildren.size());
    for (int i = cs.origin->activeJointChildren.size() - 1; i >= 0; --i) {
        auto ajointOrigin = cs.origin->activeJointChildren[i];
        auto ajointClone = cs.clone->activeJointChildren[i];
        if (ajointOrigin && ajointClone) {

            CPPUNIT_ASSERT(ajointOrigin.get() != ajointClone.get());
            jointCloneTest bct = {ajointOrigin, ajointClone};
            hAnimJoint::_privateTestHook(bct);
        } else {
            CPPUNIT_ASSERT(!ajointOrigin && !ajointClone);
        }
    }
    //JointWeakPtr parent;                      
    //std::vector<JointPtr> children;      
    //hAnimBoneWeak parentBone;                 
    //std::vector<hAnimBonePtr> childrenBones;  
    //hAnimJointWeak activeParent;              
    //std::vector<hAnimJointPtr> activeJointChildren;
}

template<>
void hAnimSkeleton::_privateTestHook<cloneTest>(cloneTest& cs)
{
    jointCloneTest joints = {cs.origin->root, cs.clone->root};
    hAnimJoint::_privateTestHook(joints);

    TEST_EQ(joints.size());
    TEST_EQ(bones.size());
    TEST_EQ(mappingSchemes.size());

    for (int i = cs.origin->mappingSchemes.size() - 1; i >= 0; --i) {
        auto mapping = cs.origin->mappingSchemes[i]->getMappingDictionary();
        auto mappingClone = cs.clone->mappingSchemes[i]->getMappingDictionary();
        for (auto el = mapping.begin(); el != mapping.end(); ++el) {
            CPPUNIT_ASSERT_EQUAL(mappingClone[el->first], el->second);
        }       
    }
    
    for (auto it1 = cs.origin->joints.begin(), it2 = cs.clone->joints.begin(); it1 != cs.origin->joints.end(); it1++, it2++) {
        
        CPPUNIT_ASSERT_EQUAL(it1->first, it2->first );
        CPPUNIT_ASSERT(it1->second.get() != it2->second.get());
        jointCloneTest jct = { it1->second, it2->second };
        hAnimJoint::_privateTestHook(jct);
    }

    for (auto it1 = cs.origin->bones.begin(), it2 = cs.clone->bones.begin(); it1 != cs.origin->bones.end(); it1++, it2++) {

        CPPUNIT_ASSERT_EQUAL(it1->first, it2->first );
        CPPUNIT_ASSERT(it1->second.get() != it2->second.get());
        boneCloneTest bct = { it1->second, it2->second };
        hAnimBone::_privateTestHook(bct);
    }
    
    
}

void KinematicModelTest::testHAnimClone()
{
    {
        hAnimSkeletonPtr origin = hAnimSkeleton::create();
        cloneTest ct = { origin, hAnimSkeletonPtr(origin->clone()) };
        hAnimSkeleton::_privateTestHook(ct);
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
        cloneTest ct = { joints->getHAnimSkeleton(), hAnimSkeletonPtr(joints->getHAnimSkeleton()->clone()) };

        hAnimSkeleton::_privateTestHook(ct);
    }
}
#else
void KinematicModelTest::testHAnimClone() {}
#endif

