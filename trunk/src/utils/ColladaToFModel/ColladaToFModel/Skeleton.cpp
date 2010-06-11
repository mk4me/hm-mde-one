#include "StdAfx.h"
#include "Skeleton.h"

Skeleton::Skeleton(void)
{
}

/*
controllerInstance->CalculateRootIds(); // TODO: To wywo�anie jest "teoretycznie" niepotrzebne. tworzy ono tylko kontener z Uri. 

// Pobieramy kontener z rootami szkielet�w. 
FCDSceneNodeList skeletonRoots; 
controllerInstance->FindSkeletonNodes(skeletonRoots); 

// Pobieramy kontroler z instancji kontrolera a nast�pnie skina
FCDController *controller = (FCDController *)controllerInstance->GetEntity(); 
//FCDSkinController *skinController = controller->GetSkinController(); 

// Je�li mamy jeden szkielet to jest ok. Wi�cej szkielet�w nie jest obs�ugiwane w tej wersji.. 
size_t rootCount = skeletonRoots.size();   
for(size_t i=0; i<rootCount; ++i)
{
  FCDSceneNode *sceneNode = skeletonRoots[i]; 
  if(sceneNode->IsJoint())
  {
    clog << "root " << i << " jest jointem" << "\n"; 
  }
}
/**/ 

Skeleton::~Skeleton(void)
{
}

void Skeleton::AddBones(FCDSceneNode *sceneNode)
{
  // TODO: Sprawdzi� czy mo�e takie ko�ci ju� nie istniej� 
  // sceneNode to root nowego szkieletu, kt�ry dodajemy do tablicy. 
  AssignBone(sceneNode, -1); 
}

int Skeleton::AssignBone(FCDSceneNode *node, int parentId)
{
  if(!node->GetJointFlag())
  {
    if(node->GetChildrenCount() == 0)
      return -1;
  }

  //HACK: Eliminujemy JOINTy kt�re maj� w nazwe "Footsteps"
  if(node->GetName().find("Footsteps") != fm::string::npos)
    return -1; 

  Bone bone; 
  int boneId = -1; 
  bone.name_ = node->GetName(); 
  bone.sceneNode_ = node; 
  bone.parentId_ = parentId; 
  bone.childIds_.clear();
  skeleton_.push_back(bone); 
  boneId = (int)skeleton_.size() - 1; 

  size_t childCount = node->GetChildrenCount(); 
  for(size_t i=0; i<childCount; ++i)
  {
    int childId = AssignBone(node->GetChild(i), boneId); 
    if(childId != -1)
      skeleton_[boneId].childIds_.push_back(childId); 
  }
  return boneId; 
}

// Pobieranie Id ko�ci 
int Skeleton::GetBoneId(FCDSceneNode *node)
{
  size_t boneCount = skeleton_.size(); 
  for(size_t i=0; i<boneCount; ++i)
  {
    if(skeleton_[i].sceneNode_ == node)
    {
      return (int)i; 
    }
  }
  return -1; 
}

// Zapisywanie kawa�ka ze szkieletem 
void Skeleton::Save(FileChunk *file)
{
  // Pobieramy rozmiar szkieletu
  size_t boneCount = skeleton_.size(); 
  // Je�li szkieletu nie ma, wychodzimy
  if(!boneCount) return; 

  // Je�li jest, zapisujemy...
  file->BeginChunk(FMODEL_PLUGIN);
  {
    // Zapis GUID'a Bone Motion
    file->WriteGUID(IID_BoneMotion); 
    // Rozpoczynamy kawa�ek szkieletu
    file->BeginChunk(FMODEL_BONES);
    {
      // Zapis ilo�ci ko�ci
      file->WriteInt(boneCount); 
      for(size_t i=0; i<boneCount; ++i)
      {
        // Pobieramy ko��
        Bone bone = skeleton_[i]; 
        // Zapis nazwy ko�ci
        file->WriteString(bone.name_.c_str()); 
        // Zapis identyfikatora rodzica
        file->WriteInt(bone.parentId_); 
        
        //-------------------------------------------------------------------------------
        // Zapis danych macierzy. Zapis kwaterniona, przesuni�cia, oraz, warunkowo, skali. 
        // TODO: Sprawdzi� jak to si� ma do rzeczywisto�ci. 
        //   Mo�e lepiej b�dzie jak u�yjemy FCDTransform wyci�gane z GetTransform()?? 
        //   Przetestowa� to jakby co� si� dzia�o. 
        FCDSceneNode *node = skeleton_[i].sceneNode_; 
 
        FMMatrix44 localTransform = node->CalculateLocalTransform(); 
        FMVector3 translation = localTransform.GetTranslation();  
        FMQuaternion rotation = FMQuaternion::MatrixRotationQuaternion(localTransform); 

        // Zapis ko�ci. Translacja i kwaternion
        file->WritePoint3(translation); 
        file->WriteQuaternion(rotation); 

        // Tworzymy Bone Space (odwr�cona macierz World TM)
        FMMatrix44 worldTransform = node->CalculateWorldTransform(); 

        //Matrix3 boneSpace = pBone->GetObjectTM().ExtractMatrix3();
        FMMatrix44 boneSpace = worldTransform.Inverted();
        // Zapis Bone Space. Translacja i kwaternion
        translation = boneSpace.GetTranslation(); 
        rotation = FMQuaternion::MatrixRotationQuaternion(boneSpace); 

        file->WritePoint3(translation); 
        file->WriteQuaternion(rotation); 
        //-------------------------------------------------------------------------------

        // Zapis struktury szkieletu. Identyfikatory ko�ci potomnych
        size_t childBoneCount = skeleton_[i].childIds_.size();
        // Zapis liczby ko�ci potomnych
        file->WriteInt(childBoneCount); 

        for(size_t j = 0; j < childBoneCount; j++)
        {
          int childBoneId = skeleton_[i].childIds_[j];
          file->WriteInt(childBoneId);
        }

        // TODO: Bug u Pina. BnmCoreBones.pas: linie 172 do 174. powienien sprawdzac jaki kawalek pomija ;) 
        file->BeginChunk(FMODEL_DOF); 
        file->EndChunk(FMODEL_DOF); 
      }
    }
    file->EndChunk(FMODEL_BONES); 
  }
  file->EndChunk(FMODEL_PLUGIN); 
}

void Skeleton::SaveAnimation(FileChunk *file)
{
  // Eksport animacji szkieletowej
  // Rozpoczynamy kawa�ek plugin'u
  file->BeginChunk(FMODEL_PLUGIN); 
  {
    // Zapisujemy GIUD Bone Motion
    file->WriteGUID(IID_BoneMotion); 
    file->BeginChunk(FMODEL_SKELETON_ANIMATION); 
    {
      // Zapisujemy nazw� animacji
      //TODO: nazwa animacji!!!
      file->WriteString("animation.name.here"); 

      size_t boneCount = skeleton_.size(); 
      // Zapisujemy ilo�� ko�ci
      file->WriteInt((int)boneCount); 

      // Przegl�damy wszystkie ko�ci
      for(size_t i=0; i<boneCount; ++i)
      {
        // Zapisujemy Id ko�ci
        file->WriteInt((int)i); 

        FCDSceneNode *node = skeleton_[i].sceneNode_; 
        FCDSceneNodeTools::GenerateSampledAnimation(node); 

        FloatList keyTimes = FCDSceneNodeTools::GetSampledAnimationKeys(); 
        FMMatrix44List transformations = FCDSceneNodeTools::GetSampledAnimationMatrices(); 

        // Zapis �cie�ki dla i-tej ko�ci 
        // Zapisujemy ilo�� kluczy w danej �cie�ce
        size_t keyCount = keyTimes.size(); 
        file->WriteInt((int)keyCount); 
        // lecimy po wszystkich kluczy dla 
        for(size_t j=0; j<keyCount; ++j)
        {
          // Zapisujemy dan� �cie�k�
          // Zapis aktualnego czasu
          float time = keyTimes[j]; 
          file->WriteFloat(time); 
          // Zapis danych macierzy, zapis kwaterniona, przesuni�cia, oraz warunkowo skali
          FMMatrix44 transformation = transformations[j];  

          FMVector3 translation = transformation.GetTranslation(); 
          FMQuaternion rotation = FMQuaternion::MatrixRotationQuaternion(transformation);

          // Zapis poszczeg�lnych element�w, najpierw wektor przesuni�cia, potem kwaternion
          // Zapis wektora przesuni�cia
          file->WritePoint3(translation); 
          // zapis kwaterniona
          file->WriteQuaternion(rotation); 
        }
      }
    }
    file->EndChunk(FMODEL_SKELETON_ANIMATION); 
  }
  file->EndChunk(FMODEL_PLUGIN); 
}