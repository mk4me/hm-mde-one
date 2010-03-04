#pragma once

struct Face 
{
  union
  {
    uint16 data[3]; 
    struct 
    {
      uint16 _0; 
      uint16 _1; 
      uint16 _2; 
    };
  }; 

  Face(): _0(0), _1(0), _2(0) {}
  Face(uint16 _A0, uint16 _A1, uint16 _A2): _0(_A0), _1(_A1), _2(_A2) {}
}; 

// Identyfikator krójk¹ta 
struct FaceIndex 
{
  // Indeks face'a 
  size_t i; 
  // Naro¿nik w którym znajduje siê face
  uint16 c; 

  FaceIndex(): i(0), c(0) {}
  FaceIndex(size_t _i, uint16 _c): i(_i), c(_c) {}
};

// Dane dodatkowe wierzcho³ka 
struct VertexData
{
  fm::vector<FMVector2> textureCoordinate; 
  FMVector3 normal; 
  FMVector3 binormal; 
  FMVector3 tangent; 

  // Face'y w których znajduje siê dany wierzcho³ek
  fm::vector<FaceIndex> facesIndex; 

  VertexData(): 
  normal(MAX_REAL, MAX_REAL, MAX_REAL)
    , binormal(MAX_REAL, MAX_REAL, MAX_REAL)
    , tangent(MAX_REAL, MAX_REAL, MAX_REAL)
  {
    textureCoordinate.clear();
    facesIndex.clear(); 
  }

  ~VertexData()
  {
    textureCoordinate.clear(); 
    facesIndex.clear(); 
  }
};

inline bool operator==(VertexData &vc1, VertexData &vc2)
{
  if( !(IsEquivalent(vc1.normal, vc2.normal)) )
    return false; 
  if( !(IsEquivalent(vc1.binormal, vc2.binormal)) )
    return false; 
  if( !(IsEquivalent(vc1.tangent, vc2.tangent)) )
    return false; 

  for(size_t i=0; i<vc1.textureCoordinate.size(); ++i)
  {
    bool eq = false; 
    for(size_t j=0; j<vc2.textureCoordinate.size(); ++j)
    {
      if( IsEquivalent(vc1.textureCoordinate[i], vc2.textureCoordinate[j]) )
      {
        eq = true; 
        break; 
      }
    }
    if(!eq)
      return false; 
  }

  return true; 
}

// Wp³yw koœci na wierzcho³ek 
struct SkinData
{
  SkinData(): boneID_(-1), weight_(0.0f)
  {}
  SkinData(int boneID, float weight, FMMatrix44 bindPose): boneID_(boneID), weight_(weight), bindPose_(bindPose)
  {}
  // Indeks koœci wp³ywaj¹cej na wierzcho³ek
  int boneID_; 
  // Wielkoœæ wp³ywu na wierzcho³ek
  float weight_; 
  // Macierz do transformacji 
  FMMatrix44 bindPose_; 
};

// Kandydat na wierzcho³ek 
struct VertexCandidate
{
  FMVector3 position; 

  // Dane wierzcho³ka: normalne (i s-ka) oraz wspólrzêdne tekstury. 
  fm::vector<VertexData> verticesData; 

  // Dane o skinie, czyli wp³yw koœci na wierzcho³ki   
  fm::vector<SkinData> skinData; 

  bool Includes(VertexData &vertexData)
  {
    size_t vertexDataCount = verticesData.size(); 
    for(size_t i=0; i<vertexDataCount; ++i)
    {
      if(verticesData[i] == vertexData)
        return true; 
    }
    return false; 
  }

  void AddFaceIndex(VertexData &vertexData, FaceIndex faceIndex)
  {    
    size_t vertexDataCount = verticesData.size(); 
    for(size_t i=0; i<vertexDataCount; ++i)
    {
      if(verticesData[i] == vertexData)
      {
        verticesData[i].facesIndex.push_back(faceIndex); 
      }
    }
  }

  VertexCandidate() : 
  position(MAX_REAL, MAX_REAL, MAX_REAL) 
  {
    clear(); 
  }

  ~VertexCandidate()
  {
    clear(); 
  }

  void clear()
  {
    position.x = 0.0f; position.y = 0.0f; position.z = 0.0f; 
    verticesData.clear(); 
    skinData.clear(); 
  }
};

class GeometryPolygons
{
  // 
  FCDGeometryPolygons *geometryPolygons_; 
  // Instancja kontrolera skina
  FCDControllerInstance *controllerInstance_; 
  // Kontroler skina 
  FCDSkinController *skinController_; 
  // Bufor wierzcho³ków w fazie poœredniej 
  fm::vector<VertexCandidate> vertices_; 
  size_t vertexCount_; 
  // Bufor trójk¹tów (polygonów)
  fm::vector<Face> faces_; 
  // Semantyka wierzcho³ków w tej siatce 
  fm::vector<VertexSemanticType> vertexSemantic_; 
  // Nazwa siatki 
  fm::string meshName_; 
  // Globalna macierz obiektu 
  FMMatrix44 worldTransform_; 
  // Szkielet 
  Skeleton *skeleton_; 
  // Inicjalizacja siatki 
  void Initialize(); 
  void InitializeBindPose(); 

  bool CheckSkinMeshCompatibility(); 

  void SaveMeshBuffer( FileChunk * pFile ); 
  void SaveMeshIndices( FileChunk * pFile ); 
  void SaveSkin( FileChunk * pFile ); 
  void SaveVertexSemantics(FileChunk * pFile); 

  size_t GetVertexSize(); 
  bool HasVertexType(VertexSemanticType type); 

  // Pobieranie wektora 3d z zadanego Ÿród³a geometrii 
  FMVector3 GetVector3(FCDGeometrySource *source, uint32 index); 
  FMVector2 GetVector2(FCDGeometrySource *source, uint32 index); 
  void NegateV(FCDGeometrySource *source); 

public:
  GeometryPolygons(void);
  GeometryPolygons(FCDGeometryPolygons *polygons, fm::string name, FMMatrix44 *worldTransform);
  GeometryPolygons(FCDGeometryPolygons *geometryPolygons, FCDControllerInstance *controllerInstance, Skeleton *skeleton, fm::string name, FMMatrix44 *worldTransform); 

  FMMatrix44 WorldTransform() const { return worldTransform_; }
  void WorldTransform(FMMatrix44 *val) { worldTransform_ = *val; }

  ~GeometryPolygons(void);

  // Zapisywanie kawa³ka z siatka 
  void Save(FileChunk *pFile); 

};
