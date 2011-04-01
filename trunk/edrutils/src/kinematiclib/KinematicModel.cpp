#include "stdafx.h"
#include <algorithm>
#include <osg/Quat>
#include <osg/Vec3>
#include <kinematiclib/SkeletalParsers.h>
#include <kinematiclib/KinematicModel.h>

using namespace std;

namespace kinematic {

void KinematicModel::backJointAndBone(std::string newJointName, std::string newBoneName,
    hAnimJointPtr& parentJoint, hAnimBonePtr& parentBone)
{
    parentJoint = joints[newJointName];
    parentBone = bones[newBoneName];
}

void KinematicModel::createJointAndBone(std::string newJointName, std::string newBoneName,
    hAnimJointPtr& parentJoint, hAnimBonePtr& parentBone)
{
    hAnimJointPtr joint(new hAnimJoint);
    hAnimBonePtr bone(new hAnimBone);
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

KinematicModel::KinematicModel(void) :
    lengthRatio(-1.0)
{
    //TODO : zamiast takiego rozwiazania wymusic zaladowanie slownika
    //jointMappingDictionary["Hips"] = "HumanoidRoot";

    jointMappingDictionary["root0"] = "vl5";
    jointMappingDictionary["root1"] = "l_hip";
    jointMappingDictionary["root2"] = "r_hip";
    jointMappingDictionary["ToSpine"] = "vl4";
    jointMappingDictionary["Spine"] = "vt12";
    jointMappingDictionary["Spine1"] = "vt1";
    jointMappingDictionary["Spine10"] = "vc7";
    jointMappingDictionary["Spine11"] = "l_sternoclavicular";
    jointMappingDictionary["Spine12"] = "r_sternoclavicular";
    jointMappingDictionary["Neck"] = "vc1";
    jointMappingDictionary["Head"] = "skullbase";
    jointMappingDictionary["LeftShoulder"] = "l_shoulder";
    jointMappingDictionary["LeftArm"] = "l_elbow";
    jointMappingDictionary["LeftForeArm"] = "l_wrist";
    jointMappingDictionary["LeftHand"] = "l_middle0";
    jointMappingDictionary["RightShoulder"] = "r_shoulder";
    jointMappingDictionary["RightArm"] = "r_elbow";
    jointMappingDictionary["RightForeArm"] = "r_wrist";
    jointMappingDictionary["RightHand"] = "r_middle0";
    jointMappingDictionary["LeftUpLeg"] = "l_knee";
    jointMappingDictionary["LeftLeg"] = "l_ankle";
    jointMappingDictionary["LeftFoot"] = "l_subtalar";
    jointMappingDictionary["LeftToeBase"] = "l_metatarsal";
    jointMappingDictionary["RightUpLeg"] = "r_knee";
    jointMappingDictionary["RightLeg"] = "r_ankle";
    jointMappingDictionary["RightFoot"] = "r_subtalar";
    jointMappingDictionary["RightToeBase"] = "r_metatarsal";
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


    jointMappingDictionary["root"] = "HumanoidRoot"; 
    jointMappingDictionary["LeftUpLeg_dum"] = "l_hip"; 
    jointMappingDictionary["RightUpLeg_dum"] = "r_hip"; 
    jointMappingDictionary["Spine_dum"] = "vl5"; 
    jointMappingDictionary["LeftUpLeg"] = "l_knee"; 
    jointMappingDictionary["LeftLeg"] = "l_ankle"; 
    jointMappingDictionary["LeftFoot"] = "l_subtalar";  
    jointMappingDictionary["LeftToeBase"] = "l_metatarsal"; 
    jointMappingDictionary["RightUpLeg"] = "r_knee"; 
    jointMappingDictionary["RightLeg"] = "r_ankle"; 
    jointMappingDictionary["RightFoot"] = "r_subtalar"; 
    jointMappingDictionary["RightToeBase"] = "r_metatarsal"; 
    jointMappingDictionary["Spine"] = "vt12"; 
    jointMappingDictionary["Spine1"] = "vt1"; 
    jointMappingDictionary["Spine1_dum1"] = "vc7";  
    jointMappingDictionary["Spine1_dum2"] = "l_sternoclavicular";  
    jointMappingDictionary["Spine1_dum3"] = "r_sternoclavicular"; 
    jointMappingDictionary["Neck"] = "vc1";  
    jointMappingDictionary["Head"] = "skullbase"; 
    jointMappingDictionary["LeftShoulder"] = "l_acromioclavicular"; 
    jointMappingDictionary["LeftArm"] = "l_shoulder"; 
    jointMappingDictionary["LeftForeArm"] = "l_elbow"; 
    jointMappingDictionary["LeftHand"] = "l_wrist"; 
    jointMappingDictionary["LeftHand_dum2"] = "l_middle0"; 
    jointMappingDictionary["LeftHand_dum1"] = "l_thumb1";  
    jointMappingDictionary["LeftHandThumb"] = "l_thumb2"; 
    jointMappingDictionary["RightShoulder"] = "r_acromioclavicular";  
    jointMappingDictionary["RightArm"] = "r_shoulder"; 
    jointMappingDictionary["RightForeArm"] = "r_elbow"; 
    jointMappingDictionary["RightHand"] = "r_wrist"; 
    jointMappingDictionary["RightHand_dum2"] = "r_middle0"; 
    jointMappingDictionary["RightHand_dum1"] = "r_thumb1";  
    jointMappingDictionary["RightHandThumb"] = "r_thumb2"; 
    
    jointMappingDictionary["lshoulderjoint"] = "l_sternoclavicular";
    jointMappingDictionary["rshoulderjoint"] = "r_sternoclavicular";

}

KinematicModel::KinematicModel(const std::string& dictionaryFilename)
{
    loadMappingDictionary(dictionaryFilename);
}

hAnimSkeleton::Ptr KinematicModel::createHAnim()
{
  hAnimJointPtr joint;
  hAnimBonePtr bone;
  hAnimJointPtr rootJoint;

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

    hAnimSkeleton::Ptr skeleton(new hAnimSkeleton);
    skeleton->setRoot(rootJoint);
    return skeleton;
}

std::string KinematicModel::mapJointName(const std::string& given)
{
    if (boneMappingDictionary.size() == 0 && jointMappingDictionary.size() == 0) {
        throw &kinematic::DictionaryNotLoadedException("dictionary with mapping scheme was not loaded");
    }
    std::map<std::string, std::string>::iterator it = boneMappingDictionary.find(given);
    if (it != boneMappingDictionary.end()) {
        hAnimBonePtr bone = bones[it->second];
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

void KinematicModel::setSkeletalData(SkeletalModelPtr skeletalModel)
{
    UTILS_ASSERT(!this->skeletalModel);
    //this->hAnimSkeleton = CreateHAnimSkeleton(skeletalModel);
    //double maxLength = getMaxBoneLength(
    this->skeletalModel = skeletalModel;
    
    this->frames.clear();
    
    std::vector<SkeletalModel::singleFramePtr> frm = skeletalModel->getFrames();
    this->frames.reserve(frm.size());
    std::for_each(frm.begin(), frm.end(), 
        [&](SkeletalModel::singleFramePtr framePtr) {
            this->frames.push_back(*framePtr);
    });
    this->haSkeleton = createHAnim(/*hmAnimation::HAnimHelper::_3_Full_HAnim_Hierarchy*/);
   double maxLength = getMaxBoneLength(skeletalModel->getSkeleton());
   // hack
   maxLength = 1.0;
   doSkeletonMapping(skeletalModel);
   lengthRatio = maxLength;

   std::for_each(joints.begin(), joints.end(), [&](std::pair<string, hAnimJointPtr> p) 
                {   
                    hAnimJointPtr joint = p.second;
                    joint->length /= maxLength;
                    
                    osg::Vec3 shift = joint->direction * joint->length;
                    shift = vectorRotation(shift, 
                        osg::DegreesToRadians(-joint->axis[0]),
                        osg::DegreesToRadians(-joint->axis[1]),
                        osg::DegreesToRadians(-joint->axis[2]));
                    joint->setLocalShift(shift);

                    osg::Quat pc;
                    JointPtr parent = joint->parent.lock();
                    if (parent) {
                        pc = rotationParentChild(parent, joint);
                    }
                    joint->setChildParentRotation(pc);
                    //joint->setLocalShift(
                    /*double mul =  osg::DegreesToRadians(1.0);
                    osg::Quat mX; mX.makeRotate(mul * (-joint->axis[0]), osg::Vec3(1,0,0));
                    osg::Quat mY; mY.makeRotate(mul * (-joint->axis[1]), osg::Vec3(0,1,0));
                    osg::Quat mZ; mZ.makeRotate(mul * (-joint->axis[2]), osg::Vec3(0,0,1));
                    osg::Quat rot2 = mX * mY * mZ;
                    joint->direction = mZ * joint->direction;
                    joint->direction = mY * joint->direction;
                    joint->direction = mX * joint->direction;*/
                } 
   );

   createQuaternionRepresentation();
}

void KinematicModel::loadMappingDictionary(const std::string& filename) {
    mappingSchemes = SkeletonMappingScheme::LoadFromXML(filename);
}

double KinematicModel::getMaxBoneLength(const Skeleton& skeleton) const 
{
    JointPtr root = skeleton.getRoot();
    return getMaxLength(root, -1.0);
}

double KinematicModel::getMaxLength(JointPtr joint, double maxLength) const
{
    maxLength = std::max(maxLength, joint->length);
    for (int i = joint->children.size() - 1; i >= 0; --i) {
        maxLength = std::max(maxLength, getMaxLength(joint->children[i], maxLength));
    }
    return maxLength;
 }

void KinematicModel::doSkeletonMapping(SkeletalModelPtr skeletalModel)
{
    SkeletalModel::JointMap& jointMap = skeletalModel->getJointMap();
    SkeletalModel::JointMap::iterator it;
    for (it = jointMap.begin(); it != jointMap.end(); it++) {
        std::string mappedName = mapJointName(it->first);
        changeNameInFrames(it->first, mappedName);
        JointPtr joint = it->second;
        hAnimJointPtr hJoint = joints[mappedName];
        Joint::copyContent(joint, hJoint);
        hJoint->name = mappedName;
        hJoint->active = true;
        JointPtr parent = joint->parent.lock();
        if (parent) {
            std::string parentName = mapJointName(parent->name);
            hAnimJointPtr hParent = joints[parentName];
            hJoint->parent = hParent;
        }

        int count = joint->children.size();
        for (int i = 0; i < count; i++) {
            std::string childName = mapJointName(joint->children[i]->name);
            hAnimJointPtr hChild = joints[childName];
            hJoint->children.push_back(hChild);
            hChild->parent = hJoint;
        }
    }
}

const std::map<std::string, osg::Quat>& KinematicModel::getQuaternionRotation(int frameNo) const
{
    return this->quaternionRepresentation[frameNo];
}

const std::map<std::string, osg::Quat>& KinematicModel::getQuaternionRotation(double time) const
{
    //std::vector<SkeletalModel::singleFrame>& frames = this->skeletalModel->getFrames();
    int framesNo = frames.size();
    // TODO : Slerp
    int frame = std::min(std::max(0, static_cast<int>(framesNo * time)), framesNo - 1);
    return getQuaternionRotation(frame);
}

void KinematicModel::createQuaternionRepresentation(void) 
{
    UTILS_ASSERT(lengthRatio > 0.0);
    int framesNo = frames.size();
    if (framesNo == 0) {
        throw kinematic::KinematicModelException("No frames loaded");
    }

    this->rootPositions.resize(framesNo);
    this->quaternionRepresentation.resize(framesNo);
    
    list<string> dummies;
    for_each(joints.begin(), joints.end(), [&](const pair<string, hAnimJointPtr>& p) {
        dummies.push_back(p.first);
    });

    vector<SkeletalModel::singleBoneState>& bdata = frames[0].bonesData;
    int bdataSize = bdata.size();

    for (int i = 0; i < framesNo; i++) {
        if (frames[i].bonesData.size() != bdataSize) {
            throw(KinematicModelException("Wrong data format"));
        }
    }

    for (int i = bdata.size() - 1; i >= 0; --i) {
        dummies.remove(bdata[i].name);
    }
    
    for (int i = 0; i < framesNo; i++) {
        int jointStatesNo = frames[i].bonesData.size();
        for (int j = 0; j < jointStatesNo; j++) {
            double rx, ry, rz;
            int index;
            std::string name = frames[i].bonesData[j].name;
            JointPtr joint = joints[name];
            if (!joint) {
                kinematic::WrongFileException("Joint " + name + " not found");
            }
            SkeletalModel::singleBoneState jointState = frames[i].bonesData[j];
            index = DegreeOfFreedom::getChannelIndex(DegreeOfFreedom::RZ, joint->dofs);
            rz = (index != -1 ? jointState.channelValues[index] : 0.0);
            index = DegreeOfFreedom::getChannelIndex(DegreeOfFreedom::RX, joint->dofs);
            rx = (index != -1 ? jointState.channelValues[index] : 0.0);
            index = DegreeOfFreedom::getChannelIndex(DegreeOfFreedom::RY, joint->dofs);
            ry = (index != -1 ? jointState.channelValues[index] : 0.0);
            /*osg::Vec3d v; v[0] = rx; v[1] = ry; v[2] = rz;
            osg::Matrix mat;*/
            double mul = skeletalModel->getAngle() == SkeletalModel::radians ? 1.0 : osg::DegreesToRadians(1.0);
            
            osg::Quat mX; mX.makeRotate(mul * (rx), osg::Vec3(1,0,0));
            osg::Quat mY; mY.makeRotate(mul * (ry), osg::Vec3(0,1,0));
            osg::Quat mZ; mZ.makeRotate(mul * (rz), osg::Vec3(0,0,1));
            
            if (name == "HumanoidRoot" || name == "root") {
                //quaternionRepresentation[i][name] = mZ * mY * mX;

                index = DegreeOfFreedom::getChannelIndex(DegreeOfFreedom::TX, joint->dofs);
                rootPositions[i][0] = (index != -1 ? jointState.channelValues[index] : 0.0);
                index = DegreeOfFreedom::getChannelIndex(DegreeOfFreedom::TY, joint->dofs);
                rootPositions[i][1] = (index != -1 ? jointState.channelValues[index] : 0.0);
                index = DegreeOfFreedom::getChannelIndex(DegreeOfFreedom::TZ, joint->dofs);
                rootPositions[i][2] = (index != -1 ? jointState.channelValues[index] : 0.0);
                rootPositions[i] /= lengthRatio;
            } /*else {
                quaternionRepresentation[i][name] = mX * mY * mZ;
            }*/

            quaternionRepresentation[i][name] = createRotation(mX, mY, mZ, joint->order);
        }

        osg::Quat ident;
        for_each(dummies.begin(), dummies.end(), [&](const string& name) {
            quaternionRepresentation[i][name] = ident;
        });
    }


}

template <typename T>
std::vector<T> KinematicModel::getRotation(double time)
{
	throw NotImplementedException();
}

//template<> std::vector<osg::Quat> KinematicModel::getRotation(double time) {
//    return getQuaternionRotation(time);
//}
//
//template<> std::vector<osg::Vec3d> KinematicModel::getRotation(double time) {
//    UTILS_ASSERT(false, "Nie zaimplementowano");
//    std::vector<osg::Vec3d> euler;
//    std::vector<osg::Quat>& q = getQuaternionRotation(time);
//    euler.resize(q.size());
//    for (int i = q.size() - 1; i >= 0; --i) {
//        // TODO : znalezc czy w osg mozliwa jest zmiana quat -> euler, jesli nie to zrobic to recznie
//        euler[i] = q[i].asVec3();
//    }
//
//    return euler;
//}
//----------------------------------------------------------------------------------
//template<> std::vector<osg::Matrixd> KinematicModel::getRotation(double time) {
//    std::vector<osg::Matrixd> mat;
//    std::vector<osg::Quat>& q = getQuaternionRotation(time);
//    mat.resize(q.size());
//    for (int i = q.size() - 1; i >= 0; --i) {
//        osg::Matrixd m(q[i]);
//        mat[i] = m;
//    }
//
//    return mat;
//}

hAnimJointPtr KinematicModel::getJointByName(const std::string&name)
{
    std::map<std::string, hAnimJointPtr>::iterator it = joints.find(name);
    if (it != joints.end()) {
        return it->second;
    }
    throw NotHAnimJointException(name);
}

hAnimBonePtr KinematicModel::getBoneByName(const std::string&name)
{
	std::map<std::string, hAnimBonePtr>::iterator it = bones.find(name);
    if (it != bones.end()) {
        return it->second;
    }
    throw NotHAnimJointException("Bone " + name + " not found", name);
}

void KinematicModel::activateJoint(hAnimJointPtr joint, bool active)
{
    if (joint->isActive() == active) {
        return;
    }

    hAnimBonePtr boneParent = joint->getParentBone().lock();
    hAnimJointPtr null;
    hAnimJointPtr prev = joint;
    hAnimJointPtr parent = boneParent ? boneParent->parentJoint.lock() : null;
    while (parent && parent->isActive() == false) {
        prev = parent;
        boneParent = parent->getParentBone().lock();
        parent = boneParent ? boneParent->parentJoint.lock() : null;
    }

    if (!parent) {
        throw KinematicModelException("Unknown error");
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
                hAnimJointPtr temp = boost::shared_dynamic_cast<hAnimJoint>(parent->children[index]);
                parent->children[index] = joint;
                joint->children.push_back(temp);
            }
        } else {
            throw NotYetImplemented("Setting active to false when children.size > 1");
        }
    } else {
        if (active) {
            JointPtr child = parent->children[0];
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

void KinematicModel::activateJoint( const std::string& jointName, bool active )
{
    hAnimJointPtr joint = getJointByName(jointName);
    activateJoint(joint, active);
}


SkeletalModelPtr KinematicModel::createSkeletalData() const 
{
    SkeletalModelPtr model(new SkeletalModel);
    Skeleton& skeleton = model->getSkeleton();
    JointPtr newRoot = cloneRootWithActivated(this->haSkeleton->getRoot());
    skeleton.setRoot(newRoot);

    model->setName("HumanModel");
    UTILS_ASSERT(false, "testowo zmienila sie struktura ramek w parserach!");
    //model->getFrames() = frames;
    model->RecreateMaps();
    return model;
}

JointPtr KinematicModel::cloneRootWithActivated(const JointPtr origin ) const
{
    return Joint::clone(origin);
}

void KinematicModel::changeNameInFrames(const std::string& oldName, const std::string& newName)
{
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

bool KinematicModel::checkLink(hAnimJointPtr parent, hAnimJointPtr middle, hAnimJointPtr child)
{
    JointPtr tempParent = child->parent.lock();
    hAnimJointPtr cp = boost::shared_dynamic_cast<hAnimJoint>(tempParent);
    while (cp && cp != parent) {
        if (cp == middle) {
            return true;
        }
        tempParent = cp->parent.lock();
        cp = boost::shared_dynamic_cast<hAnimJoint>(tempParent);
    }

    return false;
}

osg::Vec3 KinematicModel::vectorRotation( osg::Vec3 v,double rX, double rY, double rZ )
{
    osg::Quat rotz; rotz.makeRotate(rZ, 0,0,1);
    osg::Quat roty; roty.makeRotate(rY, 0,1,0);
    osg::Quat rotx; rotx.makeRotate(rX, 1,0,0);
    v = rotz * v;
    v = roty * v;
    v = rotx * v;
    return v;
}

osg::Quat KinematicModel::rotationParentChild(JointPtr parent, JointPtr child)
{
    double mul  = osg::DegreesToRadians(1.0);
    osg::Matrix matParZ; matParZ.makeRotate(-parent->axis[2] * mul, 0.0, 0.0, 1.0);
    osg::Matrix matParY; matParY.makeRotate(-parent->axis[1] * mul, 0.0, 1.0, 0.0);
    osg::Matrix matParX; matParX.makeRotate(-parent->axis[0] * mul, 1.0, 0.0, 0.0);

    osg::Matrix matPar = matParZ * matParY * matParX;
    osg::Matrix matChiZ; matChiZ.makeRotate(child->axis[2] * mul, 0.0, 0.0, 1.0);
    osg::Matrix matChiY; matChiY.makeRotate(child->axis[1] * mul, 0.0, 1.0, 0.0);
    osg::Matrix matChiX; matChiX.makeRotate(child->axis[0] * mul, 1.0, 0.0, 0.0);

    osg::Matrix matChi = matChiX * matChiY * matChiZ;
    osg::Matrix resM = matChi * matPar ;
    osg::Quat res; 
    res.set(resM);
    return res;
}

osg::Vec3 KinematicModel::getRootPosition( int frame ) const
{
    return rootPositions[frame];
}

osg::Vec3 KinematicModel::getRootPosition( double time ) const
{
    int framesNo = frames.size();
    // TODO : Lerp
    int frame = std::min(std::max(0, static_cast<int>(framesNo * time)), framesNo - 1);
    return getRootPosition(frame);
}

osg::Quat KinematicModel::createRotation( const osg::Quat& rX, const osg::Quat& rY, const osg::Quat& rZ, Axis::Order order )
{
    switch (order) 
    {
    case Axis::XYZ: return rX * rY * rZ;
    case Axis::XZY: return rX * rZ * rY;
    case Axis::YXZ: return rY * rX * rZ;
    case Axis::YZX: return rY * rZ * rX;
    case Axis::ZXY: return rZ * rX * rY;
    case Axis::ZYX: return rZ * rY * rX;
    default:
        UTILS_ASSERT(false);
        osg::Quat q;
        return q;
    }
}


std::vector<SkeletonMappingScheme> SkeletonMappingScheme::LoadFromXML(const std::string& filename)
{
    std::vector<SkeletonMappingScheme> result;
    TiXmlDocument doc(filename.c_str());
    bool loadOkay = doc.LoadFile();
    if (loadOkay) {
        readNode(&doc, result);
    } else {
        throw UnableToOpenFileException(filename);
    }
    return result;
}

void SkeletonMappingScheme::readNode(TiXmlNode* node, vector<SkeletonMappingScheme>& result) 
{
    if (!node) {
        return;
    }

    TiXmlNode* pChild;
    int t = node->Type();

    switch (t) {
    case TiXmlNode::TINYXML_DOCUMENT:
        break;

    case TiXmlNode::TINYXML_ELEMENT:
        {
            TiXmlElement* element = node->ToElement();
            if (element && strcmp(element->Value(), "SkeletonScheme") == 0) {
                mappingDict dict = readMappingNode(element);
                if (dict.size() > 0) {
                    SkeletonMappingScheme sms(dict);
                    result.push_back(sms);
                }
                return;
            }
        }
        break;

    case TiXmlNode::TINYXML_UNKNOWN:
        UTILS_ASSERT(false, "Unable to load dictionary");
        break;

    case TiXmlNode::TINYXML_TEXT:
        break;

    case TiXmlNode::TINYXML_DECLARATION:
        break;
    default:
        break;
    }
    for ( pChild = node->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
        readNode( pChild, result);
    }
}

SkeletonMappingScheme::mappingDict SkeletonMappingScheme::readMappingNode(TiXmlNode* node)
{
    mappingDict result;
    TiXmlNode* child;
    for ( child = node->FirstChild(); child != nullptr; child = child->NextSibling()) {
        int t = node->Type();
        switch (t) {
        case TiXmlNode::TINYXML_ELEMENT:
            {
                TiXmlElement* element = child ? child->ToElement() : nullptr;
                TiXmlAttribute* pAttrib= element ? element->FirstAttribute() : nullptr;
                string name;
                string val;
                while (pAttrib) {
                    if (strcmp(pAttrib->Name(),"hanim") == 0) {
                        name = pAttrib->Value();
                    } else if (strcmp(pAttrib->Name(), "name") == 0) {
                        val = pAttrib->Value();
                    } else {
                        UTILS_ASSERT(false, "Wrong file");
                    }
                    pAttrib=pAttrib->Next();
                }

                UTILS_ASSERT(val.size() > 0 && name.size() > 0, "Wrong file");
                result[val] = name;
            }
            break;

        default:
            UTILS_ASSERT(false, "wrong dictionary file");
            break;
        }
    }
    return result;
}

} //kinematic
