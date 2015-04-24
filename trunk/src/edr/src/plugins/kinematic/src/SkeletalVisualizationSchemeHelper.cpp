#include "PCH.h"
#include "SkeletalVisualizationSchemeHelper.h"

using namespace std;
using namespace osg;
using namespace boost;
using namespace kinematic;


void SkeletonJointsMapping::init(kinematic::SkeletonConstPtr skeleton,
	const std::vector<std::string> & indices)
{
	std::map<kinematic::JointPtr, unsigned int> locVisJoints;
	std::map<std::string, unsigned int> locNamedVisJoints;
	std::map<std::string,kinematic::JointPtr> missing;

	std::map<std::string, kinematic::JointPtr> jointMap = kinematic::Skeleton::getJoints(*skeleton);
	

	int index = -1;

	for(auto it = jointMap.begin(); it != jointMap.end(); ++it) {
		if(indices.empty() == false){
			auto idxIT = std::find(indices.begin(), indices.end(), it->first);

			if(idxIT != indices.end()){

				index = std::distance(indices.begin(), idxIT);
				locVisJoints[it->second] = index;
				locNamedVisJoints[it->first] = index;	

			} else {
				missing[it->first] = it->second;
				//throw core::runtime_error("Could not perform joints mapping");
			}
		} else {
			++index;
			locVisJoints[it->second] = index;
			locNamedVisJoints[it->first] = index;	
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

kinematic::SkeletonConstPtr SkeletonJointsMapping::skeleton() const
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

const int SkeletonJointsMapping::jointIndex(kinematic::JointPtr joint) const
{
	auto it = visJoints.find(joint);

	if(it != visJoints.end()){
		return it->second;
	}

	return -1;
}

const osgutils::SegmentsDescriptors SkeletonJointsMapping::generateMappedConnectionsDescription() const
{
	osgutils::SegmentsDescriptors cds;
	auto idx = jointIndex(skeleton_->root);
	generateMappedConnectionsDescription(skeleton_->root, idx == -1 ? 0 : idx, cds);

	return cds;
}

void SkeletonJointsMapping::generateMappedConnectionsDescription( kinematic::JointPtr joint,
	const unsigned int idx,
	osgutils::SegmentsDescriptors & cds) const
{
	auto jointChildren = joint->children;
	for(auto child : jointChildren) {
		
		auto idxB = jointIndex(child);

		if(idxB != -1){

			osgutils::SegmentDescriptor cd;

			cd.range.first = idx;
			cd.range.second = idxB;
			
			cd.length = (joint->value.position - child->value.position).length();

			cds.push_back(cd);
		}else{
			idxB = idx;
		}
		
		generateMappedConnectionsDescription(child, idxB, cds);		
	}
}

SkeletalVisualizationSchemeHelper::SkeletalVisualizationSchemeHelper(osgutils::IPointsSchemeDrawer * pointsSchemeDrawer,
	osgutils::IConnectionsSchemeDrawer * connectionsSchemeDrawer) : pointsSchemeDrawer(pointsSchemeDrawer),
	connectionsSchemeDrawer(connectionsSchemeDrawer)
{

}

SkeletalVisualizationSchemeHelper::~SkeletalVisualizationSchemeHelper()
{

}

void SkeletalVisualizationSchemeHelper::updateJointTransforms(const std::vector<osg::Quat> & rotations,
	JointPtr joint, const Quat & parentRot, const Vec3 & parentPos,
	std::vector<osg::Vec3> & pointsPositions)
{
    // zapewnienie zgodności indeksów (między tablicami connections i states)	
    int idx = jointsMapping->jointIndex(joint->value.name);
	Vec3 shift = parentPos - joint->value.position;
	Quat pc = joint->value.orientation;
    Quat rotation = idx < rotations.size() ? rotations[idx] * pc  * parentRot : pc * parentRot;
	shift = rotation * shift;

    auto pos = parentPos + shift;

	pointsPositions[idx] = pos;

	
	for(auto child : joint->children) {
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

	updateJointTransforms(rotations, jointsMapping->skeleton()->root, q, rootPosition, pointsPositions);	
}

void SkeletalVisualizationSchemeHelper::init(const SkeletonJointsMapping * jointsMapping)
{	
	this->jointsMapping = jointsMapping;
	pointsSchemeDrawer->init(jointsMapping->mappedJointsNumber());
	connectionsSchemeDrawer->init(jointsMapping->generateMappedConnectionsDescription());
}

void SkeletalVisualizationSchemeHelper::init(const SkeletonJointsMapping * jointsMapping,
											 const osgutils::SegmentsDescriptors & connections)
{
	this->jointsMapping = jointsMapping;
	pointsSchemeDrawer->init(jointsMapping->mappedJointsNumber());
	connectionsSchemeDrawer->init(connections);
}
