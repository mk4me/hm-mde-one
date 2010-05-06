#ifndef CUSTOM_GROUP_H
#define CUSTOM_GROUP_H

#include <osg/Group>

#include "FModel.h"
#include "FMesh.h"
#include "FAnimation.h"

//////////////////////////////////////////////////////////////////////////
// class adding some additional information about mesh, 
// skin, skeleton to group
class CCustomGroup : public osg::Group
{
		// pointer at structure holding entire information about
		// fmodel
		SFModel*		m_FModel;

		// pointer at structure holding entire information about
		// fmesh
		SFMesh*			m_FMesh;

		// pointer at structure holding entire information about
		// animations
		SFAnimation*	m_FAnimations;

	public:

		// set fmodel
		void SetFModel		(SFModel* fmodel)			{ m_FModel = fmodel; }

		// set fmesh
		void SetFMesh		(SFMesh* fmesh)				{ m_FMesh = fmesh; }

		// set animations
		void SetAnimations	(SFAnimation* animations)	{ m_FAnimations = animations; }


		// get material list
		vector<SMaterial>*		GetMaterialList()		{ return &m_FModel->material_list; }

		// get material set list
		vector<vector<int>>*	GetMaterialSetList()	{ return &m_FModel->material_set_list; }

		// get material group list
		vector<SMeshGroup>*		GetMeshGroupList()		{ return &m_FModel->mesh_group_list; }

		// get animation list
		vector<wstring>*		GetAnimationList()		{ return &m_FModel->animation_list; }

		// get mesh
		SMesh*					GetMesh(int id = 0)		{ return m_FMesh->mesh[id]; }

		// number of meshes
		unsigned int			GetNumOfMeshes()		{ return m_FMesh->mesh.size(); }

		// get loaded skeleton - first non transformed version!
		SSkeleton*				GetLoadedSkeleton()		{ return m_FMesh->skeleton; }

		// get path
		const std::wstring&		GetPath()				{ return m_FModel->path; }

		// c - tor
		CCustomGroup();

	protected:

		// d - tors
		virtual ~CCustomGroup();
};

#endif