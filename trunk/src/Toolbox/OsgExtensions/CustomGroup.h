#ifndef CUSTOM_GROUP_H
#define CUSTOM_GROUP_H

// TODO:
// Ta klasa jest tymczasowa! w momencie, w ktorym wszystkie dane beda juz
// przekazywane do OSG od razu przez plugin - usune ja !!!
// jej jedynym zadaniem jesst uproszczenie kilku elementow zwiazanych z wczytywaniem
// danych...

#include <osg/Group>

#include "../../osgDBPlugin/FModel.h"
#include "../../osgDBPlugin/FMesh.h"
#include "../../osgDBPlugin/FAnimation.h"

//////////////////////////////////////////////////////////////////////////
// class adding some additional information about mesh, 
// skin, skeleton to group
class CCustomGroup : public osg::Group
{
		// pointer at structure holding entire information about
		// fmodel
		SFModel*		_FModel;

		// pointer at structure holding entire information about
		// fmesh
		SFMesh*			_FMesh;

		// pointer at structure holding entire information about
		// animations
		SFAnimation*	_FAnimations;

	public:

		// set fmodel
		void setFModel		(SFModel* fmodel)			{ _FModel = fmodel; }

		// set fmesh
		void setFMesh		(SFMesh* fmesh)				{ _FMesh = fmesh; }

		// set animations
		void setAnimations	(SFAnimation* animations)	{ _FAnimations = animations; }

        //set skeleton
        void setSkeleton    (SSkeleton* skeleton)       { _FMesh->skeleton = skeleton; }


        // get animiation 
        SFAnimation*            getAnimation()          {if(_FAnimations) return _FAnimations;  return NULL;}

		// get material list
		vector<SMaterial>*		getMaterialList()		{ return &_FModel->material_list; }

		// get material set list
		vector<vector<int> >*		getMaterialSetList()	{ return &_FModel->material_set_list; }

		// get material group list
		vector<SMeshGroup>*		getMeshGroupList()		{ return &_FModel->mesh_group_list; }

		// get animation list
		vector<wstring>*		getAnimationList()		{ return &_FModel->animation_list; }

		// get mesh
        SMesh* getMesh(int id = 0);

		// number of meshes
		unsigned int			getNumOfMeshes()		{ return _FMesh->mesh.size(); }

		// get loaded skeleton - first non transformed version!
		SSkeleton*				getLoadedSkeleton()		{ return _FMesh->skeleton; }

		// get path
		const std::wstring&		getPath()				{ return _FModel->path; }

		// c - tor
		CCustomGroup();

	protected:

		// d - tors
		// protected due to OSG philosophy
		virtual ~CCustomGroup();
};

#endif
