#ifndef IMMESH_H
#define IMMESH_H

class IIMesh;
class IISubMesh;

class CIMesh;
class CISubMesh;

class CIMesh: public IIMesh
{
protected:
    std::vector<CISubMesh*> m_SubMeshes;
    std::string* m_OriginalId;

public:
    // Konstruktor i destruktor
    CIMesh();
    ~CIMesh();

public:
    unsigned int AddSubMesh(CISubMesh* subMesh);

public:
    // Implementacja interfejsu IITMMesh
    IISubMesh* GetSubMesh(unsigned int subMeshID);
    unsigned int GetSubMeshCount();
    float* GetTranslation();
    float* GetRotation();
    std::string* GetOriginalId();

    float m_Translation[3];				// translacja
    float m_Rotation[4];				// kwaternion rotacji
};

class CISubMesh: public IISubMesh
{
protected:
    // Bufory danych wierzch�k�w
    std::vector<float> m_Positions;      // Pozycja (x,y,z)
    std::vector<float> m_Normals;        // Normalna (x,y,z)
    std::vector<float> m_Tangents;       // Przestrze� tangent�w (x,y,z)
    std::vector<float> m_Binormals;      // 
    std::vector<float> m_Colors;         // Kolor (r,g,b,a)
    std::vector<float> m_UV0Coords;      // Koordynaty tekstury kana� 0 (u,v)
    std::vector<float> m_UV1Coords;      // Koordynaty tekstury kana� 1 (u,v)

    //		STRING m_OriginalId;

    // Bufor indeks�w
    std::vector<unsigned long> m_Indices;

    friend class CMeshWriter;

public:
    // Konstruktor i destruktor
    CISubMesh();
    ~CISubMesh();

public:
    // Implementacja interfejsu IITMSubMesh

    // Format wierzcho�ka
    bool HasNormals();
    bool HasTangents();
    bool HasBinormals();
    bool HasColors();
    bool HasUV0Coords();
    bool HasUV1Coords();

    // Rozmiar bufor�w
    unsigned int GetVertexCount();

    // Zwraca wska�nik na pierwszy element bufora
    float* GetPositions();
    float* GetNormals();
    float* GetTangents();
    float* GetBinormals();
    float* GetColors();
    float* GetUV0Coords();
    float* GetUV1Coords();

    unsigned int GetIndicesCount();
    unsigned long* GetIndices();
    //		STRING* GetOriginalId();
};


#endif