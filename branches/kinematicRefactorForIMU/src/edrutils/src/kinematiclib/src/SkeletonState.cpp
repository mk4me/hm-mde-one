#include <kinematiclib/SkeletonState.h>
#include <osg/PositionAttitudeTransform>
#include <boost/lexical_cast.hpp>
#include <kinematicUtils/RotationConverter.h>
#include <kinematiclib/Joint.h>
#include <utils/TreeNode.h>

using namespace kinematic;

class SkeletonState::JointData::JointDataImpl
{
	friend class SkeletonState;

public:
	osg::Vec3 originTranslation;
	osg::Quat originRotation;
	std::string name;
	osg::Matrix shiftMatrix;
	osg::Matrix localMatrix;
	osg::Matrix globalMatrix;

	JointDataImpl(const std::string & name, const osg::Vec3 & translation, const osg::Quat & rotation) :
		originRotation(rotation), originTranslation(translation), name(name)
	{
		shiftMatrix.setTrans(translation);
	}

	~JointDataImpl()
	{

	}

	osg::Vec3 globalPosition() const
	{
		return globalMatrix.getTrans();
	}
	
	osg::Quat globalOrientation() const
	{
		return globalMatrix.getRotate();
	}

	void setGlobalOrientation(const osg::Quat& rotation)
	{
		globalMatrix.setRotate(rotation);
	}

	void setGlobalTranslation(const osg::Vec3& translation)
	{
		globalMatrix.setTrans(translation);
	}

	
	//! \param translation Lokalne przesuni�cie
	//! \param rotation Lokalna rotacja
	void update(const osg::Vec3 & translation, const osg::Quat & rotation, const osg::Matrix& parentG)
	{
		osg::Matrix m;
		m.set(rotation);
		m.setTrans(translation);
		localMatrix =  shiftMatrix * m;
		globalMatrix = localMatrix * parentG;
	}
};

SkeletonState::JointData::JointData(const std::string & name, const osg::Vec3 & translation, const osg::Quat & rotation) :
	impl(new JointDataImpl(name, translation, rotation))
{

}

SkeletonState::JointData::~JointData()
{

}

std::string SkeletonState::JointData::name() const
{
	return impl->name;
}

osg::Matrix kinematic::SkeletonState::JointData::getG() const
{
	return impl->globalMatrix;
}

void kinematic::SkeletonState::JointData::update(const osg::Vec3 & translation, const osg::Quat & rotation, const osg::Matrix& parentG)
{
	impl->update(translation, rotation, parentG);
}

osg::Vec3 SkeletonState::JointData::globalPosition() const 
{
	return impl->globalPosition();
}


osg::Quat kinematic::SkeletonState::JointData::globalOrientation() const
{
	return impl->globalOrientation();
}

void kinematic::SkeletonState::JointData::setGlobal(const osg::Quat& rotation)
{
	impl->setGlobalOrientation(rotation);
}

void kinematic::SkeletonState::JointData::setGlobal(const osg::Vec3& translation) {
	impl->setGlobalTranslation(translation);
}

SkeletonState::JointPtr SkeletonState::create(kinematic::JointConstPtr joint, const osg::Vec3 parentPosition)
{
	auto j = Joint::create(JointData(joint->value.name, joint->value.position, joint->value.orientation));
	j->value.setGlobal(parentPosition + joint->value.position);
	for (auto jj : joint->children)
	{
		auto pp = j->value.globalPosition();
		auto jjj = create(jj, pp);
		j->children.push_back(jjj);
		jjj->parent = j;
	//	// hierarchia osg!!
	//	j->value.impl->node->addChild(jjj->value.impl->node);
	}

	return j;
}

SkeletonState::SkeletonState(JointPtr root)
	: root_(root)
{

}

SkeletonState::SkeletonState(SkeletonState && Other)
	: root_(std::move(Other.root_))
{

}

SkeletonState SkeletonState::create(const Skeleton & skeleton)
{
	if (skeleton.root != nullptr){
		return SkeletonState(create(skeleton.root, osg::Vec3()));
	}

	return SkeletonState(JointPtr());
}

class LevelOrderVisitor
{
public:
	LevelOrderVisitor() : currentIDX(0) {}
	~LevelOrderVisitor() {}

	void operator()(JointConstPtr joint, const Joint::size_type level)
	{
		if (joint != nullptr){
			mapping.insert(SkeletonState::LinearizedNodesMapping::value_type(currentIDX++, joint->value.name));
		}
	}

	SkeletonState::LinearizedNodesMapping mapping;

private:

	SkeletonState::NodeIDX currentIDX;
};

SkeletonState::LinearizedNodesMapping SkeletonState::createMapping(const Skeleton & skeleton)
{
	LevelOrderVisitor lov;
	::Joint::visitLevelOrder(skeleton.root, lov);
	return std::move(lov.mapping);
}

template<typename T>
void updateOrientations(SkeletonState::JointPtr joint, T & valIT)
{
	joint->value.update(*valIT++);

	for (auto j : joint->children){		
		if (j->isLeaf() == false){
			updateOrientations(j, valIT);
		}
	}
}

template<typename T>
void updateOrientationsAndPosition(SkeletonState::JointPtr joint, T & valIT, const osg::Matrix& parentG)
{
	joint->value.update((*valIT).translation, (*valIT).rotation, parentG);
	++valIT;
	for (auto j : joint->children){		
		//if (j->isLeaf() == false){
			updateOrientationsAndPosition(j, valIT, joint->value.getG());
		//}
	}
}

void SkeletonState::update(SkeletonState & skeletonState, const RigidCompleteStateChange & stateChange)
{
	//skeletonState.root()->value.update(stateChange.translation);
	//auto it = stateChange.rotations.begin();
	//updateOrientations<decltype(stateChange.rotations.begin())>(skeletonState.root(), it);
}

void updateOrientationsAndPosition(SkeletonState::JointPtr joint, const SkeletonState::NonRigidCompleteStateChange & stateChange, const SkeletonState::LinearizedNodesMapping & mapping, const osg::Matrix& parentG)
{
	auto idx = mapping.right.at(joint->value.name());
	auto data = stateChange[idx];
	joint->value.update(data.translation, data.rotation, parentG);
	for (auto j : joint->children) {
		//if (j->isLeaf() == false){
		updateOrientationsAndPosition(j, stateChange, mapping, joint->value.getG());
		//}
	}
}

void SkeletonState::update(SkeletonState & skeletonState, const NonRigidCompleteStateChange & stateChange, const LinearizedNodesMapping & mapping)
{
	updateOrientationsAndPosition(skeletonState.root(), stateChange, mapping, osg::Matrix());
}

void SkeletonState::update(SkeletonState & skeletonState, const RigidPartialStateChange & stateChange)
{
	//auto it = stateChange.rotations.begin();
	//if (it == stateChange.rotations.end()){
	//	return;
	//}

	//skeletonState.root()->value.update(stateChange.translation);

	//NodeIDX nodeIDX = 0;
	//JointPtr root = skeletonState.root();
	//auto visitorL = [&it, &stateChange, &nodeIDX](JointPtr node, Joint::size_type level)
	//		{
	//			if (it->first == nodeIDX){

	//				node->value.update(it->second);
	//				++it;
	//			}
	//			++nodeIDX;
	//			return it != stateChange.rotations.end();
	//		};
	//Joint::visitLevelOrderWhile(root, visitorL);
}

//void SkeletonState::update(SkeletonState & skeletonState, const NonRigidPartialStateChange & stateChange)
//{
//
//}
//
//void SkeletonState::setGlobal(SkeletonState & skeletonState, const RigidCompleteStateChange & newState)
//{
//
//}
//
//void SkeletonState::setLocal(SkeletonState & skeletonState, const RigidCompleteStateChange & newState)
//{
//
//}
//
//void SkeletonState::setGlobal(SkeletonState & skeletonState, const NonRigidCompleteStateChange & newState)
//{
//
//}

//void SkeletonState::setLocal(SkeletonState & skeletonState, const NonRigidCompleteStateChange & newState)
//{
//
//}

//void updateGlobalState(SkeletonState::JointConstPtr joint, SkeletonState::NonRigidCompleteStateChange & state)
//{
//	SkeletonState::JointStateChange jsc;
//	jsc.rotation = joint->value.globalOrientation();
//	jsc.translation = joint->value.globalPosition();
//	state.push_back(jsc);
//	for (auto j : joint->children){
//		updateGlobalState(j, state);
//	}
//}

//void updateLocalState(SkeletonState::JointConstPtr joint, SkeletonState::NonRigidCompleteStateChange & state)
//{
//	SkeletonState::JointStateChange jsc;
//	jsc.rotation = joint->value.localOrientation();
//	jsc.translation = joint->value.localPosition();
//	state.push_back(jsc);
//	for (auto j : joint->children){
//		updateLocalState(j, state);
//	}
//}
//
//SkeletonState::NonRigidCompleteStateChange SkeletonState::globalState(const SkeletonState & skeletonState)
//{
//	NonRigidCompleteStateChange ret;
//
//	if (skeletonState.root() != nullptr){
//		updateGlobalState(skeletonState.root(), ret);
//	}
//
//	return ret;
//}
//
//SkeletonState::NonRigidCompleteStateChange SkeletonState::localState(const SkeletonState & skeletonState)
//{
//	NonRigidCompleteStateChange ret;
//
//	if (skeletonState.root() != nullptr){
//		updateLocalState(skeletonState.root(), ret);
//	}
//
//	return ret;
//}

SkeletonState::JointPtr SkeletonState::root()
{
	return root_;
}

SkeletonState::JointConstPtr SkeletonState::root() const
{
	return root_;
}

osg::Vec3 resolveRadians(const acclaim::Skeleton & skeleton, const osg::Vec3& toResolve)
{
	return skeleton.units.isAngleInRadians() ? static_cast<osg::Vec3d>(toResolve) : kinematicUtils::toRadians(toResolve);
}

SkeletonState::JointStateChange convert(const acclaim::Skeleton & skeleton,
	const acclaim::MotionData::BoneData & boneData)
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

	SkeletonState::JointStateChange ret;
	ret.translation = osg::Vec3(0, 0, 0);
	ret.rotation = osg::Quat(0, 0, 0, 1);

	osg::Vec3 rot(0, 0, 0);
	unsigned int i = 0;
	unsigned int roti = 0;
	unsigned int transi = 0;
	for (const auto & dof : bone.dofs)
	{
		switch (dof.channel)
		{
		case kinematicUtils::ChannelType::TX:
			ret.translation.x() = boneData.channelValues[i];
			++transi;
			break;

		case kinematicUtils::ChannelType::TY:
			ret.translation.y() = boneData.channelValues[i];
			++transi;
			break;

		case kinematicUtils::ChannelType::TZ:
			ret.translation.z() = boneData.channelValues[i];
			++transi;
			break;

		case kinematicUtils::ChannelType::RX:
			rot[0] = boneData.channelValues[i];
			break;

		case kinematicUtils::ChannelType::RY:
			rot[1] = boneData.channelValues[i];
			break;

		case kinematicUtils::ChannelType::RZ:
			rot[2] = boneData.channelValues[i];
			break;

		case acclaim::DegreeOfFreedom::L:
			//TODO - przesun�� dziecko
			break;

		default:
			throw std::runtime_error("Unsupported acclaim degree of freedom: " + boost::lexical_cast<std::string>(dof.channel));
		}

		++i;
	}


	osg::Quat c = kinematicUtils::convert(resolveRadians(skeleton, bone.axis), bone.axisOrder);
	osg::Quat cinv = c.inverse();
	osg::Quat rot2 = kinematicUtils::convert(resolveRadians(skeleton, rot), bone.axisOrder);
	ret.rotation = cinv * rot2 * c;

	return ret;
}

void orderChanges(const acclaim::Skeleton & skeleton, const int currentBoneID,
	const std::map<std::string, SkeletonState::JointStateChange> & changes,
	SkeletonState::NonRigidCompleteStateChange & change)
{
	const auto & b = skeleton.bones.find(currentBoneID)->second;	
	auto changeIT = changes.find(b.name);
	if (changeIT != changes.end()) {
		change.push_back(changeIT->second);
	}
	
	auto lIT = skeleton.hierarchy.left.lower_bound(currentBoneID);
	auto uIT = skeleton.hierarchy.left.upper_bound(currentBoneID);

	for (; lIT != uIT; ++lIT)
	{
		orderChanges(skeleton, lIT->get_right(), changes, change);
	}
	
}

SkeletonState::RigidPartialStateChange SkeletonState::convert(const acclaim::Skeleton & skeleton,
	const acclaim::MotionData::FrameData & motionData, const LinearizedNodesMapping & mapping)
{
	RigidPartialStateChange ret;

	for (const auto & bd : motionData.bonesData)
	{
		auto it = mapping.right.find(bd.name);
		if (it != mapping.right.end()){
			auto nodeID = it->get_left();
			auto jointStateChange = ::convert(skeleton, bd);
			ret.rotations.insert(std::map<NodeIDX, osg::Quat>::value_type(nodeID, jointStateChange.rotation));

			if (nodeID == 0){
				ret.translation = jointStateChange.translation;
			}
		}
	}

	//orderChanges(skeleton, skeleton.root, changes, ret);

	return ret;
}

SkeletonState::JointStateChange convert(biovision::JointConstPtr joint, const biovision::MotionData::ChannelData & frameData)
{
	if (joint->channels.size() != frameData.size()){
		throw std::runtime_error("Biovision motion data mismatch skeleton structure - channels size mismatch. Joint: " + joint->name + ". Motion data channels size: " + boost::lexical_cast<std::string>(frameData.size()) + ". Joint channels size: " + boost::lexical_cast<std::string>(joint->channels.size()));
	}

	SkeletonState::JointStateChange ret;
	ret.translation = osg::Vec3(0, 0, 0);
	ret.rotation = osg::Quat(0, 0, 0, 1);

	osg::Vec3 rot(0, 0, 0);
	unsigned int roti = 0;
	unsigned int i = 0;	
	for (const auto & channel : joint->channels)
	{
		switch (channel)
		{
		case kinematicUtils::ChannelType::TX:
			ret.translation.x() = frameData[i];
			break;

		case kinematicUtils::ChannelType::TY:
			ret.translation.y() = frameData[i];
			break;

		case kinematicUtils::ChannelType::TZ:
			ret.translation.z() = frameData[i];
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

	ret.rotation = kinematicUtils::convert(rot, joint->rotationOrder);

	return ret;
}

void orderChanges(biovision::JointConstPtr joint,
	const std::map<std::string, SkeletonState::JointStateChange> & changes,
	SkeletonState::RigidCompleteStateChange & change)
{
	change.rotations.push_back(changes.find(joint->name)->second.rotation);

	for (auto j : joint->joints)
	{
		orderChanges(j, changes, change);
	}
}

SkeletonState::RigidCompleteStateChange SkeletonState::convert(const biovision::Skeleton & skeleton,
	const biovision::MotionData::FrameJointData & motionData)
{
	RigidCompleteStateChange ret;

	std::map<std::string, JointStateChange> changes;	

	unsigned int i = 0;	
	for (const auto & joint : skeleton.sourceOrderJoints)
	{
		changes.insert(std::map<std::string, JointStateChange>::value_type(joint->name, ::convert(joint, motionData[i++])));
	}

	orderChanges(skeleton.root, changes, ret);

	auto rIT = changes.find(skeleton.root->name);

	if (rIT != changes.end()){
		ret.translation = rIT->second.translation;
	}

	return ret;
}

kinematic::SkeletonState::Joint2Index kinematic::SkeletonState::createJoint2IndexMapping(const kinematic::SkeletonState &skeleton, const kinematic::SkeletonState::LinearizedNodesMapping& mapping)
{
	kinematic::SkeletonState::Joint2Index m;
	kinematic::SkeletonState::JointConstPtr root = skeleton.root();
	auto visitor = [&](kinematic::SkeletonState::JointConstPtr joint, kinematic::SkeletonState::Joint::size_type lvl)
	{
		auto i = mapping.right.at(joint->value.name());
		std::pair<kinematic::SkeletonState::JointConstPtr, unsigned int> p = std::make_pair(joint, i);
		m.insert(p);
	};
	kinematic::SkeletonState::Joint::visitLevelOrder(root, visitor);
	return m;
}

void createConnectionRec(kinematic::SkeletonState::JointConstPtr parent, const std::map<kinematic::SkeletonState::JointConstPtr, unsigned int>& indices, osgutils::SegmentsDescriptors& sd)
{
	for (auto& child : parent->children) {
		osgutils::SegmentDescriptor d;
		d.length = (parent->value.globalPosition() - child->value.globalPosition()).length();
		d.range = std::make_pair(indices.at(parent), indices.at(child));
		sd.push_back(d);
		createConnectionRec(child, indices, sd);
	}
}

osgutils::SegmentsDescriptors kinematic::SkeletonState::createConnections(const kinematic::SkeletonState& skeleton, const Joint2Index& mapping)
{
	osgutils::SegmentsDescriptors sd;
	createConnectionRec(skeleton.root(), mapping, sd);
	return sd;
}

kinematic::SkeletonState::NonRigidCompleteStateChange kinematic::SkeletonState::convertStateChange(const LinearizedNodesMapping &mapping, const RigidPartialStateChange &sChange)
{
	NonRigidCompleteStateChange frame;
	auto count = mapping.size();
	frame.push_back(kinematic::SkeletonState::JointStateChange{ sChange.translation, osg::Quat() });
	for (int i = 1; i < count; i++) {
		auto it = sChange.rotations.find(i);
		frame.push_back(kinematic::SkeletonState::JointStateChange{ osg::Vec3(), it != sChange.rotations.end() ? it->second : osg::Quat() });
	}
	return frame;
}


