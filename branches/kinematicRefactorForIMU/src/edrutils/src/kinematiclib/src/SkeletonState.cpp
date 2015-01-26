#include <kinematiclib/SkeletonState.h>
#include <osg/PositionAttitudeTransform>
#include <boost/lexical_cast.hpp>
#include <kinematicUtils/RotationConverter.h>

using namespace kinematic;

class SkeletonState::JointData::JointDataImpl
{
	friend class SkeletonState;

private:
	osg::Vec3 originTranslation;
	osg::Quat originRotation;

	void refreshGlobal() const
	{
		if (globalRequiresRefresh == true){
			auto mw = node->getWorldMatrices();
			if (mw.empty() == true){
				globalMatrix.makeScale(1, 1, 1);
				globalMatrix.makeTranslate(localPosition());
				globalMatrix.makeRotate(localOrientation());
			}
			else{
				globalMatrix = mw[0];
			}

			globalRequiresRefresh = false;
		}
	}

	void localUpdate(const osg::Vec3 & translation)
	{
		node->setPosition(originTranslation + translation);
	}

	void localUpdate(const osg::Quat & rotation)
	{
		node->setAttitude(originRotation * rotation);
	}

public:

	JointDataImpl(const std::string & name, const osg::Vec3 & translation, const osg::Quat & rotation) :
		node(new osg::PositionAttitudeTransform), originRotation(rotation), 
		originTranslation(translation), globalRequiresRefresh(true)
	{
		node->setName(name);		
		node->setPosition(translation);
		node->setAttitude(rotation);
		node->setScale(osg::Vec3(1, 1, 1));
		globalMatrix.makeIdentity();		
	}

	~JointDataImpl()
	{

	}

	std::string name() const
	{
		return node->getName();
	}

	osg::Vec3 localPosition() const
	{
		return node->getPosition();
	}

	osg::Vec3 globalPosition() const
	{
		refreshGlobal();
		return globalMatrix.getTrans();
	}
	
	osg::Quat localOrientation() const
	{
		return node->getAttitude();
	}

	//! \return Globalna orientacja
	osg::Quat globalOrientation() const
	{
		refreshGlobal();
		return globalMatrix.getRotate();
	}

	//! \param translation Lokalne przesuniêcie
	void update(const osg::Vec3 & translation)
	{
		localUpdate(translation);
		globalRequiresRefresh = true;
	}

	//! \param position Globalna pozycja
	void setGlobal(const osg::Vec3 & position)
	{
		node->setPosition(node->getPosition() + position - globalPosition());
		globalMatrix.setTrans(position);
	}

	//! \param position Globalna pozycja
	void setLocal(const osg::Vec3 & position)
	{
		node->setPosition(position);
		globalRequiresRefresh = true;
	}

	//! \param rotation Lokalna rotacja
	void update(const osg::Quat & rotation)
	{
		localUpdate(rotation);
		globalRequiresRefresh = true;
	}

	//! \param orientation Globalna orientacja
	void setGlobal(const osg::Quat & orientation)
	{
		node->setAttitude(node->getAttitude() * orientation / globalOrientation());
		globalMatrix.setRotate(orientation);
	}

	//! \param orientation Globalna orientacja
	void setLocal(const osg::Quat & orientation)
	{
		node->setAttitude(orientation);
		globalRequiresRefresh = true;
	}

	//! \param translation Lokalne przesuniêcie
	//! \param rotation Lokalna rotacja
	void update(const osg::Vec3 & translation, const osg::Quat & rotation)
	{
		localUpdate(translation);
		localUpdate(rotation);
		globalRequiresRefresh = true;
	}

	//! \param position Globalna pozycja
	//! \param orientation Globalna orientacja
	void setGlobal(const osg::Vec3 & position, const osg::Quat & orientation)
	{
		setGlobal(position);
		setGlobal(orientation);
	}

	//! \param position Globalna pozycja
	//! \param orientation Globalna orientacja
	void setLocal(const osg::Vec3 & position, const osg::Quat & orientation)
	{
		setLocal(position);
		setLocal(orientation);
	}

private:
	//! Wêze³ osg
	osg::ref_ptr<osg::PositionAttitudeTransform> node;
	//! Czy stan globalny wymaga odœwie¿enia
	mutable bool globalRequiresRefresh;
	//! Macierz opisuj¹ca stan globalny
	mutable osg::Matrix globalMatrix;
};

SkeletonState::JointData::JointData(const std::string & name, const osg::Vec3 & translation,
	const osg::Quat & rotation) : impl(new JointDataImpl(name, translation, rotation))
{

}

SkeletonState::JointData::~JointData()
{

}

std::string SkeletonState::JointData::name() const
{
	return impl->name();
}

osg::Vec3 SkeletonState::JointData::localPosition() const
{
	return impl->localPosition();
}

osg::Vec3 SkeletonState::JointData::globalPosition() const
{
	return impl->globalPosition();
}

osg::Quat SkeletonState::JointData::localOrientation() const
{
	return impl->localOrientation();
}

osg::Quat SkeletonState::JointData::globalOrientation() const
{
	return impl->globalOrientation();
}

void SkeletonState::JointData::update(const osg::Vec3 & translation)
{
	impl->update(translation);
}

void SkeletonState::JointData::setGlobal(const osg::Vec3 & position)
{
	impl->setGlobal(position);
}

void SkeletonState::JointData::setLocal(const osg::Vec3 & position)
{
	impl->setLocal(position);
}

void SkeletonState::JointData::update(const osg::Quat & rotation)
{
	impl->update(rotation);
}

void SkeletonState::JointData::setGlobal(const osg::Quat & orientation)
{
	impl->setGlobal(orientation);
}

void SkeletonState::JointData::setLocal(const osg::Quat & orientation)
{
	impl->setLocal(orientation);
}

void SkeletonState::JointData::update(const osg::Vec3 & translation, const osg::Quat & rotation)
{
	impl->update(translation, rotation);
}

void SkeletonState::JointData::setGlobal(const osg::Vec3 & position, const osg::Quat & orientation)
{
	impl->setGlobal(position, orientation);
}

void SkeletonState::JointData::setLocal(const osg::Vec3 & position, const osg::Quat & orientation)
{
	impl->setLocal(position, orientation);
}

SkeletonState::JointPtr SkeletonState::create(kinematic::JointConstPtr joint)
{
	auto j = Joint::create(JointData(joint->value.name, joint->value.position, joint->value.orientation));	

	for (auto jj : joint->children)
	{
		auto jjj = create(jj);
		j->children.push_back(jjj);
		jjj->parent = j;
		// hierarchia osg!!
		j->value.impl->node->addChild(jjj->value.impl->node);
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
		return SkeletonState(create(skeleton.root));
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
void updateOrientationsAndPosition(SkeletonState::JointPtr joint, T & valIT)
{
	joint->value.update((*valIT).translation, (*valIT).rotation);
	++valIT;
	for (auto j : joint->children){		
		//if (j->isLeaf() == false){
			updateOrientationsAndPosition(j, valIT);
		//}
	}
}

void SkeletonState::update(SkeletonState & skeletonState, const RigidCompleteStateChange & stateChange)
{
	skeletonState.root()->value.update(stateChange.translation);
	auto it = stateChange.rotations.begin();
	updateOrientations<decltype(stateChange.rotations.begin())>(skeletonState.root(), it);
}

void SkeletonState::update(SkeletonState & skeletonState, const NonRigidCompleteStateChange & stateChange)
{
	auto it = stateChange.begin();
	updateOrientationsAndPosition<decltype(stateChange.begin())>(skeletonState.root(), it);
}

void SkeletonState::update(SkeletonState & skeletonState, const RigidPartialStateChange & stateChange)
{
	auto it = stateChange.rotations.begin();
	if (it == stateChange.rotations.end()){
		return;
	}

	skeletonState.root()->value.update(stateChange.translation);

	NodeIDX nodeIDX = 0;
	Joint::visitLevelOrderWhile(skeletonState.root(), [&it, &stateChange, &nodeIDX](JointPtr node, Joint::size_type level)
	{
		if (it->first == nodeIDX){

			node->value.update(it->second);
			++it;
		}
		++nodeIDX;
		return it != stateChange.rotations.end();
	});
}

void SkeletonState::update(SkeletonState & skeletonState, const NonRigidPartialStateChange & stateChange)
{

}

void SkeletonState::setGlobal(SkeletonState & skeletonState, const RigidCompleteStateChange & newState)
{

}

void SkeletonState::setLocal(SkeletonState & skeletonState, const RigidCompleteStateChange & newState)
{

}

void SkeletonState::setGlobal(SkeletonState & skeletonState, const NonRigidCompleteStateChange & newState)
{

}

void SkeletonState::setLocal(SkeletonState & skeletonState, const NonRigidCompleteStateChange & newState)
{

}

void updateGlobalState(SkeletonState::JointConstPtr joint, SkeletonState::NonRigidCompleteStateChange & state)
{
	SkeletonState::JointStateChange jsc;
	jsc.rotation = joint->value.globalOrientation();
	jsc.translation = joint->value.globalPosition();
	state.push_back(jsc);
	for (auto j : joint->children){
		updateGlobalState(j, state);
	}
}

void updateLocalState(SkeletonState::JointConstPtr joint, SkeletonState::NonRigidCompleteStateChange & state)
{
	SkeletonState::JointStateChange jsc;
	jsc.rotation = joint->value.localOrientation();
	jsc.translation = joint->value.localPosition();
	state.push_back(jsc);
	for (auto j : joint->children){
		updateLocalState(j, state);
	}
}

SkeletonState::NonRigidCompleteStateChange SkeletonState::globalState(const SkeletonState & skeletonState)
{
	NonRigidCompleteStateChange ret;

	if (skeletonState.root() != nullptr){
		updateGlobalState(skeletonState.root(), ret);
	}

	return ret;
}

SkeletonState::NonRigidCompleteStateChange SkeletonState::localState(const SkeletonState & skeletonState)
{
	NonRigidCompleteStateChange ret;

	if (skeletonState.root() != nullptr){
		updateLocalState(skeletonState.root(), ret);
	}

	return ret;
}

SkeletonState::JointPtr SkeletonState::root()
{
	return root_;
}

SkeletonState::JointConstPtr SkeletonState::root() const
{
	return root_;
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
			rot[roti++] = boneData.channelValues[i];
			break;

		case kinematicUtils::ChannelType::RY:
			rot[roti++] = boneData.channelValues[i];
			break;

		case kinematicUtils::ChannelType::RZ:
			rot[roti++] = boneData.channelValues[i];
			break;

		case acclaim::DegreeOfFreedom::L:
			//TODO - przesun¹æ dziecko
			break;

		default:
			throw std::runtime_error("Unsupported acclaim degree of freedom: " + boost::lexical_cast<std::string>(dof.channel));
		}

		++i;
	}

	
	ret.rotation = kinematicUtils::convert(skeleton.units.isAngleInRadians() ? rot : kinematicUtils::toRadians(rot), bone.axisOrder);

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