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
	const osg::Vec3& parentPos,	const acclaim::Bone& currentBone,
	std::set<std::string> & names)
{
	if (names.find(currentBone.name) != names.end()){
		throw std::runtime_error("Joint with given name already exists: " + currentBone.name + ". Improper skeleton hierarchy - names must be unique.");
	}

	names.insert(currentBone.name);

	JointData jointData;
	jointData.name = currentBone.name;
	jointData.position = parentPos;
	jointData.orientation = osg::Quat(0, 0, 0, 1);

	auto joint = Joint::addChild(parentJoint, jointData);
	auto range = skeleton.hierarchy.left.equal_range(currentBone.id);	
	const auto nextPos = currentBone.direction * currentBone.length;
	//taka jest konwencja ASF - podajemy przesuniecie rodzica, dla wyjaœnienia spojrzeæ na tworzenie liœci hierarchi
	for (auto it = range.first; it != range.second; ++it){
		createJoint(joint, skeleton, nextPos, skeleton.bones.at(it->second), names);
	}
	//przerywamy rekurencje tworzenia hierarchi
	static const int TERMINATOR = -0xdead;
	if (range.first == range.second && currentBone.id != TERMINATOR) {
		acclaim::Bone b;
		b.id = TERMINATOR;
		b.name = currentBone.name + "_leaf";
		createJoint(joint, skeleton, nextPos, b, names);
	}
}

bool Skeleton::convert(const acclaim::Skeleton & srcSkeleton, Skeleton & destSkeleton)
{
	if (srcSkeleton.bones.empty()){
		return false;
	}

	std::set<std::string> names;

	//root
	JointData jointData;
	jointData.name = "root";
	jointData.position = srcSkeleton.position;
	jointData.orientation = kinematicUtils::convert(srcSkeleton.orientation, srcSkeleton.axisOrder);
	names.insert(jointData.name);

	JointPtr joint = Joint::create(jointData);
	auto range = srcSkeleton.hierarchy.left.equal_range(srcSkeleton.root);
	for (auto it = range.first; it != range.second; ++it) {
		try{
			createJoint(joint, srcSkeleton, jointData.position, srcSkeleton.bones.at(it->second), names);
		}
		catch (...){
			return false;
		}
	}

	if (joint->children.empty() == true){
		return false;
	}
				
	destSkeleton.name = srcSkeleton.name;
	destSkeleton.root = joint;
		
	return true;
}

kinematicUtils::AxisOrder::Type findJointAxis(const std::string & jointName,
	const Skeleton::JointsAxis & jointsAxis, kinematicUtils::AxisOrder::Type defaultJointAxis)
{
	auto it = jointsAxis.find(jointName);
	if (it != jointsAxis.end()){
		return it->second;
	}

	return defaultJointAxis;
}

bool Skeleton::convert(const Skeleton & srcSkeleton, acclaim::Skeleton & destSkeleton,
	const kinematicUtils::AxisOrder::Type axisOrder,
	const kinematicUtils::AngleUnitType angleUnit,
	const JointsAxis & jointsAxis)
{
	if (srcSkeleton.root == nullptr){
		return false;
	}

	try{
		acclaim::Skeleton localSkeleton;
		localSkeleton.name = srcSkeleton.name;
		localSkeleton.axisOrder = axisOrder;
		localSkeleton.units.setAngleType(angleUnit);
		localSkeleton.position = srcSkeleton.root->value.position;
		localSkeleton.orientation = kinematicUtils::convert(srcSkeleton.root->value.orientation, axisOrder);

		if (angleUnit == kinematicUtils::Deg){
			localSkeleton.orientation = kinematicUtils::toDegrees(localSkeleton.orientation);
		}

		acclaim::Skeleton::ID id = 0;

		acclaim::Bone bone;
		bone.id = id;
		bone.name = "root";
		bone.axisOrder = localSkeleton.axisOrder;
		bone.direction = srcSkeleton.root->value.orientation * osg::Vec3(1, 0, 0);
		bone.length = 0.0;

		localSkeleton.bones.insert(acclaim::Skeleton::Bones::value_type(bone.id, bone));
		localSkeleton.root = bone.id;

		for (const auto c : srcSkeleton.root->children){

			utils::TreeNode::visitPreOrder(c, [&id, &localSkeleton,
				&jointsAxis, axisOrder, angleUnit](kinematic::JointPtr joint)
			{
				if (joint == nullptr){
					throw std::runtime_error("Uninitialized joint");
				}

				if (joint->isLeaf() == true){
					return;
				}

				acclaim::Bone bone;
				bone.name = joint->value.name;
				bone.axisOrder = findJointAxis(bone.name, jointsAxis, axisOrder);

				if (joint->children.size() == 1){
					bone.length = joint->children.front()->value.position.length();
					bone.direction = joint->children.front()->value.position;
					bone.direction.normalize();
				}
				else{
					bone.length = 0.0;
					bone.direction = joint->parent.lock()->value.orientation * osg::Vec3(1, 0, 0);					
				}				
				
				bone.axis = osg::Vec3(0, 0, 0);

				/*bone.axis = kinematicUtils::convert(joint->value.orientation, bone.axisOrder);
				if (angleUnit == kinematicUtils::Deg){
					bone.axis = kinematicUtils::toDegrees(bone.axis);
				}*/
				bone.id = id + 1;

				localSkeleton.bones.insert(acclaim::Skeleton::Bones::value_type(bone.id, bone));

				auto parentID = localSkeleton.root;

				if (joint->parent.lock()->isRoot() == false){
					auto parentName = joint->parent.lock()->value.name;
					auto it = std::find_if(localSkeleton.bones.begin(), localSkeleton.bones.end(),
						[&parentName](const acclaim::Skeleton::Bones::value_type & bd)
					{
						return bd.second.name == parentName;
					});

					if (it == localSkeleton.bones.end()){
						throw std::runtime_error("Could not find parent bone");
					}

					parentID = it->first;
				}				

				localSkeleton.hierarchy.left.insert(acclaim::Skeleton::Hierarchy::left_value_type(parentID, bone.id));
				id = bone.id;
			});
		}

		destSkeleton = localSkeleton;
	}
	catch (...){
		return false;
	}

	return true;
}

void createJoint(JointPtr parentJoint, const biovision::JointPtr & srcJoint,
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
		createJoint(joint, j, names);
	}
}

bool Skeleton::convert(const biovision::Skeleton & srcSkeleton, Skeleton & destSkeleton)
{
	if (srcSkeleton.root == nullptr){
		return false;
	}

	std::set<std::string> names;

	JointData jointData;

	jointData.name = "root";
	jointData.position = osg::Vec3(0, 0, 0);
	jointData.orientation = osg::Quat(0, 0, 0, 1);

	names.insert(jointData.name);

	auto root = Joint::create(jointData);

	createJoint(root, srcSkeleton.root, names);

	destSkeleton.name = "biovision_model";
	destSkeleton.root = root;

	return true;	
}

bool Skeleton::convert(const hAnim::Humanoid & srcSkeleton, Skeleton & destSkeleton)
{
	if (srcSkeleton.joints.empty() == true){
		return false;
	}

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
