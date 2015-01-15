#include <kinematiclib/SkeletonState.h>
#include <osg/PositionAttitudeTransform>
#include <boost/lexical_cast.hpp>
#include <kinematicUtils/RotationConverter.h>

using namespace kinematic;

class SkeletonState::Joint::JointImpl
{
	friend class SkeletonState;

private:

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
		node->setPosition(node->getPosition() + translation);
	}

	void localUpdate(const osg::Quat & rotation)
	{
		node->setAttitude(node->getAttitude() * rotation);
	}

public:

	JointImpl(const std::string & name, const osg::Vec3 & translation,
		const osg::Quat & rotation) : node(new osg::PositionAttitudeTransform),
		globalRequiresRefresh(true)
	{
		node->setName(name);		
		node->setPosition(translation);
		node->setAttitude(rotation);
		node->setScale(osg::Vec3(1, 1, 1));
		globalMatrix.makeIdentity();		
	}

	~JointImpl()
	{

	}

	JointPtr parent()
	{
		return parent_.lock();
	}
	
	JointConstPtr parent() const
	{
		return parent_.lock();
	}

	std::string name() const
	{
		return node->getName();
	}

	JointPtr child(const unsigned int idx)
	{
		return children[idx];
	}

	JointConstPtr child(const unsigned int idx) const
	{
		return children[idx];
	}

	const unsigned int childrenCount() const
	{
		return children.size();
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
	//! Staw rodzic
	SkeletonState::JointWeakPtr parent_;
	//! Stawy dzieci
	std::vector<SkeletonState::JointPtr> children;
	//! Wêze³ osg
	osg::ref_ptr<osg::PositionAttitudeTransform> node;
	//! Czy stan globalny wymaga odœwie¿enia
	mutable bool globalRequiresRefresh;
	//! Macierz opisuj¹ca stan globalny
	mutable osg::Matrix globalMatrix;
};

SkeletonState::Joint::Joint(const std::string & name, const osg::Vec3 & translation,
	const osg::Quat & rotation) : impl(new JointImpl(name, translation, rotation))
{

}

SkeletonState::Joint::~Joint()
{

}

SkeletonState::JointConstPtr SkeletonState::Joint::parent() const
{
	return impl->parent();
}

SkeletonState::JointPtr SkeletonState::Joint::parent()
{
	return impl->parent();
}

std::string SkeletonState::Joint::name() const
{
	return impl->name();
}

SkeletonState::JointConstPtr SkeletonState::Joint::child(const unsigned int idx) const
{
	return impl->child(idx);
}

SkeletonState::JointPtr SkeletonState::Joint::child(const unsigned int idx)
{
	return impl->child(idx);
}

const unsigned int SkeletonState::Joint::childrenCount() const
{
	return impl->childrenCount();
}

osg::Vec3 SkeletonState::Joint::localPosition() const
{
	return impl->localPosition();
}

osg::Vec3 SkeletonState::Joint::globalPosition() const
{
	return impl->globalPosition();
}

osg::Quat SkeletonState::Joint::localOrientation() const
{
	return impl->localOrientation();
}

osg::Quat SkeletonState::Joint::globalOrientation() const
{
	return impl->globalOrientation();
}

void SkeletonState::Joint::update(const osg::Vec3 & translation)
{
	impl->update(translation);
}

void SkeletonState::Joint::setGlobal(const osg::Vec3 & position)
{
	impl->setGlobal(position);
}

void SkeletonState::Joint::setLocal(const osg::Vec3 & position)
{
	impl->setLocal(position);
}

void SkeletonState::Joint::update(const osg::Quat & rotation)
{
	impl->update(rotation);
}

void SkeletonState::Joint::setGlobal(const osg::Quat & orientation)
{
	impl->setGlobal(orientation);
}

void SkeletonState::Joint::setLocal(const osg::Quat & orientation)
{
	impl->setLocal(orientation);
}

void SkeletonState::Joint::update(const osg::Vec3 & translation, const osg::Quat & rotation)
{
	impl->update(translation, rotation);
}

void SkeletonState::Joint::setGlobal(const osg::Vec3 & position, const osg::Quat & orientation)
{
	impl->setGlobal(position, orientation);
}

void SkeletonState::Joint::setLocal(const osg::Vec3 & position, const osg::Quat & orientation)
{
	impl->setLocal(position, orientation);
}

SkeletonState::JointPtr SkeletonState::create(kinematic::JointConstPtr joint, unsigned int & activeJointsCount)
{
	SkeletonState::JointPtr j(new SkeletonState::Joint(joint->name, joint->position, joint->orientation));

	for (auto jj : joint->children)
	{
		j->impl->children.push_back(create(jj, activeJointsCount));
	}

	if (joint->children.empty() == false){
		++activeJointsCount;
	}

	return j;
}

SkeletonState::SkeletonState(JointPtr root, const unsigned int activeJointsCount)
	: root_(root), activeJointsCount(activeJointsCount)
{

}

SkeletonState::SkeletonState(SkeletonState && Other)
	: root_(std::move(Other.root_)), activeJointsCount(Other.activeJointsCount)
{
	Other.activeJointsCount = 0;
}

SkeletonState SkeletonState::create(const Skeleton & skeleton)
{
	if (skeleton.root != nullptr){
		unsigned int activeJointsCount = 0;
		auto root = create(skeleton.root, activeJointsCount);
		return SkeletonState(root, activeJointsCount);
	}

	return SkeletonState(JointPtr(), 0);
}

template<typename T>
void updateOrientations(SkeletonState::JointPtr joint, T & valIT)
{
	joint->update(*valIT++);
	for (unsigned int i = 0; i < joint->childrenCount(); ++i){
		auto j = joint->child(i);
		if (j->childrenCount() > 0){
			updateOrientations(j, valIT);
		}
	}
}

template<typename T>
void updateOrientationsAndPosition(SkeletonState::JointPtr joint, T & valIT)
{
	joint->update((*valIT).translation, (*valIT).rotation);
	++valIT;
	for (unsigned int i = 0; i < joint->childrenCount(); ++i){
		auto j = joint->child(i);
		if (j->childrenCount() > 0){
			updateOrientationsAndPosition(j, valIT);
		}
	}
}

void SkeletonState::update(SkeletonState & skeletonState, const SimpleStateChange & stateChange)
{
	if (stateChange.rotations.size() != skeletonState.activeJointsCount){
		throw std::runtime_error("StateChange size differs from state active joints count");
	}

	skeletonState.root()->update(stateChange.translation);
	auto it = stateChange.rotations.begin();
	updateOrientations<decltype(stateChange.rotations.begin())>(skeletonState.root(), it);
}

void SkeletonState::update(SkeletonState & skeletonState, const FullStateChange & stateChange)
{
	if (stateChange.size() != skeletonState.activeJointsCount){
		throw std::runtime_error("StateChange size differs from state active joints count");
	}

	auto it = stateChange.begin();
	updateOrientationsAndPosition<decltype(stateChange.begin())>(skeletonState.root(), it);
}

void SkeletonState::setGlobal(SkeletonState & skeletonState, const SimpleStateChange & newState)
{

}

void SkeletonState::setLocal(SkeletonState & skeletonState, const SimpleStateChange & newState)
{

}

void SkeletonState::setGlobal(SkeletonState & skeletonState, const FullStateChange & newState)
{

}

void SkeletonState::setLocal(SkeletonState & skeletonState, const FullStateChange & newState)
{

}

void updateGlobalState(SkeletonState::JointConstPtr joint, SkeletonState::FullStateChange & state)
{
	SkeletonState::JointStateChange jsc;
	jsc.rotation = joint->globalOrientation();
	jsc.translation = joint->globalPosition();
	state.push_back(jsc);
	for (unsigned int i = 0; i < joint->childrenCount(); ++i){
		updateGlobalState(joint->child(i), state);
	}
}

void updateLocalState(SkeletonState::JointConstPtr joint, SkeletonState::FullStateChange & state)
{
	SkeletonState::JointStateChange jsc;
	jsc.rotation = joint->localOrientation();
	jsc.translation = joint->localPosition();
	state.push_back(jsc);
	for (unsigned int i = 0; i < joint->childrenCount(); ++i){
		updateLocalState(joint->child(i), state);
	}
}

SkeletonState::FullStateChange SkeletonState::globalState(const SkeletonState & skeletonState)
{
	FullStateChange ret;

	if (skeletonState.root() != nullptr){
		updateGlobalState(skeletonState.root(), ret);
	}

	return ret;
}

SkeletonState::FullStateChange SkeletonState::localState(const SkeletonState & skeletonState)
{
	FullStateChange ret;

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

	ret.rotation = kinematicUtils::convert(rot, bone.rotationOrder);

	return ret;
}

void orderChanges(const acclaim::Skeleton & skeleton, const int currentBoneID,
	const std::map<std::string, SkeletonState::JointStateChange> & changes,
	SkeletonState::FullStateChange & change)
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

SkeletonState::FullStateChange SkeletonState::convert(const acclaim::Skeleton & skeleton,
	const acclaim::MotionData::FrameData & motionData)
{
	FullStateChange ret;

	std::map<std::string, JointStateChange> changes;

	for (const auto & bd : motionData.bonesData)
	{
		changes.insert(std::map<std::string, JointStateChange>::value_type(bd.name, ::convert(skeleton, bd)));
	}

	orderChanges(skeleton, skeleton.root, changes, ret);

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
	SkeletonState::FullStateChange & change)
{
	change.push_back(changes.find(joint->name)->second);

	for (auto j : joint->joints)
	{
		orderChanges(j, changes, change);
	}
}

SkeletonState::FullStateChange SkeletonState::convert(const biovision::Skeleton & skeleton,
	const biovision::MotionData::FrameJointData & motionData)
{
	FullStateChange ret;

	std::map<std::string, JointStateChange> changes;	

	unsigned int i = 0;	
	for (const auto & joint : skeleton.sourceOrderJoints)
	{
		changes.insert(std::map<std::string, JointStateChange>::value_type(joint->name, ::convert(joint, motionData[i++])));
	}

	orderChanges(skeleton.root, changes, ret);

	return ret;
}


void getJointsRec(kinematic::SkeletonState::JointConstPtr parent, std::vector<kinematic::SkeletonState::JointConstPtr>& joints)
{
	joints.push_back(parent);
	int count = parent->childrenCount();
	for (int i = 0; i < count; ++i) {
		getJointsRec(parent->child(i), joints);
	}
}
std::vector<kinematic::SkeletonState::JointConstPtr> kinematic::SkeletonState::getJoints(const SkeletonState & skeletonState)
{
	std::vector<JointConstPtr> ret;
	getJointsRec(skeletonState.root(), ret);
	return ret;
}
