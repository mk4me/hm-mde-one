#ifndef I_SERVICE_MANAGER_H
#define I_SERVICE_MANAGER_H

#include <core/IBaseService.h>
#include <map>

class IModel;
class IDataManager;
class IBaseService;

class IServiceManager
{
public:
    virtual ~IServiceManager(void) {};
    virtual IBaseService* GetSystemService(ClassID classID) = 0;
    virtual void RegisterServiceAs(IBaseService* newService, ClassID classID) = 0;
    virtual void SetModel(IDataManager* dataManager) = 0;
};


#endif //I_SERVICE_MANAGER_H
