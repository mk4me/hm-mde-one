#ifndef BONE_H
#define BONE_H

//class Bone
struct Bone
{
    struct Bone *sibling;		// Pointer to the sibling (branch bone) in the hierarchy tree 
    struct Bone *child;			// Pointer to the child (outboard bone) in the hierarchy tree 

    int idx;					// Bone index

    float dir[3];				// Unit vector describes the direction from local origin to 
    // the origin of the child bone 
    // Notice: stored in local coordinate system of the bone

    float length;				// Bone length  

    float axis_x, axis_y, axis_z;// orientation of each bone's local coordinate 
    //system as specified in ASF file (axis field)

    float aspx, aspy;			// aspect ratio of bone shape

    int dof;					// number of bone's degrees of freedom 
    int dofx, dofy, dofz;		// degree of freedom mask in x, y, z axis (local)
    int doftx, dofty, doftz;
    int doftl;
    // dofx=1 if this bone has x degree of freedom, otherwise dofx=0.

    char name[256];
    // rotation matrix from the local coordinate of this bone to the local coordinate system of it's parent
    double matrix[4][4];			

    //Rotation angles for this bone at a particular time frame (as read from AMC file) in local coordinate system, 
    // this is position what we get "po" loading matrix;
    float positionx,positiony,positionz;
};

#endif