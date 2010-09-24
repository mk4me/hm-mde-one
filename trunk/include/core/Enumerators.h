#ifndef CORE_ENUMERATIONS_H
#define CORE_ENUMERATIONS_H


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

enum POSITION
{
  LEFT,
  RIGHT,
  UP,
  DOWN
};


#endif //CORE_ENUMERATIONS_H
