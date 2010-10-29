#ifndef SKELETON_H
#define SKELETON_H

#include "Bone.h"

#define MAX_BONES_IN_ASF_FILE 256

// Bone segment names used in ASF file
static int root = 0;

class Skeleton
{
    //Member functions
public: 

    Skeleton() {};
    ~Skeleton();                                
     
private:

    //Member Variables
public:
    // root position in world coordinate system
    float m_RootPos[3];
    int tx,ty,tz;
    int rx,ry,rz;


    int NUM_BONES_IN_ASF_FILE;
    int MOV_BONES_IN_ASF_FILE;
//private:
    Bone *m_pRootBone;							// Pointer to the root bone, m_RootBone = &bone[0]
	std::vector<Bone*> m_pBoneList;				// Array with all skeleton bones
};

#endif 