#include "stdafx.h"
#include <kinematiclib/hmException.h>
#include <kinematiclib/hAnimSkeleton.h>
#include <kinematiclib/SkeletalMappingScheme.h>

namespace kinematic 
{
hAnimSkeleton::hAnimSkeleton()
{
	//mappingSchemes.push_back(SkeletonMappingScheme::createDefault());
}

void hAnimSkeleton::backJointAndBone(std::string newJointName, std::string newBoneName,
	hAnimJointPtr& parentJoint, hAnimBonePtr& parentBone)
{
	parentJoint = joints[newJointName];
	parentBone = bones[newBoneName];
}

void hAnimSkeleton::createJointAndBone(std::string newJointName, std::string newBoneName,
	hAnimJointPtr& parentJoint, hAnimBonePtr& parentBone)
{
	hAnimJointPtr joint(new hAnimJoint);
	hAnimBonePtr bone(new hAnimBone);
	if (parentBone) {
		parentBone->getChildrenJoints().push_back(joint);
	}
	joint->setParentBone(parentBone);
	joint->setName(newJointName);
	joint->getChildrenBones().push_back(bone);

	bone->setParentJoint(joint);
	bone->setName(newBoneName);

	joints[joint->getName()] = joint;
	bones[bone->getName()] = bone;
	parentJoint = joint;
	parentBone = bone;
}

void hAnimSkeleton::registerMappingScheme(SkeletonMappingSchemePtr mappingScheme)
{
	if(mappingScheme == nullptr){
		throw std::invalid_argument("Uninitialized mapping scheme");
	}

	mappingSchemes.push_back(mappingScheme);
}

void hAnimSkeleton::loadMappingDictionary(const std::string& filename) {
	 SkeletonMappingScheme::loadFromXML(filename, mappingSchemes);
}

hAnimJointPtr hAnimSkeleton::getJointByName(const std::string&name)
{
	std::map<std::string, hAnimJointPtr>::iterator it = joints.find(name);
	if (it != joints.end()) {
		return it->second;
	}
	throw NotHAnimJointException(name);
}

hAnimBonePtr hAnimSkeleton::getBoneByName(const std::string& name)
{
	std::map<std::string, hAnimBonePtr>::iterator it = bones.find(name);
	if (it != bones.end()) {
		return it->second;
	}
	throw NotHAnimJointException("Bone " + name + " not found", name);
}

void hAnimSkeleton::clearActiveHierarchy( hAnimJointPtr parent )
{
	hAnimJointWeak emptyWeakPtr;
	for(auto bone : parent->getChildrenBones()) {
		for(auto joint : bone->getChildrenJoints()) {
			joint->getActiveJointChildren().clear();
			joint->setActiveParent(emptyWeakPtr);
		}
	}
}

void hAnimSkeleton::activateJoint(hAnimJointPtr joint, bool active)
{
	// todo;
	throw kinematic::Exception("not implemented");
}

void hAnimSkeleton::activateJoint( const std::string& jointName, bool active )
{
	hAnimJointPtr joint = getJointByName(jointName);
	activateJoint(joint, active);
}

void hAnimSkeleton::createActiveHierarchy( hAnimJointPtr parent, hAnimJointPtr lastActive )
{
	for(auto bone : parent->getChildrenBones()) {
		for(auto joint : bone->getChildrenJoints()) {
			if (joint->isActive()) {
				hAnimJointWeak parentWeak(lastActive);
				joint->setActiveParent(parentWeak);
				lastActive->getActiveJointChildren().push_back(joint);
				createActiveHierarchy(joint, joint);
			} else {
				createActiveHierarchy(joint, lastActive);
			}
		}
	}
}

void hAnimSkeleton::createActiveHierarchy()
{
	UTILS_ASSERT(root);
	root->getActiveJointChildren().clear();
	root->setActiveParent(hAnimJointWeak());
	clearActiveHierarchy(root);
	createActiveHierarchy(root, root);
}

std::string hAnimSkeleton::mapJointName(const std::string& given) const
{
	// hack
	const std::map<std::string, std::string>& jointMappingDictionary = mappingSchemes[0]->getJointsMappingDictionary();
	if (jointMappingDictionary.empty() == true) {
		throw kinematic::DictionaryNotLoadedException("dictionary with mapping scheme was not loaded");
	}
	auto it = jointMappingDictionary.find(given);
	if (it != jointMappingDictionary.end()) {
		return it->second;
	}
	
	throw UnableToMapJointException(given);
}

const std::pair<std::string, std::string> hAnimSkeleton::mapBoneNames(const std::string& given) const
{
	// hack
	const auto & bonesMappingDictionary = mappingSchemes[0]->getSegmentsMappingDictionary();
	if (bonesMappingDictionary.empty() == true) {
		throw kinematic::DictionaryNotLoadedException("dictionary with mapping scheme was not loaded");
	}

	auto it = bonesMappingDictionary.find(given);
	if (it != bonesMappingDictionary.end()) {
		return it->second;
	}	

	throw UnableToMapBoneException(given);	
}

void hAnimSkeleton::doSkeletonMapping(SkeletalModelConstPtr skeletalModel)
{
	const SkeletalModel::JointMap& jointMap = skeletalModel->getJointMap();

	if(skeletalModel->getJointsInterpretation() == kinematic::SkeletalModel::Joints){
		for (auto it = jointMap.cbegin(); it != jointMap.cend(); ++it) {
			std::string mappedName = mapJointName(it->first);
			JointPtr joint = it->second;
			hAnimJointPtr hJoint = joints[mappedName];
			
			hJoint->setDirection(joint->direction);
			hJoint->setAxis(joint->axis);
			hJoint->setOrder(joint->order);
			hJoint->setDofs(joint->dofs);

			auto s = hJoint->getChildrenBones().size();

			for(auto it = hJoint->getChildrenBones().begin();
				it != hJoint->getChildrenBones().end(); ++it){

				auto bone = *it;
				bone->setLength(joint->length);
				bone->setMass(joint->bodymass / s);
				bone->setCofMass(joint->cofmass);
			}
		
			hJoint->setName(mappedName);
			hJoint->setActive(true);
		}
	}else{

		std::set<std::string> usedJoints;

		for (auto it = jointMap.cbegin(); it != jointMap.cend(); ++it) {
			auto mappedNamesRange = mapBoneNames(it->first);

			//weryfikuje zakres
			std::set<std::string> locallyUsedJoints;

			hAnimBonePtr bone = bones[mappedNamesRange.second];
			locallyUsedJoints.insert(bone->getName());

			while(bone != nullptr && bone->getName() != mappedNamesRange.first){				
				locallyUsedJoints.insert(bone->getName());
				bone = bone->getParentJoint().lock()->getParentBone().lock();
			}

			if(bone == nullptr){
				throw std::runtime_error("Invalid bones mapping range");
			}

			std::vector<std::string> overlap(std::min(usedJoints.size(), locallyUsedJoints.size()));
			auto oIT = std::set_intersection(usedJoints.begin(), usedJoints.end(), locallyUsedJoints.begin(), locallyUsedJoints.end(), overlap.end());

			if(oIT != overlap.begin()){
				throw std::runtime_error("Overlapping bones mapping range");
			}

			usedJoints.insert(locallyUsedJoints.begin(), locallyUsedJoints.end());

			bone->getParentJoint().lock()->setActive(true);

			bone = bones[mappedNamesRange.second];

			auto cJ = bone->getChildrenJoints();

			for(auto jIT = cJ.begin(); jIT != cJ.end(); ++jIT){
				(*jIT)->setActive(true);
			}
		}
	}
}

kinematic::hAnimSkeletonPtr hAnimSkeleton::create()
{
    return kinematic::hAnimSkeletonPtr(createRaw());
}

hAnimSkeleton* hAnimSkeleton::createRaw()
{
	hAnimJointPtr joint;
	hAnimBonePtr bone;
	hAnimJointPtr rootJoint;
	std::auto_ptr<hAnimSkeleton> skeleton(new hAnimSkeleton);

	skeleton->createJointAndBone("HumanoidRoot", "sacrum", joint, bone); rootJoint = joint;
	skeleton->createJointAndBone("sacroiliac", "pelvis", joint, bone);
	skeleton->createJointAndBone("l_hip", "l_thigh", joint, bone);
	skeleton->createJointAndBone("l_knee", "l_calf", joint, bone);
	skeleton->createJointAndBone("l_ankle", "l_hindfoot", joint, bone);
	skeleton->createJointAndBone("l_subtalar", "l_midproximal", joint, bone);
	skeleton->createJointAndBone("l_midtarsal", "l_middistal", joint, bone);
	skeleton->createJointAndBone("l_metatarsal", "l_forefoot", joint, bone);
	skeleton->backJointAndBone("sacroiliac", "pelvis", joint, bone);
	skeleton->createJointAndBone("r_hip", "r_thigh", joint, bone);
	skeleton->createJointAndBone("r_knee", "r_calf", joint, bone);
	skeleton->createJointAndBone("r_ankle", "r_hindfoot", joint, bone);
	skeleton->createJointAndBone("r_subtalar",  "r_midproximal", joint, bone);
	skeleton->createJointAndBone("r_midtarsal", "r_middistal", joint, bone);
	skeleton->createJointAndBone("r_metatarsal", "r_forefoot", joint, bone);
	skeleton->backJointAndBone("HumanoidRoot", "sacrum", joint, bone);
	skeleton->createJointAndBone("vl5", "l5", joint, bone);
	skeleton->createJointAndBone("vl4", "l4", joint, bone);
	skeleton->createJointAndBone("vl3", "l3", joint, bone);
	skeleton->createJointAndBone("vl2", "l2", joint, bone);
	skeleton->createJointAndBone("vl1", "l1", joint, bone);
	skeleton->createJointAndBone("vt12", "t12", joint, bone);
	skeleton->createJointAndBone("vt11", "t11", joint, bone);
	skeleton->createJointAndBone("vt10", "t10", joint, bone);
	skeleton->createJointAndBone("vt9", "t9", joint, bone);
	skeleton->createJointAndBone("vt8", "t8", joint, bone);
	skeleton->createJointAndBone("vt7", "t7", joint, bone);
	skeleton->createJointAndBone("vt6", "t6", joint, bone);
	skeleton->createJointAndBone("vt5", "t5", joint, bone);
	skeleton->createJointAndBone("vt4", "t4", joint, bone);
	skeleton->createJointAndBone("vt3", "t3", joint, bone);
	skeleton->createJointAndBone("vt2", "t2", joint, bone);
	skeleton->createJointAndBone("vt1", "t1", joint, bone);
	/*|*/skeleton->createJointAndBone("vc7", "c7", joint, bone);
	skeleton->createJointAndBone("vc6", "c6", joint, bone);
	skeleton->createJointAndBone("vc5", "c5", joint, bone);
	skeleton->createJointAndBone("vc4", "c4", joint, bone);
	skeleton->createJointAndBone("vc3", "c3", joint, bone);
	skeleton->createJointAndBone("vc2", "c2", joint, bone);
	skeleton->createJointAndBone("vc1", "c1", joint, bone);
	skeleton->createJointAndBone("skullbase", "skull", joint, bone);
	skeleton->createJointAndBone("l_eyelid_joint", "l_eyelid", joint, bone);
	skeleton->backJointAndBone("skullbase", "skull", joint, bone);
	skeleton->createJointAndBone("r_eyelid_joint", "r_eyelid", joint, bone);
	skeleton->backJointAndBone("skullbase", "skull", joint, bone);
	skeleton->createJointAndBone("l_eyeball_joint", "l_eyeball", joint, bone);
	skeleton->backJointAndBone("skullbase", "skull", joint, bone);
	skeleton->createJointAndBone("r_eyeball_joint", "r_eyeball", joint, bone);
	skeleton->backJointAndBone("skullbase", "skull", joint, bone);
	skeleton->createJointAndBone("l_eyebrow_joint", "l_eyebrow", joint, bone);
	skeleton->backJointAndBone("skullbase", "skull", joint, bone);
	skeleton->createJointAndBone("r_eyebrow_joint", "r_eyebrow", joint, bone);
	skeleton->backJointAndBone("skullbase", "skull", joint, bone);
	skeleton->createJointAndBone("temporomandibular", "jaw", joint, bone);
	skeleton->backJointAndBone("vt1", "t1", joint, bone);
	/*|*/skeleton->createJointAndBone("l_sternoclavicular", "l_clavicle", joint, bone);
	skeleton->createJointAndBone("l_acromioclavicular", "l_scapula", joint, bone);
	skeleton->createJointAndBone("l_shoulder", "l_upperarm", joint, bone);
	skeleton->createJointAndBone("l_elbow", "l_forearm", joint, bone);
	skeleton->createJointAndBone("l_wrist", "l_hand", joint, bone);
	skeleton->createJointAndBone("l_thumb1", "l_thumb_metacarpal", joint, bone);
	skeleton->createJointAndBone("l_thumb2", "l_thumb_proximal", joint, bone);
	skeleton->createJointAndBone("l_thumb3", "l_thumb_distal", joint, bone);
	skeleton->backJointAndBone("l_wrist", "l_hand", joint, bone);
	skeleton->createJointAndBone("l_index0", "l_index_metacarpal", joint, bone);
	skeleton->createJointAndBone("l_index1", "l_index_proximal", joint, bone);
	skeleton->createJointAndBone("l_index2", "l_index_middle", joint, bone);
	skeleton->createJointAndBone("l_index3", "l_index_distal", joint, bone);
	skeleton->backJointAndBone("l_wrist", "l_hand", joint, bone);
	skeleton->createJointAndBone("l_middle0", "l_middle_metacarpal", joint, bone);
	skeleton->createJointAndBone("l_middle1", "l_middle_proximal", joint, bone);
	skeleton->createJointAndBone("l_middle2", "l_middle_middle", joint, bone);
	skeleton->createJointAndBone("l_middle3", "l_middle_distal", joint, bone);
	skeleton->backJointAndBone("l_wrist", "l_hand", joint, bone);
	skeleton->createJointAndBone("l_ring0", "l_ring_metacarpal", joint, bone);
	skeleton->createJointAndBone("l_ring1", "l_ring_proximal", joint, bone);
	skeleton->createJointAndBone("l_ring2", "l_ring_middle", joint, bone);
	skeleton->createJointAndBone("l_ring3", "l_ring_distal", joint, bone);
	skeleton->backJointAndBone("l_wrist", "l_hand", joint, bone);
	skeleton->createJointAndBone("l_pinky0", "l_pinky_metacarpal", joint, bone);
	skeleton->createJointAndBone("l_pinky1", "l_pinky_proximal", joint, bone);
	skeleton->createJointAndBone("l_pinky2", "l_pinky_middle", joint, bone);
	skeleton->createJointAndBone("l_pinky3", "l_pinky_distal", joint, bone);
	skeleton->backJointAndBone("vt1", "t1", joint, bone);
	/*|*/skeleton->createJointAndBone("r_sternoclavicular", "r_clavicle", joint, bone);
	skeleton->createJointAndBone("r_acromioclavicular", "r_scapula", joint, bone);
	skeleton->createJointAndBone("r_shoulder", "r_upperarm", joint, bone);
	skeleton->createJointAndBone("r_elbow", "r_forearm", joint, bone);
	skeleton->createJointAndBone("r_wrist", "r_hand", joint, bone);
	skeleton->createJointAndBone("r_thumb1", "r_thumb_metacarpal", joint, bone);
	skeleton->createJointAndBone("r_thumb2", "r_thumb_proximal", joint, bone);
	skeleton->createJointAndBone("r_thumb3", "r_thumb_distal", joint, bone);
	skeleton->backJointAndBone("r_wrist", "r_hand", joint, bone);
	skeleton->createJointAndBone("r_index0", "r_index_metacarpal", joint, bone);
	skeleton->createJointAndBone("r_index1", "r_index_proximal", joint, bone);
	skeleton->createJointAndBone("r_index2", "r_index_middle", joint, bone);
	skeleton->createJointAndBone("r_index3", "r_index_distal", joint, bone);
	skeleton->backJointAndBone("r_wrist", "r_hand", joint, bone);
	skeleton->createJointAndBone("r_middle0", "r_middle_metacarpal", joint, bone);
	skeleton->createJointAndBone("r_middle1", "r_middle_proximal", joint, bone);
	skeleton->createJointAndBone("r_middle2", "r_middle_middle", joint, bone);
	skeleton->createJointAndBone("r_middle3", "r_middle_distal", joint, bone);
	skeleton->backJointAndBone("r_wrist", "r_hand", joint, bone);
	skeleton->createJointAndBone("r_ring0", "r_ring_metacarpal", joint, bone);
	skeleton->createJointAndBone("r_ring1", "r_ring_proximal", joint, bone);
	skeleton->createJointAndBone("r_ring2", "r_ring_middle", joint, bone);
	skeleton->createJointAndBone("r_ring3", "r_ring_distal", joint, bone);
	skeleton->backJointAndBone("r_wrist", "r_hand", joint, bone);
	skeleton->createJointAndBone("r_pinky0", "r_pinky_metacarpal", joint, bone);
	skeleton->createJointAndBone("r_pinky1", "r_pinky_proximal", joint, bone);
	skeleton->createJointAndBone("r_pinky2", "r_pinky_middle", joint, bone);
	skeleton->createJointAndBone("r_pinky3", "r_pinky_distal", joint, bone);

	
	skeleton->setRoot(rootJoint);
	return skeleton.release();
}

hAnimSkeleton* hAnimSkeleton::clone() const
{
    hAnimSkeleton* clone = hAnimSkeleton::createRaw();
    clone->mappingSchemes.clear();
    for (auto it = mappingSchemes.cbegin(); it != mappingSchemes.cend(); ++it) {
        clone->mappingSchemes.push_back(SkeletonMappingSchemePtr((*it)->clone()));
    }

    for (auto it = joints.begin(); it != joints.end(); ++it) {
        hAnimJoint::copyContent(*it->second, *clone->joints[it->first]);
    }

    clone->createActiveHierarchy();
    return clone;
}

kinematic::hAnimJointPtr hAnimSkeleton::cloneHierarchy( hAnimJointConstPtr root, std::map<std::string, hAnimJointPtr>& joints, std::map<std::string, hAnimBonePtr>& bones ) const
{
	hAnimJointPtr current(new hAnimJoint());
	hAnimJoint::copyContent(*root, *current);
	joints[current->getName()] = current;
	for (auto it = root->getChildrenBones().cbegin(); it != root->getChildrenBones().cend(); ++it) {
		hAnimBonePtr bone(new hAnimBone);
		bone->setName((*it)->getName());
		bones[bone->getName()] = bone;
		bone->setParentJoint(hAnimJointWeak(current));
		current->getChildrenBones().push_back(bone);
		for (auto it2 = (*it)->getChildrenJoints().cbegin(); it2 != (*it)->getChildrenJoints().cend(); ++it2) {
			cloneHierarchy(utils::const_pointer_cast<hAnimJoint>(*it2), bone, joints, bones);
		}
		
	}
	return current;
}

kinematic::hAnimJointPtr hAnimSkeleton::cloneHierarchy( hAnimJointConstPtr currentSourceJoint, hAnimBonePtr destinationParentBone, std::map<std::string, hAnimJointPtr>& joints, std::map<std::string, hAnimBonePtr>& bones ) const
{
	hAnimJointPtr current(new hAnimJoint());
	hAnimJoint::copyContent(*currentSourceJoint, *current);
	joints[current->getName()] = current;
	current->setParentBone(hAnimBoneWeak(destinationParentBone));
	for (auto it = currentSourceJoint->getChildrenBones().cbegin(); it != currentSourceJoint->getChildrenBones().cend(); ++it) {
		hAnimBonePtr bone(new hAnimBone);
		bone->setName((*it)->getName());
		bones[bone->getName()] = bone;
		bone->setParentJoint(hAnimJointWeak(current));
		current->getChildrenBones().push_back(bone);
		for (auto it2 = (*it)->getChildrenJoints().cbegin(); it2 != (*it)->getChildrenJoints().cend(); ++it2) {
			cloneHierarchy(utils::const_pointer_cast<hAnimJoint>(*it2), bone, joints, bones);
		}
	}
	return current;
}


} // kinematic
