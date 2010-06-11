#pragma once

class ModelExport
{
public:
  ModelExport(void);
  ~ModelExport(void);

  bool Save(FCDocument *colladaDocument, fm::string outputDir); 
  bool IfFileExist( fm::string outputDir, fm::string fileName ); 
private: 
  FAnimation *SaveAnimations( VisualScene * scene, fm::string fileName, fm::string outputDir ); 
};
