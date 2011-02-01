#ifndef MESHWRITER_H
#define MESHWRITER_H

#include "stdafx.h"

#include "BaseWriter.h"
#include "IMesh.h"

#include "COLLADAFWMesh.h"
#include "COLLADAFWMeshPrimitive.h"
#include "COLLADAFWArrayPrimitiveType.h"
#include "COLLADAFWMeshVertexData.h"

class COLLADAFW::Mesh;


class CMeshWriter : public CBaseWriter
{
private:
    struct Tuple
    {
        Tuple( unsigned int positionI, unsigned int normalI, unsigned int textureI, unsigned int colorI)
            : positionIndex( positionI), normalIndex(normalI), textureIndex(textureI), colorIndex(colorI){}
        unsigned int positionIndex;
        unsigned int normalIndex;
        unsigned int textureIndex;
        unsigned int colorIndex;
        bool operator<(const Tuple& rhs) const;
    };

    typedef COLLADAFW::ArrayPrimitiveType<float> FloatList;
    typedef COLLADAFW::ArrayPrimitiveType<double> DoubleList;
    typedef COLLADAFW::ArrayPrimitiveType<int> IntList;
    typedef COLLADAFW::ArrayPrimitiveType<unsigned int> UIntList;
    typedef std::map<Tuple, int> TupleIndexMap;
    typedef std::vector<std::string> StringList;

private:
    const COLLADAFW::Mesh* mMesh;
    TupleIndexMap m_TupleMap;
    int m_NextTupleIndex;
    size_t mMeshSize;
    size_t mSubmeshCount;
    size_t mSubmeshNameTableSize;

    // float minx, miny, minz
    float mMeshBoundsMin[ 3 ];
    float mMeshBoundsMax[ 3 ];

    bool mMeshBoundsSet;


    const COLLADAFW::MeshVertexData& mMeshPositions;
    const COLLADAFW::MeshVertexData& mMeshNormals;
    const COLLADAFW::MeshVertexData& mMeshUVCoordinates;
    const COLLADAFW::MeshVertexData& mMeshColors;

    // DoubleList mOgrPositions;
    // DoubleList mOgreNormals;
    // DoubleList mOgreUVCoordinates;
    // DoubleList mOgreColors;
    // UIntList mOgreIndices;

    bool mHasNormals;
    bool mHasUVCoords;

    COLLADABU::Math::Matrix3 mCurrentRotationMatrix;
    COLLADABU::Math::Vector3 mCurrentTranslationVector;

    StringList mSubMeshNames;

    CIMesh* m_CurrentMesh;

public:

    /** Constructor. */
    CMeshWriter( CDAE2MotionWriter* motionWriter, const COLLADAFW::Mesh* mesh );

    /** Destructor. */
    virtual ~CMeshWriter();

    bool write(CIMesh** ppITMMesh);

private:

    /** Disable default copy ctor. */
    CMeshWriter( const CMeshWriter& pre );

    /** Disable default assignment operator. */
    const CMeshWriter& operator= ( const CMeshWriter& pre );

    void writeSubMesh(COLLADAFW::MeshPrimitive* meshPrimitive, const COLLADABU::Math::Matrix4& matrix);
    void addTupleIndex( const Tuple& tuple, CISubMesh* subMesh );

    void writeSubMeshOperation( COLLADAFW::MeshPrimitive::PrimitiveType primitiveType );
    void calculateMeshBounds();

};

#endif //MESHWRITER_H
