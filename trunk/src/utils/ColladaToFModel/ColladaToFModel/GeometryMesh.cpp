#include "StdAfx.h"
#include "GeometryMesh.h"

GeometryMesh::GeometryMesh(void): 
  mesh_(NULL)
, worldTransform_(FMMatrix44::Identity)
{
  geometries_.clear(); 
}

GeometryMesh::GeometryMesh(FCDGeometryMesh *fcmesh, FMMatrix44 &worldTransform): 
  mesh_(fcmesh)
, controllerInstance_(NULL)
, worldTransform_(worldTransform)
{
  geometries_.clear(); 
  Initialize(); 
}

GeometryMesh::GeometryMesh(FCDGeometryMesh *fcmesh, FCDControllerInstance *controllerInstance, Skeleton *skeleton, FMMatrix44 &worldTransform): 
  mesh_(fcmesh)
, controllerInstance_(controllerInstance)
, skeleton_(skeleton)
, worldTransform_(worldTransform)
{
  geometries_.clear(); 
  Initialize(); 
}


GeometryMesh::~GeometryMesh(void)
{
  size_t geometryCount = geometries_.size(); 
  for(size_t i=0; i<geometryCount; ++i)
  {
    if(geometries_[i]) delete geometries_[i]; 
  }
  geometries_.clear(); 
}

// Inicjalizacja siatki 
void GeometryMesh::Initialize()
{
  // Poruszamy si� po tr�jk�tach i wyci�gamy poszczeg�lne dane dla poszczeg�lnych wierzcho�k�w 
  size_t polygonGroupCount = mesh_->GetPolygonsCount(); 

  if( polygonGroupCount <= 0 ) 
    clog << "Error: Za ma�o grup polygon�w...\n"; 
  
  // Pobieramy rodzica 
  FCDGeometry *geometry = mesh_->GetParent();
  meshName_ = geometry->GetName(); 

  //-------------------------------------------------------------------------------
  // Zmiany uk�adu wpo�rz�dnych wierzcho�k�w czy tekstur i jednocze�nie przeliczenie 
  // przestrzeni tanget�w (ze wzgl�du na zmiany tych ca�ych UVa�ek)
  AdaptCoordinates(mesh_); 
  //-------------------------------------------------------------------------------

  // Zapisujemy grupy polygon�w  
  for(size_t i=0; i<polygonGroupCount; ++i)
  {
    FCDGeometryPolygons *polygons = mesh_->GetPolygons(i); 
    fm::string name(meshName_); 

    if(polygonGroupCount > 1)
    {
      char buffer[64]; 
      // Dodajemy postfix 
      name.append("_"); 
      // TODO: Tu mo�e by� ewentualnie niebezpiecznie z size_t do int....
      _itoa_s((int)i, buffer, 64, 10); 
      name.append(buffer); 
    }
    
    GeometryPolygons *geometryPolygons = NULL; 
    if(controllerInstance_)
    {
      geometryPolygons = new GeometryPolygons(polygons, controllerInstance_, skeleton_, name, &worldTransform_); 
    }
    else
    {
      geometryPolygons = new GeometryPolygons(polygons, name, &worldTransform_); 
    }

    geometries_.push_back(geometryPolygons); 
  }
}

void GeometryMesh::Save(FileChunk *pFile) 
{
  // Zapisujemy poszczeg�lne siatki 
  size_t geometryCount = geometries_.size(); 
  for(size_t i=0; i<geometryCount; ++i)
  {
    geometries_[i]->Save(pFile); 
  }
  // Zapisujemy szkielet 

}

void GeometryMesh::AdaptCoordinates(FCDGeometryMesh *mesh)
{
  // Binormalne 
  FCDGeometrySource *binormalSource = NULL; 
  // Tangenty  
  FCDGeometrySource *tangentSource = NULL; 
  // Wsp�rz�dne tekstury 
  FCDGeometrySource *texcoordSource = NULL; 

  // Krok 1. Wywalamy �rod�a z tangentami i bitangentami oraz dostosowujemy UVki
  // HACK: Usuwane jest tylko JEDEN �r�d�o tangent i bitangent! Trzeba by to jako� zabezpieczy� na wypadek wiekszej ilo�ci... 
  size_t sourceCount = mesh->GetSourceCount(); 
  for(size_t i=0; i<sourceCount; ++i)
  {
    FCDGeometrySource *source = mesh->GetSource(i); 
    switch(source->GetType())
    {
    case FUDaeGeometryInput::TEXBINORMAL:
      binormalSource = source; 
      break;
    case FUDaeGeometryInput::TEXTANGENT:
      tangentSource = source; 
      break;
    case FUDaeGeometryInput::TEXCOORD:
      texcoordSource = source; 
      NegateV(texcoordSource); 
      break;
    default:
      break;
    }
  }
  SAFE_RELEASE(binormalSource); 
  SAFE_RELEASE(tangentSource); 
  // Krok 2. Obliczamy tangent space 
  Utils::GenerateTextureTangentBasis(mesh, texcoordSource, true); 
}

void GeometryMesh::NegateV(FCDGeometrySource *source)
{
  int stride = source->GetStride(); 
  float *data = source->GetData(); 
  size_t count = source->GetDataCount()/stride; 
  if(stride < 2)
  {
    clog << "B��d �r�d�a geometrii\n"; 
  }
  for(size_t i=0; i<count; ++i)
  {
    data[i*stride+1] = -data[i*stride+1]; 
  }
}