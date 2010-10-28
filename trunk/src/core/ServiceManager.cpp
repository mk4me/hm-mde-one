#include "CorePCH.h"
#include "ServiceManager.h"

#include <core/IModel.h>
#include <core/IDataManager.h>


ServiceManager::ServiceManager(void)
: updateMarker(NULL)
{
    resetTime();
}


ServiceManager::~ServiceManager(void)
{
	// free allocated memory
    // for (ServicesMap::iterator i = servicesMap.begin(); i != servicesMap.end(); ++i) {
    //     delete i->second;
    // }
}

void ServiceManager::registerService(IServicePtr service)
{
    if (servicesMap.find(service->getID()) == servicesMap.end()) {
        servicesMap.insert( std::make_pair(service->getID(), service)); 
        servicesList.push_back(service);
        //service->init(this); 
    } else {
        throw std::runtime_error("Service with this ID already registered.");
    }
};

void ServiceManager::updatePass()
{
    // ustawienia markera update'a
    updateMarker = OpenThreads::Thread::CurrentThread();

    // aktualizacja czasu
    updateTime();

    // aktualizacja us�ug
    for (ServicesList::iterator it = servicesList.begin(); it != servicesList.end(); ++it ) {
        (*it)->update(getTime(), getDeltaTime());
    }
    // drugi cykl aktualizacji
    for (ServicesList::iterator it = servicesList.begin(); it != servicesList.end(); ++it ) {
        (*it)->lateUpdate(getTime(), getDeltaTime());
    }

    // ustawienia markera update'a
    updateMarker = NULL;
}

void ServiceManager::computePass()
{
    // aktualizacja us�ug
    for (ServicesList::iterator it = servicesList.begin(); it != servicesList.end(); ++it ) {
        (*it)->compute();
    }
}

void ServiceManager::loadDataPass(IDataManager* dataManager)
{
    //TODO: prawdopodobnie trzeba to bedzie jakos poprawi�? zeby bylo asynchronicznie
    ServicesMap::iterator it = servicesMap.begin();  
    while (it != servicesMap.end())
    {
        it->second->loadData(this, dataManager); 
        it++; 
    }
}

int ServiceManager::getNumServices() const
{
    return static_cast<int>(servicesList.size());
}

IServicePtr ServiceManager::getService( int idx )
{
    if ( idx < static_cast<int>(servicesList.size()) ) {
        return servicesList[idx];
    } else {
        return IServicePtr();
    }
}

IServicePtr ServiceManager::getService( UniqueID id )
{
    ServicesMap::iterator it = servicesMap.find(id);
    if (it != servicesMap.end()) {
        return it->second; 
    } else {
        return IServicePtr();
    }
}

double ServiceManager::getTime()
{
    UTILS_ASSERT(updateMarker == OpenThreads::Thread::CurrentThread());
    return serviceTime;
}

double ServiceManager::getDeltaTime()
{
    UTILS_ASSERT(updateMarker == OpenThreads::Thread::CurrentThread());
    return serviceDeltaTime;
}

void ServiceManager::resetTime()
{
    serviceTimer.setStartTick();
    serviceUpdateTime = serviceTimer.getStartTick();
    serviceDeltaTime = 0.0;
}

void ServiceManager::updateTime()
{
    osg::Timer_t tick = serviceTimer.tick();
    serviceDeltaTime = serviceTimer.delta_s( serviceUpdateTime, tick );
    serviceUpdateTime = tick;
    serviceTime = serviceTimer.delta_s( serviceTimer.getStartTick(), tick );
}

