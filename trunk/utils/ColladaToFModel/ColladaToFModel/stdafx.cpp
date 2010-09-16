// stdafx.cpp : source file that includes just the standard includes
// ColladaToFModel.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file
LogStdout g_stdlog; 

bool MaterialInstance::compare(MaterialInstance *m2)
{
  if(diffuseMap_ != m2->DiffuseMap()) return false; 
  if(specularMap_ != m2->SpecularMap()) return false; 
  if(normalBumpMaps_.size() != m2->GetNormalBumpMapCount()) return false; 
  for(size_t i=0; i<normalBumpMaps_.size(); ++i)
  {
    if(normalBumpMaps_[i] != m2->GetNormalBumpMap(i)) return false; 
  }
  return true; 
}