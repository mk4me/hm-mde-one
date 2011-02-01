#pragma once

class VisualScene
{
  // Wêze³ korzeniowy sceny
  FCDSceneNode *sceneRoot_; 
  // Dokument COLLADAy
  FCDocument *colladaDocument_; 
  // Instancje geometrii znalezione na scenie 
  GeometryMeshInstanceVector meshInstances_; 
  // Szkielet. Jest tylko jeden na scene, jest jednak wiele rootów. 
  Skeleton *skeleton_; 

  // Inicjalizacja sceny 
  void Initialize(); 
  // Analizuje wêzê³ 
  void ParseSceneNode(FCDSceneNode *node); 
  // Analizowanie treœci wêz³a na scenie
  bool AnalizeSceneNode( FCDSceneNode *node ); 
  // Analizowanie wêz³a (nie Jointa)
  bool AnalizeNode( FCDSceneNode * node ); 
  // Analizowanie Jointa, czyli w zasadzie koœci szkieletu 
  bool AnalizeJoint( FCDSceneNode * node );  
  // Sprawdza czy joint jest rootem 
  bool IsJointRoot( FCDSceneNode *node ); 
  // Pobieramy animacje z biblioteki animacji 
  void GetAnimations(); 
  

  void GetUnskinnedGeometry( const FCDEntityInstance * entityInstance, FMMatrix44 &nodeTransform ); 
  void GetSkinnedGeometry( FCDControllerInstance * controllerInstance, FCDGeometry *geometry, FMMatrix44 &nodeTransform ); 
  // Czyœci klasê 
  void Clear(); 

public:
  VisualScene(void); 
  VisualScene(FCDocument *colladaDocument); 
  ~VisualScene(void);

  size_t GetMeshInstanceCount(); 
  MeshInstance *GetMeshInstance(size_t idx); 

  Skeleton *GetSkeleton(); 

  // Pobiera wszystkie instancje meshów z ich macierz¹ transformacji 
  //size_t GetMeshCount(); 
  //void GetMesh(size_t idx, GeometryMesh *pMesh); 

};
