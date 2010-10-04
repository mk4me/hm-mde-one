#ifndef I_ANIMATION_SERVICE_H
#define I_ANIMATION_SERVICE_H

#include <map>
#include <vector>
#include <string>

#include <plugins/animation/Animation.h>
#include <core/GlobalServicesIDs.h>
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
class IDataManager;
class IServiceManager;



//--------------------------------------------------------------------------------------------------
// Animation Service
//--------------------------------------------------------------------------------------------------
class IAnimationService: public IBaseService
{
public:
    virtual ~IAnimationService() {};
    virtual AsyncResult OnTick(double delta) = 0; 
    virtual AsyncResult OnAdded(IServiceManager* serviceManager) = 0; 

    virtual void SetModel(IDataManager* dataManager) = 0;

    virtual void RegisterAnimation(Animation* object, void (Animation::*fun)(double)) = 0; // add function to caller
    virtual void SetSelectedAnimationName(std::string& name) = 0; // set act selected animation name 
    virtual void PlayAnimation(std::string animationName) = 0;
    virtual void SetScene(osgViewer::Scene* scene) = 0; // set animated scene
    virtual void LoadAnimation(IModel* model) = 0;
    virtual void ClearCaller() = 0; // clear caller
    virtual void NotifyStop() = 0; // notify stop

    virtual bool UnregisterAnimation() = 0; // remove function from caller


    virtual std::string& GetSelectedAnimationName() = 0; // returns name of act selected animation

    virtual Animation* GetAnimation() = 0; // returns act animation
    virtual std::map<std::string, Animation*>* GetAnimations() = 0;

    virtual void UpdateBone(osg::PositionAttitudeTransform* bone) = 0;
    virtual void RecalculateChanges() = 0;
    virtual void UpdateSkeleton() = 0;
    virtual void UpdateMesh() = 0;
    virtual void Clear() = 0;
};

#endif //I_ANIMATION_SERVICE_H