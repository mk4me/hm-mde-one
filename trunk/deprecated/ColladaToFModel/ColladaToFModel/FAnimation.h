#pragma once

class FAnimation
{
  FileChunk *outputFile_; 
  fm::string fileName_; 
  fm::string outputDir_; 

  void OpenFile(); 
  void CloseFile(); 
public:
  FAnimation(void);
  FAnimation(fm::string fileName, fm::string outputDir); 
  ~FAnimation(void);

  void SaveAnimation(Skeleton *skeleton); 
  fm::string FileName() const { return fileName_; }

};
