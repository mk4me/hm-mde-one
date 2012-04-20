#include "PCH.h"
#include <plugins/kinematic/skeletalVisualizationScheme.h>
#include "uniqueCollection.h"
#include "ISchemeDrawer.h"

using namespace std;
using namespace osg;
using namespace boost;
using namespace kinematic;

kinematic::VskParserPtr Vsk::Count16(new kinematic::VskParser());
kinematic::VskParserPtr Vsk::Count39(new kinematic::VskParser());
kinematic::VskParserPtr Vsk::Count53(new kinematic::VskParser());

VisualizationScheme::VisualizationScheme() :
    normalizedTime(0)
{
}

void VisualizationScheme::setNormalizedTime( double val )
{
    UTILS_ASSERT(hasData());
    if (normalizedTime != val) {
        normalizedTime = val;
        if (val < 0.0 || val > 1.0) {
            LOGGER(Logger::Debug, "SkeletalVisualizationScheme : value out of <0,1>");
        }
        update(val);
    }
}

osg::Quat rotationParentChild(JointPtr parent, JointPtr child)
{
    double mul  = osg::DegreesToRadians(1.0);
    osg::Matrix matParZ; matParZ.makeRotate(-parent->axis[2] * mul, 0.0, 0.0, 1.0);
    osg::Matrix matParY; matParY.makeRotate(-parent->axis[1] * mul, 0.0, 1.0, 0.0);
    osg::Matrix matParX; matParX.makeRotate(-parent->axis[0] * mul, 1.0, 0.0, 0.0);

    osg::Matrix matPar = matParZ * matParY * matParX;

    osg::Matrix matChiZ; matChiZ.makeRotate(child->axis[2] * mul, 0.0, 0.0, 1.0);
    osg::Matrix matChiY; matChiY.makeRotate(child->axis[1] * mul, 0.0, 1.0, 0.0);
    osg::Matrix matChiX; matChiX.makeRotate(child->axis[0] * mul, 1.0, 0.0, 0.0);

    osg::Matrix matChi = matChiX * matChiY * matChiZ;
    osg::Matrix resM = matChi * matPar ;
    Quat res; res.set(resM);
    return res;
}

osg::Vec3 vectorRotation(Vec3 v, double a, double b , double c)
{
    Quat rz; rz.makeRotate(c, 0,0,1);
    Quat ry; ry.makeRotate(b, 0,1,0);
    Quat rx; rx.makeRotate(a, 1,0,0);

    v = rz * v;
    v = ry * v;
    v = rx * v;
    return v;
}

void SkeletalVisualizationScheme::updateJointTransforms(const std::vector<osg::Quat>& rotations, hAnimJointPtr joint, Quat parentRot, Vec3 parentPos)
{
    // zapewnienie zgodnosci indeksow (miedzy tablicami connections i states)
    int i = visJoints[joint];
    Quat ident;
    double mul = osg::DegreesToRadians(1.0);
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
	// rewizja - hierarchia
    Vec4 color(1, 1, 1, 0.5f);
	BOOST_FOREACH(hAnimJointPtr child, joint->getActiveJointChildren()) {
		Connection c;
		c.index1 = visJoints[joint];
		c.index2 = visJoints[child];
		c.color = color;
		connections.push_back(c);
		createSkeletonConnections(child);
	}
}

void MarkersVisualizationScheme::updateMarkers( double normalizedTime )
{
	UTILS_ASSERT(markers);
    if (markers) {
        double time = normalizedTime * markers->getLength();
        currentPosition = getRootPosition(time);

        int count =  markers->getNumChannels();
        for (int i = 0; i < count; i++) {
            states[i].position = markers->getValue(i, time) - currentPosition;
        }
    }
}

void MarkersVisualizationScheme::setMarkersDataFromVsk( kinematic::VskParserConstPtr vsk )
{
    UTILS_ASSERT(markers, "There are no markers in kinematic model");

    std::map<std::string, int> markersIndices;
    for (int i = markers->getNumChannels() - 1; i >= 0; --i) {
        markersIndices[markers->getMarkerName(i)] = i;
    }
   
    auto vskMarkers = vsk->getMarkers();
    for (auto it = vskMarkers.first; it != vskMarkers.second; it++) {
        std::map<std::string, int>::iterator found = markersIndices.find(it->name);
        if (found != markersIndices.end()) {
            states[found->second].color = it->color;
        }
    }
    auto sticks = vsk->getSticks();
    connections.reserve(std::distance(sticks.first, sticks.second));
    for (auto it = sticks.first; it != sticks.second; it++) {
        Connection c;
        auto it1 = markersIndices.find(it->name1);
        auto it2 = markersIndices.find(it->name2);
        if (it1 != markersIndices.end() && it2 != markersIndices.end()) {
            c.index1 = markersIndices[it->name1];
            c.index2 = markersIndices[it->name2];
            c.color = it->color;
            connections.push_back(c);
        }
    }
}

void MarkersVisualizationScheme::setMarkers( MarkerCollectionConstPtr val )
{
	UTILS_ASSERT(!markers && val);
	this->markers = val;
    int count =  markers->getNumChannels();
	if (count && states.size() != count) {
		states.resize(count);
	}
	osg::Vec4 blue(0,0,1,1);
	for (int i = 0; i < count; i++) {
        states[i].color = blue;
        states[i].name = markers->getMarkerName(i);
	}

    updateMarkers(0.0);
}

osg::Vec3 MarkersVisualizationScheme::getRootPosition( double time ) const
{
    int count =  markers->getNumChannels();
    osg::Vec3 position;
    position.set(0.0, 0.0, 0.0);
    for (int i = 0; i < count; i++) {
        position += markers->getValue(i, time);
    }
    position *= (1.0f / count);
    return position;
}

void SkeletalVisualizationScheme::setJoints( JointAnglesCollectionConstPtr val )
{
	UTILS_ASSERT(val && val->getSkeletalData());
	this->joints = val;
	const auto& jointMap = joints->getHAnimSkeleton()->getJoints();
	int count = jointMap.size();
	
	auto it = jointMap.begin();
	for (int index = 0; it != jointMap.end(); it++) {
	    if (it->second->isActive()) {
	        visJoints[it->second] = index++;
	    }
	}
	
	states.resize(visJoints.size());
	osg::Vec4 gold(1,1,0,1);
	for (auto it = visJoints.begin(); it != visJoints.end(); it++) {
        states[it->second].color = gold;
        states[it->second].name = it->first->getName();
	}
	hAnimSkeletonPtr skeleton = joints->getHAnimSkeleton();
	createSkeletonConnections(skeleton->getRoot());
	updateJointTransforms(0.0);
}

