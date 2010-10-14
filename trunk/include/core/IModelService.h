#ifndef I_MODEL_SERVICE_H
#define I_MODEL_SERVICE_H


class IModel;
class IServiceManager;

//--------------------------------------------------------------------------------------------------
// Model Service
//--------------------------------------------------------------------------------------------------
class IModelService: public IBaseService
{
public:

    virtual ~IModelService() {};

    virtual AsyncResult OnAdded(IServiceManager* serviceManager) = 0; 

    virtual void SetModel(IDataManager* dataManager) = 0;

    virtual void AddModel(IModel* model) = 0;
    virtual void Set(IModel* model, int i = 0) = 0;
    virtual void Clear() = 0;
    virtual IModel* GetModel(int i = 0) = 0;
};


#endif //I_MODEL_SERVICE_H