#ifndef SERVICE_MANAGER_H
#define SERVICE_MANAGER_H

#include <core/IServiceManager.h>
#include <core/IBaseService.h>
#include <map>

class IModel;
class IDataManager;

class ServiceManager: public IServiceManager
{
public:
    ServiceManager(void);
	virtual ~ServiceManager(void);


    virtual IBaseService* GetSystemService(ClassID classID);
    virtual void RegisterServiceAs(IBaseService* newService, ClassID classID);

    virtual void SetModel(IModel* model, IDataManager* manager);
    virtual void SetModel(IModel* model);

    virtual void OnTick(double delta); 

private: 

    std::map<ClassID, IBaseService *> _services; 
};

#endif //SERVICE_MANAGER_H
