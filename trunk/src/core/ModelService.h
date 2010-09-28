#ifndef MODEL_SERVICE_H
#define MODEL_SERVICE_H

#include <iostream>
#include <vector>

#include <core/GlobalServicesIDs.h>
#include <core/SimpleFunctors.h>
#include <core/IModelService.h>

class IModel;
class ServiceManager;

//--------------------------------------------------------------------------------------------------
// Model Service
//--------------------------------------------------------------------------------------------------
class ModelService: public IModelService
{
    UNIQUE_ID('MODL','SRVC');
public:
    ModelService();
    virtual ~ModelService();

    virtual AsyncResult OnTick(double delta); 
    virtual AsyncResult OnAdded(IServiceManager* serviceManager); 

    void AddModel(IModel* model);
    void Set(IModel* model, int i = 0);
    void Clear();
    IModel* GetModel(int i = 0);

private: 
    M_DECLARE_CLASS(); 

    std::vector<IModel* > _modelList;

    ServiceManager* m_pServiceManager;
};


#endif //MODEL_SERVICE_H