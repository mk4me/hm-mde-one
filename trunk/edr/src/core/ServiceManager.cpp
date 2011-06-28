#include "CorePCH.h"
#include <core/ILog.h>
#include "Log.h"
#include "ServiceManager.h"

#include <core/IDataManager.h>


ServiceManager::ServiceManager(void)
: updateMarker(nullptr)
{
    resetTime();
}


ServiceManager::~ServiceManager(void)
{
	
}

void ServiceManager::finalizeServices()
{
    for(auto it = servicesList.begin(); it != servicesList.end(); it++){
        try{
            (*it)->finalize();
            LOG_DEBUG(std::string("ServiceManager: finalized correctly ") + (*it)->getName() + std::string(" service"));
        }
        catch(std::runtime_error e){
            LOG_ERROR(std::string("ServiceManager: Error finalizing ") + (*it)->getName() + std::string(" service with error ") + e.what());
        }
        catch(std::invalid_argument e){
            LOG_ERROR(std::string("ServiceManager: Error finalizing ") + (*it)->getName() + std::string(" service with error ") + e.what());
        }
        catch(std::exception e){
            LOG_ERROR(std::string("ServiceManager: Error finalizing ") + (*it)->getName() + std::string(" service with error ") + e.what());
        }
        catch(...){
            LOG_ERROR(std::string("ServiceManager: Unknown error"));
        }
    }
}

void ServiceManager::registerService(core::IServicePtr service)
{
    if (servicesMap.find(service->getID()) == servicesMap.end()) {
        servicesMap.insert( std::make_pair(service->getID(), service)); 
        servicesList.push_back(service);
        LOG_INFO("Service "<<service->getName()<<" registered.");
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

    dataChanged = false;
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

