#ifndef __HEADER_GUARD_MISCTEST_H__
#define __HEADER_GUARD_MISCTEST_H__

#include <limits>
#include <cppunit/extensions/HelperMacros.h>
#include <KinematicModelLibrary/SkeletalParsers.h>

class MiscTest : public CPPUNIT_NS::TestCase
{
    CPPUNIT_TEST_SUITE(MiscTest);
    //CPPUNIT_TEST(test1);
    CPPUNIT_TEST(testOK);
    CPPUNIT_TEST(testFail);
    CPPUNIT_TEST_SUITE_END();
public:
    MiscTest(void);
    virtual ~MiscTest(void);

public:
    void testOK();
    void testFail();
    //void test1();

    //bool substestAsfAmc(const std::string& asfFile, const std::string& amcFile);

public:
    #define _epsilon = 0.000000001
    static bool testJointContentEquality(hmAnimation::Joint::Ptr joint1, hmAnimation::Joint::Ptr joint2, bool testNames = true, bool testDofs = true);
    template <class T>
    static bool isEqual(T left, T right, double epsilon = 0.001) { 
        return fabs(left - right) < epsilon;
    }

    static bool isEqual(osg::Vec3d left, osg::Vec3d right, double epsilon = 0.001) {
        return isEqual(left[0], right[0], epsilon) &&
               isEqual(left[1], right[1], epsilon) &&
               isEqual(left[2], right[2], epsilon);
    }

    //static bool testSkeletonEquality(const hmAnimation::Skeleton& s1, const hmAnimation::Skeleton& s2);
};
#endif