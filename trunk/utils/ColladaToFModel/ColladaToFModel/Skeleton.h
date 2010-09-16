#pragma once

struct Bone
{
  Bone() :
    sceneNode_(NULL)
  {
    childIds_.clear(); 
  }

  Bone(const Bone &bone)
  {
    *this = bone; 
  }

  Bone &operator=(const Bone &bone)
  {
    sceneNode_ = bone.sceneNode_; 
    parentId_ = bone.parentId_; 
    name_ = bone.name_; 
    childIds_.clear(); 
    size_t childCount = bone.childIds_.size(); 
    for(size_t i=0; i<childCount; ++i)
    {
      childIds_.push_back(bone.childIds_[i]); 
    }
    return *this; 
  }

  // Nazwa ko�ci 
  fm::string name_; 
  // Wska�nik na SceneNode kt�ry reprezentuje dana ko�� 
  FCDSceneNode *sceneNode_; 
  // Indeks rodzica
  int parentId_; 
  // Indeksy ko�ci potomnych 
  fm::vector<int> childIds_; 

  // Transformacja 
};

class Skeleton
{
  fm::vector<Bone> skeleton_; 

  // Dodaje hierarchi� ko�ci do tablicy 
  int AssignBone(FCDSceneNode *node, int parentId); 

public:
  Skeleton(void);
  ~Skeleton(void);

  // Dodaje hierarchi� ko�ci do aktualnego szkieletu 
  void AddBones(FCDSceneNode *sceneNode); 

  // Zapisywanie kawa�ka ze szkieletem  
  void Save(FileChunk *file); 

  // Zapisywanie animacji szkieletowej 
  void SaveAnimation(FileChunk *file); 

  // Sprawdzanie czy szkielet jest pusty 
  bool IsEmpty() { return skeleton_.size() == 0; }

  // Pobieranie liczby ko�ci 
  size_t GetBoneCount() { return skeleton_.size(); }

  // Pobieranie ko�ci 
  Bone *GetBone(size_t idx) {if(idx<skeleton_.size() && idx>=0) return &skeleton_[idx]; else return NULL; }

  // Pobieranie Id ko�ci 
  int GetBoneId(FCDSceneNode *node); 
};
