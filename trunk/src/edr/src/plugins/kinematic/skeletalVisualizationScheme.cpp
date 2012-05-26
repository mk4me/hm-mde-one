#include "PCH.h"
#include "skeletalVisualizationScheme.h"
#include "uniqueCollection.h"
#include "ISchemeDrawer.h"

using namespace std;
using namespace osg;
using namespace boost;
using namespace kinematic;

SkeletalVisualizationScheme::SkeletalVisualizationScheme() : 
    counterHelper(-1),
    dotColor(1,1,0,1),
    connectionColor(1, 1, 1, 0.5f)
{
}



void SkeletalVisualizationScheme::updateJointTransforms(const std::vector<osg::Quat>& rotations, hAnimJointPtr joint, Quat parentRot, Vec3 parentPos)
{
    // zapewnienie zgodnosci indeksow (miedzy tablicami connections i states)
    int i = visJoints[joint];
    Quat ident;
    Vec3 shift = joint->getLocalShift();
       
    Quat pc = joint->getChildParentRotation();
	int index = joints->tryGetIndex(joint->getName());
    //auto it = rotations.find(joint->name);
    Quat rotation = index >= 0 ? rotations[index] * pc  * parentRot : pc * parentRot;

    shift = rotation * shift;

    states[i].position = (parentPos + shift);

	BOOST_FOREACH(hAnimJointPtr child, joint->getActiveJointChildren()) {
		updateJointTransforms(rotations, child, rotation, parentPos + shift);
	}
}

void SkeletalVisualizationScheme::updateJointTransforms( double normalizedTime )
{
    UTILS_ASSERT(joints);
    counterHelper = 0;
    
    hAnimSkeletonPtr skeleton = joints->getHAnimSkeleton();
    currentPosition = joints->getRootPosition(normalizedTime);
    updateJointTransforms(joints->getValues(static_cast<float>(normalizedTime * joints->getLength())), skeleton->getRoot(), osg::Quat(), osg::Vec3());
 }

void SkeletalVisualizationScheme::createSkeletonConnections(kinematic::hAnimJointPtr joint)
{ 
	BOOST_FOREACH(hAnimJointPtr child, joint->getActiveJointChildren()) {
		Connection c;
		c.index1 = visJoints[joint];
		c.index2 = visJoints[child];
		c.color = connectionColor;
		connections.push_back(c);
		createSkeletonConnections(child);
	}
}

void SkeletalVisualizationScheme::setJoints( JointAnglesCollectionConstPtr val )
{
	UTILS_ASSERT(val && val->getHAnimSkeleton());
	this->joints = val;
	const auto& jointMap = joints->getHAnimSkeleton()->getJoints();
	
	auto it = jointMap.begin();
	for (int index = 0; it != jointMap.end(); it++) {
	    if (it->second->isActive()) {
	        visJoints[it->second] = index++;
	    }
	}
	
	states.resize(visJoints.size());
	for (auto it = visJoints.begin(); it != visJoints.end(); it++) {
        states[it->second].color = dotColor;
        states[it->second].name = it->first->getName();
	}
	hAnimSkeletonPtr skeleton = joints->getHAnimSkeleton();
	createSkeletonConnections(skeleton->getRoot());
	updateJointTransforms(0.0);
}

void SkeletalVisualizationScheme::update( double time )
{
    updateJointTransforms(time);
    currentPosition = getRootPosition(time);
}

osg::Vec3 SkeletalVisualizationScheme::getRootPosition( double time )
{
    return joints->getRootPosition(time);
}

double SkeletalVisualizationScheme::getDuration() const
{ 
    if (joints) {
        return static_cast<double>(joints->getLength()); 
    } 
    UTILS_ASSERT(false);
    return 0.0;
}

double SkeletalVisualizationScheme::getFrameTime() const
{
    UTILS_ASSERT(joints); 
    return static_cast<double>(joints->getLength()) / joints->getNumPointsPerChannel(); 
}

int SkeletalVisualizationScheme::getNumFrames() const
{ 
    UTILS_ASSERT(joints);  
    return joints->getNumPointsPerChannel(); 
}