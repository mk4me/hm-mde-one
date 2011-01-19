#include "CorePCH.h"
#include "Mesh.h"

#include "Intersection.h"
//#include "DataTransfer.h"
#include <core/Enumerators.h>
#include <core/Vec3.h>
#include <core/ModelData.h>

#include <osg/PrimitiveSet>



//--------------------------------------------------------------------------------------------------
Mesh::Mesh()
{
    m_vertexCount = 0;
    m_faceCount = 0;
    m_nbTris = 0;

    m_pVerts = NULL;
    m_pVertNormals = NULL;
    m_pTriNormals = NULL;
    m_pUVs = NULL;
    m_pTris = NULL;
    m_pTriFlags = NULL;
    m_pBiNormals = NULL;
    m_pTangent = NULL;
    m_pBuffer = NULL;
    m_pSementicVertexFormat = NULL;
    m_pFaces = NULL;
    m_pSkin = NULL;
    vertex = NULL;

    m_SelectMode = SELECT_ADD;
}

//--------------------------------------------------------------------------------------------------
Mesh::~Mesh()
{
    Deallocate();
    Clear();
}

//--------------------------------------------------------------------------------------------------
void Mesh::Clear()
{
    m_vertexCount = 0;
    m_faceCount = 0;
    m_nbTris = 0;

    m_pVerts = NULL;
    m_pVertNormals = NULL;
    m_pTriNormals = NULL;
    m_pUVs = NULL;
    m_pTris = NULL;
    m_pTriFlags = NULL;
    m_pBiNormals = NULL;
    m_pTangent = NULL;
    m_pBuffer = NULL;
    m_pSementicVertexFormat = NULL;
    m_pFaces = NULL;
    m_pSkin = NULL;
    vertex = NULL;

    m_pShader = NULL;
    m_pProgram = NULL;
}

//--------------------------------------------------------------------------------------------------
void Mesh::Allocate( int vertexCoun)
{
    m_pRootVerts = new CVec3 [vertexCoun];
    m_pRootVertNormals = new CVec3[vertexCoun];

    m_pVerts = new CVec3 [vertexCoun];
    m_pUVs  = new TexCoord[vertexCoun];
    m_pVertNormals = new CVec3[vertexCoun];
    m_pBiNormals = new CVec3[vertexCoun];
    m_pTangent = new CVec3[vertexCoun];
    m_pTriNormals = new CVec3[vertexCoun];
}

//--------------------------------------------------------------------------------------------------
void Mesh::Deallocate()
{
    delete[] m_pVerts;
    delete[] m_pUVs;
    delete[] m_pVertNormals;
    delete[] m_pTris;
    delete[] m_pTriFlags;
    delete[] m_pTriNormals;

    delete[] m_pRootVerts;
    delete[] m_pRootVertNormals;
    delete[] m_pBiNormals;
    delete[] m_pTangent;
}


//--------------------------------------------------------------------------------------------------
size_t Mesh::GetVertexSize()
{
    size_t vertexSize = 0; 
    for(int i=0; i<m_semanticCount; ++i)
    {
        size_t size = 0; 
        switch(m_pSementicVertexFormat[i].type)
        {
        case Position: 
            size = 3 * SIZES_FLOAT_SIZE; 
            break; 
        case Normal: 
            size = 3 * SIZES_FLOAT_SIZE; 
            break; 
        case Binormal: 
            size = 3 * SIZES_FLOAT_SIZE; 
            break; 
        case Tangent: 
            size = 3 * SIZES_FLOAT_SIZE; 
            break; 
        case TexCoord0: 
            size = 2 * SIZES_FLOAT_SIZE; 
            break; 
        default:
            size = 0; 
        }
        vertexSize += size; 
    }
    return vertexSize; 
}

//--------------------------------------------------------------------------------------------------
bool Mesh::Inicialize()
{
    if(m_pBuffer)
        InicializeFromFMeshFile();
    else
        InicializeFromDAEFile();

    osg::Vec4Array* colors = new osg::Vec4Array;

    // aktualizacja faców i ustawienie prymitywów dla osg::Geometry
    for (int f = 0; f < m_faceCount; f++)
    {
        colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f) );
        colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f) );
        colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f) );

        osg::DrawElementsUInt* face = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
        //  face->reserve(3);
        face->push_back(m_pFaces[f].index[0]);
        face->push_back(m_pFaces[f].index[1]);
        face->push_back(m_pFaces[f].index[2]);

        this->addPrimitiveSet(face);
    }


    // załadownie listy kolorów do VBO
    this->setColorArray(colors);
    // ustwaienie sposobu powiazania z VBO
    this->setColorBinding(osg::Geometry::BIND_OVERALL);


    // Data needed to GPU update - hardware animation
    m_pInfluanceArray = new osg::Vec4Array();
    m_pWeightArrary = new osg::Vec4Array();
    m_pNumberOfBoneArrary = new osg::FloatArray();

    // aktualizacja wierzchołkow uzywających skinningu
    // dane zpisujemy w postaci vektóru - łatwiej potem uzyc w shaderze.
    // max 4 kosci wpływu na wierzchołek
    for (int i = 0; i < m_pSkin->n; i++)
    {
        // vertice
        SSkinnedVertice* vertice = &m_pSkin->skinned_vertices[i];

        osg::Vec4 influance;
        osg::Vec4 weight;

        // for every affecting bone
        for (int b = 0; b < vertice->n; b++)
        {
            // przypisz numer kosci oraz wartosc wpływu danej kosci na wierzchołek 
            switch (b)
            {
            case 0:
                influance.x() =  vertice->bones[b].boneID;
                weight.x() = vertice->bones[b].weight;
                break;

            case 1:
                influance.y() =  vertice->bones[b].boneID;
                weight.y() = vertice->bones[b].weight;
                break;

            case 2:
                influance.z() =  vertice->bones[b].boneID;
                weight.z() = vertice->bones[b].weight;
                break;

            case 3:
                influance.w() =  vertice->bones[b].boneID;
                weight.w() = vertice->bones[b].weight;
                break;
            }
        }

        // zapisania danych potrzebnych przy wywoływaniu shadera - hardware animation
        m_pInfluanceArray->push_back(influance);  // indexy kosci które wpływaja na wierzchołek
        m_pWeightArrary->push_back(weight); // wartosci wag które wpływaja na wierzchołek - zsynchronizowane z m_pInfluanceArray
        m_pNumberOfBoneArrary->push_back(vertice->n); // ilosc kości które wływa na dany wierchołek
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
void Mesh::InicializeFromFMeshFile()
{
    bool isTexture = false;
    m_vertexCount = m_bufferSize/GetVertexSize();

    Allocate(m_vertexCount);

    // przygotowanie daneych dla osg::Geometry - VBO
    osg::Vec3Array* vertices     = new osg::Vec3Array();
    osg::Vec2Array* texcoords    = new osg::Vec2Array();
    osg::Vec3Array* normals      = new osg::Vec3Array();

    int indexComplexBuffor = 0;

    // powtarzaj prezz ilość wierzchołków
    for(int k = 0; k<m_vertexCount; k++)
    {
        // prezszukaj i odpowiedni przypisz wartosc danych z wierzchołka poprzez zapisana w ChunkFile-u semantyke
        for(size_t j=0; j<m_semanticCount; j++)
        {
            size_t size = 0; 
            // 
            switch(m_pSementicVertexFormat[j].type)
            {
            // pozycja
            case Position: 
                size = 3 * SIZES_FLOAT_SIZE;

                memcpy (&m_pVerts[k]._v[0], &m_pBuffer[indexComplexBuffor], SIZES_FLOAT_SIZE);
                memcpy (&m_pVerts[k]._v[1], &m_pBuffer[indexComplexBuffor + SIZES_FLOAT_SIZE], SIZES_FLOAT_SIZE);
                memcpy (&m_pVerts[k]._v[2], &m_pBuffer[indexComplexBuffor + (2*SIZES_FLOAT_SIZE)], SIZES_FLOAT_SIZE);

                // zrobienie kopi zapasowej dla wierzchołków okreslajacych początkową pozycje - potrzebne przy skinningu
                memcpy (&m_pRootVerts[k]._v[0], &m_pBuffer[indexComplexBuffor], SIZES_FLOAT_SIZE);
                memcpy (&m_pRootVerts[k]._v[1], &m_pBuffer[indexComplexBuffor + SIZES_FLOAT_SIZE], SIZES_FLOAT_SIZE);
                memcpy (&m_pRootVerts[k]._v[2], &m_pBuffer[indexComplexBuffor + (2*SIZES_FLOAT_SIZE)], SIZES_FLOAT_SIZE);

                vertices->push_back(m_pVerts[k]);

                indexComplexBuffor += size;
                break; 

            // normalna
            case Normal: 
                size = 3 * SIZES_FLOAT_SIZE; 

                memcpy (&m_pVertNormals[k]._v[0], &m_pBuffer[indexComplexBuffor], SIZES_FLOAT_SIZE);
                memcpy (&m_pVertNormals[k]._v[1], &m_pBuffer[indexComplexBuffor + SIZES_FLOAT_SIZE], SIZES_FLOAT_SIZE);
                memcpy (&m_pVertNormals[k]._v[2], &m_pBuffer[indexComplexBuffor + (2*SIZES_FLOAT_SIZE)], SIZES_FLOAT_SIZE);

                // zrobienie kopi zapasowej dla normalnych okreslajacych początkową pozycje - potrzebne przy skinningu
                memcpy (&m_pRootVertNormals[k]._v[0], &m_pBuffer[indexComplexBuffor], SIZES_FLOAT_SIZE);
                memcpy (&m_pRootVertNormals[k]._v[1], &m_pBuffer[indexComplexBuffor + SIZES_FLOAT_SIZE], SIZES_FLOAT_SIZE);
                memcpy (&m_pRootVertNormals[k]._v[2], &m_pBuffer[indexComplexBuffor + (2*SIZES_FLOAT_SIZE)], SIZES_FLOAT_SIZE);

                normals->push_back(m_pVertNormals[k]);

                indexComplexBuffor += size;
                break; 

            // Binormalna
            case Binormal: 
                size = 3 * SIZES_FLOAT_SIZE; 

                memcpy (&m_pBiNormals[k]._v[0], &m_pBuffer[indexComplexBuffor], SIZES_FLOAT_SIZE);
                memcpy (&m_pBiNormals[k]._v[1], &m_pBuffer[indexComplexBuffor + SIZES_FLOAT_SIZE], SIZES_FLOAT_SIZE);
                memcpy (&m_pBiNormals[k]._v[2], &m_pBuffer[indexComplexBuffor + (2*SIZES_FLOAT_SIZE)], SIZES_FLOAT_SIZE);

                indexComplexBuffor += size;
                break; 

            // Tangens
            case Tangent: 
                size = 3 * SIZES_FLOAT_SIZE; 

                memcpy (&m_pTangent[k]._v[0], &m_pBuffer[indexComplexBuffor], SIZES_FLOAT_SIZE);
                memcpy (&m_pTangent[k]._v[1], &m_pBuffer[indexComplexBuffor + SIZES_FLOAT_SIZE], SIZES_FLOAT_SIZE);
                memcpy (&m_pTangent[k]._v[2], &m_pBuffer[indexComplexBuffor + (2*SIZES_FLOAT_SIZE)], SIZES_FLOAT_SIZE);

                indexComplexBuffor += size;
                break; 
            // Współzzedne tekstury
            case TexCoord0: 
                size = 2 * SIZES_FLOAT_SIZE; 

                memcpy (&m_pUVs[k].x, &m_pBuffer[indexComplexBuffor], SIZES_FLOAT_SIZE);
                memcpy (&m_pUVs[k].y, &m_pBuffer[indexComplexBuffor + SIZES_FLOAT_SIZE], SIZES_FLOAT_SIZE);

                texcoords->push_back(osg::Vec2(fabs(m_pUVs[k].x), fabs(m_pUVs[k].y)));

                indexComplexBuffor += size;
                break; 

            default:
                size = 0; 
            }
        } 
    }

    // zapisanie danych do VBO poprzez osg::Geometry
    // zapisanie listy wierzchłkow VBO
    this->setVertexArray(vertices); 
    // zapisanie listy normalnych VBO
    this->setNormalArray(normals);
    // ustalenie sposobu powiązania z VBO
    this->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);

    for(size_t j=0; j<m_semanticCount; j++)
    {
        // zapisanie list wsp. textur jesli takowe zostały przekazane
        if(m_pSementicVertexFormat[j].type == TexCoord0)
            this->setTexCoordArray(0, texcoords);
    }
}

//--------------------------------------------------------------------------------------------------
void Mesh::InicializeFromDAEFile()
{
    osg::Vec3Array* vertices     = new osg::Vec3Array();
    osg::Vec2Array* texcoords    = new osg::Vec2Array();
    osg::Vec3Array* normals      = new osg::Vec3Array();

    for(int k = 0; k<m_vertexCount; k++)
    {
        vertices->push_back(m_pVerts[k]);
        normals->push_back(m_pVertNormals[k]);
    }

    // zapisanie listy wierzchłkow
    this->setVertexArray(vertices);
    // zapisanie listy normalnych
    this->setNormalArray(normals);
}

//--------------------------------------------------------------------------------------------------
void Mesh::Update()
{
    osg::Vec3Array* vertices     = new osg::Vec3Array();
    osg::Vec2Array* texcoords    = new osg::Vec2Array();
    osg::Vec3Array* normals      = new osg::Vec3Array();

    for(int k = 0; k<m_vertexCount; k++)
    {
        vertices->push_back(m_pVerts[k]);
        normals->push_back(m_pVertNormals[k]);
    }


    this->setVertexArray(vertices);
    this->setNormalArray(normals);
 //   this->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);


    // aktualizacja faców i ustawienie prymitywów dla osg::Geometry
    for (int f = 0; f < m_faceCount; f++)
    {
        osg::DrawElementsUInt* face = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
        //  face->reserve(3);
        face->push_back(m_pFaces[f].index[0]);
        face->push_back(m_pFaces[f].index[1]);
        face->push_back(m_pFaces[f].index[2]);

        this->setPrimitiveSet(f,face);
    }
}

//--------------------------------------------------------------------------------------------------
//!/ shaderPath - ścieżka do shadera
void Mesh::HardwareAnimation( osg::Uniform* uniform, osg::Uniform* bonespaceUniform, std::string shaderPath)
{
    // Aby robic modyfikacje na bierząco
    //   if(m_pShader.valid())
    //       m_pShader->loadShaderSourceFromFile(shaderPath);

    // załadownie shaera tylko raz - w wiadomym celu
    if (!m_pProgram.valid()) {
        m_pProgram = new osg::Program;
        m_pProgram->setName("HardwareSkinning");
        // inicjalizacja shedrera wraz z wczytaniem źródła : shader path - scieżka do shadera
        if (!m_pShader.valid())
            m_pShader = osg::Shader::readShaderFile(osg::Shader::VERTEX, shaderPath);

        if (!m_pShader.valid()) {
            osg::notify(osg::WARN) << "RigTransformHardware can't load VertexShader" << std::endl;
            return;
        }

        m_pProgram->addShader(m_pShader.get());

        {
            // przypisanie numery danych (atrybutów) zapisanych w obiekcie do nazwy zmiennej atribture uzywanej w shaderze glslowym
            m_pProgram->addBindAttribLocation("index", 12);
            m_pProgram->addBindAttribLocation("weight", 13);
            m_pProgram->addBindAttribLocation("numBones", 14);
        }

        //okreslenie rodzaju powiązania danych w VBO
        this->setVertexAttribData(12, osg::Geometry::ArrayData(m_pInfluanceArray,osg::Geometry::BIND_PER_VERTEX));
        this->setVertexAttribData(13, osg::Geometry::ArrayData(m_pWeightArrary,osg::Geometry::BIND_PER_VERTEX));
        this->setVertexAttribData(14, osg::Geometry::ArrayData(m_pNumberOfBoneArrary,osg::Geometry::BIND_PER_VERTEX));
    } 


    osg::ref_ptr<osg::StateSet> ss = new osg::StateSet;

    // update shadera danymi uniform - w naszym przypadku sa to dane : macierze kosci oraz bonesapce
    ss->addUniform(uniform);
    ss->addUniform(bonespaceUniform);
    ss->setAttributeAndModes(m_pProgram.get());

    this->setStateSet(ss.get());
}

//--------------------------------------------------------------------------------------------------
// DataIO is used to read or write from/to a file
// ( in the future this could also be used for cloning, stats, etc. )
//
// bool Mesh::DataIO( CDataTransfer &IO )
// {
//     return true;
// }

//--------------------------------------------------------------------------------------------------
// Compute the face normals from the crossproduct and put them in m_pTriNormals
//
void Mesh::CalculateFaceNormals( )
{
    m_faceCount = m_nbTris/3;
    m_pFaces = new SFace[m_faceCount];
    m_nbTris -= 3;
    for ( int nTri = 0; nTri <= m_nbTris;)
    {
        m_pFaces[nTri/3].index[0] = m_pTris[ nTri ];
        m_pFaces[nTri/3].index[1] = m_pTris[ nTri+1 ];
        m_pFaces[nTri/3].index[2] = m_pTris[ nTri+2 ];

        nTri += 3;
//         m_pTriNormals[ nTri ] = (P1-P0).CrossProduct(P2-P0);
//         m_pTriNormals[ nTri ].Normalize();
    }
}

//--------------------------------------------------------------------------------------------------
// Compute normals for each vertex by averaging the normals of adjacent faces.
// Requires face normals to be computed first.
//
void Mesh::CalculateVertNormals( )
{
    for ( int i = 0; i < m_vertexCount; i++ )
    {
        m_pVertNormals[ i ].SetZero();
    }
    for ( int nTri = 0; nTri < m_nbTris; nTri++ )
    {
        int Index = nTri*3;
        m_pVertNormals[ m_pTris[Index] ]   += m_pTriNormals[ nTri ];
        m_pVertNormals[ m_pTris[Index+1] ] += m_pTriNormals[ nTri ];
        m_pVertNormals[ m_pTris[Index+2] ] += m_pTriNormals[ nTri ];
    }
    for ( int i = 0; i < m_vertexCount; i++ )
    {
        m_pVertNormals[ i ].Normalize();
    }
}

//--------------------------------------------------------------------------------------------------
// Zero out m_pTriFlags
void Mesh::ClearTriFlags()
{
    memset( m_pTriFlags, 0, m_nbTris * sizeof(int) );
}


//--------------------------------------------------------------------------------------------------
// MESH SELECTION
//
void Mesh::ClearSelection( )
{
    for (int nTri = 0; nTri < m_nbTris; nTri++ )
    {
        m_pTriFlags[ nTri ] &= ~TF_SELECTED;
    }
}

//--------------------------------------------------------------------------------------------------
void Mesh::SetSelectionMode( eSelectionModes Mode )
{
    m_SelectMode = Mode;
}

//--------------------------------------------------------------------------------------------------
// Sets the TF_SELECTED flag for the triangle at nTri
void Mesh::SelectTriangle( int nTri ) 
{
    if ( nTri < 0 || nTri >= m_nbTris ) return;
    if ( m_SelectMode == SELECT_ADD ) m_pTriFlags[ nTri ] |= TF_SELECTED;
    if ( m_SelectMode == SELECT_SUB ) m_pTriFlags[ nTri ] &= ~TF_SELECTED;
}

//--------------------------------------------------------------------------------------------------
// Sets the TF_BACKFACING flag for triangles that are backfacing from CamPos's point of view
void Mesh::MarkBackfacing( CVec3 CamPos )
{
    for (int nTri = 0; nTri < m_nbTris; nTri++ )
    {
        if ( m_pTriNormals[ nTri ].Dot( CamPos - m_pVerts[ m_pTris[ nTri*3 ] ] ) < 0 ) 
            m_pTriFlags[ nTri ] |= TF_BACKFACING;
        else
            m_pTriFlags[ nTri ] &= ~TF_BACKFACING;
    }
}

//--------------------------------------------------------------------------------------------------
// Test a line( LP1-LP2 ) against the mesh
// Selects the closest front-facing triangle
//
int Mesh::LineSelect( const CVec3 &LP1, const CVec3 &LP2 )
{
    CVec3 HitP;
    int nbHits = 0;
    int nSelTri = -1;
    float fDistance = 1000000000.0f;

    for (int nTri = 0; nTri < m_nbTris; nTri++ )
    {
        if ( m_pTriFlags[ nTri ] & TF_BACKFACING ) continue; // Check only front facing triangles

        int nV = nTri*3;	

        bool bHit = CheckLineTri( LP2, LP1, m_pVerts[ m_pTris[nV] ], m_pVerts[ m_pTris[nV+1] ], m_pVerts[ m_pTris[nV+2] ], HitP );
        if ( bHit ) 
        {
            if ( HitP.Distance( LP1 ) < fDistance ) {
                fDistance = HitP.Distance( LP1 );
                nSelTri = nTri;
            }
            nbHits++;
        }
    }

    SelectTriangle( nSelTri );

    return nbHits;
}

//--------------------------------------------------------------------------------------------------
// Select mesh triangles that are in a frustum defined by 8 Points( and 4 face normals )
// I'm currently ignoring near and far planes, but they could be easily added
//
int Mesh::FrustumSelect( CVec3 Normals[4], CVec3 Points[8] )
{
    int nbHits = 0;
    CVec3 Tri[3];

    for (int nTri = 0; nTri < m_nbTris; nTri++ )
    {	
        int nV = nTri*3;
        Tri[0] = m_pVerts[ m_pTris[ nV ] ];
        Tri[1] = m_pVerts[ m_pTris[ nV+1 ] ];
        Tri[2] = m_pVerts[ m_pTris[ nV+2 ] ];

        if ( TriInFrustum( Tri, Normals, Points ) )
        {
            SelectTriangle( nTri );			
            nbHits++;			
        }
    }

    return nbHits;
}

//--------------------------------------------------------------------------------------------------
void Mesh::SetSementicVertexFormat(SVertexElement* semanticFormat)
{
    m_pSementicVertexFormat = semanticFormat;
}

//--------------------------------------------------------------------------------------------------
void Mesh::SetRootVertNormals(CVec3 * verNormal)
{
    m_pRootVertNormals = verNormal;
}

//--------------------------------------------------------------------------------------------------
void Mesh::SetBuffer(unsigned char * buffer)
{
    m_pBuffer = buffer;
}

//--------------------------------------------------------------------------------------------------
void Mesh::SetVertNormals(CVec3 * normals)
{
    m_pVertNormals = normals;
}

//--------------------------------------------------------------------------------------------------
void Mesh::SetBiNormals(CVec3 * biNormal)
{
    m_pBiNormals = biNormal;
}

//--------------------------------------------------------------------------------------------------
void Mesh::SetSemanticCount(int semanticCount)
{
    m_semanticCount = semanticCount;
}

//--------------------------------------------------------------------------------------------------
void Mesh::SetRootVerts(CVec3 * rootVertex)
{
    m_pRootVerts = rootVertex;
}

//--------------------------------------------------------------------------------------------------
void Mesh::SetName(std::string name)
{
    m_name = name;
}

//--------------------------------------------------------------------------------------------------
void Mesh::SetClsid(unsigned char* clasID)
{
    //  m_clsid = clasID;
}

//--------------------------------------------------------------------------------------------------
void Mesh::SetTangent(CVec3 * tangent)
{
    m_pTangent = tangent;
}

//--------------------------------------------------------------------------------------------------
void Mesh::SetVertexCount(int vertexCount)
{
    m_vertexCount = vertexCount;
}

//--------------------------------------------------------------------------------------------------
void Mesh::SetUVs(TexCoord * uvs)
{
    m_pUVs = uvs;
}

//--------------------------------------------------------------------------------------------------
void Mesh::SetBufferSize(int bufferSize)
{
    m_bufferSize = bufferSize;
}

//--------------------------------------------------------------------------------------------------
void Mesh::SetVerts(CVec3 * verts)
{
    m_pVerts = verts;
}

//--------------------------------------------------------------------------------------------------
void Mesh::SetFaceCount(int faceCount)
{
    m_faceCount = faceCount;
}

//--------------------------------------------------------------------------------------------------
void Mesh::SetFaces( SFace* faces )
{
    m_pFaces = faces;
}

//--------------------------------------------------------------------------------------------------
void Mesh::SetSkin(SSkin* skin)
{
    m_pSkin = skin;
}

//--------------------------------------------------------------------------------------------------
int& Mesh::GetSemanticCount()
{
    return m_semanticCount;
}

//--------------------------------------------------------------------------------------------------
int& Mesh::GetVertexCount()
{
    return m_vertexCount;
}

//--------------------------------------------------------------------------------------------------
int& Mesh::GetBufferSize()
{
    return m_bufferSize;
}

//--------------------------------------------------------------------------------------------------
int& Mesh::GetFaceCount()
{
    return m_faceCount;
}

//--------------------------------------------------------------------------------------------------
unsigned char * Mesh::GetBuffer() const
{
    return m_pBuffer;
}

//--------------------------------------------------------------------------------------------------
unsigned char* Mesh::GetClsid()
{
    return m_clsid;
}

//--------------------------------------------------------------------------------------------------
std::string Mesh::GetName() const
{
    return m_name;
}

//--------------------------------------------------------------------------------------------------
CVec3 * Mesh::GetRootVertNormals() const
{
    return m_pRootVertNormals;
}

//--------------------------------------------------------------------------------------------------
CVec3 * Mesh::GetVertNormals() const
{
    return m_pVertNormals;
}

//--------------------------------------------------------------------------------------------------
CVec3 * Mesh::GetBiNormals() const
{
    return m_pBiNormals;
}

//--------------------------------------------------------------------------------------------------
CVec3 * Mesh::GetRootVerts() const
{
    return m_pRootVerts;
}

//--------------------------------------------------------------------------------------------------
CVec3 * Mesh::GetTangent() const
{
    return m_pTangent;
}

//--------------------------------------------------------------------------------------------------
CVec3 * Mesh::GetVerts() const
{
    return m_pVerts;
}

//--------------------------------------------------------------------------------------------------
SVertexElement* Mesh::GetSementicVertexFormat() const
{
    return m_pSementicVertexFormat;
}

//--------------------------------------------------------------------------------------------------
TexCoord * Mesh::GetUVs() const
{
    return m_pUVs;
}

//--------------------------------------------------------------------------------------------------
SFace* Mesh::GetFaces() const
{
    return m_pFaces;
}

//--------------------------------------------------------------------------------------------------
SSkin* Mesh::GetSkin() const
{
    return m_pSkin;
}
//--------------------------------------------------------------------------------------------------
int& Mesh::GetMaterialIndex()
{ 
    return m_materialIndex; 
}

//--------------------------------------------------------------------------------------------------
void Mesh::SetMaterialIndex(int val) 
{ 
    m_materialIndex = val;
}

//--------------------------------------------------------------------------------------------------
int Mesh::GetTrisCount() const
{ 
    return m_nbTris;
}

//--------------------------------------------------------------------------------------------------
void Mesh::SetTrisCount(int val) 
{ 
    m_nbTris = val; 
}

//--------------------------------------------------------------------------------------------------
unsigned long * Mesh::GetTris() const
{
    return m_pTris;
}

//--------------------------------------------------------------------------------------------------
void Mesh::SetTris( unsigned long * val )
{
    m_pTris = val;
}

//--------------------------------------------------------------------------------------------------
float* Mesh::GetVertex() const
{
    return vertex;
}

//--------------------------------------------------------------------------------------------------
void Mesh::SetVertex( float* val )
{
    vertex = val;
}