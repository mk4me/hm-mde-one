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

  // Nazwa koœci 
  fm::string name_; 
  // WskaŸnik na SceneNode który reprezentuje dana koœæ 
  FCDSceneNode *sceneNode_; 
  // Indeks rodzica
  int parentId_; 
  // Indeksy koœci potomnych 
  fm::vector<int> childIds_; 

  // Transformacja 
};

class Skeleton
{
  fm::vector<Bone> skeleton_; 

  // Dodaje hierarchiê koœci do tablicy 
  int AssignBone(FCDSceneNode *node, int parentId); 

public:
  Skeleton(void);
  ~Skeleton(void);

  // Dodaje hierarchiê koœci do aktualnego szkieletu 
  void AddBones(FCDSceneNode *sceneNode); 

  // Zapisywanie kawa³ka ze szkieletem  
  void Save(FileChunk *file); 

  // Zapisywanie animacji szkieletowej 
  void SaveAnimation(FileChunk *file); 

  // Sprawdzanie czy szkielet jest pusty 
  bool IsEmpty() { return skeleton_.size() == 0; }

  // Pobieranie liczby koœci 
  size_t GetBoneCount() { return skeleton_.size(); }

  // Pobieranie koœci 
  Bone *GetBone(size_t idx) {if(idx<skeleton_.size() && idx>=0) return &skeleton_[idx]; else return NULL; }

  // Pobieranie Id koœci 
  int GetBoneId(FCDSceneNode *node); 
};
