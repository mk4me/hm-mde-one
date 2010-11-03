#ifndef BONE_H
#define BONE_H

#include "Frame.h"
#include <vector>
#include <osg/Matrixd>

// This holds the names of a joint's children
typedef std::vector<char*> childVector;

//class Bone
struct Bone
{
    Bone(){ matrix = new osg::Matrixd(); }
    ~Bone() { delete matrix; }
	int idx;					// Bone inde

	char *name;
    Bone *parent;		// Pointer to the sibling (branch bone) in the hierarchy tree 
	std::vector<Bone *> child;			// Pointer to the child (outboard bone) in the hierarchy tree 
	std::vector<Frame*> frame;			// Animacja TODO: zrobiæ pointera na jak¹s liste przechowuj¹c¹ te ramki - dzieki temu istnieje szybka podmiana animacji
	childVector			children;			// STL vector of char* to children names
	char *parentName;

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

    // rotation matrix from the local coordinate of this bone to the local coordinate system of it's parent
    osg::Matrixd *matrix;			

    //Rotation angles for this bone at a particular time frame (as read from AMC file) in local coordinate system, 
    // this is position what we get "po" loading matrix;
    float positionx,positiony,positionz;

	osg::Vec3d initialPosition;
	bool isInitialPosition;
};

#endif