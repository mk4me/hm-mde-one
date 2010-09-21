#ifndef FILECHUNKREADER_H
#define FILECHUNKREADER_H
#pragma once

// System 
#include <stdio.h>
#include <stdlib.h>

// STL
#include <iostream>
#include <stack>
//#include <string>
#include <vector>
#include <stdlib.h>
#include "../OsgExtensions/Enumerators.h"

typedef unsigned char byte;


//osg
#include <osg/ref_ptr>

struct SModelData;
struct SSkeleton;
struct SSkeletonAnimation;
struct SFModel;
struct SFAnimation;

class Model;

/*
// Klasa trzyma stos (!) kawa³ków które s¹ w u¿yciu

// identyfikatory kawa³ków
#define FMODEL_HEADER             0x524E  // NR
#define FMODEL_MESH               0x484D  // MH
#define FMODEL_MESH_VERTICES      0x564D  // MV
#define FMODEL_MESH_TVERTICES     0x544D  // MT
#define FMODEL_MESH_NORMALS       0x4E4D  // MN
#define FMODEL_MESH_TANGENTS      0x474D  // MG 
#define FMODEL_MESH_BINORMALS     0x424D  // MB 
#define FMODEL_MESH_INDICES       0x464D  // MF
#define FMODEL_MESH_DATA          0x454D  // ME
#define FMODEL_MESH_DATA_2_1      0x324D  // M2
#define FMODEL_MESH_BUFFER        0x554D  // MU
#define FMODEL_MESH_MATERIALSET   0x534D  // MS
#define FMODEL_MESH_BOUNDINGBOX   0x4242  // BB
#define FMODEL_MATERIAL_LIBRARY   0x494C  // LI
#define FMODEL_MATERIAL           0x4C4D  // ML
#define FMODEL_TEXTURE            0x5854  // TX
#define FMODEL_STRING             0x5453  // ST
#define FMODEL_SKIN               0x4B53  // SK
#define FMODEL_MATRIX3            0x334D  // M3
#define FMODEL_MTX_PARTS          0x504D  // MP
#define FMODEL_BONES              0x4E42  // BN 
#define FMODEL_SKELETON_ANIMATION 0x5341  // AS 
#define FMODEL_MESH_ANIMATION     0x4D41  // AM 
#define FMODEL_LOD_DATA           0x4C55  // UL
#define FMODEL_DOF                0x4644  // DF
#define FMODEL_MODIFIER           0x444D  // MD
#define FMODEL_PLUGIN             0x4750  // PG


// Aktywne stopnie swobody
#define FMD_DOF_ROTX    0x01 //(0000 0001)
#define FMD_DOF_ROTY    0x02 //(0000 0010)
#define FMD_DOF_ROTZ    0x04 //(0000 0100)
#define FMD_DOF_TRANSX    0x10 //(0001 0000)
#define FMD_DOF_TRANSY    0x20 //(0010 0000)
#define FMD_DOF_TRANSZ    0x40 //(0100 0000)

// Aktywne ograniczenia ruchu koœci
#define FMD_DOF_LOW_ROTX  0x0001  //(0000 0000 0000 0001)
#define FMD_DOF_LOW_ROTY  0x0002  //(0000 0000 0000 0010)
#define FMD_DOF_LOW_ROTZ  0x0004  //(0000 0000 0000 0100)
#define FMD_DOF_UPP_ROTX  0x0010  //(0000 0000 0001 0000)
#define FMD_DOF_UPP_ROTY  0x0020  //(0000 0000 0010 0000)
#define FMD_DOF_UPP_ROTZ  0x0040  //(0000 0000 0100 0000)

#define FMD_DOF_LOW_TRANSX  0x0100  //(0000 0001 0000 0000)
#define FMD_DOF_LOW_TRANSY  0x0200  //(0000 0010 0000 0000)
#define FMD_DOF_LOW_TRANSZ  0x0400  //(0000 0100 0000 0000)
#define FMD_DOF_UPP_TRANSX  0x1000  //(0001 0000 0000 0000)
#define FMD_DOF_UPP_TRANSY  0x2000  //(0010 0000 0000 0000)
#define FMD_DOF_UPP_TRANSZ  0x4000  //(0100 0000 0000 0000)


//------------------------------------------------------------------------------- 
// Rozmiary
//-------------------------------------------------------------------------------
enum sizes
{
    SIZES_INT_SIZE = 4,
    SHORT_INT_SIZE = 2,
    WORD_SIZE = 2,
    SIZES_FLOAT_SIZE = 4,
    SIZES_DOUBLE_SIZE = 8,
    SIZES_CHAR_SIZE = 1,
    // rozmiary pojedynczych danych w kawa³kach
    VERT_DATA_SIZE = 12,	  // 3*SIZES_FLOAT_SIZE
    NORMAL_DATA_SIZE = 12,	// 3*SIZES_FLOAT_SIZE
    TVERT_DATA_SIZE = 8,	  // 2*SIZES_FLOAT_SIZE
    FACE_DATA_SIZE = 40,	  // 3 indeksy na wierz, 3 na TVerts, 3 na normalne, 
    // 1 na ID Materia³u (wszystko to s¹ DWORD'y - 4 bajty)
    HEADER_SIZE = 6,		    // ID + chunk size (2+4)
    HEADER_SIZE_EXP = 10,	  // ID + chunk size + number of elements in chunk (3*4)
    GUID_SIZE = 16,
};*/


// Nag³ówek chunk'a
struct Header
{
    short unsigned int chType;
    size_t size;
    fpos_t headerPos;
    size_t BytesRead;
};

// Klasa zajmuj¹ca siê czytaniem z pliku oraz zarz¹dzaniem chunk'ami
class FileChunkReader
{
    // Nazwa pliku
    std::string m_pFileName; 
    // WskaŸnik na plik
    FILE *m_pFile; 
    FILE* m_pMeshFile;
    // Stos z chunk'ami
    std::stack<Header> m_chunkStack; 
    // Aktualnie obrabiany kawa³ek
    Header *m_pCurrentChunk; 

public:
    FileChunkReader(std::string name);
    ~FileChunkReader(void);

    bool LoadMesh(Model* model);
    bool LoadMeshVersion2(SModelData* fmesh);

    bool LoadSkeleton(SSkeleton* skeleton);
    void LoadSkeletonAnimation(SSkeletonAnimation* anim);
    void LoadAnimation();
    void ChangeFile(const std::string name);

    void SkipChunk();


    // Rozpoczyna chunk'a
    bool BeginChunk(int &fileSize);
    bool ReadNextChunk( short &header, int &chunkSize );

    bool ReadInt(int &data); 
    bool ReadString(char *stri , int size);

    bool ReadByte(byte &data);
    bool ReadBytes(byte &data, int size);

    bool ReadFloat(float &data);
    bool ReadShortInt(short int &data);






    // Zapisuje double'a
    void ReadDouble(double data); 

    // Zapisuje GUID
    void ReadGUID();
    // Zapisuje wektor 3d
    void ReadPoint3();
    void ReadPoint2(); 
    // Zapisuje kwaternion
    void ReadQuaternion();
};


//TODO: NOWA Wersja Chunk Readera - nied³ugo do poprawy - narazie ma³o istotne.

/*
struct SChunkHeader
{
    WORD  chunkID;
    WORD  chunkVersion;
    DWORD dataSize;
    DWORD subChunkCount;
};

class ChunkFile;

typedef vector<ChunkFile*> ChunkVector;

class ChunkFile
{
public:
    ChunkFile();
    ~ChunkFile();

    // Informacje o nag³ówku
    EChunkID::TYPE GetChunkType();
    EChunkVersion::TYPE GetChunkVersion();
    void SetChunkType(EChunkID::TYPE type);
    void SetChunkVersion(EChunkVersion::TYPE version);

    // Odczyt i zapis
    void Read(IModelStream* stream);
    void Write(IModelStream* stream);

    // Dane
    void AllocateDataBuffer(UINT size);
    uint GetDataBufferSize();
    void* GetDataBuffer();

    // Pobieranie, dodwanie i usuwanie chunków z hierarchii 
    ChunkFile* GetSubChunk(UINT index);
    uint GetSubChunkCount();
    void AddSubChunk(ChunkFile* chunk);
    void RemoveSubChunk(UINT index);
    void RemoveAllSubChunks();

private:
    void ReadHeader(IModelStream* stream);
    void ReadData(IModelStream* stream);
    void WriteHeader(IModelStream* stream);
    void WriteData(IModelStream* stream);

    void ClearHeader();

    SChunkHeader m_Header;
    ChunkVector m_SubChunks;
    void* m_DataBuffer;
    uint m_DataBufferSize;
};*/



#endif //FILECHUNKREADER_H


