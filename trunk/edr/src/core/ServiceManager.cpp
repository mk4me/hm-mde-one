#include "CorePCH.h"
#include "ServiceManager.h"

#include <core/Log.h>
//#include <core/IModel.h>
#include <core/IDataManager.h>


ServiceManager::ServiceManager(void)
: updateMarker(nullptr)
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

void ServiceManager::registerService(core::IServicePtr service)
{
    if (servicesMap.find(service->getID()) == servicesMap.end()) {
        servicesMap.insert( std::make_pair(service->getID(), service)); 
        servicesList.push_back(service);
        LOG_INFO("Service "<<service->getName()<<" registered.");
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

    // aktualizacja us³ug
    for (ServicesList::iterator it = servicesList.begin(); it != servicesList.end(); ++it ) {
        (*it)->update(getTime(), getDeltaTime());
    }
    // drugi cykl aktualizacji
    for (ServicesList::iterator it = servicesList.begin(); it != servicesList.end(); ++it ) {
        (*it)->lateUpdate(getTime(), getDeltaTime());
    }

    // ustawienia markera update'a
    updateMarker = nullptr;
}

//void ServiceManager::computePass()
//{
//    // aktualizacja us³ug
//    for (ServicesList::iterator it = servicesList.begin(); it != servicesList.end(); ++it ) {
//        (*it)->compute();
//    }
//}

void ServiceManager::loadDataPass(core::IDataManager* dataManager)
{
    //TODO: prawdopodobnie trzeba to bedzie jakos poprawiæ? zeby bylo asynchronicznie
    ServicesMap::iterator it = servicesMap.begin();  
    while (it != servicesMap.end())
    {
        try {
            it->second->loadData(this, dataManager); 
        } catch (std::runtime_error& ex) {
            LOG_ERROR(it->second->getName() << ": " << ex.what());
        }
        it++;
    }
}

int ServiceManager::getNumServices() const
{
    return static_cast<int>(servicesList.size());
}

core::IServicePtr ServiceManager::getService( int idx )
{
    if ( idx < static_cast<int>(servicesList.size()) ) {
        return servicesList[idx];
    } else {
        return core::IServicePtr();
    }
}

core::IServicePtr ServiceManager::getService( UniqueID id )
{
    ServicesMap::iterator it = servicesMap.find(id);
    if (it != servicesMap.end()) {
        return it->second; 
    } else {
        return core::IServicePtr();
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

