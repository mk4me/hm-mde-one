#pragma once

class MeshInstance
{
  // Dane o geometrii 
  GeometryMesh *geometryMesh_; 
  // Transformacja globalna siatki 
  FMMatrix44 worldTransform_; 
  // Materia³y przypisane do instancji geometrii 
  fm::vector<MaterialInstance> materials_; 

  void GetTextures(); 
  void InitializeMaterials( FCDGeometryInstance * geometryInstance ); 
  void Initialize(); 
  void Clear();

public:
  MeshInstance(void);
  MeshInstance(FCDGeometryInstance *meshInstance, FMMatrix44 &worldTransform);
  MeshInstance(FCDControllerInstance *controllerInstance, Skeleton *skeleton, FCDGeometry *geometry, FMMatrix44 &worldTransform); 
  ~MeshInstance(void);
  GeometryMesh *GetGeometryMesh() const { return geometryMesh_; }
  size_t GetMaterialCount(); 
  MaterialInstance *GetMaterial(size_t i); 
};
