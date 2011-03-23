#include "stdafx.h"
#include <kinematiclib/DegreeOfFreedom.h>
#include <kinematiclib/Joint.h>
#include <kinematiclib/Skeleton.h>
#include <kinematiclib/SkeletalModel.h>

namespace kinematic {

void SkeletalModel::RecreateMaps() {
    jointsMap.clear();
    jointsIds.clear();
    Recreate(getSkeleton().getRoot());
}

void SkeletalModel::Recreate( JointPtr joint ) {
    static int id = 0;
    jointsMap[joint->name] = joint;
    jointsIds[id++] = joint;
    joint->id = id;
    for (int i = joint->children.size() - 1; i >= 0; --i) {
        Recreate(joint->children[i]);
    }
}

} //kinematic
