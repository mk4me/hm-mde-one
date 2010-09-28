#ifndef I_MESH_H
#define I_MESH_H


//class CDataTransfer;
class CVec3;
struct SVertexElement;
struct SFace;
struct SSkin;

struct TexCoord {
    float x, y;
};

class IMesh
{
public:
    enum eSelectionModes{ SELECT_ADD, SELECT_SUB };
    enum eTriFlags{ TF_SELECTED = 1, TF_BACKFACING = (1<<1) };	

    virtual ~IMesh() {};
    virtual void Clear() = 0;

    virtual void Allocate(int nVerts, int nTris) = 0;
    virtual void MarkBackfacing(CVec3 CamPos) = 0;
    virtual void SelectTriangle(int nTri) = 0;
    virtual void CalculateFaceNormals() = 0;
    virtual void CalculateVertNormals() = 0;
    virtual void ClearSelection() = 0;
    virtual void ClearTriFlags() = 0;
    virtual void Deallocate() = 0;
    virtual void Update() = 0;

    //Akccessors - Set
    virtual void SetSementicVertexFormat(SVertexElement* val) = 0;
    virtual void SetSelectionMode(eSelectionModes nMode) = 0;
    virtual void SetRootVertNormals(CVec3 * val) = 0;
    virtual void SetBuffer(unsigned char * val) = 0;
    virtual void SetClsid(unsigned char* val) = 0;
    virtual void SetVertNormals(CVec3 * val) = 0;
    virtual void SetMaterialIndex(int val) = 0;
    virtual void SetBiNormals(CVec3 * val) = 0;
    virtual void SetSemanticCount(int val) = 0;
    virtual void SetRootVerts(CVec3 * val) = 0;
    virtual void SetName(std::string val) = 0;
    virtual void SetTangent(CVec3 * val) = 0;
    virtual void SetVertexCount(int val) = 0;
    virtual void SetUVs(TexCoord * val) = 0;
    virtual void SetBufferSize(int val) = 0;
    virtual void SetVerts(CVec3 * val) = 0;
    virtual void SetFaceCount(int val) = 0;
    virtual void SetFaces(SFace* val) = 0;
    virtual void SetSkin(SSkin* val) = 0;

    //virtual bool DataIO(class CDataTransfer& IO) = 0;
    virtual bool Inicialize() = 0;

    virtual int FrustumSelect(CVec3 Normals[4], CVec3 Points[8]) = 0;
    virtual int LineSelect(const CVec3 &LP1, const CVec3 &LP2) = 0;
    virtual int& GetMaterialIndex() = 0;
    virtual int& GetSemanticCount() = 0;
    virtual int& GetVertexCount() = 0;
    virtual int& GetBufferSize() = 0;
    virtual int& GetFaceCount() = 0;

    virtual size_t GetVertexSize() = 0;

    virtual unsigned char * GetBuffer() const = 0;
    virtual unsigned char* GetClsid() = 0;
    virtual std::string GetName() const = 0;

    virtual CVec3 * GetRootVertNormals() const = 0;
    virtual CVec3 * GetVertNormals() const = 0;
    virtual CVec3 * GetBiNormals() const = 0;
    virtual CVec3 * GetRootVerts() const = 0;   
    virtual CVec3 * GetTangent() const = 0;
    virtual CVec3 * GetVerts() const = 0;

    virtual SVertexElement* GetSementicVertexFormat() const = 0;
    virtual TexCoord * GetUVs() const = 0;
    virtual SFace* GetFaces() const = 0;
    virtual SSkin* GetSkin() const = 0;
};


#endif //I_MESH_H
