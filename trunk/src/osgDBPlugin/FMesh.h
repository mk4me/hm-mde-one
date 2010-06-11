#ifndef FMESH_H
#define FMESH_H

#include <cstring>
#include <string>
#include <vector>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Geometry>


typedef unsigned char byte;


//////////////////////////////////////////////////////////////////////////
// rozmiary
enum sizes
{
	INT_SIZE = 4,
	SHORT_INT_SIZE = 2,
	WORD_SIZE = 2,
	FLOAT_SIZE = 4,
	DOUBLE_SIZE = 8,
	CHAR_SIZE = 1,
	// rozmiary pojedynczych danych w kawa³kach
	VERT_DATA_SIZE = 12,	  // 3*FLOAT_SIZE
	NORMAL_DATA_SIZE = 12,	// 3*FLOAT_SIZE
	TVERT_DATA_SIZE = 8,	  // 2*FLOAT_SIZE
	FACE_DATA_SIZE = 40,	  // 3 indeksy na wierz, 3 na TVerts, 3 na normalne, 
	// 1 na ID Materia³u (wszystko to s¹ DWORD'y - 4 bajty)
	HEADER_SIZE = 6,		    // ID + chunk size (2+4)
	HEADER_SIZE_EXP = 10,	  // ID + chunk size + number of elements in chunk (3*4)
	GUID_SIZE = 16,
	VERTICES_SIZE = 56
};

//////////////////////////////////////////////////////////////////////////
// .fmesh defs

#define FMODEL_HEADER             0x524E  // NR
#define FMODEL_MESH               0x484D  // MH
#define FMODEL_MESH_VERTICES      0x564D  // MV
#define FMODEL_MESH_TVERTICES     0x544D  // MT
#define FMODEL_MESH_NORMALS       0x4E4D  // MN
#define FMODEL_MESH_TANGENTS      0x474D  // MG 
#define FMODEL_MESH_BINORMALS     0x424D  // MB 
#define FMODEL_MESH_INDICES       0x464D  // MF
#define FMODEL_MESH_DATA          0x454D  // ME
#define FMODEL_MESH_DATA_2_1      0x324D  // M2
#define FMODEL_MESH_BUFFER        0x554D  // MU
#define FMODEL_MESH_MATERIALSET   0x534D  // MS
#define FMODEL_MESH_BOUNDINGBOX   0x4242  // BB
#define FMODEL_MATERIAL_LIBRARY   0x494C  // LI
#define FMODEL_MATERIAL           0x4C4D  // ML
#define FMODEL_TEXTURE            0x5854  // TX
#define FMODEL_STRING             0x5453  // ST
#define FMODEL_SKIN               0x4B53  // SK
#define FMODEL_MATRIX3            0x334D  // M3
#define FMODEL_MTX_PARTS          0x504D  // MP
#define FMODEL_BONES              0x4E42  // BN 
#define FMODEL_SKELETON_ANIMATION 0x5341  // AS 
#define FMODEL_MESH_ANIMATION     0x4D41  // AM 
#define FMODEL_LOD_DATA           0x4C55  // UL
#define FMODEL_DOF                0x4644  // DF
#define FMODEL_MODIFIER           0x444D  // MD
#define FMODEL_PLUGIN             0x4750  // PG


//////////////////////////////////////////////////////////////////////////
// structs representing particular parts of .mesh

struct SVertice
{
	float position       [3];   // Position
	float normal         [3];   // Normal
	float tangent        [3];   // Tangent
	float binormal       [3];   // Binormal
	float texture_vertex [2];   // Texture Vertex
};

struct SVertexElement
{
	int semantic;
	int type;
}; 

struct SMeshBuffer
{
	int				n;				// buffer size
	byte*			buffer;			// bufor wierzcholkow
	int				element_count;	// liczba elementów w formacie wierzcho³ka
	SVertexElement*  vertex_format;   

	SMeshBuffer() : buffer(NULL), vertex_format(NULL) {}
	~SMeshBuffer() { if (buffer)        delete [] buffer,        buffer        = NULL;
					if (vertex_format) delete [] vertex_format, vertex_format = NULL;  }
};

struct SFace
{
	short int index[3];	// index to vertices/texture vertices/normals table to each corner of the face
};

struct SMeshFaces
{
	int		face_count;	// number of faces
	SFace*	faces;		// faces

	SMeshFaces() : faces(NULL) {}
	~SMeshFaces() { if (faces) delete [] faces, faces = NULL; }
};

struct SAffectingBone
{
	int   boneID; // ID of affecting bone (index to bones array) 
	float weight; // bone affecting weight
};

struct SSkinnedVertice
{
	int				vert_id;	// ID of affected vertex  
	int				n;			// number of affecting bones
	SAffectingBone*	bones;		// bones affecting vertice

	SSkinnedVertice() : bones(NULL) {}
	~SSkinnedVertice() { if (bones) delete [] bones, bones = NULL; }
};

struct SSkin
{
	int				n;					 // number of skinned verts
	SSkinnedVertice* skinned_vertices;	 // skinned vertices

	SSkin() : skinned_vertices(NULL) {}
	~SSkin() { if (skinned_vertices) delete [] skinned_vertices, skinned_vertices = NULL; }
};

struct SMeshModifier
{
	byte	clsid[GUID_SIZE];	// CLSID
	SSkin*	skin;

	SMeshModifier() : skin(NULL) {}
	~SMeshModifier() { if (skin) delete skin, skin = NULL; }
};

struct SMesh
{
	int             i;			// material index in material set library
	std::string     name;
	SMeshBuffer*    mesh_buffer;
	SMeshFaces*     mesh_faces;
	SMeshModifier*  modifier;

	osg::ref_ptr<osg::Geode>	geode;
	osg::ref_ptr<osg::Geometry>	geometry;

	SMesh() : mesh_buffer(new SMeshBuffer()), mesh_faces(new SMeshFaces()), modifier(new SMeshModifier()), geode(new osg::Geode()), geometry(new osg::Geometry()) {}

	~SMesh() { if (mesh_buffer)  delete mesh_buffer,  mesh_buffer  = NULL;
			  if (mesh_faces)   delete mesh_faces,   mesh_faces   = NULL;
         	  if (modifier)     delete modifier,     modifier     = NULL; }
};

struct SDof
{
	byte				active_d_of_freedom;   // active Degrees of Freedom
	unsigned short int  active_dof_limit_axis; // active DoF limits axis
	float				X_l_rot_bound;		   // x axis lower rotation bound limited
	float				Y_l_rot_bound;		   // y axis lower rotation bound limited
	float				Z_l_rot_bound;		   // z axis lower rotation bound limited
	float				X_u_rot_bound;		   // x axis upper rotation bound limited
	float				Y_u_rot_bound;		   // y axis upper rotation bound limited
	float				Z_u_rot_bound;		   // z axis upper rotation bound limited
	float				X_l_trans_bound;	   // x axis lower translation bound limited
	float				Y_l_trans_bound;	   // y axis lower translation bound limited
	float				Z_l_trans_bound;	   // z axis lower translation bound limited
	float				X_u_trans_bound;	   // x axis upper translation bound limited
	float				Y_u_trans_bound;	   // y axis upper translation bound limited
	float				Z_u_trans_bound;	   // z axis upper translation bound limited
};

struct SBone
{	
	osg::ref_ptr<osg::Group> node;

	std::string name;
	int			parent_id;

	float translation      [3]; // bone translation vector 
	float quaternion       [4]; // bone quaternion 
	float bone_space_trans [3]; // BoneSpace translation vector 
	float bone_space_quate [4]; // BoneSpace quaternion 

	int   n;				// number of child bones
	int*  child_bone_id;	// child bones

	SDof   dof;				// dof (?)

	SBone() : child_bone_id(NULL) { memset(&dof, 0, sizeof(SDof)); }
	~SBone() { if (child_bone_id) delete [] child_bone_id, child_bone_id = NULL; }
};

struct SSkeleton
{
	byte	clsid[GUID_SIZE];	// CLSID
	int		bones_count;

	SBone*	bones;

	SSkeleton() : bones(NULL) {}
	~SSkeleton() { if (bones) delete [] bones, bones = NULL; }
};

struct SFMesh
{
	std::vector<SMesh*>		mesh;
	SSkeleton*				skeleton;
	
	SFMesh() : skeleton(NULL) {}
	~SFMesh() 
	{ 
		for (std::vector<SMesh*>::iterator i = mesh.begin(); i != mesh.end(); ++i)
			delete *i;
		mesh.clear();

		if (skeleton)
			delete skeleton, skeleton = NULL;
	}
};

#endif
