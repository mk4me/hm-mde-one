/********************************************************************
	created:  
	created:  
	filename: IAnimationServicec.h
	author:	  Rafal Zowal
	
	purpose: Interfejs do u³ugi zarz¹dzania Animacji

*********************************************************************/

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
class IAnimationService: public core::IService
{
public:
    virtual ~IAnimationService() {};

        //************************************
    // Method:    RegisterAnimation
    // FullName:  AnimationService::RegisterAnimation
    // Access:    public 
    // Returns:   void
    // Qualifier:
    // Parameter: Animation * object  - obiekt prezchowujacy dane do animacji
    // Parameter: void (Animation::*fun)(double)) - wskaŸnik na metode wywo³ywana prezz functor - metoda znajdujaca sie w obiekcie wskazywanym prezz paramtr object
    //************************************
    virtual void RegisterAnimation(Animation* object, void (Animation::*fun)(double)) = 0; // add function to caller
    virtual void SetSelectedAnimationName(const std::string& name) = 0; // set act selected animation name 
    virtual void PlayAnimation(std::string animationName) = 0;
    //! \£adownie animcji z obiektów Model
    virtual void LoadAnimation(IModel* model) = 0;
    //! \£adownie animcji z obiektów C3DModel
    virtual void LoadAnimation(IC3DModel* model) = 0;
    virtual void ClearCaller() = 0; // clear caller
    virtual void NotifyStop() = 0; // notify stop


    virtual std::string& GetSelectedAnimationName() = 0; // returns name of act selected animation

    virtual Animation* GetAnimation() = 0; // returns act animation
    virtual std::map<std::string, Animation*>* GetAnimations() = 0;

    virtual void GPUUpdateMesh() = 0;
    virtual void CPUUpdateMesh() = 0;
    virtual void Clear() = 0;

    //! \ Sprawdzenie czy istenieje animacja z modelu - amc
    virtual bool IsModelAnimation() = 0;

    virtual void CreatingAndRenderMarkerPath(std::vector<int > markerSelectedList) = 0;
};

#endif //I_ANIMATION_SERVICE_H