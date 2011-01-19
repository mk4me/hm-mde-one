// ColladaToFModel.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

void SaveFile( size_t i, fm::string &filename )
{
  //clog << "Processing: \"" << filename.c_str() << "\"" << "\n"; 
  FCDocument *document = FCollada::NewTopDocument(); 

  if( !FCollada::LoadDocumentFromFile(document, filename.c_str()) )
  {
    clog << "Error: Could not open the file \"" << filename.c_str() << "\"" << "\n"; 
    document->Release(); 
  }
  else 
  {
    //clog << "File \"" << filename.c_str() << "\" successfully loaded." << "\n"; 
    // Konwersja do FModel 
    fm::string outputDir = Parameters::GetOutputDir(); 
    if(outputDir == TO_STRING(""))
    {
      outputDir = Utils::GetDirNameFromPath(filename); 
    }

    if(!Parameters::IsVerboseMode())
      g_stdlog.setDispatchFlag(false);
    ModelExport *modelExport = new ModelExport(); 
    modelExport->Save(document, outputDir); 
    document->Release();
    delete modelExport; 

    g_stdlog.setDispatchFlag(true);
    clog << "File \"" << filename.c_str() << "\" successfully converted." << "\n"; 
  }
  g_stdlog.increment();  
  //clog << "\n"; 
}

int _tmain(int argc, _TCHAR* argv[])
{
  cout << "Collada to FModel converter. " << "Version: " << COLLADA_CONVERTER_VERSION << "." << "\n"; 
  Parameters::ProcessCommandLine(argc, argv); 
  FCollada::Initialize(); 
  size_t inputFileCount = Parameters::GetFileCount(); 
  if(inputFileCount<=0) Parameters::ShowHelp(); 

  for(size_t i=0; i<inputFileCount; ++i)
  {
    fm::string filename = Parameters::GetFile(i); 
    SaveFile(i, filename);
  }
	return 0;
}

