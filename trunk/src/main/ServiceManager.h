#ifndef SERVICE_MANAGER_H
#define SERVICE_MANAGER_H

#include "../include/IServiceManager.h"
#include "../include/IBaseService.h"
#include <map>

class ServiceManager: public IServiceManager
{
public:
    ServiceManager(void);
	virtual ~ServiceManager(void);


    virtual IBaseService* GetSystemService(ClassID classID);
    virtual void RegisterServiceAs(IBaseService* newService, ClassID classID);

    virtual void OnTick(double delta); 

private: 

    std::map<ClassID, IBaseService *> _services; 
};

#endif //SERVICE_MANAGER_H
