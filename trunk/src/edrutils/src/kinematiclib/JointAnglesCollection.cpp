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
    //preferedConnectionColor(1, 1, 1, 0.5f),
    //preferedDotColor(1,1,0,1)
{
}


void JointAnglesCollection::setSkeletal( kinematic::SkeletalModelConstPtr skeletalModel, kinematic::SkeletalDataConstPtr skeletalData )
{
    UTILS_ASSERT(!initialized);
    //UTILS_ASSERT(!this->skeletalModel && !this->skeletalData);
    //this->skeletalModel = skeletalModel;
	//this->skeletalData = skeletalData;
    
   const std::vector<SkeletalData::singleFramePtr>& frm = skeletalData->getFrames();
   this->haSkeleton = hAnimSkeleton::create();
   this->haSkeleton->doSkeletonMapping(skeletalModel);
   this->haSkeleton->createActiveHierarchy();

   std::map<std::string, hAnimJointPtr>& joints = this->haSkeleton->getJoints();
   std::for_each(joints.begin(), joints.end(), [&](std::pair<string, hAnimJointPtr> p) 
    {   
        hAnimJointPtr joint = p.second;
		if (joint->isActive()) {
			joint->setLength(joint->getLength() / lengthRatio);
                    
			osg::Vec3 shift = joint->getDirection() * joint->getLength();
			shift = vectorRotation(shift, 
				osg::DegreesToRadians(-joint->getAxis()[0]),
				osg::DegreesToRadians(-joint->getAxis()[1]),
				osg::DegreesToRadians(-joint->getAxis()[2]));
			joint->setLocalShift(shift);

			osg::Quat pc;
			hAnimJointPtr parent = joint->getActiveParent().lock();
			if (parent) {
				pc = rotationParentChild(parent, joint);
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

double JointAnglesCollection::getMaxBoneLength(const Skeleton& skeleton) const 
{
    JointConstPtr root = skeleton.getRoot();
    return getMaxLength(root, -1.0);
}

double JointAnglesCollection::getMaxLength(const JointConstPtr & joint, double maxLength) const
{
    maxLength = std::max(maxLength, joint->length);
    for (int i = joint->children.size() - 1; i >= 0; --i) {
        maxLength = std::max(maxLength, getMaxLength(joint->children[i], maxLength));
    }
    return maxLength;
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
			channelsMap[name]->addPoint(createRotation(mX, mY, mZ, joint->getOrder()));
        }
    }
}



// rewizja tworzenie skeletal data
//SkeletalModelPtr JointAnglesCollection::createSkeletalData() const 
//{
//    SkeletalModelPtr model(new SkeletalModel);
//    Skeleton& skeleton = model->getSkeleton();
//	
//    JointPtr newRoot = cloneRootWithActivated(this->haSkeleton->getRoot());
//    skeleton.setRoot(newRoot);
//
//    model->setName("HumanModel");
//    UTILS_ASSERT(false, "testowo zmienila się struktura ramek w parserach!");
//    //model->getFrames() = frames;
//    model->RecreateMaps();
//    return model;
//}

//rewizja
//JointPtr JointAnglesCollection::cloneRootWithActivated(const JointPtr origin ) const
//{
//    return Joint::clone(origin);
//}

//void JointAnglesCollection::changeNameInFrames(const std::string& oldName, const std::string& newName)
//{
//    int index = -1;
//    for (int i = frames[0]->jointsData.size() - 1; i >= 0; --i) {
//        if (frames[0]->jointsData[i].name == oldName) {
//            index = i;
//            break;
//        }
//    }
//    if (index == -1) {
//        return;
//    }
//    for (int i = frames.size() -1; i >= 0; --i) {
//        frames[i]->jointsData[index].name = newName;
//    }
//}


osg::Vec3 JointAnglesCollection::vectorRotation( osg::Vec3 v, double rX, double rY, double rZ )
{
    osg::Quat rotz; rotz.makeRotate(rZ, 0,0,1);
    osg::Quat roty; roty.makeRotate(rY, 0,1,0);
    osg::Quat rotx; rotx.makeRotate(rX, 1,0,0);
    v = rotz * v;
    v = roty * v;
    v = rotx * v;
    return v;
}

osg::Quat JointAnglesCollection::rotationParentChild(hAnimJointPtr parent, hAnimJointPtr child)
{
    double mul  = osg::DegreesToRadians(1.0);
    osg::Matrix matParZ; matParZ.makeRotate(-parent->getAxis()[2] * mul, 0.0, 0.0, 1.0);
    osg::Matrix matParY; matParY.makeRotate(-parent->getAxis()[1] * mul, 0.0, 1.0, 0.0);
    osg::Matrix matParX; matParX.makeRotate(-parent->getAxis()[0] * mul, 1.0, 0.0, 0.0);

    osg::Matrix matPar = matParZ * matParY * matParX;
    osg::Matrix matChiZ; matChiZ.makeRotate(child->getAxis()[2] * mul, 0.0, 0.0, 1.0);
    osg::Matrix matChiY; matChiY.makeRotate(child->getAxis()[1] * mul, 0.0, 1.0, 0.0);
    osg::Matrix matChiX; matChiX.makeRotate(child->getAxis()[0] * mul, 1.0, 0.0, 0.0);

    osg::Matrix matChi = matChiX * matChiY * matChiZ;
    osg::Matrix resM = matChi * matPar ;
    osg::Quat res; 
    res.set(resM);
    return res;
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

osg::Quat JointAnglesCollection::createRotation( const osg::Quat& rX, const osg::Quat& rY, const osg::Quat& rZ, Axis::Order order )
{
    switch (order) 
    {
    case Axis::XYZ: return rX * rY * rZ;
    case Axis::XZY: return rX * rZ * rY;
    case Axis::YXZ: return rY * rX * rZ;
    case Axis::YZX: return rY * rZ * rX;
    case Axis::ZXY: return rZ * rX * rY;
    case Axis::ZYX: return rZ * rY * rX;
    default:
        UTILS_ASSERT(false);
        osg::Quat q;
        return q;
    }
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
	JointAnglesCollection* obj = new JointAnglesCollection();
	int count = static_cast<int>(channels.size());
	obj->channels.resize(count);
	for (int i = 0; i < count; ++i) {
		obj->channels[i] = JointAngleChannelPtr(this->channels[i]->clone());
	}
	obj->configurationID = this->configurationID;
	obj->haSkeleton = hAnimSkeletonPtr(this->haSkeleton->clone());
	obj->lengthRatio = this->lengthRatio;
    //obj->preferedConnectionColor = this->preferedConnectionColor;
    //obj->preferedDotColor = this->preferedDotColor;
	//obj->skeletalData = SkeletalDataPtr(this->skeletalData->clone());
	//obj->skeletalModel = SkeletalModelPtr(this->skeletalModel->clone());
	return obj;
}

} //kinematic
