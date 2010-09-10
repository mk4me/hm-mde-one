#ifndef MODEL_SERVICE_H
#define MODEL_SERVICE_H

#include <iostream>
#include <vector>

#include "../include/GlobalServicesIDs.h"
#include "../include/SimpleFunctors.h"

class Model;
class ServiceManager;

//--------------------------------------------------------------------------------------------------
// Model Service
//--------------------------------------------------------------------------------------------------
class ModelService: public IBaseService
{
public:
    ModelService();
    virtual ~ModelService();

    virtual AsyncResult OnTick(double delta); 
    virtual AsyncResult OnAdded(IServiceManager* serviceManager); 

    void AddModel(Model* model);
    void Set(Model* model, int i = 0);
    void Clear();
    Model* GetModel(int i = 0);

protected: 


private: 
    M_DECLARE_CLASS(); 

    std::vector<Model* > _modelList;

    ServiceManager* m_pServiceManager;
};


#endif //MODEL_SERVICE_H