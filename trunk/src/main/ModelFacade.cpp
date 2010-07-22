#include "ModelFacade.h"

//---------------------------------------------------------------------------------------------
// do poprawy potem.
// Asserts
//---------------------------------------------------------------------------------------------
#define EE_ASSERT(pred) (void)false


//---------------------------------------------------------------------------------------------
void ModelFacade::inicjalization()
{
    _objectService = ServiceManager::GetInstance()->GetSystemServiceAs<ObjectService>();
    EE_ASSERT(_objectService);

    _animationService = ServiceManager::GetInstance()->GetSystemServiceAs<AnimationService>();
    EE_ASSERT(_animationService);
}

//---------------------------------------------------------------------------------------------
Model* ModelFacade::getModelWithSkeleton()
{
    if(_modelWithSkeleton)
        return _modelWithSkeleton;

    return NULL;
}

//---------------------------------------------------------------------------------------------
Animation* ModelFacade::getAnimation()
{
    if(_animation)
        return _animation;

    return NULL;
}

//---------------------------------------------------------------------------------------------
SMesh* ModelFacade::getMesh()
{
    if(_mesh)
        return _mesh;

    return NULL;
}

//---------------------------------------------------------------------------------------------
osg::ref_ptr<osg::Group> ModelFacade::getRoot()
{
    if(_root)
        return _root;

    return NULL;
}

//---------------------------------------------------------------------------------------------
osg::ref_ptr<osg::Group> ModelFacade::getSkeleton()
{
    if(_skeleton)
        return _skeleton;

    return NULL;
}

//---------------------------------------------------------------------------------------------
ModelFacade::ModelFacade(
    Model* modelWithSkeleton,
    Animation* animation,
    SMesh* mesh,
    osg::ref_ptr<osg::Group> root,
    osg::ref_ptr<osg::Group> skeleton )
    : _modelWithSkeleton(modelWithSkeleton)
    , _animation(animation)
    , _mesh(mesh)
    , _root(root)
    , _skeleton(skeleton)
{
    inicjalization();
}

//---------------------------------------------------------------------------------------------
void ModelFacade::loadToSystem()
{

}

void ModelFacade::setNewModel(Model* model)
{
    _objectService->AddModel(model);
}
/*
//---------------------------------------------------------------------------------------------
bool ModelFacade::SetModel(ModelFacade* modelFacade)
{
    _modelFacade = modelFacade;
    return true;
}

//---------------------------------------------------------------------------------------------
ModelFacade* ModelFacade::getModel()
{
    if(_modelFacade)
        return _modelFacade;

    return NULL;
}
*/