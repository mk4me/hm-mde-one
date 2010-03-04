#include "StdAfx.h"

//#define __DEBUG

GeometryPolygons::GeometryPolygons(void) : 
  geometryPolygons_(NULL)
, skinController_(NULL)
, meshName_(DEFAULT_MODEL_FILE_NAME)
, worldTransform_(FMMatrix44::Identity)
, vertexCount_(0)
{
  vertices_.clear(); 
  vertexSemantic_.clear(); 
}

GeometryPolygons::GeometryPolygons(FCDGeometryPolygons *geometryPolygons, fm::string name, FMMatrix44 *worldTransform) : 
  geometryPolygons_(geometryPolygons)
, skinController_(NULL)
, skeleton_(NULL)
, meshName_(name)
, worldTransform_(*worldTransform)
, vertexCount_(0)
{
  vertices_.clear(); 
  vertexSemantic_.clear(); 
  meshName_.clear(); 

  Initialize(); 
}

GeometryPolygons::GeometryPolygons(FCDGeometryPolygons *geometryPolygons, FCDControllerInstance *controllerInstance, Skeleton *skeleton, fm::string name, FMMatrix44 *worldTransform) : 
  geometryPolygons_(geometryPolygons)
, controllerInstance_(controllerInstance)
, skeleton_(skeleton)
, meshName_(name)
, worldTransform_(*worldTransform)
, vertexCount_(0)
{
  FCDController *controller = (FCDController *)controllerInstance_->GetEntity(); 
  skinController_ = controller->GetSkinController(); 

  vertices_.clear(); 
  vertexSemantic_.clear(); 
  meshName_.clear(); 

  Initialize(); 
}

GeometryPolygons::~GeometryPolygons(void)
{
  //if(geometryPolygons_) delete geometryPolygons_; 
  vertices_.clear(); 
  vertexSemantic_.clear(); 
  meshName_.clear(); 
}

bool GeometryPolygons::CheckSkinMeshCompatibility()
{
  // Sprawdzanie czy geometria jest zgodna z tym co trzyma skin... 
  FCDEntity *entity = skinController_->GetTarget(); 
  FCDEntity::Type type = entity->GetType(); 
  if(type == FCDEntity::GEOMETRY)
  {
    //clog << "na szczescie target jest geometria...\n"; 
    FCDGeometry *geometry = (FCDGeometry *)entity; 
    if(geometry->IsMesh())
    {
      FCDGeometryMesh *geometryMesh = geometry->GetMesh(); 
      size_t polygonCount = geometryMesh->GetPolygonsCount(); 
      for(size_t i=0; i<polygonCount; ++i)
      {
        FCDGeometryPolygons *polygon = geometryMesh->GetPolygons(i); 
        if(polygon == this->geometryPolygons_)
        {
          return true; 
        }
      }
    }
  }
  else
  {
    clog << "Na nieszczescie target nie jest geometria...\n"; 
  }
  return false; 
}

// Inicjalizacja siatki 
void GeometryPolygons::Initialize() 
{
  FCDGeometryMesh *mesh = geometryPolygons_->GetParent(); 
  // Pobieramy kolejne ¿ród³a danych. Pozycja 
  FCDGeometrySource *positionSource = mesh->GetPositionSource(); 
  // Normalne 
  FCDGeometrySource *normalSource = NULL; 
  // Binormalne 
  FCDGeometrySource *binormalSource = NULL; 
  // Tangenty  
  FCDGeometrySource *tangentSource = NULL; 
  // Wspó³rzêdne tekstury 
  FCDGeometrySource *texcoordSource = NULL;  

  size_t sourceCount = mesh->GetSourceCount(); 
  for(size_t i=0; i<sourceCount; ++i)
  {
    FCDGeometrySource *source = mesh->GetSource(i); 
    switch(source->GetType())
    {
    case FUDaeGeometryInput::NORMAL:
      normalSource = source; 
      break;
    case FUDaeGeometryInput::TEXBINORMAL:
      binormalSource = source; 
      break;
    case FUDaeGeometryInput::TEXTANGENT:
      tangentSource = source; 
      break;
    case FUDaeGeometryInput::TEXCOORD:
      texcoordSource = source; 
      break;
    default:
      break;
    }
  }

  // TODO: Sprawdzamy czy s¹ wszystkie zród³a 
  // TODO: Triangulacja siatki 

  //-------------------------------------------------------------------------------
  // Wierzcho³ki 
  //-------------------------------------------------------------------------------
  size_t dataCount = positionSource->GetDataCount(); 
  uint32 positionStride = positionSource->GetStride(); 
  size_t positionCount = dataCount/positionStride; 

  VertexCandidate vertex; 
  FMVector3 position;

  FCDSkinControllerVertex *skinControllerVertices = NULL; 
  size_t skinControllerVertexCount = 0; 
  if(skinController_)
  {
    if( CheckSkinMeshCompatibility() )
    {
      clog << "SkinMesh: ok..\n";  
      skinControllerVertices = skinController_->GetVertexInfluences(); 
      skinControllerVertexCount = skinController_->GetInfluenceCount(); 
    }
  }

  float *vericesArray = positionSource->GetData(); 
  // Rezerwujemy odpowiednia iloœæ miejsca w wektorze 
  vertices_.reserve(positionCount); 
  // Wype³nianie bufora 
  for(size_t i=0; i<positionCount; ++i)
  {
    vertex.clear(); 
    position.Set(vericesArray[positionStride*i], vericesArray[positionStride*i+1], vericesArray[positionStride*i+2]); 
 
    if(skinControllerVertices && (skinControllerVertexCount > 0))
    {
      FMMatrix44 bindShapeMatrix = skinController_->GetBindShapeTransform(); 
      // Zapisujemy informacje o skinie 
      FCDSkinControllerVertex vertexSkinData = skinControllerVertices[i]; 
      size_t pairCount = vertexSkinData.GetPairCount(); 
      for(size_t j=0; j<pairCount; ++j)
      {
        FCDJointWeightPair *skinPair = vertexSkinData.GetPair(j); 
        FCDSkinControllerJoint *skinJoint = skinController_->GetJoint(skinPair->jointIndex); 
        FMMatrix44 bindPoseInverse = skinJoint->GetBindPoseInverse(); 
        FMMatrix44 bindPose = bindPoseInverse.Inverted(); 
        FMMatrix44 wordTransform = controllerInstance_->GetJoint(skinPair->jointIndex)->CalculateWorldTransform(); 
        FMVector3 p = FMVector3::Zero; 
        FMMatrix44 m = wordTransform * bindPoseInverse;  
        SkinData skinData(skinPair->jointIndex, skinPair->weight, m); 

        vertex.skinData.push_back(skinData); 
      }
      //position = bindShapeMatrix.TransformCoordinate(position); 
    }

    vertex.position = position; 
    vertices_.push_back(vertex); 
  }

  if( (geometryPolygons_->GetPrimitiveType() == FCDGeometryPolygons::POLYGONS) && geometryPolygons_->TestPolyType())
  {
    // Pobieramy obiekty reprezentuj¹ce okreœlone dane w trójk¹tach 
    FCDGeometryPolygonsInput *positionInput = geometryPolygons_->FindInput(positionSource); 
    FCDGeometryPolygonsInput *normalInput = geometryPolygons_->FindInput(normalSource); 
    FCDGeometryPolygonsInput *binormalInput = geometryPolygons_->FindInput(binormalSource); 
    FCDGeometryPolygonsInput *tangentInput = geometryPolygons_->FindInput(tangentSource); 
    FCDGeometryPolygonsInput *texcoordInput = geometryPolygons_->FindInput(texcoordSource); 

    if(positionInput)
    {
      vertexSemantic_.push_back(Position); 
    }
    if(normalInput)
    {
      vertexSemantic_.push_back(Normal); 
    }
    if(tangentInput) 
    {
      vertexSemantic_.push_back(Tangent); 
    }
    if(binormalInput)
    {
      vertexSemantic_.push_back(Binormal); 
    }
    if(texcoordInput)
    {
      vertexSemantic_.push_back(TexCoord0); 
    }

    //-------------------------------------------------------------------------------
    // Po wszystkich polygonach grupy 
    size_t faceCount = geometryPolygons_->GetFaceCount(); 

    //-------------------------------------------------------------------------------
    // Wype³nianie wstêpne bufora trójk¹tów 
    faces_.reserve(faceCount); 

    if(positionInput)
    {
      uint32 *positionIndicies = positionInput->GetIndices(); 
      for(size_t i=0; i<faceCount; ++i)
      {
        Face face((uint16)positionIndicies[i*3+0], (uint16)positionIndicies[i*3+1], (uint16)positionIndicies[i*3+2]);        
        faces_.push_back(face); 
      }
    }
    //-------------------------------------------------------------------------------

    for(size_t j=0; j<faceCount; ++j)
    {
      // Po kolejnych wierzcho³kach trójk¹tów 
      for(size_t k=0; k<3; ++k)
      {
        uint32 positionIndex = 0xFFFFFFFF; 
        uint32 normalIndex = 0xFFFFFFFF; 
        uint32 binormalIndex = 0xFFFFFFFF; 
        uint32 tangentIndex = 0xFFFFFFFF; 
        uint32 texcoordIndex = 0xFFFFFFFF; 

        VertexData vertexData; 

        if(positionInput)
        {
          // Aktualny j*3+k
          uint32 *positionIndicies = positionInput->GetIndices(); 
          positionIndex = positionIndicies[j*3+k]; 
        }
        if(normalInput)
        {
          uint32 *normalIndicies = normalInput->GetIndices(); 
          normalIndex = normalIndicies[j*3+k]; 

          vertexData.normal = GetVector3(normalSource, normalIndex); 
          vertexData.normal.NormalizeIt(); 
        }
        if(binormalInput)
        {
          uint32 *binormalIndicies = binormalInput->GetIndices(); 
          binormalIndex = binormalIndicies[j*3+k]; 

          vertexData.binormal = GetVector3(binormalSource, binormalIndex); 
          vertexData.binormal.NormalizeIt(); 
        }
        if(tangentInput)
        {
          uint32 *tangentIndicies = tangentInput->GetIndices(); 
          tangentIndex = tangentIndicies[j*3+k]; 

          vertexData.tangent = GetVector3(tangentSource, tangentIndex); 
          vertexData.tangent.NormalizeIt(); 
        }
        if(texcoordInput)
        {
          uint32 *texcoordIndicies = texcoordInput->GetIndices(); 
          texcoordIndex = texcoordIndicies[j*3+k]; 
          FMVector2 texCoord = GetVector2(texcoordSource, texcoordIndex); 
          vertexData.textureCoordinate.push_back( texCoord ); 
        }

        // Sprawdzamy czy taki wierzcho³ek istnieje 
        if(vertices_[positionIndex].Includes(vertexData))
        {
          // Istnieje 
        }
        else
        {
          // Nie istnieje. Dodajemy: 
          vertices_[positionIndex].verticesData.push_back(vertexData); 
        }

        FaceIndex faceIndex(j, (uint16)k); 
        vertices_[positionIndex].AddFaceIndex(vertexData, faceIndex); 

      }
    }
    //-------------------------------------------------------------------------------
  }
  else
  {
    clog << "To nie s¹ normalne trójk¹ty ;(" << "\n"; 
  }

  if(skinControllerVertices && (skinControllerVertexCount > 0))
  {
    InitializeBindPose(); 
  }
}

void GeometryPolygons::InitializeBindPose()
{
  FMMatrix44 bindShapeMatrix = skinController_->GetBindShapeTransform(); 
  size_t vertexCount = vertices_.size(); 
  for(size_t i=0; i<vertexCount; ++i)
  {
    size_t vertexDataCount = vertices_[i].verticesData.size(); 
    vertices_[i].position = bindShapeMatrix.TransformCoordinate(vertices_[i].position); 
    for(size_t k=0; k<vertexDataCount; ++k)
    {
      vertices_[i].verticesData[k].normal = bindShapeMatrix.TransformVector(vertices_[i].verticesData[k].normal); 
      vertices_[i].verticesData[k].tangent = bindShapeMatrix.TransformVector(vertices_[i].verticesData[k].tangent); 
      vertices_[i].verticesData[k].binormal = bindShapeMatrix.TransformVector(vertices_[i].verticesData[k].binormal); 
    }

    const VertexCandidate vc = vertices_[i]; 

    vertices_[i].position = FMVector3::Zero; 
    for(size_t k=0; k<vertexDataCount; ++k)
    {
      vertices_[i].verticesData[k].normal = FMVector3::Zero; 
      vertices_[i].verticesData[k].tangent = FMVector3::Zero; 
      vertices_[i].verticesData[k].binormal = FMVector3::Zero; 
    }

    size_t skinDataCount = vc.skinData.size(); 
    for(size_t j=0; j<skinDataCount; ++j)
    {
      vertices_[i].position += vc.skinData[j].bindPose_.TransformCoordinate(vc.position)*vc.skinData[j].weight_; 
      for(size_t k=0; k<vertexDataCount; ++k)
      {
        vertices_[i].verticesData[k].normal += vc.skinData[j].bindPose_.TransformVector(vc.verticesData[k].normal)*vc.skinData[j].weight_; 
        vertices_[i].verticesData[k].tangent += vc.skinData[j].bindPose_.TransformVector(vc.verticesData[k].tangent)*vc.skinData[j].weight_; 
        vertices_[i].verticesData[k].binormal += vc.skinData[j].bindPose_.TransformVector(vc.verticesData[k].binormal)*vc.skinData[j].weight_; 
      }
    }

    for(size_t k=0; k<vertexDataCount; ++k)
    {
      vertices_[i].verticesData[k].normal.NormalizeIt(); 
      vertices_[i].verticesData[k].tangent.NormalizeIt(); 
      vertices_[i].verticesData[k].binormal.NormalizeIt(); 
    }
  }
}

FMVector3 GeometryPolygons::GetVector3(FCDGeometrySource *source, uint32 index)
{
  int stride = source->GetStride(); 
  float *data = source->GetData(); 
  if(stride < 3)
  {
    clog << "B³¹d Ÿród³a geometrii" << "\n"; 
  }
  FMVector3 vec3(data[index*stride], data[index*stride+1], data[index*stride+2]); 
  return vec3; 
}

FMVector2 GeometryPolygons::GetVector2(FCDGeometrySource *source, uint32 index)
{
  int stride = source->GetStride(); 
  float *data = source->GetData(); 
  if(stride < 2)
  {
    clog << "B³¹d Ÿród³a geometrii" << "\n"; 
  }
  FMVector2 vec2(data[index*stride], data[index*stride+1]); 
  return vec2; 
}

void GeometryPolygons::NegateV(FCDGeometrySource *source)
{
  int stride = source->GetStride(); 
  float *data = source->GetData(); 
  size_t count = source->GetDataCount()/stride; 
  if(stride < 2)
  {
    clog << "B³¹d Ÿród³a geometrii" << "\n"; 
  }
  for(size_t i=0; i<count; ++i)
  {
    data[i*stride+1] = -data[i*stride+1]; 
  }
}

// Zapisywanie kawa³ka z siatka 
void GeometryPolygons::Save(FileChunk *pFile) 
{
  // Zapis bufora z przygotowanymi wierzcho³kami oraz trójk¹tami do pliku 
  pFile->BeginChunk(FMODEL_MESH); 

  //-------------------------------------------------------------------------------
  // TODO: Zapisujemy na razie atrapê numeru materia³u!! w wersji FRSModel 2.5 
  // i tak wartoœæ w tym miejscu jest nieistotna. 
  // Zapisujemy numer materia³u 
  pFile->WriteInt(-1); 
  //-------------------------------------------------------------------------------

  // Nazwa siatki 
  pFile->WriteString((char*)meshName_.c_str()); 

  SaveMeshBuffer(pFile);
  SaveMeshIndices(pFile);
  if(skinController_)
    SaveSkin(pFile); 

  pFile->EndChunk(FMODEL_MESH); 
  // vertices 
}

size_t GeometryPolygons::GetVertexSize()
{
  size_t vertexSize = 0; 
  size_t semanticCount = vertexSemantic_.size(); 
  for(size_t i=0; i<semanticCount; ++i)
  {
    size_t size = 0; 
    switch(vertexSemantic_[i])
    {
    case Position: 
      size = 3 * FLOAT_SIZE; 
      break; 
    case Normal: 
      size = 3 * FLOAT_SIZE; 
      break; 
    case Binormal: 
      size = 3 * FLOAT_SIZE; 
      break; 
    case Tangent: 
      size = 3 * FLOAT_SIZE; 
      break; 
    case TexCoord0: 
      size = 2 * FLOAT_SIZE; 
      break; 
    default:
      size = 0; 
    }
    vertexSize += size; 
  }
  return vertexSize; 
}

bool GeometryPolygons::HasVertexType(VertexSemanticType type)
{
  size_t semanticCount = vertexSemantic_.size(); 
  for(size_t i=0; i<semanticCount; ++i)
  {
    if(type == vertexSemantic_[i])
      return true; 
  }
  return false; 
}

void GeometryPolygons::SaveMeshBuffer( FileChunk * pFile )
{
  // Obliczamy rozmiar buforów 
  size_t vertexCandidateCount = vertices_.size(); 

  for(size_t i=0; i<vertexCandidateCount; ++i)
  {
    vertexCount_ += vertices_[i].verticesData.size(); 
    if(vertices_[i].verticesData.size() <= 0)
    {
      clog << "B³¹d bufora wierzcho³ków..." << "\n"; 
    }
  }
  
  // Semantyka 
  size_t bufferSize = 0; 
  size_t vertexSize = GetVertexSize(); 
  bufferSize = vertexSize*vertexCount_; 

  // Zapisujemy bufory 
  pFile->BeginChunk(FMODEL_MESH_BUFFER); 
  // Rozmiar bufora 
  pFile->WriteInt(bufferSize); 

  vertexCount_ = 0; 

  for(size_t i=0; i<vertexCandidateCount; ++i)
  {
    size_t vertexDataCount = vertices_[i].verticesData.size(); 
    for(size_t j=0; j<vertexDataCount; ++j)
    {
      // Update face'ów 
      size_t vertexFaceCount = vertices_[i].verticesData[j].facesIndex.size(); 
      for(size_t f=0; f<vertexFaceCount; ++f)
      {
        FaceIndex faceIndex = vertices_[i].verticesData[j].facesIndex[f]; 
        faces_[faceIndex.i].data[faceIndex.c] = (uint16)vertexCount_; 
      }
      ++vertexCount_; 

      // Zapisujemy pozycje 
      if(HasVertexType(Position))
      {
#ifdef __DEBUG
        clog << "P: (" << vertices_[i].position.x << ", " << vertices_[i].position.y << ", " << vertices_[i].position.z << "), "; 
#endif
        pFile->WritePoint3( worldTransform_.TransformCoordinate(vertices_[i].position) ); 
      }
      // Zapisujemy normaln¹ 
      if(HasVertexType(Normal))
      {
#ifdef __DEBUG
        clog << "N: (" << vertices_[i].verticesData[j].normal.x << ", " << vertices_[i].verticesData[j].normal.y << ", " << vertices_[i].verticesData[j].normal.z << "), "; 
#endif
        //pFile->WritePoint3( vertices_[i].verticesData[j].normal ); 
        pFile->WritePoint3( worldTransform_.TransformVector(vertices_[i].verticesData[j].normal).Normalize() ); 
      }
      // Zapisujemy binormaln¹ 
      if(HasVertexType(Binormal))
      {
        //FMVector3 b = vertices_[i].verticesData[j].binormal; 
        FMVector3 b = worldTransform_.TransformVector(vertices_[i].verticesData[j].binormal).Normalize(); 
#ifdef __DEBUG
        clog << "B: (" << b.x << ", " << b.y << ", " << b.z << "), "; 
#endif
        //b = -b; 
        pFile->WritePoint3( b ); 
      }
      // Zapisujemy tangentê 
      if(HasVertexType(Tangent))
      {
        //-------------------------------------------------------------------------------
        // HACK: Negowanie tangenty sprawia ze dzia³a u nas dobrze. Sprawdziæ dlaczego tak siê dzieje. 
        // TODO: Wychodzi na to, ze to przez inny typ zapisu UVek. W Colladzie mamy orgin w lewym dolnym naro¿niku
        // a u nas w lewym górnym. Generalnie neguje siê V tylko, ale trzeba te¿ odpowiendnio przeliczyæ tangenty teraz. 
        //  Sprawdzanie znaku rezultatu Dot(Cross(T,B), N) i w zale¿noœci od rezultatu decydujemy czy zanegowaæ 
        //  tangente czy nie nic nie daje, zreszta czemu mia³oby jeœli tangent space jest wyliczona "poprawnie" 
        //  (trzeba jakoœ odnieœæ siê jakoœ do dyskusji na forum sourceforge o tym czy tangent space jest liczony 
        //  dobrze przez fcollade). 

        float res = 0.0f; 
        //FMVector3 t = vertices_[i].verticesData[j].tangent; 
        FMVector3 t = worldTransform_.TransformVector(vertices_[i].verticesData[j].tangent).Normalize(); 

        //-------------------------------------------------------------------------------
#ifdef __DEBUG
        clog << "T (" << res << "): (" << t.x << ", " << t.y << ", " << t.z << "), "; 
#endif
        pFile->WritePoint3( t ); 
      }
      // Zapisujemy wspo³rzêdn¹ tekstury  
      if(HasVertexType(TexCoord0))
      {
#ifdef __DEBUG
        clog << "TV: (" << vertices_[i].verticesData[j].textureCoordinate[0].x << ", " << vertices_[i].verticesData[j].textureCoordinate[0].y << ") " << "\n"; 
#endif
        // TODO: Na razie tylko texCoord0 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        // Sprawdziæ to. 
        pFile->WritePoint2(vertices_[i].verticesData[j].textureCoordinate[0]); 
      }
    }
  }

  SaveVertexSemantics(pFile); 
  
  pFile->EndChunk(FMODEL_MESH_BUFFER); 
}

void GeometryPolygons::SaveVertexSemantics(FileChunk * pFile)
{
  size_t vertexSemanticsSize = 0; 
  if(HasVertexType(Position)) ++vertexSemanticsSize; 
  // Zapisujemy normaln¹ 
  if(HasVertexType(Normal)) ++vertexSemanticsSize; 
  // Zapisujemy binormaln¹ 
  if(HasVertexType(Binormal)) ++vertexSemanticsSize; 
  // Zapisujemy tangentê 
  if(HasVertexType(Tangent)) ++vertexSemanticsSize; 
  // Zapisujemy wspo³rzêdn¹ tekstury  
  if(HasVertexType(TexCoord0)) ++vertexSemanticsSize; 

  pFile->WriteInt(vertexSemanticsSize); 

  if(HasVertexType(Position))
  {
    pFile->WriteInt(Position); 
    pFile->WriteInt(Float3); 
  }
  // Zapisujemy normaln¹ 
  if(HasVertexType(Normal))
  {
    pFile->WriteInt(Normal);
    pFile->WriteInt(Float3); 
  }
  // Zapisujemy binormaln¹ 
  if(HasVertexType(Binormal))
  {
    pFile->WriteInt(Binormal);
    pFile->WriteInt(Float3); 
  }
  // Zapisujemy tangentê 
  if(HasVertexType(Tangent))
  {
    pFile->WriteInt(Tangent); 
    pFile->WriteInt(Float3); 
  }
  // Zapisujemy wspo³rzêdn¹ tekstury  
  if(HasVertexType(TexCoord0))
  {
    pFile->WriteInt(TexCoord0); 
    pFile->WriteInt(Float2); 
  }
}

void GeometryPolygons::SaveMeshIndices( FileChunk * pFile )
{
  // Zapisujemy bufory indeksów 
  pFile->BeginChunk(FMODEL_MESH_INDICES); 

  size_t faceCount = faces_.size(); 

  pFile->WriteInt(faceCount); 

  for(size_t i=0; i<faceCount; ++i)
  {
    pFile->WriteShortInt(faces_[i]._0); 
    pFile->WriteShortInt(faces_[i]._1); 
    pFile->WriteShortInt(faces_[i]._2); 
  }

  pFile->EndChunk(FMODEL_MESH_INDICES); 
}

void GeometryPolygons::SaveSkin( FileChunk * pFile )
{
  // Zapis skin'a
  pFile->BeginChunk(FMODEL_MODIFIER); 
  {
    // Zapis GUID'a Bone Motion
    pFile->WriteGUID(IID_BoneMotion); 
    // Kawa³ek skin'a
    pFile->BeginChunk(FMODEL_SKIN); 
    {
      // Iloœæ wierzcho³ków z modyfikatorem (czyli wszystkie jeœli w ogóle jest skin)
      pFile->WriteInt(vertexCount_); 
      // Pobieramy liczbê wierzcho³ków przed transformacj¹
      size_t vertexCount = vertices_.size(); 
      // Indeks wierzcho³ka po transformacji
      size_t idx = 0;
      // Zapis wierzcho³ków podlekaj¹cych skin'owaniu
      for(size_t i=0; i<vertexCount; ++i)
      {
        VertexCandidate vertex = vertices_[i]; 
        size_t vertexDataCount = vertex.verticesData.size(); 

        for(size_t j=0; j<vertexDataCount; ++j)
        {
          // Identyfikator wierzcho³ka
          pFile->WriteInt(idx); 
          // Zapis liczby wp³ywaj¹cych koœci
          size_t boneCount = vertex.skinData.size(); 
          pFile->WriteInt(boneCount); 

          for(size_t k=0; k<boneCount; k++)
          {
            SkinData skinData = vertex.skinData[k]; 
            // Zapis danych o wp³ywie koœci
            FCDSceneNode *skinJoint = controllerInstance_->GetJoint(skinData.boneID_); 
            int boneId = skeleton_->GetBoneId(skinJoint); 

            pFile->WriteInt(boneId); 
            pFile->WriteFloat(skinData.weight_); 
          }
          idx++; 
        }            
      }
    }
    pFile->EndChunk(FMODEL_SKIN); 
  }
  pFile->EndChunk(FMODEL_MODIFIER); 
}