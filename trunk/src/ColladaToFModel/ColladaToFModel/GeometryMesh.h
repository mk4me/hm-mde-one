#pragma once

//-------------------------------------------------------------------------------
// Klasa reprezentuj¹ca jedn¹ siatkê trójk¹tów (która w efekcie mo¿e jednak daæ
// kilka wynikowych siatek)
//-------------------------------------------------------------------------------
class GeometryMesh
{
private: 
  // Siatka FCollady 
  FCDGeometryMesh *mesh_; 
  FCDControllerInstance *controllerInstance_; 
  Skeleton *skeleton_; 
  FMMatrix44 worldTransform_; 

  // 
  fm::vector<GeometryPolygons *> geometries_; 

  // Nazwa mesha 
  fm::string meshName_; 

  // Inicjalizacja siatki 
  void Initialize(void);
  void AdaptCoordinates(FCDGeometryMesh *mesh); 
  void NegateV(FCDGeometrySource *source); 

public:
  GeometryMesh(void);
  GeometryMesh(FCDGeometryMesh *fcmesh, FMMatrix44 &worldTransform);
  GeometryMesh(FCDGeometryMesh *fcmesh, FCDControllerInstance *controllerInstance, Skeleton *skeleton, FMMatrix44 &worldTransform);
  ~GeometryMesh(void);

  // Export 
  void Save(FileChunk *pFile); 
};
