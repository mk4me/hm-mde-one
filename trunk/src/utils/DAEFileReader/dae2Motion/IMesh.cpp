#include "stdafx.h"

#include "dae2Motion.h"
#include "IMesh.h"


// Konstruktor i destruktor
CIMesh::CIMesh()
{

}

CIMesh::~CIMesh()
{
    for(unsigned int i=0; i< m_SubMeshes.size(); i++)
    {
        delete m_SubMeshes[i];
    }

    m_SubMeshes.clear();
}


//STRING* CITMMesh::GetOriginalId()
//{
//	return m_OriginalId;
//}

unsigned int CIMesh::AddSubMesh(CISubMesh* subMesh)
{
    m_SubMeshes.push_back(subMesh);
    return m_SubMeshes.size() - 1;
}

// Implementacja interfejsu IITMMesh
IISubMesh* CIMesh::GetSubMesh(unsigned int subMeshID)
{
    if (subMeshID < m_SubMeshes.size())
        return m_SubMeshes[subMeshID];
    return NULL;
}

unsigned int CIMesh::GetSubMeshCount()
{
    return m_SubMeshes.size();
}

float* CIMesh::GetTranslation()
{
    return m_Translation;
}	

float* CIMesh::GetRotation()
{
    return m_Rotation;
}


// Konstruktor i destruktor
CISubMesh::CISubMesh()
{

}

CISubMesh::~CISubMesh()
{

}


// Implementacja interfejsu IITMSubMesh

//STRING* CITMSubMesh::GetOriginalId()
//{
//	return &(m_OriginalId);
//}

// Format wierzcho³ka
bool CISubMesh::HasNormals()
{
    return (m_Normals.size() != 0);
}

bool CISubMesh::HasTangents()
{
    return (m_Tangents.size() != 0);
}

bool CISubMesh::HasBinormals()
{
    return (m_Binormals.size() != 0);
}

bool CISubMesh::HasColors()
{
    return (m_Colors.size() != 0);
}

bool CISubMesh::HasUV0Coords()
{   
    return (m_UV0Coords.size() != 0);
}

bool CISubMesh::HasUV1Coords()
{
    return (m_UV1Coords.size() != 0);
}

// Rozmiar buforów
unsigned int CISubMesh::GetVertexCount()
{
    return m_Positions.size();
}



// Zwraca wskaŸnik na pierwszy element bufora
float* CISubMesh::GetPositions()
{
    if (m_Positions.size())
        return &(m_Positions[0]);
    return NULL;
}

float* CISubMesh::GetNormals()
{
    if (m_Normals.size())
        return &(m_Normals[0]);
    return NULL;
}

float* CISubMesh::GetTangents()
{
    if (m_Tangents.size())
        return &(m_Tangents[0]);
    return NULL;
}

float* CISubMesh::GetBinormals()
{
    if (m_Binormals.size())
        return &(m_Binormals[0]);
    return NULL;
}

float* CISubMesh::GetColors()
{
    if (m_Colors.size())
        return &(m_Colors[0]);
    return NULL;
}

float* CISubMesh::GetUV0Coords()
{
    if (m_UV0Coords.size())
        return &(m_UV0Coords[0]);
    return NULL;
}

float* CISubMesh::GetUV1Coords()
{
    if (m_UV1Coords.size())
        return &(m_UV1Coords[0]);
    return NULL;
}

unsigned int CISubMesh::GetIndicesCount()
{
    return m_Indices.size();
}

unsigned long* CISubMesh::GetIndices()
{
    if (m_Indices.size())
        return &(m_Indices[0]);
    return NULL;
}
