#include "stdafx.h"
#include <KinematicModelLibrary/SkeletalParsers.h>
#include <osgAnimation/Bone>
#include <osgAnimation/Skeleton>
#include <KinematicModelLibrary/KinematicModel.h>


using namespace hmAnimation;
using namespace std;
//----------------------------------------------------------------------------------
void KinematicModel::backJointAndBone(std::string newJointName, std::string newBoneName, hAnimJoint::Ptr& parentJoint, hAnimBone::Ptr& parentBone) {
    parentJoint = joints[newJointName];
    parentBone = bones[newBoneName];
}
//----------------------------------------------------------------------------------
void KinematicModel::createJointAndBone(std::string newJointName, std::string newBoneName, hAnimJoint::Ptr& parentJoint, hAnimBone::Ptr& parentBone) {
    hAnimJoint::Ptr joint(new hAnimJoint);
    hAnimBone::Ptr bone(new hAnimBone);
    if (parentBone) {
        parentBone->childrenJoints.push_back(joint);
    }
    joint->parentBone = parentBone;
    joint->setName(newJointName);
    joint->childrenBones.push_back(bone);
    
    bone->parentJoint = joint;
    bone->name = newBoneName;

    joints[joint->getName()] = joint;
    bones[bone->name] = bone;
    parentJoint = joint;
    parentBone = bone;
}
//----------------------------------------------------------------------------------
hmAnimation::KinematicModel::KinematicModel(void) {
    //TODO : zamiast takiego rozwiazania wymusic zaladowanie slownika
    //loadMappingDictionary("dictionary.txt");
     jointMappingDictionary["root"] = "HumanoidRoot";
     jointMappingDictionary["lhipjoint"] = "l_hip";
     jointMappingDictionary["lfemur"] = "l_knee";
     jointMappingDictionary["ltibia"] = "l_ankle";
     jointMappingDictionary["lfoot"] = "l_subtalar";
     jointMappingDictionary["ltoes"] = "l_metatarsal";
     jointMappingDictionary["rhipjoint"] = "r_hip";
     jointMappingDictionary["rfemur"] = "r_knee";
     jointMappingDictionary["rtibia"] = "r_ankle";
     jointMappingDictionary["rfoot"] = "r_subtalar";
     jointMappingDictionary["rtoes"] = "r_metatarsal";
     jointMappingDictionary["lowerback"] = "vl5";
     jointMappingDictionary["upperback"] = "vt12";
     jointMappingDictionary["thorax"] = "vt1";
     jointMappingDictionary["lowerneck"] = "vc7";
     jointMappingDictionary["upperneck"] = "vc1";
     jointMappingDictionary["head"] = "skullbase";
     jointMappingDictionary["lclavicle"] = "l_sternoclavicular";
     jointMappingDictionary["lhumerus"] = "l_shoulder";
     jointMappingDictionary["lradius"] = "l_elbow";
     jointMappingDictionary["lwrist"] = "l_wrist";
     jointMappingDictionary["lhand"] = "l_middle0";
     jointMappingDictionary["lfingers"] = "l_middle1";
     jointMappingDictionary["lthumb"] = "l_thumb1";
     jointMappingDictionary["rclavicle"] = "r_sternoclavicular";
     jointMappingDictionary["rhumerus"] = "r_shoulder";
     jointMappingDictionary["rradius"] = "r_elbow";
     jointMappingDictionary["rwrist"] = "r_wrist";
     jointMappingDictionary["rhand"] = "r_middle0";
     jointMappingDictionary["rfingers"] = "r_middle1";
     jointMappingDictionary["rthumb"] = "r_thumb1";
}
//----------------------------------------------------------------------------------
hmAnimation::KinematicModel::KinematicModel(const std::string& dictionaryFilename)
{
	loadMappingDictionary(dictionaryFilename);
}
//----------------------------------------------------------------------------------
hmAnimation::KinematicModel::~KinematicModel(void) {
}
//----------------------------------------------------------------------------------
hAnimSkeleton::Ptr KinematicModel::createHAnim(/*HAnimHelper::LevelOfArticulation loa*/) {
  hAnimJoint::Ptr joint;
  hAnimBone::Ptr bone;
  hAnimJoint::Ptr rootJoint;

  createJointAndBone("HumanoidRoot", "sacrum", joint, bone); rootJoint = joint;
  createJointAndBone("sacroiliac", "pelvis", joint, bone);
     createJointAndBone("l_hip", "l_thigh", joint, bone);
       createJointAndBone("l_knee", "l_calf", joint, bone);
         createJointAndBone("l_ankle", "l_hindfoot", joint, bone);
           createJointAndBone("l_subtalar", "l_midproximal", joint, bone);
             createJointAndBone("l_midtarsal", "l_middistal", joint, bone);
               createJointAndBone("l_metatarsal", "l_forefoot", joint, bone);
     backJointAndBone("sacroiliac", "pelvis", joint, bone);
     createJointAndBone("r_hip", "r_thigh", joint, bone);
       createJointAndBone("r_knee", "r_calf", joint, bone);
         createJointAndBone("r_ankle", "r_hindfoot", joint, bone);
           createJointAndBone("r_subtalar",  "r_midproximal", joint, bone);
             createJointAndBone("r_midtarsal", "r_middistal", joint, bone);
               createJointAndBone("r_metatarsal", "r_forefoot", joint, bone);
  backJointAndBone("HumanoidRoot", "sacrum", joint, bone);
  createJointAndBone("vl5", "l5", joint, bone);
    createJointAndBone("vl4", "l4", joint, bone);
      createJointAndBone("vl3", "l3", joint, bone);
        createJointAndBone("vl2", "l2", joint, bone);
          createJointAndBone("vl1", "l1", joint, bone);
            createJointAndBone("vt12", "t12", joint, bone);
              createJointAndBone("vt11", "t11", joint, bone);
                createJointAndBone("vt10", "t10", joint, bone);
                  createJointAndBone("vt9", "t9", joint, bone);
                    createJointAndBone("vt8", "t8", joint, bone);
                      createJointAndBone("vt7", "t7", joint, bone);
                        createJointAndBone("vt6", "t6", joint, bone);
                          createJointAndBone("vt5", "t5", joint, bone);
                            createJointAndBone("vt4", "t4", joint, bone);
                              createJointAndBone("vt3", "t3", joint, bone);
                                createJointAndBone("vt2", "t2", joint, bone);
                                  createJointAndBone("vt1", "t1", joint, bone);
                                  /*|*/createJointAndBone("vc7", "c7", joint, bone);
                                      createJointAndBone("vc6", "c6", joint, bone);
                                        createJointAndBone("vc5", "c5", joint, bone);
                                          createJointAndBone("vc4", "c4", joint, bone);
                                            createJointAndBone("vc3", "c3", joint, bone);
                                              createJointAndBone("vc2", "c2", joint, bone);
                                                createJointAndBone("vc1", "c1", joint, bone);
                                                  createJointAndBone("skullbase", "skull", joint, bone);
                                                    createJointAndBone("l_eyelid_joint", "l_eyelid", joint, bone);
                                                    backJointAndBone("skullbase", "skull", joint, bone);
                                                    createJointAndBone("r_eyelid_joint", "r_eyelid", joint, bone);
                                                    backJointAndBone("skullbase", "skull", joint, bone);
                                                    createJointAndBone("l_eyeball_joint", "l_eyeball", joint, bone);
                                                    backJointAndBone("skullbase", "skull", joint, bone);
                                                    createJointAndBone("r_eyeball_joint", "r_eyeball", joint, bone);
                                                    backJointAndBone("skullbase", "skull", joint, bone);
                                                    createJointAndBone("l_eyebrow_joint", "l_eyebrow", joint, bone);
                                                    backJointAndBone("skullbase", "skull", joint, bone);
                                                    createJointAndBone("r_eyebrow_joint", "r_eyebrow", joint, bone);
                                                    backJointAndBone("skullbase", "skull", joint, bone);
                                                    createJointAndBone("temporomandibular", "jaw", joint, bone); 
                                       backJointAndBone("vt1", "t1", joint, bone);
                                  /*|*/createJointAndBone("l_sternoclavicular", "l_clavicle", joint, bone);
                                      createJointAndBone("l_acromioclavicular", "l_scapula", joint, bone);
                                        createJointAndBone("l_shoulder", "l_upperarm", joint, bone);
                                          createJointAndBone("l_elbow", "l_forearm", joint, bone);
                                            createJointAndBone("l_wrist", "l_hand", joint, bone);
                                              createJointAndBone("l_thumb1", "l_thumb_metacarpal", joint, bone);
                                                createJointAndBone("l_thumb2", "l_thumb_proximal", joint, bone);
                                                    createJointAndBone("l_thumb3", "l_thumb_distal", joint, bone);
                                              backJointAndBone("l_wrist", "l_hand", joint, bone);
                                              createJointAndBone("l_index0", "l_index_metacarpal", joint, bone);
                                                createJointAndBone("l_index1", "l_index_proximal", joint, bone);
                                                  createJointAndBone("l_index2", "l_index_middle", joint, bone);
                                                      createJointAndBone("l_index3", "l_index_distal", joint, bone);
                                              backJointAndBone("l_wrist", "l_hand", joint, bone);
                                              createJointAndBone("l_middle0", "l_middle_metacarpal", joint, bone);
                                                createJointAndBone("l_middle1", "l_middle_proximal", joint, bone);
                                                  createJointAndBone("l_middle2", "l_middle_middle", joint, bone);
                                                    createJointAndBone("l_middle3", "l_middle_distal", joint, bone);
                                              backJointAndBone("l_wrist", "l_hand", joint, bone);
                                              createJointAndBone("l_ring0", "l_ring_metacarpal", joint, bone);
                                                createJointAndBone("l_ring1", "l_ring_proximal", joint, bone);
                                                  createJointAndBone("l_ring2", "l_ring_middle", joint, bone);
                                                    createJointAndBone("l_ring3", "l_ring_distal", joint, bone);
                                              backJointAndBone("l_wrist", "l_hand", joint, bone);
                                              createJointAndBone("l_pinky0", "l_pinky_metacarpal", joint, bone);
                                                createJointAndBone("l_pinky1", "l_pinky_proximal", joint, bone);
                                                  createJointAndBone("l_pinky2", "l_pinky_middle", joint, bone);
                                                    createJointAndBone("l_pinky3", "l_pinky_distal", joint, bone);
                                       backJointAndBone("vt1", "t1", joint, bone);
                                  /*|*/createJointAndBone("r_sternoclavicular", "r_clavicle", joint, bone);
                                      createJointAndBone("r_acromioclavicular", "r_scapula", joint, bone);
                                        createJointAndBone("r_shoulder", "r_upperarm", joint, bone);
                                          createJointAndBone("r_elbow", "r_forearm", joint, bone);
                                            createJointAndBone("r_wrist", "r_hand", joint, bone);
                                              createJointAndBone("r_thumb1", "r_thumb_metacarpal", joint, bone);
                                                createJointAndBone("r_thumb2", "r_thumb_proximal", joint, bone);
                                                  createJointAndBone("r_thumb3", "r_thumb_distal", joint, bone);
                                              backJointAndBone("r_wrist", "r_hand", joint, bone);
                                              createJointAndBone("r_index0", "r_index_metacarpal", joint, bone);
                                                createJointAndBone("r_index1", "r_index_proximal", joint, bone);
                                                  createJointAndBone("r_index2", "r_index_middle", joint, bone);
                                                    createJointAndBone("r_index3", "r_index_distal", joint, bone);
                                              backJointAndBone("r_wrist", "r_hand", joint, bone);
                                              createJointAndBone("r_middle0", "r_middle_metacarpal", joint, bone);
                                                createJointAndBone("r_middle1", "r_middle_proximal", joint, bone);
                                                  createJointAndBone("r_middle2", "r_middle_middle", joint, bone);
                                                    createJointAndBone("r_middle3", "r_middle_distal", joint, bone);
                                              backJointAndBone("r_wrist", "r_hand", joint, bone);
                                              createJointAndBone("r_ring0", "r_ring_metacarpal", joint, bone);
                                                createJointAndBone("r_ring1", "r_ring_proximal", joint, bone);
                                                  createJointAndBone("r_ring2", "r_ring_middle", joint, bone);
                                                    createJointAndBone("r_ring3", "r_ring_distal", joint, bone);
                                              backJointAndBone("r_wrist", "r_hand", joint, bone);
                                              createJointAndBone("r_pinky0", "r_pinky_metacarpal", joint, bone);
                                                createJointAndBone("r_pinky1", "r_pinky_proximal", joint, bone);
                                                  createJointAndBone("r_pinky2", "r_pinky_middle", joint, bone);
                                                    createJointAndBone("r_pinky3", "r_pinky_distal", joint, bone);

    hAnimSkeleton::Ptr skeleton(new hmAnimation::hAnimSkeleton);
    skeleton->setRoot(rootJoint);
    return skeleton;
}
//----------------------------------------------------------------------------------
std::string hmAnimation::KinematicModel::mapJointName(const std::string& given) {
    if (boneMappingDictionary.size() == 0 && jointMappingDictionary.size() == 0) {
        throw hmAnimation::DictionaryNotLoadedException("dictionary with mapping scheme was not loaded");
    }
    std::map<std::string, std::string>::iterator it = boneMappingDictionary.find(given);
    if (it != boneMappingDictionary.end()) {
        hAnimBone::Ptr bone = bones[it->second];
        return bone->parentJoint.lock()->name;
    } else {
        it = jointMappingDictionary.find(given);
        if (it != jointMappingDictionary.end()) {
            return it->second;
        } else {
            throw UnableToMapJointException(given);
        }
    }
}
//---------------------------------------------------------------------------------- 
//void hmAnimation::KinematicModel::createHAnimSkeleton(hmAnimation::SkeletalModel::Ptr SkeletalModel) {
//	Joint::Ptr root = skeletalModel->getSkeleton().getRoot();
//    root = Joint::clone(root);
//}
//----------------------------------------------------------------------------------
void hmAnimation::KinematicModel::setSkeletalData(hmAnimation::SkeletalModel::Ptr skeletalModel) {
    //this->hAnimSkeleton = CreateHAnimSkeleton(skeletalModel);
    //double maxLength = getMaxBoneLength(
    this->skeletalModel = skeletalModel;

    this->frames = skeletalModel->getFrames();

   this->haSkeleton = createHAnim(/*hmAnimation::HAnimHelper::_3_Full_HAnim_Hierarchy*/);
   double maxLength = getMaxBoneLength(skeletalModel->getSkeleton());
   doSkeletonMapping(skeletalModel);
   lengthRatio = maxLength;

   map<string, hAnimJoint::Ptr>::iterator it;
   for (it = joints.begin(); it != joints.end(); it++) {
       (it->second)->length /= maxLength;
   }
}
//----------------------------------------------------------------------------------
void hmAnimation::KinematicModel::loadMappingDictionary(const std::string& filename) {
    
    ifstream stream(filename.c_str(), ios_base::in);

    if (!stream) {
       throw AcclaimUnableToOpenFileException(filename);
    }

    std::string l;
    while(std::getline(stream, l)) {
        stringstream ss(l); 
        vector<string> tokens;
        string buffer;
        while (ss >> buffer) {
            tokens.push_back(buffer);
        }

        int count = tokens.size();
        for (int i = 0; i < count; i++) {
            // wpis [0] = [0] tez jest potrzebny (bo szkielet moze miec juz wlasciwe nazwy)
            jointMappingDictionary[tokens[0] ] = tokens[i];
        }
    }
}
//----------------------------------------------------------------------------------
double hmAnimation::KinematicModel::getMaxBoneLength(const Skeleton& skeleton) const {
	Joint::Ptr root = skeleton.getRoot();
    return getMaxLength(root, -1.0);
}
//----------------------------------------------------------------------------------
double hmAnimation::KinematicModel::getMaxLength(Joint::Ptr joint, double maxLength) const {
    maxLength = std::max(maxLength, joint->length);
    for (int i = joint->children.size() - 1; i >= 0; --i) {
        maxLength = std::max(maxLength, getMaxLength(joint->children[i], maxLength));
    }
	return(maxLength);
}


//----------------------------------------------------------------------------------
void hmAnimation::KinematicModel::doSkeletonMapping(SkeletalModel::Ptr skeletalModel) {
    SkeletalModel::JointMap& jointMap = skeletalModel->getJointMap();
    SkeletalModel::JointMap::iterator it;
    for (it = jointMap.begin(); it != jointMap.end(); it++) {
        std::string mappedName = mapJointName(it->first);
        changeNameInFrames(it->first, mappedName);
        Joint::Ptr joint = it->second;
        hAnimJoint::Ptr hJoint = joints[mappedName];
        Joint::copyContent(joint, hJoint);
        hJoint->name = mappedName;
        hJoint->active = true;
        Joint::Ptr parent = joint->parent.lock();
        if (parent) {
            std::string parentName = mapJointName(parent->name);
            hAnimJoint::Ptr hParent = joints[parentName];
            hJoint->parent = hParent;
        }

        int count = joint->children.size();
        for (int i = 0; i < count; i++) {
            std::string childName = mapJointName(joint->children[i]->name);
            hAnimJoint::Ptr hChild = joints[childName];
            hJoint->children.push_back(hChild);
            hChild->parent = hJoint;
        }
    }
}
//----------------------------------------------------------------------------------
std::vector<osg::Quat>& hmAnimation::KinematicModel::getQuaternionRotation(int frameNo) {
    return this->quaternionRepresentation[frameNo];
}
//----------------------------------------------------------------------------------
std::vector<osg::Quat>& hmAnimation::KinematicModel::getQuaternionRotation(double time) {
	//std::vector<SkeletalModel::singleFrame>& frames = this->skeletalModel->getFrames();
    int framesNo = frames.size();
    // TODO : Slerp
    time /= 100;
    int frame = std::min(std::max(0, static_cast<int>(framesNo * time)), framesNo - 1);
    return getQuaternionRotation(frame);
}
//----------------------------------------------------------------------------------
void hmAnimation::KinematicModel::createQuaternionRepresentation(void) {
    //std::vector<SkeletalModel::singleFrame>& frames = this->skeletalModel->getFrames();
    int framesNo = frames.size();
    this->quaternionRepresentation.resize(framesNo);
    //SkeletalModel::JointMap& jointsMap = this->skeletalModel->getJointMap();
    //int jointsNo = jointsMap.size();

    for (int i = 0; i < framesNo; i++) {
        //SkeletalModel::JointMap::iterator it;
        int jointStatesNo = frames[i].bonesData.size();
        this->quaternionRepresentation[i].resize(jointStatesNo);
        //for (it = jointsMap.begin(); it != jointsMap.end(); it++) {
        for (int j = 0; j < jointStatesNo; j++) {
            double rx, ry, rz;
            int index;
            std::string name = frames[i].bonesData[j].name;
            Joint::Ptr joint = joints[name];//it->second;
            
            SkeletalModel::singleBoneState jointState = frames[i].bonesData[j];
            index = DegreeOfFreedom::getChannelIndex(DegreeOfFreedom::RZ, joint->dofs);
            rz = (index != -1 ? jointState.channelValues[index] : 0.0);
            index = DegreeOfFreedom::getChannelIndex(DegreeOfFreedom::RX, joint->dofs);
            rx = (index != -1 ? jointState.channelValues[index] : 0.0);
            index = DegreeOfFreedom::getChannelIndex(DegreeOfFreedom::RY, joint->dofs);
            ry = (index != -1 ? jointState.channelValues[index] : 0.0);
            osg::Vec3d v; v[0] = rx; v[1] = ry; v[2] = rz;
            osg::Matrix mat; 
            double mul = skeletalModel->getAngle() == SkeletalModel::radians ? 1.0 : osg::DegreesToRadians(1.0);
            osg::Quat mX; mX.makeRotate(mul * (joint->axis[0]), osg::Vec3(1,0,0));
            osg::Quat mY; mY.makeRotate(mul * (joint->axis[1]), osg::Vec3(0,1,0));
            osg::Quat mZ; mZ.makeRotate(mul * (joint->axis[2]), osg::Vec3(0,0,1));
            quaternionRepresentation[i][j] = mX * mY * mZ;
        }
    }
}
//----------------------------------------------------------------------------------
template <typename T>
std::vector<T> hmAnimation::KinematicModel::getRotation(double time)
{
	throw hmAnimation::NotImplementedException();
}
//----------------------------------------------------------------------------------
template<> std::vector<osg::Quat> KinematicModel::getRotation(double time) {
    return getQuaternionRotation(time);
}
//----------------------------------------------------------------------------------
template<> std::vector<osg::Vec3d> KinematicModel::getRotation(double time) {
    std::vector<osg::Vec3d> euler;
    std::vector<osg::Quat>& q = getQuaternionRotation(time);
    euler.resize(q.size());
    for (int i = q.size() - 1; i >= 0; --i) {
        // TODO : znalezc czy w osg mozliwa jest zmiana quat -> euler, jesli nie to zrobic to recznie
        euler[i] = q[i].asVec3();
    }

    return euler;
}
//----------------------------------------------------------------------------------
template<> std::vector<osg::Matrixd> KinematicModel::getRotation(double time) {
    std::vector<osg::Matrixd> mat;
    std::vector<osg::Quat>& q = getQuaternionRotation(time);
    mat.resize(q.size());
    for (int i = q.size() - 1; i >= 0; --i) {
        osg::Matrixd m(q[i]);
        mat[i] = m;
    }

    return mat;
}
//----------------------------------------------------------------------------------
//osg::Node* createBase(const osg::Vec3& center,float radius)
//{
//    int numTilesX = 10;
//    int numTilesY = 10;
//    
//    float width = 2*radius;
//    float height = 2*radius;
//    
//    osg::Vec3 v000(center - osg::Vec3(width*0.5f,height*0.5f,0.0f));
//    osg::Vec3 dx(osg::Vec3(width/((float)numTilesX),0.0,0.0f));
//    osg::Vec3 dy(osg::Vec3(0.0f,height/((float)numTilesY),0.0f));
//    
//    // fill in vertices for grid, note numTilesX+1 * numTilesY+1...
//    osg::Vec3Array* coords = new osg::Vec3Array;
//    int iy;
//    for(iy=0;iy<=numTilesY;++iy)
//    {
//        for(int ix=0;ix<=numTilesX;++ix)
//        {
//            coords->push_back(v000+dx*(float)ix+dy*(float)iy);
//        }
//    }
//    
//    //Just two colours - black and white.
//    osg::Vec4Array* colors = new osg::Vec4Array;
//    colors->push_back(osg::Vec4(1.0f,1.0f,1.0f,1.0f)); // white
//    colors->push_back(osg::Vec4(0.0f,0.0f,0.0f,1.0f)); // black
//    int numColors=colors->size();
//    
//    
//    int numIndicesPerRow=numTilesX+1;
//    osg::UByteArray* coordIndices = new osg::UByteArray; // assumes we are using less than 256 points...
//    osg::UByteArray* colorIndices = new osg::UByteArray;
//    for(iy=0;iy<numTilesY;++iy)
//    {
//        for(int ix=0;ix<numTilesX;++ix)
//        {
//            // four vertices per quad.
//            coordIndices->push_back(ix    +(iy+1)*numIndicesPerRow);
//            coordIndices->push_back(ix    +iy*numIndicesPerRow);
//            coordIndices->push_back((ix+1)+iy*numIndicesPerRow);
//            coordIndices->push_back((ix+1)+(iy+1)*numIndicesPerRow);
//            
//            // one color per quad
//            colorIndices->push_back((ix+iy)%numColors);
//        }
//    }
//    
//
//    // set up a single normal
//    osg::Vec3Array* normals = new osg::Vec3Array;
//    normals->push_back(osg::Vec3(0.0f,0.0f,1.0f));
//    
//
//    osg::Geometry* geom = new osg::Geometry;
//    geom->setVertexArray(coords);
//    geom->setVertexIndices(coordIndices);
//    
//    geom->setColorArray(colors);
//    geom->setColorIndices(colorIndices);
//    geom->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE);
//    
//    geom->setNormalArray(normals);
//    geom->setNormalBinding(osg::Geometry::BIND_OVERALL);
//    
//    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,coordIndices->size()));
//    
//    osg::Geode* geode = new osg::Geode;
//    geode->addDrawable(geom);
//    
//    return geode;
//}
//
//osg::Node* createMovingModel(const osg::Vec3& center, float radius)
//{ return NULL; }
////{
////    //float animationLength = 10.0f;
////
////    //osg::AnimationPath* animationPath = createAnimationPath(center,radius,animationLength);
////
////    //osg::Group* model = new osg::Group;
////
////    //osg::Node* glider = osgDB::readNodeFile("glider.osg");
////    if (glider)
////    {
////        //const osg::BoundingSphere& bs = glider->getBound();
////
////        //float size = radius/bs.radius()*0.3f;
////        //osg::MatrixTransform* positioned = new osg::MatrixTransform;
////        //positioned->setDataVariance(osg::Object::STATIC);
////        //positioned->setMatrix(osg::Matrix::translate(-bs.center())*
////        //                             osg::Matrix::scale(size,size,size)*
////        //                             osg::Matrix::rotate(osg::inDegrees(-90.0f),0.0f,0.0f,1.0f));
////    
////        //positioned->addChild(glider);
////    
////        //osg::PositionAttitudeTransform* xform = new osg::PositionAttitudeTransform;    
////        //xform->setUpdateCallback(new osg::AnimationPathCallback(animationPath,0.0,1.0));
////        //xform->addChild(positioned);
////
////        //model->addChild(xform);
////    }
//// 
////    osg::Node* cessna = osgDB::readNodeFile("cessna.osg");
////    //if (cessna)
////    //{
////    //    const osg::BoundingSphere& bs = cessna->getBound();
////
////    //    float size = radius/bs.radius()*0.3f;
////    //    osg::MatrixTransform* positioned = new osg::MatrixTransform;
////    //    positioned->setDataVariance(osg::Object::STATIC);
////    //    positioned->setMatrix(osg::Matrix::translate(-bs.center())*
////    //                                 osg::Matrix::scale(size,size,size)*
////    //                                 osg::Matrix::rotate(osg::inDegrees(180.0f),0.0f,0.0f,1.0f));
////    //
////    //    positioned->addChild(cessna);
////    //
////    //    osg::MatrixTransform* xform = new osg::MatrixTransform;
////    //    xform->setUpdateCallback(new osg::AnimationPathCallback(animationPath,0.0f,2.0));
////    //    xform->addChild(positioned);
////
////    //    model->addChild(xform);
////    //}
////    
////    return model;
////}
//
//osg::Node* createModel(bool overlay)
//{
//    osg::Vec3 center(0.0f,0.0f,0.0f);
//    float radius = 100.0f;
//
//    osg::Group* root = new osg::Group;
//
//    float baseHeight = center.z()-radius*0.5;
//    osg::Node* baseModel = createBase(osg::Vec3(center.x(), center.y(), baseHeight),radius);
//    osg::Node* movingModel = createMovingModel(center,radius*0.8f);
//
//    /*if (overlay)
//    {
//        osgSim::OverlayNode* overlayNode = new osgSim::OverlayNode(technique);
//        overlayNode->setContinuousUpdate(true);
//        overlayNode->setOverlaySubgraph(movingModel);
//        overlayNode->setOverlayBaseHeight(baseHeight-0.01);
//        overlayNode->addChild(baseModel);
//        root->addChild(overlayNode);
//    }
//    else*/
//    {
//    
//        root->addChild(baseModel);
//    }
//    
//    root->addChild(movingModel);
//
//    return root;
//}
// 
// 

hAnimJoint::Ptr hmAnimation::KinematicModel::getJointByName(const std::string&name) {
    std::map<std::string, hAnimJoint::Ptr>::iterator it = joints.find(name);
    if (it != joints.end()) {
        return it->second;
    }
    throw NotHAnimJointException(name);
}

hAnimBone::Ptr hmAnimation::KinematicModel::getBoneByName(const std::string&name) {
	std::map<std::string, hAnimBone::Ptr>::iterator it = bones.find(name);
    if (it != bones.end()) {
        return it->second;
    }
    throw NotHAnimJointException("Bone " + name + " not found", name);
}

void hmAnimation::KinematicModel::activateJoint(hAnimJoint::Ptr joint, bool active) {
    if (joint->isActive() == active) {
        return;
    }

    hAnimBone::Ptr boneParent = joint->getParentBone().lock();
    hAnimJoint::Ptr null;
    hAnimJoint::Ptr prev = joint;
    hAnimJoint::Ptr parent = boneParent ? boneParent->parentJoint.lock() : null;
    while (parent && parent->isActive() == false) {
        prev = parent;
        boneParent = parent->getParentBone().lock();
        parent = boneParent ? boneParent->parentJoint.lock() : null;
    }
    
    if (!parent) {
        throw hmAnimation::KinematicModelException("Unknown error");
    }

    int childCount = parent->children.size();
    if (childCount > 1) {
        //throw hmAnimation::AcclaimNotYetImplemented("");
        int index = -1;
        for (int i = 0; i < childCount; i++) {
            if (checkLink(parent, joint, boost::shared_dynamic_cast<hAnimJoint>(parent->children[i]))) {
                index = i; 
                break;
            }
        }

        if (active) {
            joint->active = true;
            joint->children.clear();
            if (index == -1) {
                parent->children.push_back(joint);
            } else {
                hAnimJoint::Ptr temp = boost::shared_dynamic_cast<hAnimJoint>(parent->children[index]);
                parent->children[index] = joint;
                joint->children.push_back(temp);
                    
            }
        } else {
            throw hmAnimation::NotYetImplemented("Setting active to false when children.size > 1");
        }
    } else {
        if (active) {
            Joint::Ptr child = parent->children[0];
            joint->active = true;
            joint->children.clear();
            joint->children.push_back(child);
            joint->parent = parent;
            parent->children[0] = joint;
            int size = frames.size();
            /*for (int i = 0; i < size; ++i) {
                SkeletalModel::singleBoneState state;
                state.name = joint->name;
                frames[i].bonesData.push_back(state);
            }*/
        } else {
            parent->children = joint->children;
            joint->children.clear();
            joint->parent = null;
            for (int i = parent->children.size() - 1; i >= 0; --i) {
                parent->children[i]->parent = parent;
            }
        }
    }
    this->createQuaternionRepresentation();
}

void hmAnimation::KinematicModel::activateJoint( const std::string& jointName, bool active ) {
    hAnimJoint::Ptr joint = getJointByName(jointName);
    activateJoint(joint, active);
}

hmAnimation::SkeletalModel::Ptr hmAnimation::KinematicModel::createSkeletalData() const {
    SkeletalModel::Ptr model(new SkeletalModel);
    Skeleton& skeleton = model->getSkeleton();
    Joint::Ptr newRoot = cloneRootWithActivated(this->haSkeleton->getRoot());
    skeleton.setRoot(newRoot);

    model->setName("HumanModel");
    model->getFrames() = frames;
    model->RecreateMaps();
    return model;
}

Joint::Ptr hmAnimation::KinematicModel::cloneRootWithActivated(const Joint::Ptr origin ) const {
    return Joint::clone(origin);
}

void hmAnimation::KinematicModel::changeNameInFrames( const std::string& oldName, const std::string& newName ) {
    // TODO : ASSERT
    int index = -1;
    for (int i = frames[0].bonesData.size() - 1; i >= 0; --i) {
        if (frames[0].bonesData[i].name == oldName) {
            index = i;
            break;
        }
    }
    if (index == -1) {
        return;
    }
    for (int i = frames.size() -1; i >= 0; --i) {
        frames[i].bonesData[index].name = newName;
    }
}

bool hmAnimation::KinematicModel::checkLink( hAnimJoint::Ptr parent, hAnimJoint::Ptr middle, hAnimJoint::Ptr child )
{
    Joint::Ptr tempParent = child->parent.lock();
    hAnimJoint::Ptr cp = boost::shared_dynamic_cast<hAnimJoint>(tempParent);
    while (cp && cp != parent) {
        if (cp == middle) {
            return true;
        }
        tempParent = cp->parent.lock();
        cp = boost::shared_dynamic_cast<hAnimJoint>(tempParent);
    }

    return false;
}


