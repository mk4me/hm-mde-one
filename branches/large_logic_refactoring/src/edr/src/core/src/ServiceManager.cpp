#include "CorePCH.h"
#include <core/ILog.h>
#include "Log.h"
#include "ServiceManager.h"

namespace core {

ServiceManager::ServiceManager()
{
}


ServiceManager::~ServiceManager(void)
{

}

void ServiceManager::update(double deltaTime)
{
    for(auto it = servicesList.begin(); it != servicesList.end(); ++it){
        try{
            (*it)->update(deltaTime);
        }catch(std::exception & e){
            CORE_LOG_ERROR("Service: " << (*it)->getName() << " ID: " << (*it)->getID() << " caused an error during update: " << e.what());
        }catch(...){
            CORE_LOG_ERROR("Service: " << (*it)->getName() << " ID: " << (*it)->getID() << " caused an UNKNOWN error during update");
        }
    }
}

void ServiceManager::finalizeServices()
{
    for(auto it = servicesList.begin(); it != servicesList.end(); ++it){
        try{
            (*it)->finalize();
            CORE_LOG_DEBUG("ServiceManager: finalized correctly " << (*it)->getName() << " service");
        }
        catch(std::runtime_error & e){
            CORE_LOG_ERROR("ServiceManager: Error finalizing " << (*it)->getName() << " service with error " << e.what());
        }
        catch(std::invalid_argument & e){
            CORE_LOG_ERROR("ServiceManager: Error finalizing " << (*it)->getName() << " service with error " << e.what());
        }
        catch(std::exception & e){
            CORE_LOG_ERROR("ServiceManager: Error finalizing " << (*it)->getName() << " service with error " << e.what());
        }
        catch(...){
            CORE_LOG_ERROR("ServiceManager: Unknown error");
        }
    }
}

void ServiceManager::registerService(plugin::IServicePtr service)
{
    if (servicesMap.find(service->getID()) == servicesMap.end()) {
        servicesMap.insert( std::make_pair(service->getID(), service));
        servicesList.push_back(service);
        CORE_LOG_INFO("Service " << service->getName() << " registered.");
    } else {
        throw std::runtime_error("Service with this ID already registered.");
    }
};

int ServiceManager::getNumServices() const
{
    return static_cast<int>(servicesList.size());
}

plugin::IServicePtr ServiceManager::getService( int idx )
{
    if ( idx < static_cast<int>(servicesList.size()) ) {
        return servicesList[idx];
    } else {
        return plugin::IServicePtr();
    }
}

plugin::IServicePtr ServiceManager::getService( UniqueID id )
{
    ServicesMap::iterator it = servicesMap.find(id);
    if (it != servicesMap.end()) {
        return it->second;
    } else {
        return plugin::IServicePtr();
    }
}

}