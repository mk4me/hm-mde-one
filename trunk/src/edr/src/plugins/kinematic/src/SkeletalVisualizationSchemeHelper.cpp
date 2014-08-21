#include "PCH.h"
#include "SkeletalVisualizationSchemeHelper.h"

using namespace std;
using namespace osg;
using namespace boost;
using namespace kinematic;

void SkeletonJointsMapping::init(kinematic::hAnimSkeletonPtr skeleton,
	const std::vector<std::string> & indices)
{
	std::map<kinematic::hAnimJointPtr, unsigned int> locVisJoints;
	std::map<std::string, unsigned int> locNamedVisJoints;
	std::map<std::string,kinematic::hAnimJointPtr> missing;

	const auto& jointMap = skeleton->getJoints();

	int index = -1;

	for(auto it = jointMap.begin(); it != jointMap.end(); ++it) {
		if (it->second->isActive()) {

			if(indices.empty() == false){
				auto idxIT = std::find(indices.begin(), indices.end(), it->first);

				if(idxIT != indices.end()){

					index = std::distance(indices.begin(), idxIT);
					locVisJoints[it->second] = index;
					locNamedVisJoints[it->first] = index;	

				}else{

					missing[it->first] = it->second;
					//throw std::runtime_error("Could not perform joints mapping");
				}
			}else{
				++index;
				locVisJoints[it->second] = index;
				locNamedVisJoints[it->first] = index;	
			}		
		}
	}

	if(missing.empty() == false){
		index = indices.size();
		for(auto it = missing.begin(); it != missing.end(); ++it){			
			locVisJoints[it->second] = index;
			locNamedVisJoints[it->first] = index;
			++index;
		}
	}	

	namedVisJoints.swap(locNamedVisJoints);
	visJoints.swap(locVisJoints);
	skeleton_ = skeleton;
}

kinematic::hAnimSkeletonPtr SkeletonJointsMapping::skeleton() const
{
	return skeleton_;
}

const unsigned int SkeletonJointsMapping::mappedJointsNumber() const
{
	return visJoints.size();
}

const int SkeletonJointsMapping::jointIndex(const std::string & jointName) const
{	
	auto it = namedVisJoints.find(jointName);

	if(it != namedVisJoints.end()){
		return it->second;
	}

	return -1;
}

const int SkeletonJointsMapping::jointIndex(kinematic::hAnimJointPtr joint) const
{
	auto it = visJoints.find(joint);

	if(it != visJoints.end()){
		return it->second;
	}

	return -1;
}

const SegmentsDescriptors SkeletonJointsMapping::generateMappedConnectionsDescription() const
{
	SegmentsDescriptors cds;
	auto idx = jointIndex(skeleton_->getRoot());
	generateMappedConnectionsDescription(skeleton_->getRoot(), idx == -1 ? 0 : idx, cds);

	return cds;
}

void SkeletonJointsMapping::generateMappedConnectionsDescription( kinematic::hAnimJointPtr joint,
	const unsigned int idx,
	SegmentsDescriptors & cds) const
{
	auto jointChildren = joint->getActiveJointChildren();
	BOOST_FOREACH(hAnimJointPtr child, jointChildren) {
		
		auto idxB = jointIndex(child);

		if(idxB != -1){

			SegmentDescriptor cd;

			cd.range.first = idx;
			cd.range.second = idxB;
			//TODO - wyznaczyc odleglosci czy to wystarczy?

			auto cb = child->getChildrenBones();

			cd.length = 0.0;

			for(auto it = cb.begin(); it != cb.end(); ++it){
				cd.length += (*it)->getLength();
			}

			cd.length /= cb.size();

			cds.push_back(cd);
		}else{
			idxB = idx;
		}
		
		generateMappedConnectionsDescription(child, idxB, cds);		
	}
}

SkeletalVisualizationSchemeHelper::SkeletalVisualizationSchemeHelper(IPointsSchemeDrawer * pointsSchemeDrawer,
	IConnectionsSchemeDrawer * connectionsSchemeDrawer) : pointsSchemeDrawer(pointsSchemeDrawer),
	connectionsSchemeDrawer(connectionsSchemeDrawer)
{

}

SkeletalVisualizationSchemeHelper::~SkeletalVisualizationSchemeHelper()
{

}

void SkeletalVisualizationSchemeHelper::updateJointTransforms(const std::vector<osg::Quat> & rotations,
	hAnimJointPtr joint, const Quat & parentRot, const Vec3 & parentPos,
	std::vector<osg::Vec3> & pointsPositions)
{
    // zapewnienie zgodności indeksów (między tablicami connections i states)	
    int idx = jointsMapping->jointIndex(joint->getName());
    Vec3 shift = joint->getLocalShift();
    Quat pc = joint->getChildParentRotation();	    
    Quat rotation = idx < rotations.size() ? rotations[idx] * pc  * parentRot : pc * parentRot;

    shift = rotation * shift;

	auto pos = parentPos + shift;

	pointsPositions[idx] = pos;

	BOOST_FOREACH(hAnimJointPtr child, joint->getActiveJointChildren()) {
		updateJointTransforms(rotations, child, rotation, pos, pointsPositions);
	}
}

void SkeletalVisualizationSchemeHelper::updateJointTransforms(const osg::Vec3 & rootPosition,
	const std::vector<osg::Quat>& rotations)
{	
	std::vector<osg::Vec3> pointsPositions(jointsMapping->mappedJointsNumber());	
	
	innerCalculatePointsPositions(pointsPositions, rootPosition, rotations);    

	updateJointTransforms(pointsPositions);
}

void SkeletalVisualizationSchemeHelper::updateJointTransforms(const std::vector<osg::Vec3> & pointsPositions)
{
	pointsSchemeDrawer->update(pointsPositions);
	connectionsSchemeDrawer->update(pointsPositions);
}

void SkeletalVisualizationSchemeHelper::calculatePointsPositions(std::vector<osg::Vec3> & pointsPositions,
	const osg::Vec3 & rootPosition,
	const std::vector<osg::Quat> & rotations)
{
	std::vector<osg::Vec3> locPointsPositions(jointsMapping->mappedJointsNumber());

	innerCalculatePointsPositions(locPointsPositions, rootPosition, rotations);

	std::swap(pointsPositions, locPointsPositions);
}

void SkeletalVisualizationSchemeHelper::innerCalculatePointsPositions(std::vector<osg::Vec3> & pointsPositions,
	const osg::Vec3 & rootPosition,
	const std::vector<osg::Quat> & rotations)
{
	osg::Quat q;

	updateJointTransforms(rotations, jointsMapping->skeleton()->getRoot(), q, rootPosition, pointsPositions);	
}

void SkeletalVisualizationSchemeHelper::init(const SkeletonJointsMapping * jointsMapping)
{	
	this->jointsMapping = jointsMapping;
	pointsSchemeDrawer->init(jointsMapping->mappedJointsNumber());
	connectionsSchemeDrawer->init(jointsMapping->generateMappedConnectionsDescription());
}

void SkeletalVisualizationSchemeHelper::init(const SkeletonJointsMapping * jointsMapping,
	const SegmentsDescriptors & connections)
{
	this->jointsMapping = jointsMapping;
	pointsSchemeDrawer->init(jointsMapping->mappedJointsNumber());
	connectionsSchemeDrawer->init(connections);
}
