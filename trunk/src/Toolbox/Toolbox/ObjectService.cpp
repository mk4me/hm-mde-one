#include "ObjectService.h"

#include "AnimationService.h"
#include "ServiceManager.h"

using namespace std;
using namespace osg;

M_DECLARED_CLASS(ObjectService, kCLASSID_ObjectService);


//--------------------------------------------------------------------------------------------------
// add model
void ObjectService::addModel(CModel* model)
{ 
//	clearAll(); 
	_model = model; 

	// update services
//	ServiceManager::GetInstance()->RegisterServiceAs<AnimationService>(); 
//	AnimationService* pAnimation = ServiceManager::GetInstance()->GetSystemServiceAs<AnimationService>(); 
//	pAnimation->recalculateScene();
}


//--------------------------------------------------------------------------------------------------
void ObjectService::clearAll()
{ 
	if (_model) 
		delete _model; 

//	ServiceManager::GetInstance()->RegisterServiceAs<AnimationService>(); 
//	AnimationService* pAnimation = ServiceManager::GetInstance()->GetSystemServiceAs<AnimationService>(); 
//	pAnimation->clearAll();
} 


//--------------------------------------------------------------------------------------------------
ObjectService::ObjectService(void)
: _model(NULL)
{
}


//--------------------------------------------------------------------------------------------------
ObjectService::~ObjectService(void)
{
	if (_model) 
		delete _model; 
}


//--------------------------------------------------------------------------------------------------
AsyncResult ObjectService::OnTick(double delta)
{ 
	return AsyncResult_Complete; 
}


//--------------------------------------------------------------------------------------------------
AsyncResult ObjectService::OnAdded()
{
    _viewModelFlag = MODEL_FLAG::MODEL | MODEL_FLAG::MATERIAL;
    _primitiveModeFlag = osg::PrimitiveSet::TRIANGLES;
	return AsyncResult_Complete; 
}