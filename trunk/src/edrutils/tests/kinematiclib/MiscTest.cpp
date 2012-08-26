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
#include <kinematiclib/VskParser.h>
#include <list>
#include "MiscTest.h"
#include <boost/filesystem.hpp>


using namespace std;
using namespace kinematic;
using namespace boost::filesystem;


MiscTest::MiscTest(void)
{
}

MiscTest::~MiscTest(void)
{
}

void MiscTest::testOK() {
    Logger::getInstance().log(Logger::Info, "Test ktory powinien sie udac");
    AsfParser asf;
    SkeletalModelPtr model(new SkeletalModel);
    asf.parse(model, "kinematic/acclaim/cmu/01/01.asf");
    asf.save(model, "test.asf");
    CPPUNIT_ASSERT(true);
}

void MiscTest::testFail() {
    Logger::getInstance().log(Logger::Info, "Test ktory NIE powinien sie udac");
    CPPUNIT_ASSERT(false);
}
void MiscTest::test1() {
    list<pair<string, string> > toTest;
    
    toTest = fill("kinematic");
    //toTest.clear();
    //toTest.push_back(std::make_pair("kinematic/acclaim/accad/eric.asf","kinematic/acclaim/accad/eric_quickwalk.amc"));
    list<pair<string, string> >::iterator it;
    for (it = toTest.begin(); it != toTest.end(); it++) {
        Logger::getInstance().log(Logger::Info, "Test : " + it->second);
        CPPUNIT_ASSERT(substestAsfAmc(it->first, it->second));
    }

}

bool MiscTest::testJointContentEquality( JointPtr joint1, JointPtr joint2, bool testNames, bool testDofs) {
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
    SkeletalModelPtr model1(new SkeletalModel), model2(new SkeletalModel);
	SkeletalDataPtr data1(new SkeletalData), data2(new SkeletalData);
    AsfParser asf1, asf2; 
    AmcParser amc1, amc2;
    asf1.setForceXYZRoot(false);
    amc1.setForceRootXYZ(false);

    asf1.parse(model1, asfFile);
    amc1.parse(data1, amcFile);

	//model1->setSkeletalData(data1);

    asf1.save(model1, "temp.asf");
    amc1.save(model1, data1, "temp.amc");

    asf2.parse(model2, "temp.asf");
    amc2.parse(data2, "temp.amc");

	//model2->setSkeletalData(data2);

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

    std::vector<SkeletalData::singleFramePtr>& frames1 = data1->getFrames();
    std::vector<SkeletalData::singleFramePtr>& frames2 = data2->getFrames();

    CPPUNIT_ASSERT(frames1.size() == frames2.size());
    for (int i = frames1.size() - 1; i >= 0 ; --i) {
        SkeletalData::singleFrame& f1 = *(frames1[i]);
        SkeletalData::singleFrame& f2 = *(frames2[i]);
        CPPUNIT_ASSERT(f1.frameNo == f2.frameNo);
        for (int j = f1.jointsData.size() - 1; j >= 0; --j) {
            vector<double>& val1 = f1.jointsData[j]->channelValues;
            vector<double>& val2 = f2.jointsData[j]->channelValues;
            CPPUNIT_ASSERT(val1.size() == val2.size());
            for (int k = val1.size() - 1; k >= 0; --k) {
                CPPUNIT_ASSERT_DOUBLES_EQUAL(val1[k], val2[k], 0.0001);
            }
        }
    }

    return result;
}

MiscTest::pairs MiscTest::fill(const std::string& rootDir) {
    pairs temp;
    if (exists(rootDir) && is_directory(rootDir)) {

        typedef vector<path> vec;          
        vec v;                             

        copy(directory_iterator(rootDir), directory_iterator(), back_inserter(v));

        //sort(v.begin(), v.end());             // sort, since directory iteration
        // is not ordered on some file systems

        string asf = "";
        for (vec::const_iterator it(v.begin()), it_end(v.end()); it != it_end; ++it) {
            if (is_directory(*it)) {
                pairs p = fill(it->string());
                temp.insert(temp.end(), p.begin(), p.end());
            } else if (is_regular_file(*it) && extension(*it) == ".asf") {
                asf = it->string();
            }
        }

        if (asf.size() > 0) {
            for (vec::const_iterator it(v.begin()), it_end(v.end()); it != it_end; ++it) {
                if (is_regular_file(*it) && extension(*it) == ".amc") {
                    temp.push_back(std::make_pair(asf, it->string()));
                }
            }
        }
    } else {
        LOGGER(Logger::Info, "Wrong path : " + rootDir);
    }
    return temp;
}

void MiscTest::testVSK()
{
    VskParser vsk;
    try {
        vsk.parse("kinematic/MS39NEXUS.vsk");
    } catch (kinematic::UnableToOpenFileException& e) {
    }
}

 //bool MiscTest::testSkeletonEquality( const hmAnimation::Skeleton& s1, const hmAnimation::Skeleton& s2 ) {
 //    
 //}
