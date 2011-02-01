/********************************************************************
	created:  
	created:  
	filename: !Model.h
	author:	  Rafal Zowal
	
	purpose: Interface Siatki (Mesh.h) Przechowywuj¹cej szereg danych zwiazanych z saitk¹
*********************************************************************/

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
    //! \Czyszczenie zasobów modelu
    virtual void Clear() = 0;

        //! \update siatki za pomoc¹ vertex shadera
    //************************************
    // Method:    HardwareAnimation
    // FullName:  Mesh::HardwareAnimation
    // Access:    public 
    // Returns:   void
    // Qualifier:
    // Parameter: osg::Uniform * uniform - dane: tablica szkieletu : macierze
    // Parameter: osg::Uniform * bonespaceUniform - dane : tablica bonspace szkieletu
    // Parameter: std::string shaderPath - œcie¿ka do shadera
    //************************************
    virtual void HardwareAnimation( osg::Uniform* uniform, osg::Uniform* bonespaceUniform, std::string shaderPath) = 0;

    //! \rezerwowanie pamieci dla zmiennych
    virtual void Allocate(int vertexCoun) = 0;
    virtual void MarkBackfacing(CVec3 CamPos) = 0;
    virtual void SelectTriangle(int nTri) = 0;
    virtual void CalculateFaceNormals() = 0;
    virtual void CalculateVertNormals() = 0;
    virtual void ClearSelection() = 0;
    virtual void ClearTriFlags() = 0;
    //! \zwalnianie pamiêci
    virtual void Deallocate() = 0;
    //! \odœwierzenie wartosci siatki dla osg::geometry ( VBO )
    virtual void Update() = 0;

    //Akccessors - Set
    virtual void SetSementicVertexFormat(SVertexElement* val) = 0;
    virtual void SetSelectionMode(eSelectionModes nMode) = 0;
    virtual void SetRootVertNormals(CVec3 * val) = 0;
    //! \za³¹dowanie buffora danymi o wierzcho³kach
    virtual void SetBuffer(unsigned char * val) = 0;
    virtual void SetTris(unsigned long * val) = 0;
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
    virtual void SetTrisCount(int val) = 0;
    virtual void SetVerts(CVec3 * val) = 0;
    virtual void SetFaceCount(int val) = 0;
    virtual void SetVertex(float* val) = 0;
    //! \za³adowanie faców w nasyzm pryzpadku danymi z fmesha
    virtual void SetFaces(SFace* val) = 0;
    virtual void SetSkin(SSkin* val) = 0;

    //virtual bool DataIO(class CDataTransfer& IO) = 0;
    virtual bool Inicialize() = 0;

    virtual int FrustumSelect(CVec3 Normals[4], CVec3 Points[8]) = 0;
    virtual int LineSelect(const CVec3 &LP1, const CVec3 &LP2) = 0;
    virtual unsigned long * GetTris() const = 0;
    virtual int GetTrisCount() const = 0;
    virtual int& GetMaterialIndex() = 0;
    virtual int& GetSemanticCount() = 0;
    virtual int& GetVertexCount() = 0;
    virtual int& GetBufferSize() = 0;
    virtual int& GetFaceCount() = 0;

    virtual float* GetVertex() const = 0;

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
