#ifndef ANIMATION_SERVICE_H
#define ANIMATION_SERVICE_H

#include <map>
#include <vector>
#include <string>

#include "Animation.h"
#include "GlobalServicesIDs.h"
#include "SimpleFunctors.h"

#include <osg/Vec3d>
#include <osg/Quat>
#include <osg/ref_ptr>

namespace osgViewer
{
    class Scene; 
}; 

class NewModel;


#define MAX_AFFECTING_BONES 8



//////////////////////////////////////////////////////////////////////////
// struct used for holding bone
struct STransform
{
    // for hierarchy recalculation 
    osg::Vec3d	position;		// start point
    osg::Quat	attitude;		// this bone rotation
    bool hasChild;


    // for skinning purpose
    osg::Vec3d	point;			// bone position in space
    osg::Vec3d	bone;			// vector describing bone
    osg::Vec3d	translation;	// vector translation of bone during animation
    osg::Quat	rotation;		// rotation: orig. bone -> act bone
};


//--------------------------------------------------------------------------------------------------
// Animation Service
//--------------------------------------------------------------------------------------------------
class AnimationService: public IBaseService
{
public:
    AnimationService(void);

    virtual ~AnimationService(void);
    virtual AsyncResult OnTick(double delta); 
    virtual AsyncResult OnAdded(); 

    void RegisterAnimation(Animation* object, void (Animation::*fun)(double)); // add function to caller
    void SetSelectedAnimationName(std::string& name); // set act selected animation name 
    void PlayAnimation(std::string animationName);
    void SetScene(osgViewer::Scene* scene); // set animated scene
    void LoadAnimation(NewModel* model);
    void ClearCaller(); // clear caller
    void NotifyStop(); // notify stop

    bool UnregisterAnimation(); // remove function from caller

    // adds function to caller - called on every animation update
    template<class T> void RegisterFunction(T* object, void (T::*fun)(double)); 
    // remove function from caller
    template<class T> bool UnregisterFunction(T* object, void (T::*fun)(double)); 
    // adds function to caller - called on stop of animation
    template<class T> void RegisterOnStopFunction(T* object, void (T::*fun)()); 

    
    std::string& GetSelectedAnimationName(); // returns name of act selected animation

    Animation* GetAnimation(); // returns act animation
    std::map<std::string, Animation*>* GetAnimations();

protected: 
    // scene
    osgViewer::Scene* m_pScene; 

private: 
    M_DECLARE_CLASS(); 

    void UpdateBone(osg::PositionAttitudeTransform* bone);
    void CreateArrayOfJoints(SkeletonNode* bone);
    void RecalculateChanges();
    void UpdateSkeleton();
    void UpdateMesh();
    void Clear();

    std::vector<std::vector<ISimpleOneArgFunctor<double>*>::iterator> m_functionsToRemove; // functions to remove from caller
    std::vector<ISimpleNoArgFunctor*> m_functionsToCallWhenAnimationStopped; // functions called when animation is being unregistered
    std::vector<ISimpleOneArgFunctor<double>*> m_functionsToCall; // functions called every frame
    std::vector<SkeletonNode*>* m_pJoints;

    std::map<std::string, Animation*> m_animations;

    ISimpleOneArgFunctor<double>* m_pAnimation;

    unsigned int m_numOfBones;

    std::string m_selectedAnimatonName;

    osg::ref_ptr<osg::Group> m_skeleton;
    osg::ref_ptr<osg::Geode> m_skeletonGeode;

    NewModel* m_pModel;

    STransform* m_pInitialBones;
    STransform* m_pActualBones;

    osg::Vec3d _tempVectors[MAX_AFFECTING_BONES][2];
};

//--------------------------------------------------------------------------------------------------
template<class T>
void AnimationService::RegisterFunction(T* object, void (T::*fun)(double))
{
    m_functionsToCall.push_back(new CSimpleOneArgFunctor<T, double>(object, fun));
}

//--------------------------------------------------------------------------------------------------
template<class T>
bool AnimationService::UnregisterFunction(T* object, void (T::*fun)(double))
{
    // TODO:
    // na razie zreb - zanim nie zaczne tego uzywac..
    CSimpleOneArgFunctor<T, double> f(object, fun);
    for (std::vector<ISimpleOneArgFunctor<double>*>::iterator i = m_functionsToCall.begin(); i != m_functionsToCall.end(); ++i)
    {
        CSimpleOneArgFunctor<T, double>* pf = dynamic_cast<CSimpleOneArgFunctor<T, double>*>(*i);

        if (pf && ((*pf) == f))
        {
            m_functionsToRemove.push_back(i);
            return true;
        }
    }
    return false;
}

//--------------------------------------------------------------------------------------------------
template<class T>
void AnimationService::RegisterOnStopFunction(T* object, void (T::*fun)())
{
    m_functionsToCallWhenAnimationStopped.push_back(new CSimpleNoArgFunctor<T>(object, fun));
}

#endif //ANIMATION_SERVICE_H