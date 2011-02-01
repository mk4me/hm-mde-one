#pragma once

// XML helper class		
class CCoInitialize {
public:
  CCoInitialize() : m_hr(CoInitialize(NULL)) { }
  ~CCoInitialize() { if (SUCCEEDED(m_hr)) CoUninitialize(); }
  operator HRESULT() const { return m_hr; }
  HRESULT m_hr;
};

class FModel
{
public:
  FModel(void);
  ~FModel(void); 

  void Save(fm::string fileName, fm::string outputDir, VisualScene *scene, FMesh *mesh, FAnimation *animation); 
};
