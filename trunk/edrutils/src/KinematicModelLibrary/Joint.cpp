#include <KinematicModelLibrary/stdafx.h>
#include <KinematicModelLibrary/DegreeOfFreedom.h>
#include <KinematicModelLibrary/Joint.h>

using namespace std;
using namespace hmAnimation;

void Joint::copyContent(Joint::Ptr source, Joint::Ptr destination) {
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

Joint::Ptr Joint::clone(Joint::Ptr bone) {
    Joint::Ptr null;
    return clone(bone, null);
}

Joint::Ptr Joint::clone(Joint::Ptr bone, Joint::Ptr clonedParent) {
    Joint::Ptr newBone(new Joint);
    copyContent(bone, newBone);
    newBone->parent = clonedParent;
    int count = bone->children.size();
    for (int i = 0; i < count; i++) {
        newBone->children.push_back(clone(bone->children[i], newBone));
    }
    return newBone;
}