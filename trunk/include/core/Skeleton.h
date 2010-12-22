#ifndef SKELETON_H
#define SKELETON_H

#include "Bone.h"
#include <osg/Vec4>

#define MAX_BONES_IN_ASF_FILE 256


struct sBoneColorInfo
{
    BoneType	    m_type;
    std::string		m_name;
    osg::Vec4		*m_pColor;
};

#define LEFT_SIDE_COLOR   new osg::Vec4(0.0, 0.0, 1.0, 0.4)
#define RIGHT_SIDE_COLOR  new osg::Vec4(1.0, 0.0, 0.0, 0.4)
#define CENTER_COLOR      new osg::Vec4(0.5, 0.0, 0.5, 0.4)

// Bone segment names used in ASF file
static int root = 0;
static sBoneColorInfo s_BonesInfo[eBONE_MAX] =
{
    {eBONE_UNKNOW,      "",                 new osg::Vec4(1.0, 1.0, 1.0, 0.0)}, // 
    {eLEFT_UP_LEG_DUM,  "LeftUpLeg_dum",                    LEFT_SIDE_COLOR}, // 
    {eRIGHT_UP_LEG_DUM,	"RightUpLeg_dum",                   RIGHT_SIDE_COLOR}, // 
    {eSPINE_DUM,	    "Spine_dum",                            CENTER_COLOR}, // 
    {eLEFT_UP_LEG,	    "LeftUpLeg",                        LEFT_SIDE_COLOR}, // 
    {eLEFT_LEG,	        "LeftLeg",                          LEFT_SIDE_COLOR}, // 
    {eLEFT_FOOT,	    "LeftFoot",                         LEFT_SIDE_COLOR}, // 
    {eLEFT_TOE_BASE,    "LeftToeBase",                      LEFT_SIDE_COLOR}, // 
    {eRIGHT_UP_LEG,	    "RightUpLeg",                       RIGHT_SIDE_COLOR}, // 
    {eRIGHT_LEG,	    "RightLeg",                         RIGHT_SIDE_COLOR}, // 
    {eRIGHT_FOOT,	    "RightFoot",                        RIGHT_SIDE_COLOR}, // 
    {eRIGHT_TOE_BASE,	"RightToeBase",                     RIGHT_SIDE_COLOR}, // 
    {eSPINE,    	    "Spine",                                CENTER_COLOR}, // 
    {eSPINE1,	        "Spine1",                               CENTER_COLOR}, // 
    {eSPINE1_DUM1,	    "Spine1_dum1",                          CENTER_COLOR}, // 
    {eSPINE1_DUM2,	    "Spine1_dum2",                          CENTER_COLOR}, // 
    {eSPINE1_DUM3,	    "Spine1_dum3",                          CENTER_COLOR}, // 
    {eNECK,	            "Neck",                                 CENTER_COLOR}, // 
    {eHEAD,	            "Head",                                 CENTER_COLOR}, // 
    {eLEFT_SHOULDER,	"LeftShoulder",                     LEFT_SIDE_COLOR}, // 
    {eLEFT_ARM,	        "LeftArm",                          LEFT_SIDE_COLOR}, // 
    {eLEFT_FORE_ARM,	"LeftForeArm",                      LEFT_SIDE_COLOR}, // 
    {eLEFT_HAND,	    "LeftHand",                         LEFT_SIDE_COLOR}, // 
    {eLEFT_HAND_DUM1,	"LeftHand_dum1",                    LEFT_SIDE_COLOR}, // 
    {eLEFT_HAND_DUM2,	"LeftHand_dum2",                    LEFT_SIDE_COLOR}, // 
    {eLEFT_HAND_THUMB,	"LeftHandThumb",                    LEFT_SIDE_COLOR}, // 
    {eRIGHT_SHOULDER,	"RightShoulder",                    RIGHT_SIDE_COLOR}, // 
    {eRIGHT_ARM,	    "RightArm",                         RIGHT_SIDE_COLOR}, // 
    {eRIGHT_FORE_ARM,	"RightForeArm",                     RIGHT_SIDE_COLOR}, // 
    {eRIGHT_HAND,	    "RightHand",                        RIGHT_SIDE_COLOR}, // 
    {eRIGHT_HAND_DUM1,	"RightHand_dum1",                   RIGHT_SIDE_COLOR}, // 
    {eRIGHT_HAND_DUM2,	"RightHand_dum2",                   RIGHT_SIDE_COLOR}, // 
    {eRIGHT_HAND_THUMB,	"RightHandThumb",                   RIGHT_SIDE_COLOR}  // 
};

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