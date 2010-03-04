#include "StdAfx.h"

FMesh::FMesh(void): 
  outputFile_(NULL)
, fileName_(DEFAULT_MODEL_FILE_NAME)
, outputDir_("")
{
  fileName_.append(".fmesh"); 
  OpenFile();
}

FMesh::FMesh(fm::string fileName, fm::string outputDir): 
  outputFile_(NULL)
, fileName_(fileName)
, outputDir_(outputDir)
{
  fileName_.append(".fmesh"); 
  OpenFile(); 
}

FMesh::~FMesh(void)
{
  CloseFile(); 
}

bool FMesh::Save(VisualScene *scene)
{
  size_t meshInstanceCount = scene->GetMeshInstanceCount(); 
  
  for(size_t i=0; i<meshInstanceCount; ++i)
  {
    MeshInstance *meshInstance = scene->GetMeshInstance(i); 
    SaveMesh(meshInstance->GetGeometryMesh()); 
  }
  
  // Zapisujemy szkielet 
  Skeleton *skeleton = scene->GetSkeleton(); 
  if(skeleton)
  {
    skeleton->Save(outputFile_); 
  }

  return true; 
}

void FMesh::OpenFile()
{
  // Otwieranie pliku 
  fm::string filename = outputDir_; 
  filename.append(fileName_); 
  outputFile_ = new FileChunk(filename.c_str()); 
}

void FMesh::CloseFile()
{
  // Zamykanie pliku 
  if(outputFile_) delete outputFile_; 
}

void FMesh::SaveMesh(GeometryMesh *geometryMesh)
{
  // Zapisywanie siatki 
  geometryMesh->Save(outputFile_); 
}