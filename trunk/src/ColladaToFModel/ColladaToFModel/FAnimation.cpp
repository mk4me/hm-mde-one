#include "StdAfx.h"
#include "FAnimation.h"

FAnimation::FAnimation(void): 
  outputFile_(NULL)
, fileName_(DEFAULT_ANIMATION_FILE_NAME)
, outputDir_("")
{
  fileName_.append(".fanimation"); 
  OpenFile(); 
}

FAnimation::FAnimation(fm::string fileName, fm::string outputDir): 
  outputFile_(NULL)
, fileName_(fileName)
, outputDir_(outputDir)
{
  fileName_.append(".fanimation"); 
  OpenFile(); 
}

FAnimation::~FAnimation(void)
{
  CloseFile(); 
}

void FAnimation::OpenFile()
{
  // Otwieranie pliku 
  fm::string filename = outputDir_; 
  filename.append(fileName_); 
  outputFile_ = new FileChunk(filename.c_str()); 
}

void FAnimation::CloseFile()
{
  // Zamykanie pliku 
  if(outputFile_) delete outputFile_; 
}

void FAnimation::SaveAnimation(Skeleton *skeleton)
{
  skeleton->SaveAnimation(outputFile_); 
}