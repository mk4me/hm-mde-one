#include <kinematiclib/SkeletonState.h>
#include <osg/PositionAttitudeTransform>
#include <boost/lexical_cast.hpp>
#include <kinematicUtils/RotationConverter.h>
#include <utils/TreeNode.h>
#include <utils/Debug.h>

using namespace kinematic;

osg::Vec3d resolveRadians(const bool angleInRadians, const osg::Vec3d& toResolve)
{
	return angleInRadians == true ? toResolve : kinematicUtils::toRadians(toResolve);
}

LinearizedSkeleton::Mapping createActiveMapping(const Skeleton & skeleton,
	const acclaim::Skeleton::Bones & bones)
{
	LinearizedSkeleton::Mapping ret = LinearizedSkeleton::createNonLeafMapping(skeleton);

	for (const auto & bd : bones)
	{
		if (bd.second.isActive() == false){
			ret.right.erase(bd.second.name);
		}
	}

	return ret;
}

SkeletonState::AcclaimActiveSkeletonMapping SkeletonState::createAcclaimActiveMapping(
	const Skeleton & skeleton, const acclaim::Skeleton::Bones & bones)
{
	const auto skeletonMapping = LinearizedSkeleton::createNonLeafMapping(skeleton);	

	AcclaimActiveSkeletonMapping ret;

	for (const auto & bd : bones)
	{
		if (bd.second.isActive() == true){
			auto it = skeletonMapping.right.find(bd.second.name);
			if (it != skeletonMapping.right.end())
			{
				ret.insert({ it->get_left(), bd.first });
			}
		}
	}

	//root - bo w mapowaniu mogą się róznić nazwy rootów
	ret.insert({ skeletonMapping.left.begin()->get_left(), bones.begin()->first});

	return ret;
}

SkeletonState::NonRigidJointState convert(const acclaim::Skeleton & skeleton,
	const acclaim::MotionData::BoneData & boneData, const bool angleInRadians,
	const acclaim::Bone::HelperMotionData & helperMotionData)
{
	auto it = std::find_if(skeleton.bones.begin(), skeleton.bones.end(), [&boneData](const acclaim::Skeleton::Bones::value_type & val)
	{
		return val.second.name == boneData.name;
	});

	if (it == skeleton.bones.end()){
		throw std::runtime_error("Acclaim motion data mismatch skeleton structure - missing " + boneData.name + " node");
	}

	const auto & bone = it->second;	

	if (bone.dofs.size() != boneData.channelValues.size()){
		throw std::runtime_error("Acclaim motion data mismatch skeleton structure degrees of freedom for node " + boneData.name + ". In model declared " + boost::lexical_cast<std::string>(bone.dofs.size()) + " dof`s but data contains " + boost::lexical_cast<std::string>(boneData.channelValues.size()) + " dof`s.");
	}

	SkeletonState::NonRigidJointState ret;
	ret.position = osg::Vec3d(0, 0, 0);
	ret.orientation = osg::Quat(0, 0, 0, 1);

	osg::Vec3d rot(0, 0, 0);
	unsigned int i = 0;
	unsigned int roti = 0;
	unsigned int transi = 0;

	for (const auto & dof : bone.dofs)
	{
		switch (dof.channel)
		{
		case kinematicUtils::ChannelType::TX:
			ret.position.x() = boneData.channelValues[i];
			++transi;
			break;

		case kinematicUtils::ChannelType::TY:
			ret.position.y() = boneData.channelValues[i];
			++transi;
			break;

		case kinematicUtils::ChannelType::TZ:
			ret.position.z() = boneData.channelValues[i];
			++transi;
			break;

		case kinematicUtils::ChannelType::RX:
			rot[roti++] = boneData.channelValues[i];
			break;

		case kinematicUtils::ChannelType::RY:
			rot[roti++] = boneData.channelValues[i];
			break;

		case kinematicUtils::ChannelType::RZ:
			rot[roti++] = boneData.channelValues[i];
			break;

		case acclaim::DegreeOfFreedom::L:
			//TODO - przesuń dziecko
			break;

		default:
			throw std::runtime_error("Unsupported acclaim degree of freedom: " + boost::lexical_cast<std::string>(dof.channel));
		}

		++i;
	}

	ret.orientation = helperMotionData.cInv * kinematicUtils::convert(resolveRadians(angleInRadians, rot), bone.rotationOrder()) * helperMotionData.c;

	return ret;
}

void orderChanges(const acclaim::Skeleton & skeleton, const int currentBoneID,
	const std::map<std::string, SkeletonState::NonRigidJointState> & changes,
	SkeletonState::NonRigidCompleteState & state)
{
	const auto & b = skeleton.bones.find(currentBoneID)->second;	
	auto changeIT = changes.find(b.name);
	if (changeIT != changes.end()) {
		state.push_back(changeIT->second);
	}
	
	auto lIT = skeleton.hierarchy.left.lower_bound(currentBoneID);
	auto uIT = skeleton.hierarchy.left.upper_bound(currentBoneID);

	for (; lIT != uIT; ++lIT)
	{
		orderChanges(skeleton, lIT->get_right(), changes, state);
	}
}

SkeletonState::RigidPartialStateLocal SkeletonState::convert(const acclaim::Skeleton & skeleton,
	const acclaim::MotionData::BonesData & motionData,
	const LinearizedSkeleton::Mapping & mapping,
	const acclaim::Skeleton::HelperMotionData & helperMotionData)
{
	RigidPartialStateLocal ret;

	const bool angleInRadians = skeleton.units.isAngleInRadians();

	for (const auto & bd : motionData)
	{
		auto it = mapping.right.find(bd.name);
		const auto nodeID = it->get_left();
		auto hIT = helperMotionData.find(nodeID);
		const auto jointState = ::convert(skeleton, bd, angleInRadians, hIT->second);
		ret.orientations.insert({ nodeID, jointState.orientation });

		if (nodeID == 0){
			ret.position = jointState.position;
		}
	}

	return ret;
}

acclaim::MotionData::BonesData SkeletonState::convert(const acclaim::Skeleton & skeleton,
	const SkeletonState::RigidCompleteState & skeletonState,
	const AcclaimActiveSkeletonMapping & activeMapping,
	const acclaim::Skeleton::HelperMotionData & helperMotionData)
{
	if (skeletonState.orientations.empty() == true && skeleton.bones.empty() == false){
		throw std::runtime_error("Empty skeleton state");
	}

	acclaim::MotionData::BonesData ret;
	const bool angleInRadians = skeleton.units.isAngleInRadians();
	
	acclaim::MotionData::BoneData bd;
	bd.name = "root";
	{
		auto helperData = helperMotionData.begin()->second;

		//osg::Quat c = kinematicUtils::convert(resolveRadians(angleInRadians, skeleton.orientation), skeleton.axisOrder);
		//osg::Quat cinv = c.inverse();

		//auto orient = kinematicUtils::convert(c * skeletonState.root()->value().localOrientation() * cinv, skeleton.rotationOrder());
		auto rotationOrder = acclaim::Skeleton::rotationOrder(skeleton);
		auto orient = kinematicUtils::convert(helperData.c * skeletonState.orientations.front() * helperData.cInv, rotationOrder);
		if (angleInRadians == false){
			orient = kinematicUtils::toDegrees(orient);
		}

		orient = kinematicUtils::orderedAngles(orient, rotationOrder);

		auto position = skeletonState.position;

		for (auto dO : skeleton.dataOrder)
		{			
			switch (dO)
			{
			case kinematicUtils::ChannelType::TX:
				bd.channelValues.push_back(position.x());
				break;
			case kinematicUtils::ChannelType::TY:
				bd.channelValues.push_back(position.y());
				break;
			case kinematicUtils::ChannelType::TZ:
				bd.channelValues.push_back(position.z());
				break;
			case kinematicUtils::ChannelType::RX:
				bd.channelValues.push_back(orient.x());
				break;
			case kinematicUtils::ChannelType::RY:
				bd.channelValues.push_back(orient.y());
				break;
			case kinematicUtils::ChannelType::RZ:
				bd.channelValues.push_back(orient.z());
				break;

			default:
				throw std::runtime_error("Unrecognized data channel for AMC file");
				break;
			}
		}
	}

	ret.push_back(bd);

	for (unsigned int i = 1; i < skeletonState.orientations.size(); ++i)
	{
		auto acclaimID = activeMapping.left.find(i)->second;
		acclaim::MotionData::BoneData bd;
		const auto & bData = skeleton.bones.find(acclaimID)->second;
		bd.name = bData.name;
		const auto & hData = helperMotionData.find(acclaimID)->second;
		//osg::Quat c = kinematicUtils::convert(resolveRadians(angleInRadians, sIT->second.axis), sIT->second.axisOrder);
		//osg::Quat cinv = c.inverse();

		//auto orient = kinematicUtils::convert(c * joint->value().localOrientation() * cinv, sIT->second.rotationOrder());
		auto rotationOrder = bData.rotationOrder();
		auto orient = kinematicUtils::convert(hData.c * skeletonState.orientations[i] * hData.cInv, rotationOrder);

		if (angleInRadians == false){
			orient = kinematicUtils::toDegrees(orient);
		}

		orient = kinematicUtils::orderedAngles(orient, rotationOrder);

		for (auto dO : bData.dofs)
		{
			switch (dO.channel)
			{
			case kinematicUtils::ChannelType::RX:
				bd.channelValues.push_back(orient.x());
				break;
			case kinematicUtils::ChannelType::RY:
				bd.channelValues.push_back(orient.y());
				break;
			case kinematicUtils::ChannelType::RZ:
				bd.channelValues.push_back(orient.z());
				break;
			case acclaim::DegreeOfFreedom::L:
				//TODO - co zrobić z tym przesunięciem?
				break;

			default:
				throw std::runtime_error("Unrecognized data channel for AMC file");
				break;
			}
		}

		ret.push_back(bd);
	}

	return ret;
}

SkeletonState::NonRigidJointState convert(biovision::JointConstPtr joint, const biovision::MotionData::ChannelData & frameData)
{
	if (joint->channels.size() != frameData.size()){
		throw std::runtime_error("Biovision motion data mismatch skeleton structure - channels size mismatch. Joint: " + joint->name + ". Motion data channels size: " + boost::lexical_cast<std::string>(frameData.size()) + ". Joint channels size: " + boost::lexical_cast<std::string>(joint->channels.size()));
	}

	SkeletonState::NonRigidJointState ret;
	ret.position = osg::Vec3(0, 0, 0);
	ret.orientation = osg::Quat(0, 0, 0, 1);

	osg::Vec3 rot(0, 0, 0);
	unsigned int roti = 0;
	unsigned int i = 0;	
	for (const auto & channel : joint->channels)
	{
		switch (channel)
		{
		case kinematicUtils::ChannelType::TX:
			ret.position.x() = frameData[i];
			break;

		case kinematicUtils::ChannelType::TY:
			ret.position.y() = frameData[i];
			break;

		case kinematicUtils::ChannelType::TZ:
			ret.position.z() = frameData[i];
			break;

		case kinematicUtils::ChannelType::RX:
			rot[roti++] = osg::DegreesToRadians(frameData[i]);
			break;

		case kinematicUtils::ChannelType::RY:
			rot[roti++] = osg::DegreesToRadians(frameData[i]);
			break;

		case kinematicUtils::ChannelType::RZ:
			rot[roti++] = osg::DegreesToRadians(frameData[i]);
			break;

		default:
			throw std::runtime_error("Unsupported biovision channel: " + boost::lexical_cast<std::string>(channel));
		}

		++i;
	}

	ret.orientation = kinematicUtils::convert(rot, joint->rotationOrder);

	return ret;
}

void orderChanges(biovision::JointConstPtr joint,
	const std::map<std::string, SkeletonState::NonRigidJointState> & changes,
	SkeletonState::RigidCompleteState & change)
{
	change.orientations.push_back(changes.find(joint->name)->second.orientation);

	for (auto j : joint->joints)
	{
		orderChanges(j, changes, change);
	}
}

SkeletonState::RigidCompleteState SkeletonState::convert(const biovision::Skeleton & skeleton,
	const biovision::MotionData::FrameJointData & motionData)
{
	RigidCompleteState ret;

	std::map<std::string, NonRigidJointState> changes;	

	unsigned int i = 0;	
	for (const auto & joint : skeleton.sourceOrderJoints)
	{
		changes.insert({ joint->name, ::convert(joint, motionData[i++]) });
	}

	orderChanges(skeleton.root, changes, ret);

	//root - pozucja, zawsze pierwszy w hierarchi
	ret.position = changes.begin()->second.position;

	return ret;
}

SkeletonState::RigidCompleteStateGlobal SkeletonState::globalRigidState(const Skeleton & skeleton)
{
	RigidCompleteStateGlobal ret;

	if (skeleton.root() != nullptr){
		ret.position = skeleton.root()->value().globalPosition();
		LinearizedSkeleton::Visitor::visitNonLeaf(skeleton, [&ret](Skeleton::JointConstPtr joint)
		{
			ret.orientations.push_back(joint->value().globalOrientation());
		});
	}

	return ret;
}

SkeletonState::RigidCompleteStateLocal SkeletonState::localRigidState(const Skeleton & skeleton)
{
	RigidCompleteStateLocal ret;

	if (skeleton.root() != nullptr){
		ret.position = skeleton.root()->value().localPosition();
		LinearizedSkeleton::Visitor::visitNonLeaf(skeleton, [&ret](Skeleton::JointConstPtr joint)
		{
			ret.orientations.push_back(joint->value().localOrientation());
		});
	}

	return ret;
}

SkeletonState::NonRigidCompleteStateGlobal SkeletonState::globalNonRigidState(const Skeleton & skeleton)
{
	NonRigidCompleteStateGlobal ret;

	if (skeleton.root() != nullptr){		
		LinearizedSkeleton::Visitor::visitNonLeaf(skeleton, [&ret](Skeleton::JointConstPtr joint)
		{
			ret.push_back({ joint->value().globalPosition(), joint->value().globalOrientation() });
		});
	}

	return ret;
}

SkeletonState::NonRigidCompleteStateLocal SkeletonState::localNonRigidState(const Skeleton & skeleton)
{
	NonRigidCompleteStateLocal ret;

	if (skeleton.root() != nullptr){
		LinearizedSkeleton::Visitor::visitNonLeaf(skeleton, [&ret](Skeleton::JointConstPtr joint)
		{
			ret.push_back({ joint->value().localPosition(), joint->value().localOrientation() });
		});
	}

	return ret;
}

void SkeletonState::applyLocalState(Skeleton & skeleton, const RigidCompleteState & state)
{
	kinematic::LinearizedSkeleton::Visitor::localIndexedVisitNonLeaf(skeleton,
		[&state](kinematic::Skeleton::JointPtr joint, kinematic::LinearizedSkeleton::NodeIDX idx)
	{
		joint->value().setLocal(state.orientations[idx]);
	});
	
	skeleton.root()->value().setGlobal(state.position);
}

void SkeletonState::applyLocalState(Skeleton & skeleton, const RigidPartialState & state)
{
	auto it = state.orientations.begin();
	if (it != state.orientations.end()){
		kinematic::LinearizedSkeleton::Visitor::localIndexedVisitNonLeaf(skeleton,
			[&state, &it](kinematic::Skeleton::JointPtr joint, kinematic::LinearizedSkeleton::NodeIDX idx)
		{
			if (it != state.orientations.end()){
				if (it->first == idx){
					joint->value().setLocal(it->second);
					++it;
				}
			}
		});
	}

	skeleton.root()->value().setGlobal(state.position);
}

void SkeletonState::applyLocalState(Skeleton & skeleton, const NonRigidCompleteState & state)
{
	kinematic::LinearizedSkeleton::Visitor::globalIndexedVisit(skeleton,
		[&state](kinematic::Skeleton::JointPtr joint, kinematic::LinearizedSkeleton::NodeIDX idx)
	{
		joint->value().setLocal(state[idx].position, state[idx].orientation);
	});	
}

void SkeletonState::applyLocalState(Skeleton & skeleton, const NonRigidPartialState & state)
{
	if (state.empty() == true){
		return;
	}

	auto it = state.begin();
	kinematic::LinearizedSkeleton::Visitor::globalIndexedVisit(skeleton,
		[&state, &it](kinematic::Skeleton::JointPtr joint, kinematic::LinearizedSkeleton::NodeIDX idx)
	{
		if (it != state.end()){
			if (it->first == idx){
				joint->value().setLocal(it->second.position, it->second.orientation);
				++it;
			}
		}
	});
}

void SkeletonState::applyGlobalState(Skeleton & skeleton, const RigidCompleteState & state)
{
	kinematic::LinearizedSkeleton::Visitor::localIndexedVisitNonLeaf(skeleton,
		[&state](kinematic::Skeleton::JointPtr joint, kinematic::LinearizedSkeleton::NodeIDX idx)
	{
		joint->value().setGlobal(state.orientations[idx]);
	});

	skeleton.root()->value().setGlobal(state.position);
}

void SkeletonState::applyGlobalState(Skeleton & skeleton, const RigidPartialState & state)
{
	auto it = state.orientations.begin();
	if (it != state.orientations.end()){
		kinematic::LinearizedSkeleton::Visitor::localIndexedVisitNonLeaf(skeleton,
			[&state, &it](kinematic::Skeleton::JointPtr joint, kinematic::LinearizedSkeleton::NodeIDX idx)
		{
			if (it != state.orientations.end()){
				if (it->first == idx){
					joint->value().setGlobal(it->second);
					++it;
				}
			}
		});
	}

	skeleton.root()->value().setGlobal(state.position);
}

void SkeletonState::applyGlobalState(Skeleton & skeleton, const NonRigidCompleteState & state)
{
	kinematic::LinearizedSkeleton::Visitor::globalIndexedVisit(skeleton,
		[&state](kinematic::Skeleton::JointPtr joint, kinematic::LinearizedSkeleton::NodeIDX idx)
	{
		joint->value().setGlobal(state[idx].position, state[idx].orientation);
	});
}

void SkeletonState::applyGlobalState(Skeleton & skeleton, const NonRigidPartialState & state)
{
	if (state.empty() == true){
		return;
	}

	auto it = state.begin();
	kinematic::LinearizedSkeleton::Visitor::globalIndexedVisit(skeleton,
		[&state, &it](kinematic::Skeleton::JointPtr joint, kinematic::LinearizedSkeleton::NodeIDX idx)
	{
		if (it != state.end()){
			if (it->first == idx){
				joint->value().setGlobal(it->second.position, it->second.orientation);
				++it;
			}
		}
	});
}