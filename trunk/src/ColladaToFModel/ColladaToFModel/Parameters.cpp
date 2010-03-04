#include "StdAfx.h"
#include "Parameters.h"

fm::vector<fm::string> Parameters::fileNames_; 
fm::string Parameters::outputDir_ = TO_STRING(""); 
fm::string Parameters::textureFileType_ = TO_STRING(""); 
bool Parameters::isVerboseMode_ = false; 
bool Parameters::forceOverwriteFModel_ = false; 

Parameters::Parameters(void)
{
}

Parameters::~Parameters(void)
{
}

// Nazwy plików 
size_t Parameters::GetFileCount() 
{
  return fileNames_.size();
}

fm::string Parameters::GetFile(size_t idx) 
{
  return fileNames_[idx]; 
}

int __cdecl _tmkdir(const _TCHAR *path)
{
  ULONG dosretval;

  /* ask OS to create directory */
  if (!CreateDirectory((LPTSTR)path, (LPSECURITY_ATTRIBUTES)NULL))
    dosretval = GetLastError();
  else
    dosretval = 0;

  /*
  if (dosretval) {
    // error occured -- map error code and return 
    _dosmaperr(dosretval);
    return -1;
  }/**/

  return 0;
}

fm::string Parameters::GetOutputDir()
{
  _tmkdir(outputDir_.c_str()); 
  return outputDir_; 
}

bool Parameters::IsVerboseMode() 
{
  return isVerboseMode_; 
}

bool Parameters::ForceOverwriteFModel() 
{ 
  return forceOverwriteFModel_; 
}

void Parameters::ShowHelp()
{
  fm::string help = TO_STRING("> help needed... \n"); 
  cout << help.c_str(); 
}

fm::string Parameters::GetTextureFileType()
{
  return textureFileType_; 
}

void Parameters::ProcessCommandLine(int argc, char* argv[])
{
  // Remove the first argument: the current application name...
  --argc; ++argv;
  while (argc > 0)
  {
    if (argv[0][0] == '-' || argv[0][0] == '/')
    {
      ++(argv[0]);
      if(!strcmp(argv[0], "v") && argc >= 2)
      {
        isVerboseMode_ = true; 
        --argc; ++argv;
        
        cout << "Verbose: true" << "\n"; 
      }
      else if(!strcmp(argv[0], "f") && argc >= 2)
      {
        forceOverwriteFModel_ = true; 
        --argc; ++argv;

        cout << "Force overwrite FModel files: true" << "\n"; 
      }
      else if(!strcmp(argv[0], "o") && argc >= 2)
      {
        outputDir_ = TO_STRING((const char*) *(argv+1)); 
        Utils::StandardizePath(outputDir_); 
        argc -= 2; argv += 2;

        //cout << "O: " << outputDir_.c_str() << "\n"; 
      }
      else if(!strcmp(argv[0], "i") && argc >= 2)
      {
        --argc; ++argv;
        while( (argc > 0) && (argv[0][0] != '-') && (argv[0][0] != '/') )
        {
          fm::string filename = TO_STRING((const char*) *(argv)); 
          fileNames_.push_back(filename); 
          --argc; ++argv;

          //cout << "I: " << filename.c_str() << "\n"; 
        }
      }
      else if(!strcmp(argv[0], "t") && argc >= 2)
      {
        textureFileType_ = TO_STRING((const char*) *(argv+1)); 
        //Utils::StandardizePath(textureFileType_); 
        argc -= 2; argv += 2;

        //cout << "T: " << textureFileType_.c_str() << "\n"; 
      }
      else
      {
        // unknown flag
        ShowHelp(); 
        break; 
      }
    }
    else
    {
      ShowHelp(); 
      break;
    }
  }
}

