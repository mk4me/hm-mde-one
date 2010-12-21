/********************************************************************
	created:  
	created:  
	filename: Model.h
	author:	  Rafal Zowal
	
	purpose: G�owny model zawieraj�cy informacje niezb�dne do reenderingu obiektu
    jak i w�ywowi na jego w�asno�ci.

    Zawiera takie elemetny jak

        - siatka
        - szkielet
        - ko�ci (jointy)

    przystosowany do danych z acclaima jak i starego sposopbu wczytuwania danych z tbsu zawieraj�cego fmesha
*********************************************************************/

#ifndef NEW_MODEL_H
#define NEW_MODEL_H


#include <vector>
#include <string>

#include <osg/PositionAttitudeTransform>
#include <osg/Group>
#include <osg/Texture2D>
#include <osg/Material>
#include <osgDB/ReadFile>

#include <core/IModel.h>
#include <core/IMesh.h>
#include <core/Skeleton.h>
#include <core/Frame.h>

class Mesh;
class ISkeletonNode;
class SkeletonNode;
struct Frame;
struct SMaterial;

#define MAX_AFFECTING_BONES 8

class Model:public IModel, public osg::Group
{
public:
    Model();
    virtual ~Model();

    //! \Czyszczenie zasob�w modelu
    virtual void Clear();
    //! \Dodanie siatki do modelu
    virtual void AddMesh(IMesh* mesh);
    //! \Inicjalizacja siatek - kt�re aktualnie posiada model
    virtual void InicializeMesh();
    //! \ustawienie szkieletu modelu
    virtual void SetSkeleton(Skeleton* skeleton);
    //! \ustawienie listy animacji modelu
    virtual void SetAnimationList(SkeletonAnimationList* animation);
    //! \ustawienie listy joint�w do modelu - uzywane zw�aszcza wczytywaja� bezpo�rdenio dane z fmesha 
    virtual void SetJoints(std::vector<ISkeletonNode* >* joints);
    //! \�adowanie materia�u - tekstury
    virtual void ApplyMaterial(std::vector<SMaterial>* materialList, std::wstring textureDir);
    //! \utworzenie geode do wy�wietlenia szkieletu
    virtual void DrawModelBone();

    
    virtual void* GetSkeletonGroup();

    virtual IMesh* GetMesh(int i = 0);
    virtual std::vector<IMesh* >& GetMeshList();
    virtual std::vector<ISkeletonNode* >* GetJoints();
    virtual osg::ref_ptr<osg::Geode> GetSkeletonGeode();

    virtual Skeleton* GetSkeleton();
    //! \Pobranie listy animacji
    virtual SkeletonAnimationList* GetAnimationList();
    //! \ usuni�ci wszystkich geaode. np. geode posiadaj�ce zestawd danych potrzebnych do reenderingu kosci
    virtual void RemoveGeode();


    osg::ref_ptr<osg::Geometry>  m_geometry;
    osg::ref_ptr<osg::Geode> m_spSkeletonGeode;

    osg::ref_ptr<osg::Geode> m_spNormalGeode;

private:
    void CreateArrayOfJoints(SkeletonNode* bone);

    //! \za�adowanie kolejnej grupy obiekt�w takich jak box w celu uyzkania wizualizacji szkieletu
    void DrawBone(Bone* bone, osg::Geode* geode);
    osg::ref_ptr<osg::Geometry> DrawLine(const osg::Vec3f* startPos, const osg::Vec3f* endPos, bool isSelected);
    osg::ref_ptr<osg::Drawable> DrawBoxBone(const osg::Vec3f* startPos, const osg::Vec3f* endPos, osg::Quat* rotation, BoneLenght* boneLength, bool isSelected, BoneType boneType);


    std::vector<ISkeletonNode* >* m_pJoints;
    Skeleton* m_pSkeleton;
    SkeletonAnimationList* m_pAnimation;
    std::vector<IMesh* > m_meshList;
};


#endif //NEW_MODEL_H