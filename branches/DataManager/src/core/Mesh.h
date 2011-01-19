/********************************************************************
	created:  
	created:  
	filename: Model.h
	author:	  Rafal Zowal
	
	purpose: Przechowywanie szereg danych zwiazanych z saitk¹
*********************************************************************/

#ifndef NEW_MESH_H
#define NEW_MESH_H

#include <osg/Geometry>
#include <core/ModelData.h>
#include <core/IMesh.h>

#include <osg/Shader>
#include <osg/Program>
#include <osg/VertexProgram>
#include <osg/FragmentProgram>

//class CDataTransfer;
class CVec3;

typedef unsigned char byte;


//! \dziedziczenie po osg::Geometry w celu lepszego i p³ynniejszego reenderowania siatki w osg
class Mesh: public IMesh, public osg::Geometry
{
public:
    // Functions
    Mesh();
    ~Mesh();

    //! \Czyszczenie zasobów modelu
    virtual void Clear();

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
    virtual void HardwareAnimation(osg::Uniform* uniform, osg::Uniform* bonespaceUniform, std::string shaderPath);

    //! \rezerwowanie pamieci dla zmiennych
    virtual void Allocate(int vertexCoun);
    virtual void MarkBackfacing(CVec3 CamPos);
    virtual void SelectTriangle(int nTri);
    virtual void CalculateFaceNormals();
    virtual void CalculateVertNormals();
    virtual void ClearSelection();
    virtual void ClearTriFlags();
    //! \zwalnianie pamiêci
    virtual void Deallocate();
    //! \odœwierzenie wartosci siatki dla osg::geometry ( VBO )
    virtual void Update();

    //Akccessors - Set
    virtual void SetSementicVertexFormat(SVertexElement* val);
    virtual void SetSelectionMode(eSelectionModes nMode);
    virtual void SetRootVertNormals(CVec3 * val);
    //! \za³¹dowanie buffora danymi o wierzcho³kach
    virtual void SetBuffer(unsigned char * val);
    virtual void SetClsid(unsigned char* val);
    virtual void SetVertNormals(CVec3 * val);
    virtual void SetMaterialIndex(int val);
    virtual void SetBiNormals(CVec3 * val);
    virtual void SetSemanticCount(int val);
    virtual void SetRootVerts(CVec3 * val);
    virtual void SetName(std::string val);
    virtual void SetTangent(CVec3 * val);
    virtual void SetVertexCount(int val);
    virtual void SetUVs(TexCoord * val);
    virtual void SetBufferSize(int val);
    virtual void SetVerts(CVec3 * val);
    virtual void SetFaceCount(int val);
    //! \za³adowanie faców w nasyzm pryzpadku danymi z fmesha
    virtual void SetFaces(SFace* val);
    virtual void SetSkin(SSkin* val);

    //virtual bool DataIO(class CDataTransfer& IO);
    virtual bool Inicialize();

    virtual int FrustumSelect(CVec3 Normals[4], CVec3 Points[8]);
    virtual int LineSelect(const CVec3 &LP1, const CVec3 &LP2);
    virtual int& GetMaterialIndex();
    virtual int& GetSemanticCount();
    virtual int& GetVertexCount();
    virtual int& GetBufferSize();
    virtual int& GetFaceCount();

    virtual unsigned char * GetBuffer() const;
    virtual unsigned char* GetClsid();
    virtual std::string GetName() const;

    virtual CVec3 * GetRootVertNormals() const;
    virtual CVec3 * GetVertNormals() const;
    virtual CVec3 * GetBiNormals() const;
    virtual CVec3 * GetRootVerts() const;
    virtual CVec3 * GetTangent() const;
    virtual CVec3 * GetVerts() const;

    virtual SVertexElement* GetSementicVertexFormat() const;
    virtual TexCoord * GetUVs() const;
    virtual SFace* GetFaces() const;
    virtual SSkin* GetSkin() const;

    virtual int GetTrisCount() const;
    virtual void SetTrisCount(int val);

    virtual unsigned long * GetTris() const;
    virtual void SetTris(unsigned long * val);

    virtual float* GetVertex() const;
    virtual void SetVertex(float* val);

private:
    //! \inicjalizacja i wczytane danych DAE file - to wynikowy plik eksporty z 3ds prezz COLLADE
    void InicializeFromDAEFile();

    //! \inicjalizacja i sprawsowanie danych dla modelu Mesh z pliku *.fmesha
    void InicializeFromFMeshFile();

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

    int m_nbTris;
    unsigned long *m_pTris;
    float* vertex;

    CVec3 *m_pTriNormals;
    int *m_pTriFlags;
    eSelectionModes m_SelectMode;

    osg::ref_ptr<osg::Program> m_pProgram;
    osg::ref_ptr<osg::Shader> m_pShader;


    osg::Vec4Array* m_pInfluanceArray;
    osg::Vec4Array* m_pWeightArrary;
    osg::FloatArray* m_pNumberOfBoneArrary;
};


#endif //NEW_MESH_H
