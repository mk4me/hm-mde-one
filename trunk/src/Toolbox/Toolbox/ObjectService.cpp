#include "ObjectService.h"

#include "AnimationService.h"
#include "ServiceManager.h"

using namespace std;
using namespace osg;

M_DECLARED_CLASS(ObjectService, kCLASSID_ObjectService);


//--------------------------------------------------------------------------------------------------
ObjectService::ObjectService(void): 
  _model(NULL)
{
}


//--------------------------------------------------------------------------------------------------
ObjectService::~ObjectService(void)
{
    if (_model) 
        delete _model; 
}

//--------------------------------------------------------------------------------------------------
void ObjectService::Clear()
{ 
    if (_model) 
        delete _model; 

    //ServiceManager::GetInstance()->RegisterServiceAs<AnimationService>(); 
    //AnimationService* pAnimation = ServiceManager::GetInstance()->GetSystemServiceAs<AnimationService>(); 
    //pAnimation->ClearAll();
} 

//--------------------------------------------------------------------------------------------------
void ObjectService::AddModel(Model* model)
{ 
    //clearAll(); 
	_model = model; 

	// Update services
    //ServiceManager::GetInstance()->RegisterServiceAs<AnimationService>(); 
    //AnimationService* pAnimation = ServiceManager::GetInstance()->GetSystemServiceAs<AnimationService>(); 
    //pAnimation->recalculateScene();
}

//--------------------------------------------------------------------------------------------------
Model* ObjectService::GetModel()
{
    return _model; 
}

//--------------------------------------------------------------------------------------------------
void ObjectService::SetModel( Model* model )
{
    AddModel(model); 
}

//--------------------------------------------------------------------------------------------------
AsyncResult ObjectService::OnTick(double delta)
{ 
	return AsyncResult_Complete; 
}

//--------------------------------------------------------------------------------------------------
AsyncResult ObjectService::OnAdded()
{
    _viewModelFlag = ObjectService::MODEL | ObjectService::MATERIAL;
    _primitiveModeFlag = osg::PrimitiveSet::TRIANGLES;
	return AsyncResult_Complete; 
}

//--------------------------------------------------------------------------------------------------
void ObjectService::AddVieModelFlag( ObjectService::ViewMode flag )
{
    if(_viewModelFlag & flag)
        RemoveVieModelFlag(flag);
    else
        _viewModelFlag |= flag;
}

//--------------------------------------------------------------------------------------------------
void ObjectService::RemoveVieModelFlag(ObjectService::ViewMode flag )
{
    _viewModelFlag &= ~flag;
}

//--------------------------------------------------------------------------------------------------
int ObjectService::GetViewModelFlag()
{
    if(_viewModelFlag == NULL)
        return ObjectService::NONE;
    return _viewModelFlag;
}

//--------------------------------------------------------------------------------------------------
void ObjectService::SetPrimitiveModeFlag( osg::PrimitiveSet::Mode flag )
{
    _primitiveModeFlag = flag;
    _viewModelFlag |= ObjectService::MODEL;
}

//--------------------------------------------------------------------------------------------------
int ObjectService::GetPrimitiveModeFlag()
{
    return _primitiveModeFlag;
}

//--------------------------------------------------------------------------------------------------
void ObjectService::SetViewModelFlag(int flag )
{
    _viewModelFlag = flag;
}

