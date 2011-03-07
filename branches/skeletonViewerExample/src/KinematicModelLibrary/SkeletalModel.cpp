#include "stdafx.h"
#include <KinematicModelLibrary/DegreeOfFreedom.h>
#include <KinematicModelLibrary/Joint.h>
#include <KinematicModelLibrary/Skeleton.h>
#include <KinematicModelLibrary/SkeletalModel.h>

void kinematic::SkeletalModel::RecreateMaps() {
    jointsMap.clear();
    jointsIds.clear();
    Recreate(getSkeleton().getRoot());
}

void kinematic::SkeletalModel::Recreate( JointPtr joint ) {
    static int id = 0;
    jointsMap[joint->name] = joint;
    jointsIds[id++] = joint;
    joint->id = id;
    for (int i = joint->children.size() - 1; i >= 0; --i) {
        Recreate(joint->children[i]);
    }
}
