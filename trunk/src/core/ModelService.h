#ifndef MODEL_SERVICE_H
#define MODEL_SERVICE_H

#include <iostream>
#include <vector>

#include <core/GlobalServicesIDs.h>
#include <core/SimpleFunctors.h>
#include <core/IModelService.h>

class IModel;
class IDataManager;

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

    virtual AsyncResult init(IServiceManager* serviceManager, osg::Node* sceneRoot); 

    virtual AsyncResult loadData(IServiceManager* serviceManager, IDataManager* dataManager);

    void AddModel(IModel* model);
    void Set(IModel* model, int i = 0);
    void Clear();
    IModel* GetModel(int i = 0);

    virtual const std::string& getName() const
    {
        return name;
    }
    virtual IWidget* getWidget()
    { 
        return NULL;
    }

private: 
    std::vector<IModel* > _modelList;
    std::string name;
    ServiceManager* m_pServiceManager;
};


#endif //MODEL_SERVICE_H