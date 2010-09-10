#ifndef NEW_MESH_H
#define NEW_MESH_H

#include <osg/Geometry>
#include "../include/ModelData.h"
#include "../include/IMesh.h"

class CDataTransfer;
class CVec3;

typedef unsigned char byte;


class Mesh: public IMesh, public osg::Geometry
{
public:
    // Functions
    Mesh();
    ~Mesh();

    virtual void Clear();//

    virtual void Allocate(int nVerts, int nTris);//
    virtual void MarkBackfacing(CVec3 CamPos);//
    virtual void SelectTriangle(int nTri);//
    virtual void CalculateFaceNormals();//
    virtual void CalculateVertNormals();//
    virtual void ClearSelection();//
    virtual void ClearTriFlags();//
    virtual void Deallocate();//
    virtual void Update();//

    //Akccessors - Set
    virtual void SetSementicVertexFormat(SVertexElement* val);//
    virtual void SetSelectionMode(eSelectionModes nMode);//
    virtual void SetRootVertNormals(CVec3 * val);//
    virtual void SetBuffer(unsigned char * val);//
    virtual void SetClsid(unsigned char* val);//
    virtual void SetVertNormals(CVec3 * val);//
    virtual void SetMaterialIndex(int val);
    virtual void SetBiNormals(CVec3 * val);//
    virtual void SetSemanticCount(int val);//
    virtual void SetRootVerts(CVec3 * val);//
    virtual void SetName(std::string val);//
    virtual void SetTangent(CVec3 * val);//
    virtual void SetVertexCount(int val);//
    virtual void SetUVs(TexCoord * val);//
    virtual void SetBufferSize(int val);//
    virtual void SetVerts(CVec3 * val);//
    virtual void SetFaceCount(int val);//
    virtual void SetFaces(SFace* val);//
    virtual void SetSkin(SSkin* val);//

    virtual bool DataIO(class CDataTransfer& IO);//
    virtual bool Load(const char *sFilename);//
    virtual bool Inicialize();//

    virtual int FrustumSelect(CVec3 Normals[4], CVec3 Points[8]);//
    virtual int LineSelect(const CVec3 &LP1, const CVec3 &LP2);//
    virtual int& GetMaterialIndex();
    virtual int& GetSemanticCount();//
    virtual int& GetVertexCount();//
    virtual int& GetBufferSize();//
    virtual int& GetFaceCount();//

    virtual unsigned char * GetBuffer() const;//
    virtual unsigned char* GetClsid();//
    virtual std::string GetName() const;//

    virtual CVec3 * GetRootVertNormals() const;//
    virtual CVec3 * GetVertNormals() const;//
    virtual CVec3 * GetBiNormals() const;//
    virtual CVec3 * GetRootVerts() const;//
    virtual CVec3 * GetTangent() const;//
    virtual CVec3 * GetVerts() const;//

    virtual SVertexElement* GetSementicVertexFormat() const;//
    virtual TexCoord * GetUVs() const;//
    virtual SFace* GetFaces() const;//
    virtual SSkin* GetSkin() const;

private:
    size_t GetVertexSize();//

    int m_materialIndex; // material index in material set library
    int m_bufferSize; // buffer size
    int m_faceCount; // number of faces
    int m_semanticCount; // liczba elementów w formacie wierzcho³ka
    int m_vertexCount;

    std::string m_name;
    byte *m_pBuffer;
    byte m_clsid[GUID_SIZE]; // CLSID

    // Data	
    CVec3 *m_pVerts;
    CVec3 *m_pVertNormals;
    CVec3 *m_pRootVerts;
    CVec3 *m_pRootVertNormals;
    CVec3 *m_pBiNormals;
    CVec3 *m_pTangent;
    TexCoord *m_pUVs;
    SVertexElement* m_pSementicVertexFormat;
    SFace*	m_pFaces; // faces
    SSkin*	m_pSkin;

    //TODO : dodane na przysz³oœæ, w ramch obs³ugi zaznaczania elementów siatki
    // NewMesh	
    int m_nbTris;
    CVec3 *m_pTriNormals;
    unsigned int *m_pTris;
    int *m_pTriFlags;

    eSelectionModes m_SelectMode;
};


#endif //NEW_MESH_H