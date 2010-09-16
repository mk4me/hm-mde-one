#include "StdAfx.h"
#include "Utils.h"

Utils::Utils(void)
{
}

Utils::~Utils(void)
{
}

uint32 Utils::CompressSortedVector(FMVector3& toInsert, FloatList& insertedList, UInt32List& compressIndexReferences)
{
  // Look for this vector within the already inserted list.
  size_t start = 0, end = compressIndexReferences.size(), mid;
  for (mid = (start + end) / 2; start < end; mid = (start + end) / 2)
  {
    uint32 index = compressIndexReferences[mid];
    if (toInsert.x == insertedList[3 * index]) break;
    else if (toInsert.x < insertedList[3 * index]) end = mid;
    else start = mid + 1;
  }

  // Look for the tolerable range within the binary-sorted dimension.
  size_t rangeStart, rangeEnd;
  for (rangeStart = mid; rangeStart > 0; --rangeStart)
  {
    uint32 index = compressIndexReferences[rangeStart - 1];
    if (!IsEquivalent(insertedList[3 * index], toInsert.x)) break;
  }
  for (rangeEnd = minimum(mid + 1, compressIndexReferences.size()); rangeEnd < compressIndexReferences.size(); ++rangeEnd)
  {
    uint32 index = compressIndexReferences[rangeEnd];
    if (!IsEquivalent(insertedList[3 * index], toInsert.x)) break;
  }
  FUAssert(rangeStart < rangeEnd || (rangeStart == rangeEnd && rangeEnd == compressIndexReferences.size()), return 0);

  // Look for an equivalent vector within the tolerable range
  for (size_t g = rangeStart; g < rangeEnd; ++g)
  {
    uint32 index = compressIndexReferences[g];
    if (IsEquivalent(toInsert, *(const FMVector3*) &insertedList[3 * index])) return index;
  }

  // Insert this new vector in the list and add the index reference at the correct position.
  uint32 compressIndex = (uint32) (insertedList.size() / 3);
  compressIndexReferences.insert(compressIndexReferences.begin() + mid, compressIndex);
  insertedList.push_back(toInsert.x);
  insertedList.push_back(toInsert.y);
  insertedList.push_back(toInsert.z);
  return compressIndex;
}

struct TangentialVertex
{
  float* normalPointer;
  float* texCoordPointer;
  FMVector3 tangent;
  FMVector3 bitangent;
  uint32 count;
  uint32 tangentId;
  uint32 binormalId;
};
typedef fm::vector<TangentialVertex> TangentialVertexList;

// Generates the texture tangents and binormals for a given source of texture coordinates.
void Utils::GenerateTextureTangentBasis(FCDGeometryMesh* mesh, FCDGeometrySource* texcoordSource, bool generateBinormals)
{
  if (texcoordSource == NULL || mesh == NULL) return;

  // First count the positions.
  FCDGeometrySource* positionSource = mesh->FindSourceByType(FUDaeGeometryInput::POSITION);
  if (positionSource == NULL) return;
  size_t globalVertexCount = positionSource->GetValueCount();

  // Allocate the tangential vertices.
  // This temporary buffer is necessary to ensure we have smooth tangents/binormals.
  TangentialVertexList* globalVertices = new TangentialVertexList[globalVertexCount];
  memset(globalVertices, 0, sizeof(TangentialVertexList) * globalVertexCount);

  // This operation is done on the tessellation: fill in the list of tangential vertices.
  size_t polygonsCount = mesh->GetPolygonsCount();
  for (size_t i = 0; i < polygonsCount; ++i)
  {
    FCDGeometryPolygons* polygons = mesh->GetPolygons(i);

    // Verify that this polygons set uses the given texture coordinate source.
    FCDGeometryPolygonsInput* texcoordInput = polygons->FindInput(texcoordSource);
    if (texcoordInput == NULL) continue;

    // Retrieve the data and index buffer of positions/normals/texcoords for this polygons set.
    FCDGeometryPolygonsInput* positionInput = polygons->FindInput(FUDaeGeometryInput::POSITION);
    FCDGeometryPolygonsInput* normalsInput = polygons->FindInput(FUDaeGeometryInput::NORMAL);
    if (positionInput == NULL || normalsInput == NULL) continue;
    FCDGeometrySource* positionSource = positionInput->GetSource();
    FCDGeometrySource* normalsSource = normalsInput->GetSource();
    FCDGeometrySource* texcoordSource = texcoordInput->GetSource();
    if (positionSource == NULL || normalsSource == NULL || texcoordSource == NULL) continue;
    uint32 positionStride = positionSource->GetStride();
    uint32 normalsStride = normalsSource->GetStride();
    uint32 texcoordStride = texcoordSource->GetStride();
    if (positionStride < 3 || normalsStride < 3 || texcoordStride < 2) continue;
    uint32* positionIndices = positionInput->GetIndices();
    uint32* normalsIndices = normalsInput->GetIndices();
    uint32* texcoordIndices = texcoordInput->GetIndices();
    size_t indexCount = positionInput->GetIndexCount();
    if (positionIndices == NULL || normalsIndices == NULL || texcoordIndices == NULL) continue;
    if (indexCount == 0 || indexCount != normalsInput->GetIndexCount() || indexCount != texcoordInput->GetIndexCount()) continue;
    float* positionData = positionSource->GetData();
    float* normalsData = normalsSource->GetData();
    float* texcoordData = texcoordSource->GetData();
    size_t positionDataLength = positionSource->GetDataCount();
    size_t normalsDataLength = normalsSource->GetDataCount();
    size_t texcoordDataLength = texcoordSource->GetDataCount();
    if (positionDataLength == 0 || normalsDataLength == 0 || texcoordDataLength == 0) continue;

    // Iterate of the faces of the polygons set. This includes holes.
    size_t faceCount = polygons->GetFaceVertexCountCount();
    size_t faceVertexOffset = 0;
    for (size_t faceIndex = 0; faceIndex < faceCount; ++faceIndex)
    {
      size_t faceVertexCount = polygons->GetFaceVertexCounts()[faceIndex];
      if(faceVertexCount == 3)
      {
        FMVector3& v1 = *(FMVector3*)&positionData[positionIndices[faceVertexOffset] * positionStride];
        FMVector2& w1 = *(FMVector2*)&texcoordData[texcoordIndices[faceVertexOffset] * texcoordStride];

        FMVector3& v2 = *(FMVector3*)&positionData[positionIndices[faceVertexOffset + 1] * positionStride];
        FMVector2& w2 = *(FMVector2*)&texcoordData[texcoordIndices[faceVertexOffset + 1] * texcoordStride];

        FMVector3& v3 = *(FMVector3*)&positionData[positionIndices[faceVertexOffset + 2] * positionStride];
        FMVector2& w3 = *(FMVector2*)&texcoordData[texcoordIndices[faceVertexOffset + 2] * texcoordStride];

        // The formulae to calculate the tangent-space basis vectors is taken from Maya 7.0 API documentation:
        // "Appendix A: Tangent and binormal vectors".
        // Prepare the edge vectors.
        float x1 = v2.x - v1.x;
        float x2 = v3.x - v1.x;
        float y1 = v2.y - v1.y;
        float y2 = v3.y - v1.y;
        float z1 = v2.z - v1.z;
        float z2 = v3.z - v1.z;

        float s1 = w2.x - w1.x;
        float s2 = w3.x - w1.x;
        float t1 = w2.y - w1.y;
        float t2 = w3.y - w1.y;

        float r = 1.0F / (s1 * t2 - s2 * t1);

        FMVector3 tangent((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
        FMVector3 bitangent((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);

        for(size_t i=0; i<faceVertexCount; ++i)
        {
          float *texcoordPtr = &texcoordData[texcoordIndices[faceVertexOffset + i] * texcoordStride];
          float* normalPtr = &normalsData[normalsIndices[faceVertexOffset + i] * normalsStride];
          // Add this tangent to our tangential vertex.
          FUAssert(positionIndices[faceVertexOffset + i] < globalVertexCount, continue);
          TangentialVertexList& list = globalVertices[positionIndices[faceVertexOffset + i]];
          size_t vertexCount = list.size();
          bool found = false;
          for (size_t v = 0; v < vertexCount; ++v)
          {
            if ((normalPtr == list[v].normalPointer) && (texcoordPtr == list[v].texCoordPointer))
            {
              list[v].tangent += tangent;
              list[v].bitangent += bitangent;
              list[v].count++;
              found = true;
            }
          }
          if (!found)
          {
            TangentialVertex v;
            v.normalPointer = normalPtr;
            v.texCoordPointer = texcoordPtr;
            v.count = 1; 
            v.tangent = tangent; 
            v.bitangent = bitangent; 
            v.tangentId = v.binormalId = ~(uint32)0;
            list.push_back(v);
          }
        }
      }
      else
      {
        // Face nie ma 3ch wierzcho³ków!
        return; 
      }
      faceVertexOffset += faceVertexCount;
    }
  }

  FCDGeometrySource* tangentSource = NULL;
  FCDGeometrySource* binormalSource = NULL;
  FloatList tangentData;
  FloatList binormalData;
  UInt32List tangentCompressionIndices;
  UInt32List binormalCompressionIndices;

  // Iterate over the polygons again: this time create the source/inputs for the tangents and binormals.
  for (size_t i = 0; i < polygonsCount; ++i)
  {
    FCDGeometryPolygons* polygons = mesh->GetPolygons(i);

    // Verify that this polygons set uses the given texture coordinate source.
    FCDGeometryPolygonsInput* texcoordInput = polygons->FindInput(texcoordSource);
    if (texcoordInput == NULL) continue;

    // Retrieve the data and index buffer of positions/normals/texcoords for this polygons set.
    FCDGeometryPolygonsInput* positionInput = polygons->FindInput(FUDaeGeometryInput::POSITION);
    FCDGeometryPolygonsInput* normalsInput = polygons->FindInput(FUDaeGeometryInput::NORMAL);
    if (positionInput == NULL || normalsInput == NULL) continue;
    FCDGeometrySource* normalsSource = normalsInput->GetSource();
    FCDGeometrySource* texcoordSource = texcoordInput->GetSource();
    if (normalsSource == NULL || texcoordSource == NULL) continue;
    uint32 normalsStride = normalsSource->GetStride();
    uint32 texcoordStride = texcoordSource->GetStride();
    if (normalsStride < 3 || texcoordStride < 2) continue;
    uint32* positionIndices = positionInput->GetIndices();
    uint32* normalsIndices = normalsInput->GetIndices();
    uint32* texcoordIndices = texcoordInput->GetIndices();
    size_t indexCount = positionInput->GetIndexCount();
    if (positionIndices == NULL || normalsIndices == NULL || texcoordIndices == NULL) continue;
    if (indexCount == 0 || indexCount != normalsInput->GetIndexCount() || indexCount != texcoordInput->GetIndexCount()) continue;
    float* normalsData = normalsSource->GetData();
    float* texcoordData = texcoordSource->GetData();
    size_t normalsDataLength = normalsSource->GetDataCount();
    size_t texcoordDataLength = texcoordSource->GetDataCount();
    if (normalsDataLength == 0 || texcoordDataLength == 0) continue;

    // Create the texture tangents/binormals sources
    if (tangentSource == NULL)
    {
      tangentSource = mesh->AddSource(FUDaeGeometryInput::TEXTANGENT);
      tangentSource->SetDaeId(texcoordSource->GetDaeId() + "-tangents");
      tangentData.reserve(texcoordSource->GetDataCount());
      if (generateBinormals)
      {
        binormalSource = mesh->AddSource(FUDaeGeometryInput::TEXBINORMAL);
        binormalSource->SetDaeId(texcoordSource->GetDaeId() + "-binormals");
        binormalData.reserve(tangentSource->GetDataCount());
      }
    }

    // Calculate the next available offset
    uint32 inputOffset = 0;
    size_t inputCount = polygons->GetInputCount();
    for (size_t j = 0; j < inputCount; ++j)
    {
      inputOffset = maximum(inputOffset, polygons->GetInput(j)->GetOffset());
    }

    // Create the polygons set input for both the tangents and binormals
    FCDGeometryPolygonsInput* tangentInput = polygons->AddInput(tangentSource, inputOffset + 1);
    tangentInput->SetSet(texcoordInput->GetSet());
    tangentInput->ReserveIndexCount(indexCount);
    FCDGeometryPolygonsInput* binormalInput = NULL;
    if (binormalSource != NULL)
    {
      binormalInput = polygons->AddInput(binormalSource, inputOffset + 2);
      binormalInput->SetSet(tangentInput->GetSet());
      binormalInput->ReserveIndexCount(indexCount);
    }

    // Iterate of the faces of the polygons set. This includes holes.
    size_t vertexCount = positionInput->GetIndexCount();
    size_t faceVertexOffset = 0;
    for (size_t vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex)
    {
      // For each face-vertex pair retrieve the current vertex normal&texcoord.
      float* normalPtr = &normalsData[normalsIndices[faceVertexOffset + vertexIndex] * normalsStride];
      FMVector3& normal = *(FMVector3*)normalPtr;
      float* texcoordPtr = &texcoordData[texcoordIndices[faceVertexOffset + vertexIndex] * texcoordStride];

      FUAssert(positionIndices[faceVertexOffset + vertexIndex] < globalVertexCount, continue);
      TangentialVertexList& list = globalVertices[positionIndices[faceVertexOffset + vertexIndex]];
      size_t tangentianVertexCount = list.size();
      for (size_t v=0; v<tangentianVertexCount; ++v)
      {
        if ((list[v].normalPointer == normalPtr) && (list[v].texCoordPointer == texcoordPtr))
        {
          if (list[v].tangentId == ~(uint32)0)
          {
            
            // Take the normal vector at this face-vertex pair, out of the calculated tangent vector
            list[v].tangent = list[v].tangent - (normal*list[v].tangent)*normal;
            list[v].tangent.NormalizeIt();
            list[v].tangentId = CompressSortedVector(list[v].tangent, tangentData, tangentCompressionIndices);
          }
          tangentInput->AddIndex(list[v].tangentId);

          if (binormalInput != NULL)
          {
            if (list[v].binormalId == ~(uint32)0)
            {
              // Calculate and store the binormal.
              list[v].bitangent = list[v].bitangent - (normal*list[v].bitangent)*normal - (list[v].tangent*list[v].bitangent)*list[v].tangent; 
              list[v].bitangent.NormalizeIt();
              list[v].binormalId = CompressSortedVector(list[v].bitangent, binormalData, binormalCompressionIndices);
            }
            binormalInput->AddIndex(list[v].binormalId);
          }
        }
      }
    }
  }

  if (tangentSource != NULL) tangentSource->SetData(tangentData, 3);
  if (binormalSource != NULL) binormalSource->SetData(binormalData, 3);
}