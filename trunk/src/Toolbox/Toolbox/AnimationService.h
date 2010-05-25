#ifndef ANIMATION_SERVICE_H
#define ANIMATION_SERVICE_H

#include <map>
#include <vector>
#include <string>

#include "Model.h"
#include "Animation.h"
#include "GlobalServicesIDs.h"
#include "SimpleFunctors.h"

namespace osgViewer
{
    class Scene; 
}; 

//--------------------------------------------------------------------------------------------------
// Animation Service
//--------------------------------------------------------------------------------------------------
class AnimationService: public IBaseService
{
public:
    AnimationService(void);
    virtual ~AnimationService(void);
    // set act selected animation name
    void SetSelectedAnimationName(std::string& name); 
    // returns name of act selected animation
    std::string& GetSelectedAnimationName(); 
    // add function to caller
    void RegisterAnimation	(Animation* object, void (Animation::*fun)(double));
    // remove function from caller
    bool UnregisterAnimation();
    // adds function to caller - called on every animation update
    template<class T> void RegisterFunction(T* object, void (T::*fun)(double)); 
    // remove function from caller
    template<class T> bool UnregisterFunction(T* object, void (T::*fun)(double)); 
    // adds function to caller - called on stop of animation
    template<class T> void RegisterOnStopFunction(T* object, void (T::*fun)()); 
    // notify stop
    void NotifyStop();
    // clear caller
    void ClearCaller();
    // set animated scene
    void SetScene(osgViewer::Scene* scene); 
    // returns act animation
    Animation* GetAnimation();
    // update
    virtual AsyncResult OnTick(double delta); 
    // 
    virtual AsyncResult OnAdded(); 
protected: 
    // scene
    osgViewer::Scene* _scene; 

private: 
    M_DECLARE_CLASS(); 

    ISimpleOneArgFunctor<double>* _animation;
    // functions called every frame
    std::vector<ISimpleOneArgFunctor<double>*> _functionsToCall;
    // functions to remove from caller
    std::vector<std::vector<ISimpleOneArgFunctor<double>*>::iterator> _functionsToRemove;
    // functions called when animation is being unregistered
    std::vector<ISimpleNoArgFunctor*> _functionsToCallWhenAnimationStopped;
    std::string _selectedAnimatonName;
    // clear all
    void Clear();
    
};

//--------------------------------------------------------------------------------------------------
template<class T>
void AnimationService::RegisterFunction(T* object, void (T::*fun)(double))
{
    _functionsToCall.push_back(new CSimpleOneArgFunctor<T, double>(object, fun));
}

//--------------------------------------------------------------------------------------------------
template<class T>
bool AnimationService::UnregisterFunction(T* object, void (T::*fun)(double))
{
    // TODO:
    // na razie zreb - zanim nie zaczne tego uzywac..
    CSimpleOneArgFunctor<T, double> f(object, fun);
    for (std::vector<ISimpleOneArgFunctor<double>*>::iterator i = _functionsToCall.begin(); i != _functionsToCall.end(); ++i)
    {
        CSimpleOneArgFunctor<T, double>* pf = dynamic_cast<CSimpleOneArgFunctor<T, double>*>(*i);

        if (pf && ((*pf) == f))
        {
            _functionsToRemove.push_back(i);
            return true;
        }
    }
    return false;
}

//--------------------------------------------------------------------------------------------------
template<class T>
void AnimationService::RegisterOnStopFunction(T* object, void (T::*fun)())
{
    _functionsToCallWhenAnimationStopped.push_back(new CSimpleNoArgFunctor<T>(object, fun));
}

#endif //ANIMATION_SERVICE_H