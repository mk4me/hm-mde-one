//==============================================================================================================//
// (szkic)                                                                                                      //
// Opis dzia�ania klasy:                                                                                        //
// Klasa ma za zadanie przechowywa� i zarz�dza� wszelkimi informacjami na temat modelu przez termin kt�ry       //
// rozumiemy animacje, szkielet, siatke, texture.                                                               //
// Budowa tego obiektu ma slu�y� do lepszej organizacji innych podzespo��w ( to jest, do tej pory istia�y       //
// obiekty typu animacje, model itp, kt�re mog�y �y� sobie w�asnym �yciem, bez uwzgl�dnienia innych             //
// obiekt�w, wynikiem tego by� brak rozeznania co by�o porzebne, b�d� uruchomienia aplikacji na �lepo,          //
// a stworzenie nowego obiektu nie gwarantowa�o poprawne jego dzia�anie, wyst�powa� wyciek pami�ci (zmiana      //
// wymaga rejestracji danych w zmiennych wy�wietlaj�cych dane w QT, przypisanie zmiennych do roota itp) albo    //
// wyrzucenie b��du przez apikacje, podczas w��czenia, b�d� wyboru jakiej� metody operuj�cej na modelu          //
// (animacja, zaznaczenie ko�ci itp...))                                                                        //
//                                                                                                              //
// (szkic)                                                                                                      //
// Dzi�ki klasie b�dzie mo�na w �awy spos�b manipulowa� modelem, b�d� wysy�anie/�adowanie (import/export)       //
// modelu w sos�b zapewniajacy jego dzia�anie. (nie tylko ca�ego modlu, ale np. tylko jego animacji,            //
// pozycji ko�ci)                                                                                               //
// Przede wszystkim �adowanie modelu, b�dzie mo�liwe, bez konieczno�ci wczytywanie wszytkiego z pliku           //
// poprzez plug-in'a - kt�ry to wczytuje wszystko po koleji (przerywajac dzia�anie, na pierwszym napotkanym     //
// problemie (chwilo na sztywno fixniete) - brak istnienia jednego z element�w modelu (mesh, bone, animacje)    //
// powoduje nie za�adownie modelu, w najlepszym przypadku nie wyrzecenie b�edu przy pr�bie ingerencji na modelu //
// to juz jest zrobione na sztywno poprzez zmine warunk�w w plug-in, ale warto by by�o pokusi� sie o prerzucenie//
// ca�ego zarz�dzania tutaj                                                                                     //
//==============================================================================================================//

// TODO:  rozwazy� dodanie nowej klasy - typu Most jako pomostu do �adowania modelu r�nymi sposobami.

// TODO:  rozwazy� my�l o tym aby by� to SINGLETON

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