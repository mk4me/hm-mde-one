#pragma once

template<class T>
inline T& maximum(T& x, T& y) { return (x > y) ? x : y; }
template<class T>
inline const T& maximum(const T& x, const T& y) { return (x > y) ? x : y; } /**< See above. */

/** Retrieves the smallest of two values. */
template <class T>
inline T& minimum(T& x, T& y) { return (x < y) ? x : y; }
template <class T>
inline const T& minimum(const T& x,const T& y) { return (x < y) ? x : y; } /**< See above. */

class Utils
{
public:
  Utils(void);
  ~Utils(void);

  static fm::string GetFileNameFromPath(const fm::string path, bool removeFileExtension=false)
  {
    char separator[] = "/"; 
    if(path.find(separator) == path.npos)
    {
      separator[0] = '\\'; 
    }

    size_t lastOfSeparator = path.find_last_of(separator); 
    if(lastOfSeparator == path.npos)
      lastOfSeparator = 0; 
    else
      ++lastOfSeparator; 

    if(!removeFileExtension)
    {
      return path.substr( lastOfSeparator ); 
    }
    else
    {
      size_t lastOfdot = path.find_last_of("."); 
      return path.substr( lastOfSeparator, lastOfdot - lastOfSeparator ); 
      //return path.substr( path.find_last_of(separator)+1, path.find_last_of(".") ); 
    }
    return fm::string(""); 
  }

  static fm::string GetDirNameFromPath(const fm::string path)
  {
    char separator[] = "/"; 
    if(path.find(separator) == path.npos)
    {
      separator[0] = '\\'; 
    }

    size_t lastOfSeparator = path.find_last_of(separator); 
    if(lastOfSeparator == path.npos)
      lastOfSeparator = 0; 
    else
      ++lastOfSeparator; 

    return path.substr( 0, lastOfSeparator ); 
  }

  static void StandardizePath(fm::string &path)
  {
    char separator[] = "/"; 
    if(path.find(separator) == path.npos)
    {
      separator[0] = '\\'; 
    }

    if(path.back() != separator[0])
    {
      path.append(separator); 
    } 
  }

  static fm::string ChangeFileType(fm::string path, fm::string newFileType)
  {
    if(newFileType == "")
      return path; 
    size_t dot = path.find_last_of("."); 
    fm::string newPath = path.substr(0, dot);
    newPath.append("."); 
    newPath.append(newFileType);
    return newPath; 
  }

  static uint32 CompressSortedVector(FMVector3& toInsert, FloatList& insertedList, UInt32List& compressIndexReferences); 

  /** Generates the texture tangents and binormals for a given source of texture coordinates.
  A source of normals and a source of vertex positions will be expected.
  @param mesh The mesh that contains the texture coordinate source.
  @param texcoordSource The source of texture coordinates that needs its tangents and binormals generated.
  @param generateBinormals Whether the binormals should also be generated.
  Do note that the binormal is always the cross-product of the tangent and the normal at a vertex-face pair. */
  static void GenerateTextureTangentBasis(FCDGeometryMesh* mesh, FCDGeometrySource* texcoordSource, bool generateBinormals = true);
};
