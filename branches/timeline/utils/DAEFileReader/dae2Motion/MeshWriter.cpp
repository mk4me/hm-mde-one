#include "stdafx.h"
#include "MeshWriter.h"
#include "DAE2MotionWriter.h"

#include "COLLADAFWMesh.h"
#include "COLLADAFWUniqueId.h"
#include "COLLADAFWTriangles.h"
#include "COLLADAFWTrifans.h"
#include "COLLADAFWTristrips.h"

#include "COLLADABUPlatform.h"

#ifdef COLLADABU_OS_WIN
#	include <windows.h>
#endif


// TODO: Timmy
std::string material("BaseWhite");

//------------------------------
CMeshWriter::CMeshWriter( CDAE2MotionWriter* cubeITMWriter, const COLLADAFW::Mesh* mesh )
: CBaseWriter(cubeITMWriter)
, mMesh(mesh)
, m_NextTupleIndex(0)
, mMeshSize(0)
, mSubmeshCount(0)
, mSubmeshNameTableSize(0) // Timmy mSubmeshNameTableSize(STREAM_OVERHEAD_SIZE)
, mMeshBoundsSet(false)
, mHasNormals( false )
, mHasUVCoords( false )
, mMeshPositions( mesh->getPositions() )
, mMeshNormals( mesh->getNormals() )
, mMeshUVCoordinates( mesh->getUVCoords() )
, mMeshColors( mesh->getColors() )
{
    // mVersion = "[MeshSerializer_v1.30]";
    // calcMeshSize();
    mMeshBoundsMin[0] = -100;
    mMeshBoundsMin[1] = -100;
    mMeshBoundsMin[2] = -100;
    mMeshBoundsMax[0] = 100;
    mMeshBoundsMax[1] = 100;
    mMeshBoundsMax[2] = 100;
}

//------------------------------
CMeshWriter::~CMeshWriter()
{
}

//------------------------------
bool CMeshWriter::Tuple::operator<( const Tuple& rhs ) const
{
    if ( positionIndex < rhs.positionIndex )
        return true;

    if ( positionIndex > rhs.positionIndex )
        return false;

    if ( normalIndex < rhs.normalIndex )
        return true;

    if ( normalIndex > rhs.normalIndex )
        return false;

    if ( textureIndex < rhs.textureIndex )
        return true;

    if ( textureIndex > rhs.textureIndex )
        return false;

    if ( colorIndex < rhs.colorIndex )
        return true;

    if ( colorIndex > rhs.colorIndex )
        return false;

    return false;
}


//------------------------------
bool CMeshWriter::write(CIMesh** ppITMMesh)
{
    // Create new ITM Mesh
    m_CurrentMesh = new CIMesh();

    const COLLADAFW::UniqueId& meshUniqueId = mMesh->getUniqueId();
    const CDAE2MotionWriter::GeometryUniqueIdInstanceGeometryInfoMap& instanceGeometryMap = getGeometryUniqueIdInstanceGeometryInfoMap();
    CDAE2MotionWriter::GeometryUniqueIdInstanceGeometryInfoMap::const_iterator lowerBound = instanceGeometryMap.lower_bound(meshUniqueId);
    CDAE2MotionWriter::GeometryUniqueIdInstanceGeometryInfoMap::const_iterator upperBound = instanceGeometryMap.upper_bound(meshUniqueId);

    for ( CDAE2MotionWriter::GeometryUniqueIdInstanceGeometryInfoMap::const_iterator it = lowerBound; it != upperBound; ++it)
    {
        const CDAE2MotionWriter::InstanceGeometryInfo& instanceGeometryInfo = it->second;
        COLLADABU::Math::Matrix4 worldMatrix = instanceGeometryInfo.worldMatrix;
        COLLADABU::Math::Matrix4 pivot = instanceGeometryInfo.pivot;
        const COLLADAFW::MeshPrimitiveArray& meshPrimitives = mMesh->getMeshPrimitives();
        const COLLADAFW::InstanceGeometry* instanceGeometry = instanceGeometryInfo.fwInstanceGeometry;

        // przeniesienie skali do pivota
        COLLADABU::Math::Vector3 scale = worldMatrix.getScale();
        COLLADABU::Math::Matrix4 scaleMatrix;
        scaleMatrix.makeScale(scale);

        pivot = scaleMatrix * pivot;

        // ustawienie mCurrentRotationMatrix i mCurrentTranslationVector danymi z pivota
        pivot.extract3x3Matrix(mCurrentRotationMatrix );
        mCurrentTranslationVector.set(pivot[0][3], pivot[1][3], pivot[2][3]);

        for ( size_t i = 0, count = meshPrimitives.getCount(); i < count; ++i )
        {
            COLLADAFW::MeshPrimitive* meshPrimitive = meshPrimitives[i];
            writeSubMesh(meshPrimitive, pivot);
        }

        //budowa macierzy 3x3 bez skalowania z macierzy swiata
        COLLADABU::Math::Vector3 col1( worldMatrix[ 0 ][ 0 ], worldMatrix[ 1 ][ 0 ], worldMatrix[ 2 ][ 0 ] );
        COLLADABU::Math::Vector3 col2( worldMatrix[ 0 ][ 1 ], worldMatrix[ 1 ][ 1 ], worldMatrix[ 2 ][ 1 ] );
        COLLADABU::Math::Vector3 col3( worldMatrix[ 0 ][ 2 ], worldMatrix[ 1 ][ 2 ], worldMatrix[ 2 ][ 2 ] );

        col1.normalise();
        col2.normalise();
        col3.normalise();

        COLLADABU::Math::Matrix3 rotMatrix;
        rotMatrix.setColumn(0, col1);
        rotMatrix.setColumn(1, col2);
        rotMatrix.setColumn(2, col3);

        /*worldMatrix.setElement(0, 0, col1.x);
        worldMatrix.setElement(1, 0, col1.y);
        worldMatrix.setElement(2, 0, col1.z);
        worldMatrix.setElement(0, 1, col2.x);
        worldMatrix.setElement(1, 1, col2.y);
        worldMatrix.setElement(2, 1, col2.z);
        worldMatrix.setElement(0, 2, col3.x);
        worldMatrix.setElement(1, 2, col3.y);
        worldMatrix.setElement(2, 2, col3.z);*/

        // wyciagniecie rotacji
        COLLADABU::Math::Quaternion rot(rotMatrix);
        //rot = worldMatrix.extractQuaternion();

        rot.normalise();

        // zapis transformacji do mesha

        m_CurrentMesh->m_Translation[0] = (float)worldMatrix[0][3];
        m_CurrentMesh->m_Translation[1] = (float)worldMatrix[1][3];
        m_CurrentMesh->m_Translation[2] = (float)worldMatrix[2][3];

        m_CurrentMesh->m_Rotation[0] = (float)rot.x;
        m_CurrentMesh->m_Rotation[1] = (float)rot.y;
        m_CurrentMesh->m_Rotation[2] = (float)rot.z;
        m_CurrentMesh->m_Rotation[3] = (float)rot.w;
    }

    // TODO
    // itmMesh->computeBounds();
    // itmMesh->computeTangentSpace();


    *ppITMMesh = m_CurrentMesh;

    return TRUE;
}






//------------------------------
void CMeshWriter::addTupleIndex( const Tuple& tuple, CISubMesh* subMesh )
{
    TupleIndexMap::const_iterator it = m_TupleMap.find(tuple);
    if ( it == m_TupleMap.end() )
    {
        // Wrzucamy nowy wierzcho³ek,
        subMesh->m_Indices.push_back( m_TupleMap[tuple] = m_NextTupleIndex++ );

        if ( mMeshPositions.getType() == COLLADAFW::MeshVertexData::DATA_TYPE_DOUBLE )
        {
            const double* positionsArray = mMeshPositions.getDoubleValues()->getData();
            positionsArray += 3*tuple.positionIndex;
            COLLADABU::Math::Vector3 position(positionsArray[0], positionsArray[1], positionsArray[2]);
            // bez transformacji
            position = mCurrentRotationMatrix * position + mCurrentTranslationVector;
            subMesh->m_Positions.push_back((float)position.x);
            subMesh->m_Positions.push_back((float)position.y);
            subMesh->m_Positions.push_back((float)position.z);
        }
        else
        {
            const float* positionsArray = mMeshPositions.getFloatValues()->getData();
            positionsArray += 3*tuple.positionIndex;
            COLLADABU::Math::Vector3 position(positionsArray[0], positionsArray[1], positionsArray[2]);
            // bez transformacji
            position = mCurrentRotationMatrix * position + mCurrentTranslationVector;
            subMesh->m_Positions.push_back((float)position.x);
            subMesh->m_Positions.push_back((float)position.y);
            subMesh->m_Positions.push_back((float)position.z);
        }

        if ( mHasNormals )
        {
            if ( mMeshNormals.getType() == COLLADAFW::MeshVertexData::DATA_TYPE_DOUBLE )
            {
                const double* normalsArray = mMeshNormals.getDoubleValues()->getData();
                normalsArray += 3*tuple.normalIndex;
                COLLADABU::Math::Vector3 normal(normalsArray[0], normalsArray[1], normalsArray[2]);
                normal = mCurrentRotationMatrix * normal;
                normal.normalise();
                subMesh->m_Normals.push_back((float)normal.x);
                subMesh->m_Normals.push_back((float)normal.y);
                subMesh->m_Normals.push_back((float)normal.z);
            }
            else
            {
                const float* normalsArray = mMeshNormals.getFloatValues()->getData();
                normalsArray += 3*tuple.normalIndex;
                COLLADABU::Math::Vector3 normal(normalsArray[0], normalsArray[1], normalsArray[2]);
                normal = mCurrentRotationMatrix * normal;
                normal.normalise();
                subMesh->m_Normals.push_back((float)normal.x);
                subMesh->m_Normals.push_back((float)normal.y);
                subMesh->m_Normals.push_back((float)normal.z);
            }
        }

        if ( mHasUVCoords )
        {
            if ( mMeshUVCoordinates.getType() == COLLADAFW::MeshVertexData::DATA_TYPE_DOUBLE )
            {
                const double* uVCoordinateArray = mMeshUVCoordinates.getDoubleValues()->getData();
                uVCoordinateArray += mMeshUVCoordinates.getStride(0)*tuple.textureIndex;
                subMesh->m_UV0Coords.push_back((float)uVCoordinateArray[0]);
                subMesh->m_UV0Coords.push_back((float)uVCoordinateArray[1]);
            }
            else
            {
                const float* uVCoordinateArray = mMeshUVCoordinates.getFloatValues()->getData();
                uVCoordinateArray += mMeshUVCoordinates.getStride(0)*tuple.textureIndex;
                subMesh->m_UV0Coords.push_back((float)uVCoordinateArray[0]);
                subMesh->m_UV0Coords.push_back((float)uVCoordinateArray[1]);
            }
        }

    }
    else
    {
        subMesh->m_Indices.push_back(it->second);
    }
}

//------------------------------
void CMeshWriter::writeSubMesh( COLLADAFW::MeshPrimitive* meshPrimitive, const COLLADABU::Math::Matrix4& matrix )
{
    // TODO - pobraæ aktualnego SubMesha
    CISubMesh* subMesh = new CISubMesh();
    m_CurrentMesh->AddSubMesh(subMesh);

    // Czyœcimy mape wierzcho³kow (aby mieæ pewnoœæ ¿e poprzedni subMesh nie namiesza)
    m_NextTupleIndex = 0;
    m_TupleMap.clear();

    int numIndices = 0;

    // Indeksy pozycji
    const COLLADAFW::UIntValuesArray& positionIndices =  meshPrimitive->getPositionIndices();
    size_t positionIndicesCount  = positionIndices.getCount();
    // Indeksy normalnych
    const COLLADAFW::UIntValuesArray& normalIndices =  meshPrimitive->getNormalIndices();
    size_t normalIndicesCount = normalIndices.getCount();
    mHasNormals = (normalIndicesCount != 0);
    // Indeksy UV

    const COLLADAFW::UIntValuesArray* uvIndices;
    size_t uvIndicesCount = 0;
    const COLLADAFW::IndexListArray& uVIndicesList = meshPrimitive->getUVCoordIndicesArray();
    if ( !uVIndicesList.empty() )
    {
        uvIndices = &uVIndicesList[0]->getIndices();
        uvIndicesCount = uvIndices->getCount();
        mHasUVCoords = (uvIndicesCount != 0);
    }
    else
    {
        mHasUVCoords = false;
    }

    // Rozmiar buforów indeksów musi siê zgadzaæ
    assert( !mHasNormals || (positionIndicesCount == normalIndicesCount));
    assert( !mHasUVCoords || (positionIndicesCount == uvIndicesCount));

    switch (meshPrimitive->getPrimitiveType())
    {
    case COLLADAFW::MeshPrimitive::TRIANGLES:
    case COLLADAFW::MeshPrimitive::POLYGONS:
        {
            const COLLADAFW::Triangles* triangles = (const COLLADAFW::Triangles*) meshPrimitive;
            numIndices = (int)positionIndicesCount;

            for ( int j = 0; j < numIndices; ++j )
            {
                unsigned int positionIndex = positionIndices[j];

                unsigned int normalIndex = 0;
                if ( mHasNormals )
                    normalIndex = normalIndices[j];

                unsigned int uvIndex = 0;
                if ( mHasUVCoords )
                    uvIndex = (*uvIndices)[j];

                Tuple tuple( positionIndex, normalIndex, uvIndex, 0);
                addTupleIndex(tuple,subMesh);
            }
        }
        break;
    default:
        break;
    }

    // TODO, timmy
    // writeSubMeshOperation( meshPrimitive->getPrimitiveType() );
    // calculateMeshBounds();

    m_NextTupleIndex = 0;
    m_TupleMap.clear();
}





//-----------------------------------------------------------------------
void CMeshWriter::writeSubMeshOperation( COLLADAFW::MeshPrimitive::PrimitiveType primitiveType )
{
    // unsigned short operationType
    unsigned short opType = 0;

    switch ( primitiveType )
    {
    case COLLADAFW::MeshPrimitive::TRIANGLES:
        //opType = static_cast<unsigned short>( Ogre::OT_TRIANGLE_LIST );
        break;
    case COLLADAFW::MeshPrimitive::TRIANGLE_FANS:
        //opType = static_cast<unsigned short>( Ogre::OT_TRIANGLE_FAN );
        break;
    case COLLADAFW::MeshPrimitive::TRIANGLE_STRIPS:
        //opType = static_cast<unsigned short>( Ogre::OT_TRIANGLE_STRIP );
        break;
    }
    // writeShorts( &opType, 1 );
}




