// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

// System 
#include <stdio.h>
#include <tchar.h>
#include <stdlib.h>

// STL
#include <iostream>
#include <stack>
//#include <string>
#include <vector>
// Collada 
#include <FCollada.h> 
#include <FMath/FMArray.h>
#include <FMath/FMath.h>
#include <FMath/FMQuaternion.h>
#include <FUtils/FUtils.h>
#include <FUtils/FUUri.h>
#include <FUtils/FUDateTime.h>
#include <FCDocument/FCDAsset.h>
#include <FCDocument/FCDEntityInstance.h>
#include <FCDocument/FCDGeometrySource.h>
#include <FCDocument/FCDGeometryPolygons.h>
#include <FCDocument/FCDGeometryPolygonsInput.h>
#include <FCDocument/FCDGeometryMesh.h>
#include <FCDocument/FCDGeometry.h>
#include <FCDocument/FCDGeometryInstance.h>
#include <FCDocument/FCDGeometryPolygonsTools.h>
#include <FCDocument/FCDController.h>
#include <FCDocument/FCDControllerInstance.h>
#include <FCDocument/FCDSkinController.h>
#include <FCDocument/FCDAnimationKey.h>
#include <FCDocument/FCDAnimationCurve.h>
#include <FCDocument/FCDAnimationChannel.h>
#include <FCDocument/FCDAnimation.h>
#include <FCDocument/FCDAnimated.h>
#include <FCDocument/FCDSceneNodeTools.h>
#include <FCDocument/FCDLibrary.h>
#include <FCDocument/FCDocument.h>
#include <FCDocument/FCDocumentTools.h>
#include <FCDocument/FCDSceneNode.h>
#include <FCDocument/FCDMaterial.h>
#include <FCDocument/FCDMaterialInstance.h>
#include <FCDocument/FCDEffect.h>
#include <FCDocument/FCDEffectProfile.h>
#include <FCDocument/FCDEffectStandard.h>
#include <FCDocument/FCDTexture.h>
#include <FCDocument/FCDImage.h>

// Namespace
using namespace std; 
using namespace FMath; 
using namespace fm; 

// Utils 
#include "Logger.h"
#include "Utils.h"
#include "Parameters.h"
#include "XMLUtility.h"

// Sta³e 
const float MAX_REAL = (float)3.402823466e+38f;
//const float MIN_REAL = (float)1.175494351e-38f;

//-------------------------------------------------------------------------------
// Typy
//-------------------------------------------------------------------------------


//-------------------------------------------------------------------------------
// Struktury 
//------------------------------------------------------------------------------

enum PrimitiveType
{
  ptPointList      = 1L,
  ptLineList       = 2L,
  ptLineStrip      = 3L,
  ptTriangleList   = 4L,
  ptTriangleStrip  = 5L,
  ptTriangleFan    = 6L
};

//------------------------------------------------------------------------------
//
enum VertexSemanticType
{
  Position = 0L,
  BlendWeight = 1L,
  BlendIndices = 2L,
  Normal = 3L,
  Color0 = 4L,
  Color1 = 5L,
  PointSize = 6L,
  TexCoord0 = 7L,
  TexCoord1 = 8L,
  TexCoord2 = 9L,
  TexCoord3 = 10L,
  TexCoord4 = 11L,
  TexCoord5 = 12L,
  TexCoord6 = 13L,
  TexCoord7 = 14L,
  Tangent = 15L,
  Binormal = 16L,
  PositionT = 17L,
  Fog = 18L,
  Depth = 19L,
  // ...
  Mass = 32L,
  Velocity = 33L,
  Force = 34L
};

enum VertexType
{
  Unused = 0L,
  Float1 = 1L,
  Float2 = 2L,
  Float3 = 3L,
  Float4 = 4L,
  PackedColor = 5L,
  UByte4 = 6L,
  Short2 = 7L,
  Short4 = 8L,
  UByte4N = 9L,
  Short2N = 10L,
  Short4N = 11L,
  HalfFloat2 = 12L,
  HalfFloat4 = 13L
};

// Materia³ przypisany do geometrii, czyli jak na razie tekstury 
class MaterialInstance
{
private:
  fm::string name_; 
  fm::string diffuseMap_; 
  fm::string specularMap_; 
  fm::vector<fm::string> normalBumpMaps_; 

  inline void Clear()
  {
    diffuseMap_.clear(); 
    specularMap_.clear(); 
    size_t normalBumpMapCount = normalBumpMaps_.size(); 
    for(size_t i=0; i<normalBumpMapCount; ++i)
    {
      normalBumpMaps_[i].clear(); 
    }
    normalBumpMaps_.clear(); 
  }
public: 
  MaterialInstance()
  {
    Clear(); 
  }
  ~MaterialInstance()
  {
    Clear(); 
  }
  fm::string Name() const { return name_; }
  void Name(fm::string val) { name_ = val; }
  fm::string DiffuseMap() const { return diffuseMap_; }
  void DiffuseMap(fm::string val) { diffuseMap_ = Utils::ChangeFileType(val, Parameters::GetTextureFileType()); }
  fm::string SpecularMap() const { return specularMap_; }
  void SpecularMap(fm::string val) { specularMap_ = Utils::ChangeFileType(val, Parameters::GetTextureFileType()); }
  void AddNormalBumpMap(fm::string normalBumpMap) { normalBumpMaps_.push_back(Utils::ChangeFileType(normalBumpMap, Parameters::GetTextureFileType())); }
  size_t GetNormalBumpMapCount() { return normalBumpMaps_.size(); }
  fm::string GetNormalBumpMap(size_t i) { if( i< normalBumpMaps_.size() ) return normalBumpMaps_[i]; return fm::string("");   }

  bool compare(MaterialInstance *m2); 
};

// Converter 
#include "FileChunk.h"
#include "Skeleton.h"
#include "GeometryPolygons.h"
#include "GeometryMesh.h"
#include "MeshInstance.h"

typedef fm::vector<MeshInstance*> GeometryMeshInstanceVector; 

#include "VisualScene.h"
#include "FMesh.h"
#include "FAnimation.h"
#include "FModel.h"
#include "ModelExport.h"

#define DEFAULT_SHADER "std-model"
//#define DEFAULT_SHADER_SKINNED "std-model-skinned"
#define DEFAULT_SHADER_UNBUMPED "std-model-unbumped"
#define FMODEL_VERSION 250
#define COLLADA_CONVERTER_VERSION "0.9.01" 
#define DEFAULT_MODEL_FILE_NAME "default"
#define DEFAULT_ANIMATION_FILE_NAME "default_anim"

extern LogStdout g_stdlog; 

#define clog LOG(g_stdlog)
