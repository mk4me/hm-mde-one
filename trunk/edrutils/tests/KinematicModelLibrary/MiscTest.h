#ifndef HEADER_GUARD_MISCTEST_H__
#define HEADER_GUARD_MISCTEST_H__

#include <limits>
#include <cppunit/extensions/HelperMacros.h>
#include <KinematicModelLibrary/SkeletalParsers.h>

class MiscTest : public CPPUNIT_NS::TestCase
{
    CPPUNIT_TEST_SUITE(MiscTest);
    CPPUNIT_TEST(testOK);
    CPPUNIT_TEST(test1);
    //CPPUNIT_TEST(testFail);
    CPPUNIT_TEST_SUITE_END();
public:
    MiscTest(void);
    virtual ~MiscTest(void);

public:
    void testOK();
    void testFail();
    void test1();

    bool substestAsfAmc(const std::string& asfFile, const std::string& amcFile);

public:
    static bool testJointContentEquality(kinematic::JointPtr joint1, kinematic::JointPtr joint2, bool testNames = true, bool testDofs = true);
    //template <class T>
    //static bool isEqual(T left, T right, double epsilon = 0.001) { 
    //    return fabs(left - right) < epsilon;
    //}

    static bool isEqual(double left, double right, double epsilon = 0.001) {
        double inf = std::numeric_limits<double>::infinity();
        double ninf = -inf;
        if (left != left || right != right) {
            CPPUNIT_ASSERT(false);
            return false;
        }
        else if ((left == inf && right == inf) || (left == ninf && right == ninf)) {
            return true;

        } else {
            return fabs(left - right) < epsilon;
        }
    }

    static bool isEqual(osg::Vec3d left, osg::Vec3d right, double epsilon = 0.001) {
        return isEqual(left[0], right[0], epsilon) &&
               isEqual(left[1], right[1], epsilon) &&
               isEqual(left[2], right[2], epsilon);
    }

    static bool testSkeletonEquality(const kinematic::Skeleton& s1, const kinematic::Skeleton& s2);
private:
    typedef std::list<std::pair<std::string, std::string> > pairs;
    pairs fill(const std::string& rootDir);
};
#endif