#include "StdAfx.h"
#include "VisualScene.h"

VisualScene::VisualScene(void) : 
  sceneRoot_(NULL)
, skeleton_(NULL)
, colladaDocument_(NULL)
{
  skeleton_ = new Skeleton(); 
}

VisualScene::VisualScene(FCDocument *colladaDocument) : 
  sceneRoot_(NULL)
, skeleton_(NULL)
, colladaDocument_(colladaDocument)
{
  sceneRoot_ = colladaDocument->GetVisualSceneInstance(); 
  skeleton_ = new Skeleton(); 
  Initialize(); 
}

VisualScene::~VisualScene(void)
{
  Clear();
  if(skeleton_)
  {
    delete skeleton_; 
  }
}

void VisualScene::Clear()
{
  if (sceneRoot_) 
    delete sceneRoot_; 
  size_t meshCount = meshInstances_.size(); 
  for(size_t i=0; i<meshCount; ++i)
  {
    if (meshInstances_[i])
      delete meshInstances_[i]; 
  }
  meshInstances_.clear(); 
}

// Inicjalizacja sceny 
void VisualScene::Initialize()
{
  // Przeszukujemy poszczególne wêz³y 
  if (sceneRoot_)
    ParseSceneNode(sceneRoot_); 
  // Pobieramy animacje 
  //GetAnimations(); 
}

// Analizuje wêzê³ 
void VisualScene::ParseSceneNode( FCDSceneNode *node )
{
  // Analizujemy wêze³ 
  if( AnalizeSceneNode(node) )
  {
    // Przeszukujemy dzieci 
    size_t childCount = node->GetChildrenCount(); 
    for(size_t i=0; i<childCount; ++i)
    {
      FCDSceneNode *pChildNode = node->GetChild(i); 
      ParseSceneNode(pChildNode); 
    }
  }
}

// Analizowanie treœci wêz³a 
bool VisualScene::AnalizeSceneNode( FCDSceneNode *node )
{
  // Analizowanie wêz³a i odpowiednie jego zapamiêtanie. 
  clog << node->GetName().c_str() << ": " << (node->GetJointFlag()? "JOINT":"NODE") << "\n"; 
  
  
  FCDEntity::Type typ = node->GetType();

  if(!node->GetJointFlag())
  {
    return AnalizeNode(node);
  }
   else
   {
     return AnalizeJoint(node); 
   }
  return true; 
}

bool VisualScene::IsJointRoot( FCDSceneNode *node )
{
  if(node->GetParentCount() == 1)
  {
    FCDSceneNode *parent = node->GetParent(0); 
    if( parent->GetType() == FCDSceneNode::SCENE_NODE )
    {
      return true; 
    }
  }
  return false; 
}

bool VisualScene::AnalizeJoint( FCDSceneNode * node )
{
  size_t parentCount = node->GetParentCount(); 
  // TODO: Sprawdziæ jak to jest z transformacjami ca³ej sceny (wêz³a <visual_scene>). Jak to wp³ywa na eksport itp. 
  if( (parentCount == 0) || IsJointRoot(node) )
  {
    // Wêze³ jest rootem. Wiêc przetwarzamy ca³¹ hierarchiê 
    skeleton_->AddBones(node);  
    // Nie chcemy przetwarzaæ wêz³ów dzieci dalej, gdy¿ przetworzyliœmy go ju¿ w tej funkcji
    return false; 
  }
  else
  {
    clog << "You shouldn't see this..." << "\n"; 
  }
  return true; 
}

bool VisualScene::AnalizeNode( FCDSceneNode * node )
{
  //FCDEntityInstanceContainer entityInstanceContainer; 
  // Parsujemy wêzê³. Czyli szukamy jaki
  const FCDEntityInstance **entityInstanceContainer = node->GetInstances(); 

  // Sprawdzic typ noda.


  size_t entityCount = node->GetInstanceCount(); 
 // clog << "Jest " << entityCount << " instancji..." << "\n"; 

  FMMatrix44 nodeTransform = node->CalculateWorldTransform(); 

  // wiem ze d³ugi if - ale konieczny :P
  // gdy w XML nie ma okreslonego typu  np: TYPE="JOINT" to collada przypisuje mu typ SCENE_NODE. a medota analizeJoint sprawdza czy wêzel rodzic jest typu Scene Node
  // w tym przypadku ka¿dy jest i czy ma jednego rodzica - i w tym przypadku tez kazdy ma  1 rodzica. Wynik jes taki iz otrzymuemy 105 zamiast np 18 poprawnych wêz³ów
  //TODO; Przeniœæ do AnalizeJoint jeœli sie da - sprawdziæ
  if(entityCount == 0 && node->GetType() == FCDEntity::SCENE_NODE && node != sceneRoot_ && node->GetParent(0) == sceneRoot_)
  {
      AnalizeJoint(node);
  }

  for(size_t i=0; i<entityCount; ++i)
  {
    const FCDEntityInstance *entityInstance = entityInstanceContainer[i]; 
    if(entityInstance == NULL) continue; 
    FCDEntityInstance::Type instanceType = entityInstance->GetType(); 

    switch(instanceType)
    {
    case FCDEntityInstance::GEOMETRY:
      // Eksport zwyk³ej siatki 
      GetUnskinnedGeometry(entityInstance, nodeTransform);
      break;
    case FCDEntityInstance::CONTROLLER:
      {
        FCDControllerInstance *controllerInstance = (FCDControllerInstance *)entityInstance; 
        FCDController *controller = (FCDController *)controllerInstance->GetEntity(); 
        FCDSkinController *skinController = NULL; 
        if(controller && controller->IsSkin())
        {
          // Eksport siatki oskinowanej 
          //clog << "Siatka jest oskinowana" << "\n"; 
          // Pobieramy kontroler skina.. 
          skinController = controller->GetSkinController(); 
          FCDEntity *entity = skinController->GetTarget(); 
          if( entity && (entity->GetType() == FCDEntity::GEOMETRY) )
          {
            //clog << "Mam geometrie ze skina" << "\n"; 
            GetSkinnedGeometry(controllerInstance, (FCDGeometry *)entity, nodeTransform); 
          }
        }
      }
      break;

//     case FCDEntityInstance::SIMPLE:
//         break;

    default:
      break;
    }
  }

//   if(entityCount == 0)
//   {
//       AnalizeJoint(node);
//   }
  return true; 
}

void VisualScene::GetUnskinnedGeometry( const FCDEntityInstance * entityInstance, FMMatrix44 &nodeTransform )
{
  // GetGeometry 
  FCDGeometryInstance *colladaGeometryInstance = (FCDGeometryInstance*)entityInstance; 
  FCDGeometry *geometry = (FCDGeometry *)colladaGeometryInstance->GetEntity(); 
  // Dodajemy do wektora z geometriami jeœli to siatka 
  if(geometry->IsMesh())
  {
    MeshInstance *meshInstance = new MeshInstance(colladaGeometryInstance, nodeTransform); 
    meshInstances_.push_back(meshInstance); 
  }
}

void VisualScene::GetSkinnedGeometry( FCDControllerInstance * controllerInstance, FCDGeometry *geometry, FMMatrix44 &nodeTransform )
{
  // Dodajemy do wektora z geometriami jeœli to siatka 
  if(geometry->IsMesh())
  {
    MeshInstance *meshInstance = new MeshInstance(controllerInstance, skeleton_, geometry, nodeTransform); 
    meshInstances_.push_back(meshInstance); 
  }
}

// Pobieramy animacje z biblioteki animacji 
void VisualScene::GetAnimations()
{
  if(!colladaDocument_) return; 

  // Sprawdzamy bibliotekê animacji 
  FCDAnimationLibrary *animationLibrary = colladaDocument_->GetAnimationLibrary(); 
  if(!animationLibrary) return; 

  size_t animationCount = animationLibrary->GetEntityCount(); 
  for(size_t i=0; i<animationCount; ++i)
  {
    FCDAnimation *animation = (FCDAnimation *)animationLibrary->GetEntity(i); 
    size_t channelCount = animation->GetChannelCount(); 
    for(size_t j=0; j<channelCount; ++j)
    {
      FCDAnimationChannel *channel = animation->GetChannel(j); 
      size_t curveCount = channel->GetCurveCount(); 
      for(size_t k=0; k<curveCount; ++k)
      {
        FCDAnimationCurve *curve = channel->GetCurve(k); 
        if(curve->HasDriver())
        {
          //curve->getd
        }
        size_t keyCount = curve->GetKeyCount(); 
        for(size_t l=0; l<keyCount; ++l)
        {
          FCDAnimationKey *key = curve->GetKey(l); 
          //clog << "Key " << l << " (" << key->interpolation << "): " << key->input << ": " << key->output << "\n"; 
        }
      }
    }
  }
}

size_t VisualScene::GetMeshInstanceCount()
{
  return meshInstances_.size(); 
}

MeshInstance *VisualScene::GetMeshInstance( size_t idx )
{
  if( idx < meshInstances_.size() )
    return meshInstances_[idx]; 
  return NULL; 
}

Skeleton *VisualScene::GetSkeleton()
{
  if(skeleton_->IsEmpty())
  {
    return NULL; 
  }
  else
    return skeleton_; 
  return skeleton_; 
}