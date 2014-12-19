#include <kinematiclib/Skeleton.h>
#include <acclaimformatslib/DegreeOfFreedom.h>
#include <kinematicUtils/RotationConverter.h>
#include <set>
#include <deque>
#include <stdexcept>

using namespace kinematic;

std::string resolveMapping(const SkeletonMappingScheme::MappingDict & mapping, const std::string & name)
{
	auto it = mapping.find(name);
	if (it != mapping.end()){
		return it->second;
	}

	return name;
}

void createJoint(JointPtr parentJoint, const acclaim::Skeleton & skeleton,
	const int currentBoneID, const SkeletonMappingScheme::MappingDict & mapping,
	const hAnim::Humanoid::Hierarchy & hAnimHierarchy,
	std::set<std::string> & names)
{
	auto joint = utils::make_shared<Joint>();
	const auto & bone = skeleton.bones.find(currentBoneID)->second;

	joint->name = resolveMapping(mapping, bone.name);

	if (names.find(joint->name) != names.end()){
		throw std::runtime_error("Joint with given name already exists: " + joint->name + ". Improper skeleton hierarchy - names must be unique.");
	}

	names.insert(joint->name);

	joint->position = bone.direction * bone.length;
	joint->orientation = kinematicUtils::convert(bone.axis, bone.axisOrder);

	parentJoint->children.push_back(joint);

	auto lIT = skeleton.hierarchy.left.lower_bound(currentBoneID);
	auto uIT = skeleton.hierarchy.left.upper_bound(currentBoneID);

	for (; lIT != uIT; ++lIT){
		createJoint(joint, skeleton, lIT->get_right(), mapping, hAnimHierarchy, names);
	}
}

bool Skeleton::convert(const acclaim::Skeleton & srcSkeleton, Skeleton & destSkeleton,
	const SkeletonMappingScheme::MappingDict & mapping)
{
	
	if (srcSkeleton.bones.empty() == false){
		return false;
	}

	int currentID = srcSkeleton.root;
	if (currentID != -1){

		const auto & humanoidHierarchy = hAnim::Humanoid::defaultHumanHierarchy();

		std::set<std::string> names;

		//root
		JointPtr joint(new Joint);

		joint->name = "HumanoidRoot";
		joint->position = srcSkeleton.position;
		joint->orientation = kinematicUtils::convert(srcSkeleton.orientation, srcSkeleton.axisOrder);

		names.insert(joint->name);
		createJoint(joint, srcSkeleton, currentID, mapping, humanoidHierarchy, names);		

		destSkeleton.name = srcSkeleton.name;
		destSkeleton.root = joint;
		
		return true;
	}	

	return false;
}

void createJoint(JointPtr parentJoint, const biovision::JointPtr & srcJoint,
	const SkeletonMappingScheme::MappingDict & mapping,
	const hAnim::Humanoid::Hierarchy & hAnimHierarchy,
	std::set<std::string> & names)
{
	auto joint = utils::make_shared<Joint>();

	joint->name = resolveMapping(mapping, srcJoint->name);

	if (names.find(joint->name) != names.end()){
		throw std::runtime_error("Joint with given name already exists: " + joint->name + ". Improper skeleton hierarchy - names must be unique.");
	}

	names.insert(joint->name);

	joint->position = srcJoint->offset;
	joint->orientation = osg::Quat(0, 0, 0, 1);

	parentJoint->children.push_back(joint);

	for (auto j : srcJoint->joints)
	{
		createJoint(joint, j, mapping, hAnimHierarchy, names);
	}
}

bool Skeleton::convert(const biovision::Skeleton & srcSkeleton, Skeleton & destSkeleton,
	const SkeletonMappingScheme::MappingDict & mapping)
{
	if (srcSkeleton.root == nullptr){
		return false;
	}

	const auto humanoidHierarchy = hAnim::Humanoid::defaultHumanHierarchy();
	std::set<std::string> names;

	JointPtr root(new Joint);

	root->name = "HumanoidRoot";
	root->position = osg::Vec3(0, 0, 0);
	root->orientation = osg::Quat(0, 0, 0, 1);

	names.insert(root->name);

	createJoint(root, srcSkeleton.root, mapping, humanoidHierarchy, names);

	destSkeleton.name = "biovision_model";
	destSkeleton.root = root;

	return true;	
}

bool Skeleton::convert(const hAnim::Humanoid & srcSkeleton, Skeleton & destSkeleton,
	const SkeletonMappingScheme::MappingDict & mapping)
{
	if (srcSkeleton.joints.empty() == true){
		return false;
	}

	const auto humanoidHierarchy = hAnim::Humanoid::defaultHumanHierarchy();
	std::set<std::string> names;

	/*
	//TODO
	*/

	return true;
}