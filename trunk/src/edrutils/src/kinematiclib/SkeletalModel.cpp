#include "stdafx.h"
#include <kinematiclib/SkeletalModel.h>

namespace kinematic {
SkeletalModel* SkeletalModel::clone() const
{
	SkeletalModel* clone = new SkeletalModel();
	clone->version = this->version;        
	clone->name = this->name;           
	clone->documentation = this->documentation;  
	clone->length = this->length;         
	clone->mass = this->mass;           
	clone->angle = this->angle;
	clone->skeleton = SkeletonPtr(this->skeleton->clone());

	SkeletalModel::createMaps(clone->skeleton->getRoot(), clone->jointsMap, clone->jointsIds);
	return clone;
}

JointPtr SkeletalModel::getJointByName( const std::string& name )
{
	JointMap::iterator it = jointsMap.find(name);
	JointPtr nullJoint;
	return it != jointsMap.end() ? it->second : nullJoint;
}

JointConstPtr SkeletalModel::getJointByName( const std::string& name ) const
{
	JointMap::const_iterator it = jointsMap.find(name);
	JointPtr nullJoint;
	return it != jointsMap.end() ? it->second : nullJoint;
}

void SkeletalModel::createMaps( JointPtr root, JointMap& jointMap, JointIdMap& jointIDMap )
{
	jointMap[root->name] = root;
	jointIDMap[root->id] = root;
	for (auto it = root->children.begin(); it != root->children.end(); it++) {
		createMaps(*it, jointMap, jointIDMap);
	}
}

}
