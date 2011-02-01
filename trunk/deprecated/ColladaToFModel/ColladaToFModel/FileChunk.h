#pragma once

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
// GUID'y plugin'ów
//-------------------------------------------------------------------------------
// BoneMotion - D15F30AB-C8AB-41C8-9C15-A4F775322487
static const GUID IID_BoneMotion = 
{ 0xD15F30AB, 0xC8AB, 0x41C8, { 0x9C, 0x15, 0xA4, 0xF7, 0x75, 0x32, 0x24, 0x87 } };

//------------------------------------------------------------------------------- 
// Rozmiary
//-------------------------------------------------------------------------------
enum sizes
{
  INT_SIZE = 4,
  SHORT_INT_SIZE = 2,
  WORD_SIZE = 2,
  FLOAT_SIZE = 4,
  DOUBLE_SIZE = 8,
  CHAR_SIZE = 1,
  // rozmiary pojedynczych danych w kawa³kach
  VERT_DATA_SIZE = 12,	  // 3*FLOAT_SIZE
  NORMAL_DATA_SIZE = 12,	// 3*FLOAT_SIZE
  TVERT_DATA_SIZE = 8,	  // 2*FLOAT_SIZE
  FACE_DATA_SIZE = 40,	  // 3 indeksy na wierz, 3 na TVerts, 3 na normalne, 
  // 1 na ID Materia³u (wszystko to s¹ DWORD'y - 4 bajty)
  HEADER_SIZE = 6,		    // ID + chunk size (2+4)
  HEADER_SIZE_EXP = 10,	  // ID + chunk size + number of elements in chunk (3*4)
  GUID_SIZE = 16,
};

// Nag³ówek chunk'a
struct Header
{
  short unsigned int chType;
  size_t size;
  fpos_t headerPos;
  size_t BytesRead;
};

// Klasa zajmuj¹ca siê zapisem do pliku oraz zarz¹dzaniem chunk'ami
class FileChunk
{
  // Nazwa pliku
  TCHAR *pFileName_; 
  // WskaŸnik na plik
  FILE *pFile_; 
  // Stos z chunk'ami
  std::stack<Header> chunkStack_; 
  // Aktualnie obrabiany kawa³ek
  Header *pCurrentChunk_; 
  
public:
  FileChunk(const TCHAR *name);
  ~FileChunk(void);
  // Rozpoczyna chunk'a
  void BeginChunk(short unsigned int chType);
  // Koñczy chunk'a
  void EndChunk(short unsigned int chType=0);
  // Zapisuje Int'a
  void WriteInt(int data); 
  // Zapisuje Int'a
  void WriteInt(size_t data);
  // Zapisuje Float'a
  void WriteFloat(float data);
  // Zapisuje double'a
  void WriteDouble(double data); 
  // Zapisuje short int'a
  void WriteShortInt(short int data);
  // Zapisuje GUID
  void WriteGUID(GUID data);
  // Zapisuje bajt danych
  void WriteByte(BYTE data);
  // Zapisuje string
  void WriteString(const TCHAR * stri);
  // Zapisuje okreœlon¹ iloœæ danych
  void WriteData(void * data, unsigned int size);
  // Zapisuje wektor 3d
  void WritePoint3(const FMVector3 &data);
  void WritePoint2(const FMVector2 &data); 
  // Zapisuje kwaternion
  void WriteQuaternion(const FMQuaternion &data);
};
