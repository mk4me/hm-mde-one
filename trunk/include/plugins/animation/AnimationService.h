#ifndef ANIMATION_SERVICE_H
#define ANIMATION_SERVICE_H

#include <map>
#include <vector>
#include <string>

#include <plugins/animation/Animation.h>
#include <core/IAnimationService.h>
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
class QtWidget;


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
class AnimationService: public IAnimationService
{
    UNIQUE_ID('ANIM','SRVC');
public:
    AnimationService(void);

    virtual ~AnimationService(void);
    virtual AsyncResult update(double time, double timeDelta); 
    virtual AsyncResult init(IServiceManager* serviceManager, osg::Node* sceneRoot); 

    virtual AsyncResult loadData(IServiceManager* serviceManager, IDataManager* dataManager);

    void RegisterAnimation(Animation* object, void (Animation::*fun)(double)); // add function to caller
    void SetSelectedAnimationName(const std::string& name); // set act selected animation name 
    void PlayAnimation(std::string animationName);
    //void SetScene(osg::Node* scene); // set animated scene
    void LoadAnimation(IModel* model);
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

// IService (Piotr Gwiazdowski)
public:
    // HACK ten podzia³ powinien byæ tymczasowy
    virtual IWidget* getWidget();
    virtual QtWidget* getQtWidget();
    
    double getTargetTime() const;
    void setTargetTime(double time);
    double getLength() const;
    void setLength(double length);
    //! \return
    bool getFollowTimeline() const;
    //! \param followTimeline
    void setFollowTimeline(bool followTimeline);

    virtual const std::string& getName() const
    {
        return name;
    }
    
private:
    double length;
    double targetTime;
    OpenThreads::Mutex stateMutex;
    bool followTimeline;
    Animation* currentAnimation;
    //! Nazwa.
    std::string name;

protected: 
    // scene
    osg::ref_ptr<osg::Node> m_pScene;

private: 
    void UpdateBone(osg::PositionAttitudeTransform* bone);
    void RecalculateChanges();
    void UpdateSkeleton();
    void UpdateMesh();
    void Clear();

    std::vector<std::vector<ISimpleOneArgFunctor<double>*>::iterator> m_functionsToRemove; // functions to remove from caller
    std::vector<ISimpleNoArgFunctor*> m_functionsToCallWhenAnimationStopped; // functions called when animation is being unregistered
    std::vector<ISimpleOneArgFunctor<double>*> m_functionsToCall; // functions called every frame
    std::vector<ISkeletonNode*>* m_pJoints;

    std::map<std::string, Animation*> m_animations;

    ISimpleOneArgFunctor<double>* m_pAnimation;

    unsigned int m_numOfBones;

    std::string m_selectedAnimatonName;

    osg::ref_ptr<osg::Group> m_skeleton;
    osg::ref_ptr<osg::Geode> m_skeletonGeode;

    IModel* m_pModel;

    STransform* m_pInitialBones;
    STransform* m_pActualBones;

    osg::Vec3d _tempVectors[MAX_AFFECTING_BONES][2];

    IServiceManager* m_pServiceManager;
    QtWidget* widget;
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


typedef CORE_SHARED_PTR(AnimationService) AnimationServicePtr;
typedef CORE_CONST_SHARED_PTR(AnimationService) AnimationServiceConstPtr;
typedef CORE_WEAK_PTR(AnimationService) AnimationServiceWeakPtr;

#endif //ANIMATION_SERVICE_H