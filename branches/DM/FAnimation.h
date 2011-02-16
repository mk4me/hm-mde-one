#ifndef F_ANIMATION_H
#define F_ANIMATION_H


#include <map>
#include <string>


#define FMD_HEADER			0x524E	// NR
#define FMD_MESH			0x484D	// MH
#define FMD_MESH_VERTS		0x564D	// MV
#define FMD_MESH_TVERTS		0x544D	// MT
#define FMD_MESH_NORMALS	0x4E4D	// MN
#define FMD_MESH_TANGENTS	0x474D	// MG
#define FMD_MESH_BINORMALS	0x424D	// MB
#define FMD_MESH_FACES		0x464D	// MF
#define FMD_MAT_LIB			0x494C	// LI
#define FMD_MATERIAL		0x4C4D	// ML
#define FMD_TEXTURE			0x5854	// TX
#define FMD_STRING			0x5453	// ST
#define FMD_SKIN			0x4B53	// SK
#define FMD_MATRIX3			0x334D	// M3
#define FMD_MTX_PARTS		0x504D	// MP
#define FMD_BONES			0x4E42	// BN 
#define FMD_SKEL_ANIM		0x5341	// AS
#define FMD_MESH_ANIM		0x4D41	// AM
#define FMD_MODIFIER		0x444D	// MD
#define FMD_PLUGIN			0x4750	// PG
#define FMD_DOF				0x4644	// DF
#define FMD_MHM_MOD			0x4D4D	// MM


struct SKeyFrame
{
	float time;		// actual key time
	float trans[3];	// bone translation vector 
	float quat [4];	// bone quaternion
};


struct SSkeletalAnimationBone
{
	int			bone_id;	// actual bone id
	int			n;			// key count
	
	SKeyFrame*	frames;		// key frames

	SSkeletalAnimationBone() : frames(NULL) {}
	~SSkeletalAnimationBone() { if (frames) delete [] frames, frames = NULL; }
};


struct SSkeletonAnimation
{
	std::string				name;			// animation name
	int						bones_count;	// bones count

	SSkeletalAnimationBone* bones;			// bones

	SSkeletonAnimation() : bones(NULL) {}
	~SSkeletonAnimation() { if (bones) delete [] bones, bones = NULL; }
};


struct SFAnimation
{
	std::map<std::wstring, SSkeletonAnimation*> m_skeletonAanimation; // map of animations

	~SFAnimation()
	{
		for (std::map<std::wstring, SSkeletonAnimation*>::iterator i = m_skeletonAanimation.begin(); i != m_skeletonAanimation.end(); ++i)
			if (i->second)
				delete i->second;
		m_skeletonAanimation.clear();
	}
};

#endif