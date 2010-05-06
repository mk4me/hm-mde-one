#include "CustomGroup.h"




//////////////////////////////////////////////////////////////////////////
// c - tor
CCustomGroup::CCustomGroup()
: m_FModel(NULL), m_FMesh(NULL)
{
}


//////////////////////////////////////////////////////////////////////////
// d - tor
CCustomGroup::~CCustomGroup()
{
	if (m_FModel)
		delete m_FModel;

	if (m_FMesh)
		delete m_FMesh;

	if (m_FAnimations)
		delete m_FAnimations;
}