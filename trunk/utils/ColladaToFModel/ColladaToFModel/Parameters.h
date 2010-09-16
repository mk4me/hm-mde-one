#pragma once

class Parameters
{
    static fm::vector<fm::string> fileNames_; 
    static fm::string outputDir_; 
    static bool isVerboseMode_; 
    static bool forceOverwriteFModel_; 
    static fm::string textureFileType_; 
    static float scale_; 
public:
    Parameters(void);
    ~Parameters(void);

    // Nazwy plików 
    static size_t GetFileCount(); 
    static fm::string GetFile(size_t idx);
    static fm::string GetOutputDir(); 
    static fm::string GetTextureFileType(); 
    static float GetScale(); 
    static bool IsVerboseMode(); 
    static bool ForceOverwriteFModel(); 
    static void ShowHelp(); 

    static void ProcessCommandLine(int argc, char* argv[]);
};
