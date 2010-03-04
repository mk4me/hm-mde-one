#include "StdAfx.h"
#include "ModelExport.h"

ModelExport::ModelExport(void)
{
}

ModelExport::~ModelExport(void)
{
}

bool ModelExport::IfFileExist( fm::string outputDir, fm::string fileName )
{
  FILE *file = NULL; 
  bool fileExist = false; 
  fm::string fullFileName = outputDir; 
  fullFileName.append(fileName); 
  fullFileName.append(".fmodel"); 
  fopen_s(&file, fullFileName.c_str(), "r"); 
  if(file)
  { 
    fileExist = true; 
    fclose(file); 
  }
  return fileExist; 
}

bool ModelExport::Save(FCDocument *colladaDocument, fm::string outputDir) 
{
  // TODO: U¿yæ tej funkcji do odpowiedniego dostosowania jednostek ??
  //FCDocumentTools::StandardizeUpAxisAndLength(...)

  // Przechodzimy po wêz³ach sceny i eksportujemy kolejne elementy
  // Analizowanie sceny 
  VisualScene *scene = new VisualScene(colladaDocument);  
  FCDAsset *asset = colladaDocument->GetAsset(); 
  FUDateTime dateTime = asset->GetModifiedDateTime(); 

  fm::string fileName = Utils::GetFileNameFromPath(colladaDocument->GetFileUrl(), true); 
  clog << fileName.c_str() << "\n"; 
  // Zapisywanie geometrii 
  // TODO: Dodaæ odpowiednie nazywanie siatki oraz animacji 
  FMesh *fmesh = NULL; 
  if(scene->GetMeshInstanceCount() > 0)
  {
    fmesh = new FMesh(fileName, outputDir); 
    if( !fmesh->Save(scene) )
    {
      delete fmesh; 
      return false; 
    }
  }
 
  FAnimation *fanimation = SaveAnimations(scene, fileName, outputDir); 

  // Zapisywanie pliku FModel 
  if( (scene->GetMeshInstanceCount() > 0) && ((!IfFileExist(outputDir, fileName)) || Parameters::ForceOverwriteFModel()) )
  {
    FModel *fmodel = new FModel(); 
    fmodel->Save(fileName, outputDir, scene, fmesh, fanimation); 
    if(fmodel) delete fmodel; 
  }

  if(scene) delete scene; 
  if(fmesh) delete fmesh; 
  if(fanimation) delete fanimation; 

  return true; 
}

FAnimation *ModelExport::SaveAnimations( VisualScene * scene, fm::string fileName, fm::string outputDir )
{
  //*
  FAnimation *fanimation = NULL; 
  Skeleton *skeleton = scene->GetSkeleton(); 
  if(skeleton)
  {
    // TODO: Dodaæ nazywanie pliku wyjœciowego animacji!!
    fanimation = new FAnimation(fileName, outputDir); 
    fanimation->SaveAnimation(skeleton); 
  }
  return fanimation; 
}