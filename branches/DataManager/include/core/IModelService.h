#ifndef I_MODEL_SERVICE_H
#define I_MODEL_SERVICE_H

#include <core/IService.h>

class IModel;
class IServiceManager;

//--------------------------------------------------------------------------------------------------
// Model Service
//--------------------------------------------------------------------------------------------------
class IModelService: public IService
{
public:

    virtual ~IModelService() {};

    virtual void AddModel(IModel* model) = 0;
    virtual void Set(IModel* model, int i = 0) = 0;
    virtual void Clear() = 0;
    virtual IModel* GetModel(int i = 0) = 0;
};


#endif //I_MODEL_SERVICE_H