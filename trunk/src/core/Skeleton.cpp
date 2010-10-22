#include "Skeleton.h"
#include "Transform.h"



//--------------------------------------------------------------------------------------------------
int numBonesInSkel(Bone item)
{
    Bone * tmp = item.sibling;
    int i = 0;
    while (tmp != NULL) 
    {
        if (tmp->child != NULL)
            i+= numBonesInSkel(*(tmp->child));
        i++; tmp = tmp->sibling; 
    }
    if (item.child != NULL)
        return i+1+numBonesInSkel(*item.child);
    else
        return i+1;
}

//--------------------------------------------------------------------------------------------------
int movBonesInSkel(Bone item)
{
    Bone * tmp = item.sibling;
    int i = 0;

    if (item.dof > 0) i++;

    while (tmp != NULL) 
    {
        if (tmp->child != NULL)
            i+= movBonesInSkel(*(tmp->child));
        if (tmp->dof > 0) i++; tmp = tmp->sibling; 
    }

    if (item.child != NULL)
        return i+movBonesInSkel(*item.child);
    else
        return i;
}

// helper function to convert ASF part name into bone index
//--------------------------------------------------------------------------------------------------
int Skeleton::name2idx(char *name)
{
    int i=0;
    while(strcmp(m_pBoneList[i].name, name) != 0 && i++ < NUM_BONES_IN_ASF_FILE);
    return m_pBoneList[i].idx;
}

//--------------------------------------------------------------------------------------------------
char * Skeleton::idx2name(int idx)
{
    int i=0;
    while(m_pBoneList[i].idx != idx && i++ < NUM_BONES_IN_ASF_FILE);
    return m_pBoneList[i].name;
}

/*
This recursive function traverces skeleton hierarchy 
and returns a pointer to the bone with index - bIndex
ptr should be a pointer to the root node 
when this function first called*/
//--------------------------------------------------------------------------------------------------
Bone* Skeleton::getBone(Bone *ptr, int bIndex)
{
    static Bone *theptr;
    if(ptr==NULL) 
        return(NULL);
    else if(ptr->idx == bIndex)
    {
        theptr=ptr;
        return(theptr);
    }
    else
    { 
        getBone(ptr->child, bIndex);
        getBone(ptr->sibling, bIndex);
        return(theptr);
    }
}

/*
This function sets sibling or child for parent bone
If parent bone does not have a child, 
then pChild is set as parent's child
else pChild is set as a sibling of parents already existing child*/
//--------------------------------------------------------------------------------------------------
int Skeleton::setChildrenAndSibling(int parent, Bone *pChild)
{
    Bone *pParent;  

    //Get pointer to root bone
    pParent = getBone(m_pRootBone, parent);

    if(pParent==NULL)
    {
        printf("inbord bone is undefined\n"); 
        return(0);
    }
    else
    {
        //if pParent bone does not have a child
        //set pChild as parent bone child
        if(pParent->child == NULL)   
        {
            pParent->child = pChild;
        }
        else
        { 
            //if pParent bone already has a child 
            //set pChils as pParent bone's child sibling
            pParent=pParent->child;              
            while(pParent->sibling != NULL) 
                pParent = pParent->sibling;            

            pParent->sibling = pChild;
        }
        return(1);
    }
}

/* 
Return the pointer to the root bone*/	
//--------------------------------------------------------------------------------------------------
Bone* Skeleton::getRoot()
{
    return(m_pRootBone);
}




/***************************************************************************************
Compute relative orientation and translation between the 
parent and child bones. That is, represent the orientation 
matrix and translation vector in the local coordinate of parent body 
*****************************************************************************************/

/*
This function sets rot_parent_current data member.
Rotation from this bone local coordinate system 
to the coordinate system of its parent*/
//--------------------------------------------------------------------------------------------------
void compute_rotation_parent_child(Bone *parent, Bone *child)
{

    double Rx[4][4], Ry[4][4], Rz[4][4], tmp[4][4], tmp1[4][4], tmp2[4][4];

    if(child != NULL)
    { 

        // The following openGL rotations are precalculated and saved in the orientation matrix. 
        //
        // glRotatef(-inboard->axis_x, 1., 0., 0.);
        // glRotatef(-inboard->axis_y, 0., 1,  0.);
        // glRotatef(-inboard->axis_z, 0., 0., 1.);
        // glRotatef(outboard->axis_z, 0., 0., 1.);
        // glRotatef(outboard->axis_y, 0., 1,  0.);
        // glRotatef(outboard->axis_x, 1., 0., 0.);

        rotationZ(Rz, -parent->axis_z);      
        rotationY(Ry, -parent->axis_y);  
        rotationX(Rx, -parent->axis_x);      
        matrix_mult(Rx, Ry, tmp);
        matrix_mult(tmp, Rz, tmp1);

        rotationZ(Rz, child->axis_z);
        rotationY(Ry, child->axis_y);
        rotationX(Rx, child->axis_x);
        matrix_mult(Rz, Ry, tmp);
        matrix_mult(tmp, Rx, tmp2);

        matrix_mult(tmp1, tmp2, tmp);
        matrix_transpose(tmp, child->matrix);    
    }
}

// loop through all bones to calculate local coordinate's direction vector and relative orientation  
//--------------------------------------------------------------------------------------------------
void ComputeRotationToParentCoordSystem(Bone *bone)
{
    int i;
    double Rx[4][4], Ry[4][4], Rz[4][4], tmp[4][4], tmp2[4][4];

    //Compute rot_parent_current for the root 

    //Compute tmp2, a matrix containing root 
    //joint local coordinate system orientation
    rotationZ(Rz, bone[root].axis_z);
    rotationY(Ry, bone[root].axis_y);
    rotationX(Rx, bone[root].axis_x);
    matrix_mult(Rz, Ry, tmp);
    matrix_mult(tmp, Rx, tmp2);
    //set bone[root].rot_parent_current to transpose of tmp2
    matrix_transpose(tmp2, bone[root].matrix);    



    //Compute rot_parent_current for all other bones
    int numbones = numBonesInSkel(bone[0]);
    for(i=0; i<numbones; i++) 
    {
        if(bone[i].child != NULL)
        {
            compute_rotation_parent_child(&bone[i], bone[i].child);

            // compute parent child siblings...
            Bone * tmp = NULL;
            if (bone[i].child != NULL) tmp = (bone[i].child)->sibling;
            while (tmp != NULL)
            {
                compute_rotation_parent_child(&bone[i], tmp);
                tmp = tmp->sibling;
            }
        }
    }
}


/******************************************************************************
Interface functions to set the pose of the skeleton 
******************************************************************************/

//Set the aspect ratio of each bone 
//--------------------------------------------------------------------------------------------------
void set_bone_shape(Bone *bone)
{
    bone[root].aspx=1;          bone[root].aspy=1;
    printf("READ %d\n",numBonesInSkel(bone[0]));
    printf("MOV %d\n",movBonesInSkel(bone[0]));
    int numbones = numBonesInSkel(bone[0]);
    for(int j=1;j<numbones;j++)
    {
        bone[j].aspx=0.25;   bone[j].aspy=0.25;
    }
}

// Constructor 
//--------------------------------------------------------------------------------------------------
Skeleton::Skeleton()
{
    sscanf("root","%s",m_pBoneList[0].name);
    NUM_BONES_IN_ASF_FILE = 1;
    MOV_BONES_IN_ASF_FILE = 1;

    //Initializaton
    m_pBoneList[0].idx = root;   // root of hierarchy
    m_pRootBone = &m_pBoneList[0];
    m_pBoneList[0].sibling = NULL;
    m_pBoneList[0].child = NULL; 
    m_pBoneList[0].dir[0] = 0; m_pBoneList[0].dir[1] = 0.; m_pBoneList[0].dir[2] = 0.;
    m_pBoneList[0].axis_x = 0; m_pBoneList[0].axis_y = 0.; m_pBoneList[0].axis_z = 0.;
    m_pBoneList[0].length = 0.05;
    m_pBoneList[0].dof = 6;
    m_pBoneList[0].dofx = m_pBoneList[0].dofy = m_pBoneList[0].dofz=1;
    m_RootPos[0] = m_RootPos[1]=m_RootPos[2]=0;

    tx = ty = tz = rx = ry = rz = 0;
}

//--------------------------------------------------------------------------------------------------
Skeleton::~Skeleton()
{
}



