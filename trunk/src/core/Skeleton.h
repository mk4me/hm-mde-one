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

    Skeleton();
    ~Skeleton();                                

    //Get root node's address; for accessing bone data
    Bone* getRoot();
     
private:
    //This recursive function traverces skeleton hierarchy 
    //and returns a pointer to the bone with index - bIndex
    //ptr should be a pointer to the root node 
    //when this function first called
    Bone* getBone(Bone *ptr, int bIndex);

    //This function sets sibling or child for parent bone
    //If parent bone does not have a child, 
    //then pChild is set as parent's child
    //else pChild is set as a sibling of parents already existing child
    int setChildrenAndSibling(int parent, Bone *pChild);


    //Member Variables
public:
    // root position in world coordinate system
    float m_RootPos[3];
    int tx,ty,tz;
    int rx,ry,rz;

    // number of DOF of the skeleton
    //DEBUG: remove this variable???
    //    int m_NumDOFs;
    int name2idx(char *);
    char * idx2name(int);
    int NUM_BONES_IN_ASF_FILE;
    int MOV_BONES_IN_ASF_FILE;
private:
    Bone *m_pRootBone;							// Pointer to the root bone, m_RootBone = &bone[0]
    Bone  m_pBoneList[MAX_BONES_IN_ASF_FILE];   // Array with all skeleton bones
};

int numBonesInSkel(Bone item);
int movBonesInSkel(Bone item);

#endif 