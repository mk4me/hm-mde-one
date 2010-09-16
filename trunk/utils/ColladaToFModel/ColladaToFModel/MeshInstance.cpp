#include "StdAfx.h"
#include "MeshInstance.h"

MeshInstance::MeshInstance(void):
  geometryMesh_(NULL)
, worldTransform_(FMMatrix44::Identity)
{
}

MeshInstance::MeshInstance(FCDGeometryInstance *geometryInstance, FMMatrix44 &worldTransform): 
  geometryMesh_(NULL)
, worldTransform_(worldTransform)
{
  Clear(); 
  FCDGeometry *geometry = (FCDGeometry *)geometryInstance->GetEntity(); 
  geometryMesh_ = new GeometryMesh( geometry->GetMesh(), worldTransform ); 
  InitializeMaterials(geometryInstance);
}

MeshInstance::MeshInstance(FCDControllerInstance *controllerInstance, Skeleton *skeleton, FCDGeometry *geometry, FMMatrix44 &worldTransform): 
  geometryMesh_(NULL)
, worldTransform_(worldTransform)
{
  Clear(); 
  geometryMesh_ = new GeometryMesh( geometry->GetMesh(), controllerInstance, skeleton, worldTransform ); 
  InitializeMaterials(controllerInstance);
}

void MeshInstance::Clear()
{
  materials_.clear(); 
}

MeshInstance::~MeshInstance(void)
{
  Clear(); 
  if (geometryMesh_) 
    delete geometryMesh_; 
}

void MeshInstance::InitializeMaterials( FCDGeometryInstance * geometryInstance )
{
  size_t materialCount = geometryInstance->GetMaterialInstanceCount(); 
  for (size_t i=0; i<materialCount; ++i)
  {
    // TODO: Sprawdziæ czy siê zwraca wszystko dobrze!!
    FCDMaterialInstance *materialInstance = geometryInstance->GetMaterialInstance(i); 
    FCDMaterial *material = materialInstance->GetMaterial(); 
    FCDEffect *effect = material->GetEffect(); 
    FCDEffectProfile *effectProfile = effect->FindProfile(FUDaeProfileType::COMMON); 
    if (effectProfile)
    {
      FCDEffectStandard *standardEffect = (FCDEffectStandard *)effectProfile; 
      MaterialInstance *material = new MaterialInstance(); 
      material->Name(materialInstance->GetMaterial()->GetName()); 
      // Wyci¹gamy diffuse 
      size_t diffuseCount = standardEffect->GetTextureCount(FUDaeTextureChannel::DIFFUSE);  
      for (size_t j=0; j<diffuseCount; ++j)
      {
        // Sprawdziæ te materia³y 
        FCDTexture *texture = standardEffect->GetTexture(FUDaeTextureChannel::DIFFUSE, j); 
        FCDImage *image = texture->GetImage();  
        fstring name = Utils::GetFileNameFromPath(image->GetFilename()); 
        material->DiffuseMap(name); 
        name.clear(); 
        //clog << diffuseName.c_str() << "\n"; 
      }
      // Wyci¹gamy specular 
      size_t specularCount = standardEffect->GetTextureCount(FUDaeTextureChannel::SPECULAR);  
      for (size_t j=0; j<specularCount; ++j)
      {
        // Sprawdziæ te materia³y 
        FCDTexture *texture = standardEffect->GetTexture(FUDaeTextureChannel::SPECULAR, j); 
        FCDImage *image = texture->GetImage();  
        fstring name = Utils::GetFileNameFromPath(image->GetFilename()); 
        material->SpecularMap(name); 
        name.clear(); 
        //clog << diffuseName.c_str() << "\n"; 
      }
      // Wyci¹gamy bump 
      size_t normalBumpCout = standardEffect->GetTextureCount(FUDaeTextureChannel::BUMP);  
      for (size_t j=0; j<normalBumpCout; ++j)
      {
        // Sprawdziæ te materia³y 
        FCDTexture *texture = standardEffect->GetTexture(FUDaeTextureChannel::BUMP, j); 
        FCDImage *image = texture->GetImage();  
        fstring name = Utils::GetFileNameFromPath(image->GetFilename()); 
        material->AddNormalBumpMap(name); 
        name.clear(); 
        //clog << diffuseName.c_str() << "\n"; 
      }
      materials_.push_back(*material); 
    }

  }
}

void MeshInstance::Initialize()
{
  // Inicjalizacja...
}

size_t MeshInstance::GetMaterialCount() 
{
  return materials_.size(); 
}

MaterialInstance *MeshInstance::GetMaterial(size_t i)
{
  if(i<materials_.size())
    return &materials_[i]; 
  else 
    return NULL; 
}

