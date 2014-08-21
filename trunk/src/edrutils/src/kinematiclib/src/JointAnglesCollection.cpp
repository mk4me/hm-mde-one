#include "stdafx.h"
#include <algorithm>
#include <osg/Quat>
#include <osg/Vec3>
#include <kinematiclib/SkeletalParsers.h>
#include <kinematiclib/JointAnglesCollection.h>

using namespace std;

namespace kinematic {

JointAnglesCollection::JointAnglesCollection(void) :
    lengthRatio(4.0),
    initialized(false)
{
}

void JointAnglesCollection::setSkeletal(kinematic::SkeletalModelConstPtr skeletalModel, kinematic::SkeletalDataConstPtr skeletalData, SkeletonMappingSchemePtr mapping)
{
    UTILS_ASSERT(!initialized);
    
   this->haSkeleton = hAnimSkeleton::create();
   this->haSkeleton->registerMappingScheme(mapping);
   this->haSkeleton->doSkeletonMapping(skeletalModel);
   this->haSkeleton->createActiveHierarchy();

   std::map<std::string, hAnimJointPtr>& joints = this->haSkeleton->getJoints();
   std::for_each(joints.begin(), joints.end(), [&](std::pair<string, hAnimJointPtr> p) 
    {   
        hAnimJointPtr joint = p.second;
		if (joint->isActive()) {

			double totalLength = 0.0;

			for(auto it = joint->getChildrenBones().begin(); it != joint->getChildrenBones().end(); ++it){

				auto bone = *it;

				bone->setLength(bone->getLength() / lengthRatio);

				totalLength += bone->getLength();
			}
                    
			osg::Vec3 shift = joint->getDirection() * totalLength / joint->getChildrenBones().size();
			shift = SkeletonUtils::vectorRotation(shift, 
				osg::DegreesToRadians(-joint->getAxis()[0]),
				osg::DegreesToRadians(-joint->getAxis()[1]),
				osg::DegreesToRadians(-joint->getAxis()[2]));
			joint->setLocalShift(shift);			

			osg::Quat pc;
			hAnimJointPtr parent = joint->getActiveParent().lock();
			if (parent) {
				pc = SkeletonUtils::rotationParentChild(parent, joint);
			}
			joint->setChildParentRotation(pc);
		}
    } 
   );
   
   createQuaternionRepresentation(skeletalModel, skeletalData);
   initialized = true;
}

void JointAnglesCollection::setSkeletal( kinematic::hAnimSkeletonPtr skeletalModel, const std::vector<osg::Vec3>& rootPositions, const Collection& channels )
{
    UTILS_ASSERT(!initialized);
    this->haSkeleton = skeletalModel;
    this->rootPositions = rootPositions;
    this->channels = channels;
    initialized = true;
}

void JointAnglesCollection::createQuaternionRepresentation(kinematic::SkeletalModelConstPtr& skeletalModel, kinematic::SkeletalDataConstPtr & data )
{
    UTILS_ASSERT(lengthRatio > 0.0);
	const std::vector<SkeletalData::singleFramePtr>& frames = data->getFrames();
    int framesNo = frames.size();
    if (framesNo == 0) {
        throw kinematic::KinematicModelException("No frames loaded");
    }

	vector<SkeletalData::singleJointStatePtr>& jointsData = frames[0]->jointsData;
	std::map<std::string, JointAngleChannelPtr> channelsMap;

	int fps = static_cast<int> (1.0 / data->getFrameTime());

	for (unsigned int i = 0; i < jointsData.size(); ++i) {
		JointAngleChannelPtr channel(new JointAngleChannel(static_cast<float>(fps))); 
		std::string name = haSkeleton->mapJointName(jointsData[i]->name);
		channel->setName(name);
		channelsMap[name] = channel;
		addChannel(channel);
	}

    this->rootPositions.resize(framesNo);
        
    int bdataSize = jointsData.size();

    for (int i = 0; i < framesNo; ++i) {
        if (frames[i]->jointsData.size() != bdataSize) {
            throw(KinematicModelException("Wrong data format"));
        }
    }

    for (int i = 0; i < framesNo; ++i) {
        int jointStatesNo = frames[i]->jointsData.size();
        for (int j = 0; j < jointStatesNo; ++j) {
            double rx, ry, rz;
            int index;
            std::string name = haSkeleton->mapJointName(frames[i]->jointsData[j]->name);
            hAnimJointPtr joint = haSkeleton->getJoints()[name];
            if (!joint) {
                kinematic::WrongFileException("Joint " + name + " not found");
            }
            SkeletalData::singleJointStatePtr jointState = frames[i]->jointsData[j];
            index = DegreeOfFreedom::getChannelIndex(DegreeOfFreedom::RZ, joint->getDofs());
            rz = (index != -1 ? jointState->channelValues[index] : 0.0);
            index = DegreeOfFreedom::getChannelIndex(DegreeOfFreedom::RX, joint->getDofs());
            rx = (index != -1 ? jointState->channelValues[index] : 0.0);
            index = DegreeOfFreedom::getChannelIndex(DegreeOfFreedom::RY, joint->getDofs());
            ry = (index != -1 ? jointState->channelValues[index] : 0.0);
            double mul = skeletalModel->getAngle() == SkeletalModel::radians ? 1.0 : osg::DegreesToRadians(1.0);
            
            osg::Quat mX; mX.makeRotate(mul * (rx), osg::Vec3(1,0,0));
            osg::Quat mY; mY.makeRotate(mul * (ry), osg::Vec3(0,1,0));
            osg::Quat mZ; mZ.makeRotate(mul * (rz), osg::Vec3(0,0,1));
            
            if (name == "HumanoidRoot" || name == "root") {
                index = DegreeOfFreedom::getChannelIndex(DegreeOfFreedom::TX, joint->getDofs());
                rootPositions[i][0] = (index != -1 ? jointState->channelValues[index] : 0.0);
                index = DegreeOfFreedom::getChannelIndex(DegreeOfFreedom::TY, joint->getDofs());
                rootPositions[i][1] = (index != -1 ? jointState->channelValues[index] : 0.0);
                index = DegreeOfFreedom::getChannelIndex(DegreeOfFreedom::TZ, joint->getDofs());
                rootPositions[i][2] = (index != -1 ? jointState->channelValues[index] : 0.0);
                rootPositions[i] /= lengthRatio;
			} 
			channelsMap[name]->addPoint(SkeletonUtils::createRotation(mX, mY, mZ, joint->getOrder()));
        }
    }
}

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
		throw kinematic::UnableToMapJointException(info);
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
	std::auto_ptr<JointAnglesCollection> obj(new JointAnglesCollection());
	int count = static_cast<int>(channels.size());
	obj->channels.resize(count);
	for (int i = 0; i < count; ++i) {
		obj->channels[i] = JointAngleChannelPtr(this->channels[i]->clone());
	}
	obj->configurationID = this->configurationID;
	obj->haSkeleton = this->haSkeleton ? hAnimSkeletonPtr(this->haSkeleton->clone()) : hAnimSkeletonPtr();
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

void JointAnglesCollectionStream::setSkeletal(kinematic::SkeletalModelConstPtr skeletalModel,
	kinematic::SkeletalDataStreamPtr skeletalDataStream)
{
	UTILS_ASSERT(!initialized);

	this->haSkeleton = hAnimSkeleton::create();
	this->haSkeleton->doSkeletonMapping(skeletalModel);
	this->haSkeleton->createActiveHierarchy();

	std::map<std::string, hAnimJointPtr>& joints = this->haSkeleton->getJoints();
	std::for_each(joints.begin(), joints.end(), [&](std::pair<string, hAnimJointPtr> p) 
	{   
		hAnimJointPtr joint = p.second;
		if (joint->isActive()) {

			double totalLength = 0.0;

			for(auto it = joint->getChildrenBones().begin(); it != joint->getChildrenBones().end(); ++it){
				auto bone = *it;
				
				bone->setLength(bone->getLength() / lengthRatio);

				totalLength += bone->getLength();
			}

			osg::Vec3 shift = joint->getDirection() * totalLength / joint->getChildrenBones().size();
			shift = SkeletonUtils::vectorRotation(shift, 
				osg::DegreesToRadians(-joint->getAxis()[0]),
				osg::DegreesToRadians(-joint->getAxis()[1]),
				osg::DegreesToRadians(-joint->getAxis()[2]));
			joint->setLocalShift(shift);			

			osg::Quat pc;
			hAnimJointPtr parent = joint->getActiveParent().lock();
			if (parent) {
				pc = SkeletonUtils::rotationParentChild(parent, joint);
			}
			joint->setChildParentRotation(pc);
		}
	} 
	);

	createQuaternionRepresentation(skeletalModel, skeletalDataStream);
	this->skeletalDataStream = skeletalDataStream;
	initialized = true;
}
	
void JointAnglesCollectionStream::setSkeletal(kinematic::hAnimSkeletonPtr skeletalModel,
	kinematic::SkeletalDataStreamPtr skeletalDataStream)
{
	this->haSkeleton = skeletalModel;
	this->skeletalDataStream = skeletalDataStream;
	initialized = true;
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
	ret.reserve(data.rotations.size());

	for(auto it = data.rotations.begin(); it != data.rotations.end(); ++it){
		ret.push_back(SkeletonUtils::getEulerFromQuat(*it));
	}

	return ret;
}

void JointAnglesCollectionStream::createQuaternionRepresentation(kinematic::SkeletalModelConstPtr& skeletalModel,
	kinematic::SkeletalDataStreamPtr skeletalDataStream)
{

}

} //kinematic
