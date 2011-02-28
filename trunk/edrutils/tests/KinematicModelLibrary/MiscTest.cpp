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
#include <KinematicModelLibrary/SkeletalParsers.h>
#include <list>
#include "MiscTest.h"

using namespace std;
using namespace hmAnimation;
CPPUNIT_TEST_SUITE_REGISTRATION( MiscTest );

MiscTest::MiscTest(void)
{
}

MiscTest::~MiscTest(void)
{

}


void MiscTest::testOK() {
    Logger::getInstance().log(Logger::Info, "Test ktory powinien sie udac");
    AsfParser asf;
    SkeletalModel::Ptr model(new SkeletalModel);
    asf.parse(model, "kinematic/acclaim/cmu/01/01.asf");
    asf.save(model, "test.asf");
    CPPUNIT_ASSERT(true);
}

void MiscTest::testFail() {
    Logger::getInstance().log(Logger::Info, "Test ktory NIE powinien sie udac");
    CPPUNIT_ASSERT(false);
}
void MiscTest::test1() {
    string asf[] = {
       "kinematic/acclaim/cmu/01/01.asf",
       "kinematic/acclaim/cmu/05/05.asf",
       "kinematic/acclaim/cmu/08/08.asf",
       "kinematic/acclaim/cmu/09/09.asf",
       "kinematic/acclaim/cmu/10/10.asf",
       "kinematic/acclaim/cmu/03/03.asf",
       "kinematic/acclaim/cmu/07/07.asf",
       "kinematic/acclaim/cmu/06/06.asf",
       "kinematic/acclaim/cmu/02/02.asf",
       "kinematic/acclaim/accad/eric.asf",
       "kinematic/acclaim/accad/mike.asf",
       "kinematic/acclaim/siggraph02/matt1.asf",
       "kinematic/dual/mirai/acclaim/skeleton1/skel1.asf",
       "kinematic/dual/mirai/acclaim/skeleton2/skel2.asf",
       "kinematic/dual/megamocap/acclaim/dancing5.asf",
       "kinematic/dual/megamocap/acclaim/dancing6.asf"
    };

    list<pair<string, string> > toTest;
    //toTest.push_back(std::make_pair(asf[2], "D:\\MotionCapture\\tmp\\acclaim\\Mirai\\acclaim\\skeleton 1\\cross_step_skel1.AMC"));
   
    //toTest.push_back(std::make_pair(asf[0], "D:\\MotionCapture\\tmp\\acclaim\\megamocap_acclaim\\dancing5.AMC"));
    //toTest.push_back(std::make_pair(asf[1], "D:\\MotionCapture\\tmp\\acclaim\\megamocap_acclaim\\dancing6.AMC"));
    //toTest.push_back(std::make_pair(asf[2], "D:\\MotionCapture\\tmp\\acclaim\\Mirai\\acclaim\\skeleton 1\\drunk_skel1.AMC"));
    //toTest.push_back(std::make_pair(asf[2], "D:\\MotionCapture\\tmp\\acclaim\\Mirai\\acclaim\\skeleton 1\\gnome_skel1.AMC"));
    //toTest.push_back(std::make_pair(asf[2], "D:\\MotionCapture\\tmp\\acclaim\\Mirai\\acclaim\\skeleton 1\\hop_over_skel1.AMC"));
    //toTest.push_back(std::make_pair(asf[2], "D:\\MotionCapture\\tmp\\acclaim\\Mirai\\acclaim\\skeleton 1\\open_shut_skel1.AMC"));
    //toTest.push_back(std::make_pair(asf[2], "D:\\MotionCapture\\tmp\\acclaim\\Mirai\\acclaim\\skeleton 1\\pull_object_skel1.AMC"));
    //toTest.push_back(std::make_pair(asf[2], "D:\\MotionCapture\\tmp\\acclaim\\Mirai\\acclaim\\skeleton 1\\push1_skel1.AMC"));
    //toTest.push_back(std::make_pair(asf[2], "D:\\MotionCapture\\tmp\\acclaim\\Mirai\\acclaim\\skeleton 1\\push2_skel1.AMC"));
    //toTest.push_back(std::make_pair(asf[2], "D:\\MotionCapture\\tmp\\acclaim\\Mirai\\acclaim\\skeleton 1\\run_slow_skel1.AMC"));
    //toTest.push_back(std::make_pair(asf[2], "D:\\MotionCapture\\tmp\\acclaim\\Mirai\\acclaim\\skeleton 1\\shoot1_skel1.AMC"));
    //toTest.push_back(std::make_pair(asf[2], "D:\\MotionCapture\\tmp\\acclaim\\Mirai\\acclaim\\skeleton 1\\sneak1_skel1.AMC"));
    //toTest.push_back(std::make_pair(asf[2], "D:\\MotionCapture\\tmp\\acclaim\\Mirai\\acclaim\\skeleton 1\\sneak2_skel1.AMC"));
    //toTest.push_back(std::make_pair(asf[2], "D:\\MotionCapture\\tmp\\acclaim\\Mirai\\acclaim\\skeleton 1\\walk_fast_skel1.AMC"));
    //toTest.push_back(std::make_pair(asf[2], "D:\\MotionCapture\\tmp\\acclaim\\Mirai\\acclaim\\skeleton 1\\walk_medium_skel1.AMC"));
    //toTest.push_back(std::make_pair(asf[2], "D:\\MotionCapture\\tmp\\acclaim\\Mirai\\acclaim\\skeleton 1\\walk_right1_skel1.AMC"));
    //toTest.push_back(std::make_pair(asf[2], "D:\\MotionCapture\\tmp\\acclaim\\Mirai\\acclaim\\skeleton 1\\walk_right2_skel1.AMC"));
    //toTest.push_back(std::make_pair(asf[2], "D:\\MotionCapture\\tmp\\acclaim\\Mirai\\acclaim\\skeleton 1\\walk_stop_skel1.AMC"));
    //toTest.push_back(std::make_pair(asf[2], "D:\\MotionCapture\\tmp\\acclaim\\Mirai\\acclaim\\skeleton 1\\walk_trip_skel1.AMC"));
    //toTest.push_back(std::make_pair(asf[2], "D:\\MotionCapture\\tmp\\acclaim\\Mirai\\acclaim\\skeleton 1\\walk_wave_skel1.AMC"));
    //toTest.push_back(std::make_pair(asf[3], "D:\\MotionCapture\\tmp\\acclaim\\Mirai\\acclaim\\skeleton 2\\back_punch_left_skel2.AMC"));
    //toTest.push_back(std::make_pair(asf[3], "D:\\MotionCapture\\tmp\\acclaim\\Mirai\\acclaim\\skeleton 2\\kick_high_right_skel2.AMC"));
    //toTest.push_back(std::make_pair(asf[3], "D:\\MotionCapture\\tmp\\acclaim\\Mirai\\acclaim\\skeleton 2\\kick_punch_skel2.AMC"));

    toTest.push_back(std::make_pair(asf[0], "kinematic/acclaim/cmu/01/01_14.amc"));
    toTest.push_back(std::make_pair(asf[0], "kinematic/acclaim/cmu/01/01_09.amc"));
    toTest.push_back(std::make_pair(asf[0], "kinematic/acclaim/cmu/01/01_12.amc"));
    toTest.push_back(std::make_pair(asf[0], "kinematic/acclaim/cmu/01/01_13.amc"));
    toTest.push_back(std::make_pair(asf[0], "kinematic/acclaim/cmu/01/01_07.amc"));
    toTest.push_back(std::make_pair(asf[0], "kinematic/acclaim/cmu/01/01_11.amc"));
    toTest.push_back(std::make_pair(asf[0], "kinematic/acclaim/cmu/01/01_04.amc"));
    toTest.push_back(std::make_pair(asf[0], "kinematic/acclaim/cmu/01/01_08.amc"));
    toTest.push_back(std::make_pair(asf[0], "kinematic/acclaim/cmu/01/01_01.amc"));
    toTest.push_back(std::make_pair(asf[0], "kinematic/acclaim/cmu/01/01_10.amc"));
    toTest.push_back(std::make_pair(asf[0], "kinematic/acclaim/cmu/01/01_03.amc"));
    toTest.push_back(std::make_pair(asf[0], "kinematic/acclaim/cmu/01/01_05.amc"));
    toTest.push_back(std::make_pair(asf[0], "kinematic/acclaim/cmu/01/01_06.amc"));
    toTest.push_back(std::make_pair(asf[0], "kinematic/acclaim/cmu/01/01_02.amc"));
    toTest.push_back(std::make_pair(asf[1], "kinematic/acclaim/cmu/05/05_17.amc"));
    toTest.push_back(std::make_pair(asf[1], "kinematic/acclaim/cmu/05/05_04.amc"));
    toTest.push_back(std::make_pair(asf[1], "kinematic/acclaim/cmu/05/05_01.amc"));
    toTest.push_back(std::make_pair(asf[1], "kinematic/acclaim/cmu/05/05_16.amc"));
    toTest.push_back(std::make_pair(asf[1], "kinematic/acclaim/cmu/05/05_15.amc"));
    toTest.push_back(std::make_pair(asf[1], "kinematic/acclaim/cmu/05/05_20.amc"));
    toTest.push_back(std::make_pair(asf[1], "kinematic/acclaim/cmu/05/05_18.amc"));
    toTest.push_back(std::make_pair(asf[1], "kinematic/acclaim/cmu/05/05_12.amc"));
    toTest.push_back(std::make_pair(asf[1], "kinematic/acclaim/cmu/05/05_05.amc"));
    toTest.push_back(std::make_pair(asf[1], "kinematic/acclaim/cmu/05/05_14.amc"));
    toTest.push_back(std::make_pair(asf[1], "kinematic/acclaim/cmu/05/05_19.amc"));
    toTest.push_back(std::make_pair(asf[1], "kinematic/acclaim/cmu/05/05_06.amc"));
    toTest.push_back(std::make_pair(asf[1], "kinematic/acclaim/cmu/05/05_08.amc"));
    toTest.push_back(std::make_pair(asf[1], "kinematic/acclaim/cmu/05/05_03.amc"));
    toTest.push_back(std::make_pair(asf[1], "kinematic/acclaim/cmu/05/05_10.amc"));
    toTest.push_back(std::make_pair(asf[1], "kinematic/acclaim/cmu/05/05_09.amc"));
    toTest.push_back(std::make_pair(asf[1], "kinematic/acclaim/cmu/05/05_11.amc"));
    toTest.push_back(std::make_pair(asf[1], "kinematic/acclaim/cmu/05/05_02.amc"));
    toTest.push_back(std::make_pair(asf[1], "kinematic/acclaim/cmu/05/05_13.amc"));
    toTest.push_back(std::make_pair(asf[1], "kinematic/acclaim/cmu/05/05_07.amc"));
    toTest.push_back(std::make_pair(asf[2], "kinematic/acclaim/cmu/08/08_09.amc"));
    toTest.push_back(std::make_pair(asf[2], "kinematic/acclaim/cmu/08/08_01.amc"));
    toTest.push_back(std::make_pair(asf[2], "kinematic/acclaim/cmu/08/08_02.amc"));
    toTest.push_back(std::make_pair(asf[2], "kinematic/acclaim/cmu/08/08_11.amc"));
    toTest.push_back(std::make_pair(asf[2], "kinematic/acclaim/cmu/08/08_08.amc"));
    toTest.push_back(std::make_pair(asf[2], "kinematic/acclaim/cmu/08/08_04.amc"));
    toTest.push_back(std::make_pair(asf[2], "kinematic/acclaim/cmu/08/08_06.amc"));
    toTest.push_back(std::make_pair(asf[2], "kinematic/acclaim/cmu/08/08_07.amc"));
    toTest.push_back(std::make_pair(asf[2], "kinematic/acclaim/cmu/08/08_10.amc"));
    toTest.push_back(std::make_pair(asf[2], "kinematic/acclaim/cmu/08/08_05.amc"));
    toTest.push_back(std::make_pair(asf[2], "kinematic/acclaim/cmu/08/08_03.amc"));
    toTest.push_back(std::make_pair(asf[3], "kinematic/acclaim/cmu/09/09_02.amc"));
    toTest.push_back(std::make_pair(asf[3], "kinematic/acclaim/cmu/09/09_04.amc"));
    toTest.push_back(std::make_pair(asf[3], "kinematic/acclaim/cmu/09/09_01.amc"));
    toTest.push_back(std::make_pair(asf[3], "kinematic/acclaim/cmu/09/09_08.amc"));
    toTest.push_back(std::make_pair(asf[3], "kinematic/acclaim/cmu/09/09_09.amc"));
    toTest.push_back(std::make_pair(asf[3], "kinematic/acclaim/cmu/09/09_12.amc"));
    toTest.push_back(std::make_pair(asf[3], "kinematic/acclaim/cmu/09/09_10.amc"));
    toTest.push_back(std::make_pair(asf[3], "kinematic/acclaim/cmu/09/09_07.amc"));
    toTest.push_back(std::make_pair(asf[3], "kinematic/acclaim/cmu/09/09_05.amc"));
    toTest.push_back(std::make_pair(asf[3], "kinematic/acclaim/cmu/09/09_06.amc"));
    toTest.push_back(std::make_pair(asf[3], "kinematic/acclaim/cmu/09/09_03.amc"));
    toTest.push_back(std::make_pair(asf[3], "kinematic/acclaim/cmu/09/09_11.amc"));
    toTest.push_back(std::make_pair(asf[4], "kinematic/acclaim/cmu/10/10_01.amc"));
    toTest.push_back(std::make_pair(asf[4], "kinematic/acclaim/cmu/10/10_05.amc"));
    toTest.push_back(std::make_pair(asf[4], "kinematic/acclaim/cmu/10/10_06.amc"));
    toTest.push_back(std::make_pair(asf[4], "kinematic/acclaim/cmu/10/10_03.amc"));
    toTest.push_back(std::make_pair(asf[4], "kinematic/acclaim/cmu/10/10_02.amc"));
    toTest.push_back(std::make_pair(asf[4], "kinematic/acclaim/cmu/10/10_04.amc"));
    toTest.push_back(std::make_pair(asf[5], "kinematic/acclaim/cmu/03/03_02.amc"));
    toTest.push_back(std::make_pair(asf[5], "kinematic/acclaim/cmu/03/03_03.amc"));
    toTest.push_back(std::make_pair(asf[5], "kinematic/acclaim/cmu/03/03_04.amc"));
    toTest.push_back(std::make_pair(asf[5], "kinematic/acclaim/cmu/03/03_01.amc"));
    toTest.push_back(std::make_pair(asf[6], "kinematic/acclaim/cmu/07/07_06.amc"));
    toTest.push_back(std::make_pair(asf[6], "kinematic/acclaim/cmu/07/07_03.amc"));
    toTest.push_back(std::make_pair(asf[6], "kinematic/acclaim/cmu/07/07_12.amc"));
    toTest.push_back(std::make_pair(asf[6], "kinematic/acclaim/cmu/07/07_02.amc"));
    toTest.push_back(std::make_pair(asf[6], "kinematic/acclaim/cmu/07/07_04.amc"));
    toTest.push_back(std::make_pair(asf[6], "kinematic/acclaim/cmu/07/07_08.amc"));
    toTest.push_back(std::make_pair(asf[6], "kinematic/acclaim/cmu/07/07_07.amc"));
    toTest.push_back(std::make_pair(asf[6], "kinematic/acclaim/cmu/07/07_01.amc"));
    toTest.push_back(std::make_pair(asf[6], "kinematic/acclaim/cmu/07/07_10.amc"));
    toTest.push_back(std::make_pair(asf[6], "kinematic/acclaim/cmu/07/07_11.amc"));
    toTest.push_back(std::make_pair(asf[6], "kinematic/acclaim/cmu/07/07_09.amc"));
    toTest.push_back(std::make_pair(asf[6], "kinematic/acclaim/cmu/07/07_05.amc"));
    toTest.push_back(std::make_pair(asf[7], "kinematic/acclaim/cmu/06/06_01.amc"));
    toTest.push_back(std::make_pair(asf[7], "kinematic/acclaim/cmu/06/06_15.amc"));
    toTest.push_back(std::make_pair(asf[7], "kinematic/acclaim/cmu/06/06_08.amc"));
    toTest.push_back(std::make_pair(asf[7], "kinematic/acclaim/cmu/06/06_13.amc"));
    toTest.push_back(std::make_pair(asf[7], "kinematic/acclaim/cmu/06/06_09.amc"));
    toTest.push_back(std::make_pair(asf[7], "kinematic/acclaim/cmu/06/06_04.amc"));
    toTest.push_back(std::make_pair(asf[7], "kinematic/acclaim/cmu/06/06_10.amc"));
    toTest.push_back(std::make_pair(asf[7], "kinematic/acclaim/cmu/06/06_14.amc"));
    toTest.push_back(std::make_pair(asf[7], "kinematic/acclaim/cmu/06/06_02.amc"));
    toTest.push_back(std::make_pair(asf[7], "kinematic/acclaim/cmu/06/06_05.amc"));
    toTest.push_back(std::make_pair(asf[7], "kinematic/acclaim/cmu/06/06_12.amc"));
    toTest.push_back(std::make_pair(asf[7], "kinematic/acclaim/cmu/06/06_03.amc"));
    toTest.push_back(std::make_pair(asf[7], "kinematic/acclaim/cmu/06/06_07.amc"));
    toTest.push_back(std::make_pair(asf[7], "kinematic/acclaim/cmu/06/06_11.amc"));
    toTest.push_back(std::make_pair(asf[7], "kinematic/acclaim/cmu/06/06_06.amc"));
    toTest.push_back(std::make_pair(asf[8], "kinematic/acclaim/cmu/02/02_03.amc"));
    toTest.push_back(std::make_pair(asf[8], "kinematic/acclaim/cmu/02/02_02.amc"));
    toTest.push_back(std::make_pair(asf[8], "kinematic/acclaim/cmu/02/02_10.amc"));
    toTest.push_back(std::make_pair(asf[8], "kinematic/acclaim/cmu/02/02_05.amc"));
    toTest.push_back(std::make_pair(asf[8], "kinematic/acclaim/cmu/02/02_07.amc"));
    toTest.push_back(std::make_pair(asf[8], "kinematic/acclaim/cmu/02/02_01.amc"));
    toTest.push_back(std::make_pair(asf[8], "kinematic/acclaim/cmu/02/02_09.amc"));
    toTest.push_back(std::make_pair(asf[8], "kinematic/acclaim/cmu/02/02_04.amc"));
    toTest.push_back(std::make_pair(asf[8], "kinematic/acclaim/cmu/02/02_06.amc"));
    toTest.push_back(std::make_pair(asf[8], "kinematic/acclaim/cmu/02/02_08.amc"));
    toTest.push_back(std::make_pair(asf[9], "kinematic/acclaim/accad/mike_hangspring.amc"));
    toTest.push_back(std::make_pair(asf[9], "kinematic/acclaim/accad/mike_swagger.amc"));
    toTest.push_back(std::make_pair(asf[10], "kinematic/acclaim/accad/eric_sprint.amc"));
    toTest.push_back(std::make_pair(asf[10], "kinematic/acclaim/accad/eric_run.amc"));
    toTest.push_back(std::make_pair(asf[10], "kinematic/acclaim/accad/eric_quickwalk.amc"));
    toTest.push_back(std::make_pair(asf[11], "kinematic/acclaim/siggraph02/matt1.mattmoves07.amc"));
    toTest.push_back(std::make_pair(asf[11], "kinematic/acclaim/siggraph02/matt1.mattmoves14.amc"));
    toTest.push_back(std::make_pair(asf[12], "kinematic/dual/mirai/acclaim/skeleton1/walk_fast_skel1.amc"));
    toTest.push_back(std::make_pair(asf[12], "kinematic/dual/mirai/acclaim/skeleton1/walk_stop_skel1.amc"));
    toTest.push_back(std::make_pair(asf[12], "kinematic/dual/mirai/acclaim/skeleton1/walk_right2_skel1.amc"));
    toTest.push_back(std::make_pair(asf[12], "kinematic/dual/mirai/acclaim/skeleton1/pull_object_skel1.amc"));
    toTest.push_back(std::make_pair(asf[12], "kinematic/dual/mirai/acclaim/skeleton1/open_shut_skel1.amc"));
    toTest.push_back(std::make_pair(asf[12], "kinematic/dual/mirai/acclaim/skeleton1/walk_right1_skel1.amc"));
    toTest.push_back(std::make_pair(asf[12], "kinematic/dual/mirai/acclaim/skeleton1/sneak2_skel1.amc"));
    toTest.push_back(std::make_pair(asf[12], "kinematic/dual/mirai/acclaim/skeleton1/shoot1_skel1.amc"));
    toTest.push_back(std::make_pair(asf[12], "kinematic/dual/mirai/acclaim/skeleton1/sneak1_skel1.amc"));
    toTest.push_back(std::make_pair(asf[12], "kinematic/dual/mirai/acclaim/skeleton1/walk_medium_skel1.amc"));
    toTest.push_back(std::make_pair(asf[12], "kinematic/dual/mirai/acclaim/skeleton1/walk_trip_skel1.amc"));
    toTest.push_back(std::make_pair(asf[12], "kinematic/dual/mirai/acclaim/skeleton1/walk_wave_skel1.amc"));
    toTest.push_back(std::make_pair(asf[12], "kinematic/dual/mirai/acclaim/skeleton1/gnome_skel1.amc"));
    toTest.push_back(std::make_pair(asf[12], "kinematic/dual/mirai/acclaim/skeleton1/run_slow_skel1.amc"));
    toTest.push_back(std::make_pair(asf[12], "kinematic/dual/mirai/acclaim/skeleton1/hop_over_skel1.amc"));
    toTest.push_back(std::make_pair(asf[12], "kinematic/dual/mirai/acclaim/skeleton1/drunk_skel1.amc"));
    toTest.push_back(std::make_pair(asf[12], "kinematic/dual/mirai/acclaim/skeleton1/push2_skel1.amc"));
    toTest.push_back(std::make_pair(asf[12], "kinematic/dual/mirai/acclaim/skeleton1/push1_skel1.amc"));
    toTest.push_back(std::make_pair(asf[12], "kinematic/dual/mirai/acclaim/skeleton1/cross_step_skel1.amc"));
    toTest.push_back(std::make_pair(asf[13], "kinematic/dual/mirai/acclaim/skeleton2/back_punch_left_skel2.amc"));
    toTest.push_back(std::make_pair(asf[13], "kinematic/dual/mirai/acclaim/skeleton2/kick_punch_skel2.amc"));
    toTest.push_back(std::make_pair(asf[13], "kinematic/dual/mirai/acclaim/skeleton2/kick_high_right_skel2.amc"));
    toTest.push_back(std::make_pair(asf[14], "kinematic/dual/megamocap/acclaim/dancing6.amc"));
    toTest.push_back(std::make_pair(asf[15], "kinematic/dual/megamocap/acclaim/dancing5.amc"));


    list<pair<string, string> >::iterator it;
    for (it = toTest.begin(); it != toTest.end(); it++) {
        Logger::getInstance().log(Logger::Info, "Test : " + it->second);
        CPPUNIT_ASSERT(substestAsfAmc(it->first, it->second));
    }

}

bool MiscTest::testJointContentEquality( Joint::Ptr joint1, Joint::Ptr joint2, bool testNames, bool testDofs) {
    bool result = true;
    result &= isEqual(joint1->axis, joint2->axis);
    //result &= isEqual(joint1->bodymass, joint2->bodymass);
    //result &= isEqual(joint1->cofmass, joint2->cofmass);
    result &= isEqual(joint1->direction, joint2->direction);
    result &= isEqual(joint1->length, joint2->length);
    result &= joint1->order == joint2->order;
    result &= joint1->children.size() == joint2->children.size();
    
    result &= !testNames || (joint1->name == joint2->name);
    if (testDofs) {
        result &= joint1->dofs.size() == joint2->dofs.size();
        if (result) {
            for (int i = joint1->dofs.size() - 1; i >= 0; --i) {
                result &= (joint1->dofs[i]).channel == (joint2->dofs[i]).channel;
                result &= isEqual((joint1->dofs[i]).minLimit, (joint2->dofs[i]).minLimit);
                result &= isEqual((joint1->dofs[i]).maxLimit, (joint2->dofs[i]).maxLimit);
            }
        }
    }

    return result;
}   

bool MiscTest::substestAsfAmc( const std::string& asfFile, const std::string& amcFile )
{
    SkeletalModel::Ptr model1(new SkeletalModel), model2(new SkeletalModel);
    AsfParser asf1, asf2; 
    AmcParser amc1, amc2;
    asf1.setForceXYZRoot(false);

    asf1.parse(model1, asfFile);
    //amc1.parse(model1, amcFile);

    asf1.save(model1, "temp.asf");
    //amc1.save(model1, "temp.amc");

    asf2.parse(model2, "temp.asf");
    //asf2.parse(model2, "test.amc");

    SkeletalModel::JointMap::const_iterator it1, it2;
    const SkeletalModel::JointMap& map1 = model1->getJointMap();
    const SkeletalModel::JointMap& map2 = model2->getJointMap();
    bool result = true;
    if (map1.size() == map2.size()) {
        for (it1 = map1.begin(), it2 = map2.begin(); it1 != map1.end() && it2 != map2.end(); it1++, it2++) {
            result &= MiscTest::testJointContentEquality(it1->second, it2->second, true, true);
        }
    } else {
        return false;
    }

    return result;
}

 //bool MiscTest::testSkeletonEquality( const hmAnimation::Skeleton& s1, const hmAnimation::Skeleton& s2 ) {
 //    
 //}
