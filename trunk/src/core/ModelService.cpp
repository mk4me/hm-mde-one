#include "ModelService.h"

#include <core/IModel.h>
#include "ServiceManager.h"



M_DECLARED_CLASS(ModelService, kCLASSID_ModelService);

//--------------------------------------------------------------------------------------------------
ModelService::ModelService()
{

}

//--------------------------------------------------------------------------------------------------
ModelService::~ModelService()
{
    Clear();
}

//--------------------------------------------------------------------------------------------------
AsyncResult ModelService::OnTick( double delta )
{
    return AsyncResult_Complete; 
}

//--------------------------------------------------------------------------------------------------
AsyncResult ModelService::OnAdded(IServiceManager* serviceManager)
{
    m_pServiceManager = (ServiceManager*)serviceManager;

    std::cout<< "ModelService ADDED-test!" << std::endl; 
    return AsyncResult_Complete; 
}

//--------------------------------------------------------------------------------------------------
void ModelService::Clear()
{
    _modelList.clear();
}

//--------------------------------------------------------------------------------------------------
void ModelService::AddModel( IModel* model )
{
    _modelList.push_back(model);
}

//--------------------------------------------------------------------------------------------------
void ModelService::Set( IModel* model, int i /*= 0*/ )
{
    if(_modelList.size() > i)
        _modelList[i] = model;
    else
        _modelList.push_back(model);
}

//--------------------------------------------------------------------------------------------------
IModel* ModelService::GetModel( int i /*= 0*/ )
{
    if(_modelList.size() > i)
        return _modelList[i];

    return NULL;
}