#pragma once

class FMesh
{
  FileChunk *outputFile_; 
  fm::string fileName_; 
  fm::string outputDir_; 
  void SaveMesh(GeometryMesh *geometryMesh); 
  void OpenFile(); 
  void CloseFile(); 

public:
  FMesh(void); 
  FMesh(fm::string fileName, fm::string outputDir);

  ~FMesh(void);

  bool Save(VisualScene *scene); 
  fm::string FileName() const { return fileName_; }

};
