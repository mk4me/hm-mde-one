#include <kinematiclib/Skeleton.h>
#include <acclaimformatslib/DegreeOfFreedom.h>
#include <kinematicUtils/RotationConverter.h>
#include <set>
#include <deque>
#include <stdexcept>

using namespace kinematic;


Skeleton::Skeleton()
{

}

Skeleton::Skeleton(const Skeleton & other)
	: name(other.name), root(Joint::clone(other.root))
{

}

Skeleton::Skeleton(Skeleton && other)
	: name(std::move(other.name)), root(std::move(other.root))
{

}

Skeleton::~Skeleton()
{

}

void createJoint(JointPtr rootJoint, TopologyNodeConstPtr topologyJoint)
{
	for (const auto & tn : topologyJoint->children)
	{
		JointData jd;
		jd.name = tn->value.name;
		jd.position = osg::Vec3(0, 0, 0);
		jd.orientation = osg::Quat(0, 0, 0, 1);
		auto rj = Joint::addChild(rootJoint, jd);
		createJoint(rj, tn);
	}
}

Skeleton Skeleton::create(TopologyNodeConstPtr topology)
{
	Skeleton ret;

	if (topology != nullptr){
		JointData jd;
		jd.name = topology->value.name;
		jd.position = osg::Vec3(0, 0, 0);
		jd.orientation = osg::Quat(0, 0, 0, 1);
		ret.root = Joint::create(jd);
		createJoint(ret.root, topology);
	}

	return ret;
}

void createJoint(JointPtr parentJoint, const acclaim::Skeleton & skeleton,
	const acclaim::Bone& currentBone, const hAnim::Humanoid::Hierarchy & hAnimHierarchy,
	std::set<std::string> & names)
{
	JointData jointData;

	jointData.name = currentBone.name;

	if (names.find(jointData.name) != names.end()){
		throw std::runtime_error("Joint with given name already exists: " + jointData.name + ". Improper skeleton hierarchy - names must be unique.");
	}

	jointData.position = parentJoint->value.position + currentBone.direction * currentBone.length;	
	names.insert(jointData.name);

	auto joint = Joint::addChild(parentJoint, jointData);
	auto range = skeleton.hierarchy.left.equal_range(currentBone.id);	

	for (auto it = range.first; it != range.second; ++it){
		createJoint(joint, skeleton, skeleton.bones.at(it->second), hAnimHierarchy, names);
	}
	const int TERMINATOR = -0xdead;
	if (range.first == range.second && currentBone.id != TERMINATOR) {
		acclaim::Bone b; b.id = TERMINATOR;
		b.name = currentBone.name + "_leaf";
		createJoint(joint, skeleton, b, hAnimHierarchy, names);
	}
}

bool Skeleton::convert(const acclaim::Skeleton & srcSkeleton, Skeleton & destSkeleton)
{
	if (srcSkeleton.bones.empty()){
		return false;
	}

	int currentID = srcSkeleton.root;
	if (currentID == -1){

		const auto & humanoidHierarchy = hAnim::Humanoid::defaultHumanHierarchy();

		std::set<std::string> names;

		//root
		JointData jointData;
		jointData.name = "root";
		jointData.position = srcSkeleton.position;
		jointData.orientation = kinematicUtils::convert(srcSkeleton.orientation, srcSkeleton.axisOrder);
		names.insert(jointData.name);

		JointPtr joint = Joint::create(jointData);
		auto range = srcSkeleton.hierarchy.left.equal_range(currentID);
		for (auto it = range.first; it != range.second; ++it) {
			createJoint(joint, srcSkeleton, srcSkeleton.bones.at(it->second), humanoidHierarchy, names);
		}
				
		destSkeleton.name = srcSkeleton.name;
		destSkeleton.root = joint;
		
		return true;
	}	

	return false;
}

void createJoint(JointPtr parentJoint, const biovision::JointPtr & srcJoint,
	const hAnim::Humanoid::Hierarchy & hAnimHierarchy,
	std::set<std::string> & names)
{
	JointData jointData;
	
	jointData.name = srcJoint->name;

	if (names.find(jointData.name) != names.end()){
		throw std::runtime_error("Joint with given name already exists: " + jointData.name + ". Improper skeleton hierarchy - names must be unique.");
	}

	jointData.position = srcJoint->offset;
	jointData.orientation = osg::Quat(0, 0, 0, 1);

	auto joint = Joint::addChild(parentJoint, jointData);
	names.insert(jointData.name);	

	for (auto j : srcJoint->joints)
	{
		createJoint(joint, j, hAnimHierarchy, names);
	}
}

bool Skeleton::convert(const biovision::Skeleton & srcSkeleton, Skeleton & destSkeleton)
{
	if (srcSkeleton.root == nullptr){
		return false;
	}

	const auto humanoidHierarchy = hAnim::Humanoid::defaultHumanHierarchy();
	std::set<std::string> names;

	JointData jointData;

	jointData.name = "HumanoidRoot";
	jointData.position = osg::Vec3(0, 0, 0);
	jointData.orientation = osg::Quat(0, 0, 0, 1);

	names.insert(jointData.name);

	auto root = Joint::create(jointData);

	createJoint(root, srcSkeleton.root, humanoidHierarchy, names);

	destSkeleton.name = "biovision_model";
	destSkeleton.root = root;

	return true;	
}

bool Skeleton::convert(const hAnim::Humanoid & srcSkeleton, Skeleton & destSkeleton)
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

void getJointsRecursive(kinematic::JointPtr j, std::map<std::string, kinematic::JointPtr>& mp) {
	mp[j->value.name] = j;
	for (auto& c : j->children) {
		getJointsRecursive(c, mp);
	}
}
std::map<std::string, kinematic::JointPtr> kinematic::Skeleton::getJoints(const Skeleton& s)
{
	std::map<std::string, kinematic::JointPtr> ret;
	getJointsRecursive(s.root, ret);
	return ret;
}
