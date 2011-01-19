/********************************************************************
	created:  
	created:  
	filename: Bone.h
	author:	  Rafal Zowal
	
	purpose: Model przechowuj¹cy dane o koœci

*********************************************************************/

#ifndef BONE_H
#define BONE_H

#include "Frame.h"
#include <vector>
#include <osg/Matrixd>

#include <string>

// This holds the names of a joint's children
typedef std::vector<char*> childVector;


enum BoneType
{
    eBONE_UNKNOW = 0,
    eLEFT_UP_LEG_DUM,
    eRIGHT_UP_LEG_DUM,
    eSPINE_DUM,
    eLEFT_UP_LEG,
    eLEFT_LEG,
    eLEFT_FOOT,
    eLEFT_TOE_BASE,
    eRIGHT_UP_LEG,
    eRIGHT_LEG,
    eRIGHT_FOOT,
    eRIGHT_TOE_BASE,
    eSPINE,
    eSPINE1,
    eSPINE1_DUM1,
    eSPINE1_DUM2,
    eSPINE1_DUM3,
    eNECK,
    eHEAD,
    eLEFT_SHOULDER,
    eLEFT_ARM,
    eLEFT_FORE_ARM,
    eLEFT_HAND,
    eLEFT_HAND_DUM1,
    eLEFT_HAND_DUM2,
    eLEFT_HAND_THUMB,
    eRIGHT_SHOULDER,
    eRIGHT_ARM,
    eRIGHT_FORE_ARM,
    eRIGHT_HAND,
    eRIGHT_HAND_DUM1,
    eRIGHT_HAND_DUM2,
    eRIGHT_HAND_THUMB,

    eBONE_MAX,
};


struct BoneLenght 
{
    float X;
    float Y;
    float Z;

    float sizeofBone;
};

//class Bone
struct Bone
{
    Bone(){ matrix = new osg::Matrixd(); m_pboneLengthInSpace = new BoneLenght(); isCleared = false; m_type = eBONE_UNKNOW; }
    ~Bone() { delete matrix; delete m_pboneLengthInSpace; }
	int idx;					// Bone inde

    int parent_id;

    char *name;
    Bone *parent;		// Pointer to the sibling (branch bone) in the hierarchy tree 
	std::vector<Bone *> child;			// Pointer to the child (outboard bone) in the hierarchy tree 
	std::vector<Frame*> frame;			// Animacja TODO: zrobiæ pointera na jak¹s liste przechowuj¹c¹ te ramki - dzieki temu istnieje szybka podmiana animacji
	childVector			children;		// STL vector of char* to children names
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

    osg::Vec3f boneSpace_translation; // dane wczytywane z *.fmesha. - jelsi aplikacja chce wczytac modele bezposrednio z naszego parsera
    osg::Quat boneSpace_quaternion; // dane wczytywane z *.fmesha. - jelsi aplikacja chce wczytac modele bezposrednio z naszego parsera

    osg::Vec3f trans;  //zmienna pomocnicze - przy ³adowniu daneych z fmesha - kosci
    osg::Quat rot; //zmienna pomocnicze - przy ³adowniu daneych z fmesha - kosci

    // Bonespace
    osg::Matrixd bonespace;  // bonespace

    std::vector<int> childBoneId;
	bool isCleared;

    BoneLenght* m_pboneLengthInSpace;  // zmienna uzywana do okreœlenia d³ugosci boxa(który jest koœci¹) przy malowanie kosci
    BoneType m_type;
};

#endif