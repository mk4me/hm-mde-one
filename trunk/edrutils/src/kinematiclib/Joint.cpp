#include "stdafx.h"
#include <kinematiclib/DegreeOfFreedom.h>
#include <kinematiclib/Joint.h>

using namespace std;
using namespace kinematic;

void Joint::copyContent(JointPtr source, JointPtr destination) {
    destination->id = source->id;
    destination->name = source->name;
    destination->direction = source->direction;
    destination->length = source->length;
    destination->axis = source->axis;
    destination->order = source->order;
    destination->dofs = source->dofs;
    destination->bodymass = source->bodymass;
    destination->cofmass = source->cofmass;
}

JointPtr Joint::clone(JointPtr bone) {
    JointPtr null;
    return clone(bone, null);
}

JointPtr Joint::clone(JointPtr bone, JointPtr clonedParent) {
    JointPtr newBone(new Joint);
    copyContent(bone, newBone);
    newBone->parent = clonedParent;
    int count = bone->children.size();
    for (int i = 0; i < count; i++) {
        newBone->children.push_back(clone(bone->children[i], newBone));
    }
    return newBone;
}
