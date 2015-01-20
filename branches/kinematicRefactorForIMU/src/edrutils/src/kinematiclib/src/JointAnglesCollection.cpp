#include <algorithm>
#include <osg/Quat>
#include <osg/Vec3>
#include <kinematiclib/JointAnglesCollection.h>

using namespace std;

namespace kinematic {

JointAnglesCollection::JointAnglesCollection(void) :
    lengthRatio(4.0),
    initialized(false)
{
}

//void JointAnglesCollection::setSkeletal(kinematic::SkeletonConstPtr skeletalModel, const kinematic::SkeletonStates& skeletalData)
//{
//    UTILS_ASSERT(!initialized);
//    
//	this->skeleton = skeletalModel;
//     std::map<std::string, kinematic::JointPtr> joints = kinematic::Skeleton::getJoints(*skeletalModel);
//     std::for_each(joints.begin(), joints.end(), [&](std::pair<string, JointPtr> p) 
//     {   
//        JointPtr joint = p.second;
//		double totalLength = 0.0;
//
//		/*for(auto it = joint->getChildrenBones().begin(); it != joint->getChildrenBones().end(); ++it){
//
//			auto bone = *it;
//
//			bone->setLength(bone->getLength() / lengthRatio);
//
//			totalLength += bone->getLength();
//		}
//        */            
//		/*osg::Vec3 shift = joint->getDirection() * totalLength / joint->getChildrenBones().size();
//		shift = SkeletonUtils::vectorRotation(shift, 
//			osg::DegreesToRadians(-joint->getAxis()[0]),
//			osg::DegreesToRadians(-joint->getAxis()[1]),
//			osg::DegreesToRadians(-joint->getAxis()[2]));
//		joint->setLocalShift(shift);			
//
//		osg::Quat pc;
//		JointPtr parent = joint->getActiveParent().lock();
//		if (parent) {
//			pc = SkeletonUtils::rotationParentChild(parent, joint);
//		}
//		joint->setChildParentRotation(pc);*/
//    } 
//   );
//   
//   createQuaternionRepresentation(skeletalModel, skeletalData);
//   initialized = true;
//}

//void JointAnglesCollection::setSkeletal( kinematic::SkeletonPtr skeletalModel, const std::vector<osg::Vec3>& rootPositions, const Collection& channels )
//{
//    UTILS_ASSERT(!initialized);
//    this->skeleton = skeletalModel;
//    this->rootPositions = rootPositions;
//    this->channels = channels;
//    initialized = true;
//}

//void JointAnglesCollection::createQuaternionRepresentation(kinematic::SkeletonConstPtr skeletalModel, const kinematic::SkeletonStates& data)
//{
//    UTILS_ASSERT(lengthRatio > 0.0);
//	auto jointsMap = Skeleton::getJoints(*skeletalModel);
//	std::vector<JointPtr> jointsByIndex;
//	for (auto& j : jointsMap) {
//		jointsByIndex.push_back(j.second);
//	}
//
//    if (data.frames.empty()) {
//        throw std::runtime_error("No frames loaded");
//    }
//
//	auto& jointsData = data.frames[0];
//	std::map<std::string, JointAngleChannelPtr> channelsMap;
//
//	float fps = 1.0f / data.frameTime;
//	//TODO
//	/*
//	for (auto& name : data.jointNames) {
//		JointAngleChannelPtr channel(new JointAngleChannel(fps)); 
//		channel->setName(name);
//		channelsMap[name] = channel;
//		addChannel(channel);
//	}*/
//
//	auto framesNo = data.frames.size();
//    this->rootPositions.resize(framesNo);
//     
//	for (auto& frame : data.frames) {
//		for (auto& joint : jointsByIndex) {
//            double rx, ry, rz;
//            int index;
//		    if (!joint) {
//                std::runtime_error("Joint not found");
//            }
//			std::string name = joint->value.name;
//		
//			//SkeletalData::singleJointStatePtr jointState = frames[i]->jointsData[j];
//			//index = DegreeOfFreedom::getChannelIndex(DegreeOfFreedom::RZ, joint->getDofs());
//			//rz = (index != -1 ? jointState->channelValues[index] : 0.0);
//			//index = DegreeOfFreedom::getChannelIndex(DegreeOfFreedom::RX, joint->getDofs());
//			//rx = (index != -1 ? jointState->channelValues[index] : 0.0);
//			//index = DegreeOfFreedom::getChannelIndex(DegreeOfFreedom::RY, joint->getDofs());
//			//ry = (index != -1 ? jointState->channelValues[index] : 0.0);
//			//double mul = skeletalModel->getAngle() == SkeletalModel::radians ? 1.0 : osg::DegreesToRadians(1.0);
//
//			//osg::Quat mX; mX.makeRotate(mul * (rx), osg::Vec3(1, 0, 0));
//			//osg::Quat mY; mY.makeRotate(mul * (ry), osg::Vec3(0, 1, 0));
//			//osg::Quat mZ; mZ.makeRotate(mul * (rz), osg::Vec3(0, 0, 1));
//
//			//channelsMap[name]->addPoint(SkeletonUtils::createRotation(mX, mY, mZ, joint->getOrder()));
//			// hack
//
//			//TODO
//			/*
//			if (name == "HumanoidRoot") continue;
//			//name = name == "HumanoidRoot" ? "root" : name;
//			auto nameIt = std::find(data.jointNames.begin(), data.jointNames.end(), name);
//			if (nameIt != data.jointNames.end()) {
//				auto j = std::distance(data.jointNames.begin(), nameIt);
//				channelsMap[name]->addPoint(frame[j].rotation);
//			}
//			*/
//        }
//    }
//}

osg::Vec3 JointAnglesCollection::getRootPosition( int frame ) const
{
    return rootPositions[frame];
}

osg::Vec3 JointAnglesCollection::getRootPosition( double time ) const
{
    int framesNo = rootPositions.size();
    // TODO : Lerp
    int frame = std::min(std::max(0, static_cast<int>(framesNo * time)), framesNo - 1);
    return getRootPosition(frame);
}

int JointAnglesCollection::getIndex(const std::string& name ) const
{
	int index = tryGetIndex(name);
	if (index == -1) {
		std::string info("Name: ");
		info += name;
		info += " does not exist";
		throw std::runtime_error(info);
	}

	return index;
}

int JointAnglesCollection::tryGetIndex(const std::string& name ) const
{
	int count = getNumChannels();
	for (int i = 0; i < count; ++i) {
		std::string channelName = getChannel(i)->getName();
		if (channelName == name) {
			return i;
		}
	}
	return -1;
}

JointAnglesCollection* JointAnglesCollection::clone() const
{
	std::unique_ptr<JointAnglesCollection> obj(new JointAnglesCollection());
	const int count = static_cast<int>(channels.size());
	obj->channels.resize(count);
	for (int i = 0; i < count; ++i) {
		obj->channels[i] = JointAngleChannelPtr(this->channels[i]->clone());
	}
	obj->configurationID = this->configurationID;
	obj->skeleton = this->skeleton ? utils::make_shared<Skeleton>(*this->skeleton) : SkeletonPtr();
	obj->lengthRatio = this->lengthRatio;
    obj->initialized = this->initialized;
    obj->rootPositions = this->rootPositions;

	return obj.release();
}

JointAnglesCollectionStream::JointAnglesCollectionStream() :
	lengthRatio(4.0), initialized(false)
{

}

JointAnglesCollectionStream::~JointAnglesCollectionStream()
{

}

void JointAnglesCollectionStream::setSkeletal(kinematic::SkeletonConstPtr skeletalModel,
	kinematic::SkeletalDataStreamPtr skeletalDataStream)
{
	//UTILS_ASSERT(!initialized);

	//this->haSkeleton = hAnimHumanoid::create();
	//this->haSkeleton->doSkeletonMapping(skeletalModel);
	//this->haSkeleton->createActiveHierarchy();

	//std::map<std::string, hAnimJointPtr>& joints = this->haSkeleton->getJoints();
	//std::for_each(joints.begin(), joints.end(), [&](std::pair<string, hAnimJointPtr> p) 
	//{   
	//	hAnimJointPtr joint = p.second;
	//	if (joint->isActive()) {

	//		double totalLength = 0.0;

	//		for(auto it = joint->getChildrenBones().begin(); it != joint->getChildrenBones().end(); ++it){
	//			auto bone = *it;
	//			
	//			bone->setLength(bone->getLength() / lengthRatio);

	//			totalLength += bone->getLength();
	//		}

	//		osg::Vec3 shift = joint->getDirection() * totalLength / joint->getChildrenBones().size();
	//		shift = SkeletonUtils::vectorRotation(shift, 
	//			osg::DegreesToRadians(-joint->getAxis()[0]),
	//			osg::DegreesToRadians(-joint->getAxis()[1]),
	//			osg::DegreesToRadians(-joint->getAxis()[2]));
	//		joint->setLocalShift(shift);			

	//		osg::Quat pc;
	//		hAnimJointPtr parent = joint->getActiveParent().lock();
	//		if (parent) {
	//			pc = SkeletonUtils::rotationParentChild(parent, joint);
	//		}
	//		joint->setChildParentRotation(pc);
	//	}
	//} 
	//);

	//createQuaternionRepresentation(skeletalModel, skeletalDataStream);
	//this->skeletalDataStream = skeletalDataStream;
	//initialized = true;
}
	


osg::Vec3 JointAnglesCollectionStream::getRootPosition() const
{
	kinematic::StreamSkeletonDataFrame data;
	skeletalDataStream->data(data);
	return data.rootPosition;
}

const std::vector<osg::Vec3> JointAnglesCollectionStream::getRotations() const
{
	kinematic::StreamSkeletonDataFrame data;
	skeletalDataStream->data(data);

	std::vector<osg::Vec3> ret;
	//ret.reserve(data.rotations.size());

	//for(auto it = data.rotations.begin(); it != data.rotations.end(); ++it){
	//	ret.push_back(SkeletonUtils::getEulerFromQuat(*it));
	//}

	return ret;
}


} //kinematic
