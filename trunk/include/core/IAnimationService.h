#ifndef I_ANIMATION_SERVICE_H
#define I_ANIMATION_SERVICE_H

#include <map>
#include <vector>
#include <string>

#include <plugins/animation/Animation.h>

#include <core/IService.h>
#include <core/SimpleFunctors.h>

#include <osg/PositionAttitudeTransform>
#include <osg/Vec3d>
#include <osg/Quat>
#include <osg/ref_ptr>

namespace osgViewer
{
    class Scene; 
}; 


class IModel;
class IC3DModel;
class IDataManager;
class IServiceManager;



//--------------------------------------------------------------------------------------------------
// Animation Service
//--------------------------------------------------------------------------------------------------
class IAnimationService: public IService
{
public:
    virtual ~IAnimationService() {};

    virtual void RegisterAnimation(Animation* object, void (Animation::*fun)(double)) = 0; // add function to caller
    virtual void SetSelectedAnimationName(const std::string& name) = 0; // set act selected animation name 
    virtual void PlayAnimation(std::string animationName) = 0;
    //virtual void SetScene(osg::Node* scene) = 0; // set animated scene
    virtual void LoadAnimation(IModel* model) = 0;
    virtual void LoadAnimation(IC3DModel* model) = 0;
    virtual void ClearCaller() = 0; // clear caller
    virtual void NotifyStop() = 0; // notify stop

    virtual bool UnregisterAnimation() = 0; // remove function from caller
    virtual bool UnregisterC3DAnimation() = 0; // remove function from caller


    virtual std::string& GetSelectedAnimationName() = 0; // returns name of act selected animation

    virtual Animation* GetAnimation() = 0; // returns act animation
    virtual std::map<std::string, Animation*>* GetAnimations() = 0;

    virtual void GPUUpdateMesh() = 0;
    virtual void CPUUpdateMesh() = 0;
    virtual void Clear() = 0;
};

#endif //I_ANIMATION_SERVICE_H