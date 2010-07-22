//==============================================================================================================//
// (szkic)                                                                                                      //
// Opis dzia³ania klasy:                                                                                        //
// Klasa ma za zadanie przechowywaæ i zarz¹dzaæ wszelkimi informacjami na temat modelu przez termin który       //
// rozumiemy animacje, szkielet, siatke, texture.                                                               //
// Budowa tego obiektu ma slu¿yæ do lepszej organizacji innych podzespo³ów ( to jest, do tej pory istia³y       //
// obiekty typu animacje, model itp, które mog³y ¿yæ sobie w³asnym ¿yciem, bez uwzglêdnienia innych             //
// obiektów, wynikiem tego by³ brak rozeznania co by³o porzebne, b¹dŸ uruchomienia aplikacji na œlepo,          //
// a stworzenie nowego obiektu nie gwarantowa³o poprawne jego dzia³anie, wystêpowa³ wyciek pamiêci (zmiana      //
// wymaga rejestracji danych w zmiennych wyœwietlaj¹cych dane w QT, przypisanie zmiennych do roota itp) albo    //
// wyrzucenie b³êdu przez apikacje, podczas w³¹czenia, b¹dŸ wyboru jakiejœ metody operuj¹cej na modelu          //
// (animacja, zaznaczenie koœci itp...))                                                                        //
//                                                                                                              //
// (szkic)                                                                                                      //
// Dziêki klasie bêdzie mo¿na w ³awy sposób manipulowaæ modelem, b¹dŸ wysy³anie/³adowanie (import/export)       //
// modelu w sosób zapewniajacy jego dzia³anie. (nie tylko ca³ego modlu, ale np. tylko jego animacji,            //
// pozycji koœci)                                                                                               //
// Przede wszystkim ³adowanie modelu, bêdzie mo¿liwe, bez koniecznoœci wczytywanie wszytkiego z pliku           //
// poprzez plug-in'a - który to wczytuje wszystko po koleji (przerywajac dzia³anie, na pierwszym napotkanym     //
// problemie (chwilo na sztywno fixniete) - brak istnienia jednego z elementów modelu (mesh, bone, animacje)    //
// powoduje nie za³adownie modelu, w najlepszym przypadku nie wyrzecenie b³edu przy próbie ingerencji na modelu //
// to juz jest zrobione na sztywno poprzez zmine warunków w plug-in, ale warto by by³o pokusiæ sie o prerzucenie//
// ca³ego zarz¹dzania tutaj                                                                                     //
//==============================================================================================================//

// TODO:  rozwazyæ dodanie nowej klasy - typu Most jako pomostu do ³adowania modelu ró¿nymi sposobami.

// TODO:  rozwazyæ myœl o tym aby by³ to SINGLETON

#ifndef MODEL_FACADE_H
#define MODEL_FACADE_H

#include "ServiceManager.h"
#include "AnimationService.h"
#include "ObjectService.h"
#include "Model.h"
#include "Animation.h"
#include "SkeletonNode.h"

#include "../osgDBPlugin/ModelData.h"


#define pPat osg::PositionAttitudeTransform*

class ModelFacade
{
public:

    ModelFacade(Model* modelWithSkeleton, Animation* animation, SMesh* mesh, osg::ref_ptr<osg::Group> root, osg::ref_ptr<osg::Group> skeleton);

    // seting method
    void setModelWithSkeleton(Model* modelWithSkeleton)    { _modelWithSkeleton = modelWithSkeleton;}

    void setAnimation(Animation* animation)                            {_animation = animation;}

    void setMesh(SMesh* mesh)                                           {_mesh = mesh;}

    void setRoot(osg::ref_ptr<osg::Group> root)                         {_root = root;}

    void setSkeleton(osg::ref_ptr<osg::Group> skeleton)                 {_skeleton = skeleton;}

    // geting method
    Model* getModelWithSkeleton();
    Animation* getAnimation();
    SMesh* getMesh();
    osg::ref_ptr<osg::Group> getRoot();
    osg::ref_ptr<osg::Group> getSkeleton();

    // get skeleton
    // set skeleton
    // get animationList
    // set animationList

  //  bool SetModel(ModelFacade* modelFacade);
  //  ModelFacade* getModel();


    void setNewModel(Model* model);
    void inicjalization();

protected:
private:
    void loadToSystem();

 //   ModelFacade                 _modelFacade;
    
  
    // services
    ObjectService*              _objectService;
    AnimationService*           _animationService;

    // variables
    Model*         _modelWithSkeleton;     // posiada juz w sobie liste animacji.
    osg::ref_ptr<osg::Group>	_root;
    Animation*                 _animation;

    SMesh*                      _mesh;

    // skeleton
    osg::ref_ptr<osg::Group>	_skeleton;              // (w momencie przeszukiwania) pPat skeleton = dynamic_cast<CSkeletonNode*>(_skeleton.get());


    // animationList

};


#endif  // MODEL_FACADE_H