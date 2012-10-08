#include "CustomGroup.h"

//--------------------------------------------------------------------------------------------------
CCustomGroup::CCustomGroup()
: _FModel(NULL), _FMesh(NULL)
{
}

//--------------------------------------------------------------------------------------------------
CCustomGroup::~CCustomGroup()
{
	if (_FModel)
		delete _FModel;

	if (_FMesh)
		delete _FMesh;

	if (_FAnimations)
		delete _FAnimations;
}

//--------------------------------------------------------------------------------------------------
SMesh* CCustomGroup::getMesh(int id)
{
    if(_FMesh->mesh.size() != 0 && _FMesh->mesh[id])
        return _FMesh->mesh[id];

    return NULL;
}

//--------------------------------------------------------------------------------------------------
void CCustomGroup::Clear()
{
    _FMesh->mesh.clear();
}
