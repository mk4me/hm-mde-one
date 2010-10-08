#include "CorePCH.h"
#include "ServiceManager.h"

#include <core/IModel.h>
#include <core/IDataManager.h>




//--------------------------------------------------------------------------------------------------
ServiceManager::ServiceManager(void)
{
}

//--------------------------------------------------------------------------------------------------
ServiceManager::~ServiceManager(void)
{
	// free allocated memory
	for (ServicesMap::iterator i = servicesMap.begin(); i != servicesMap.end(); ++i)
		delete i->second;
	// clear container
	servicesMap.clear();
}

//--------------------------------------------------------------------------------------------------
IBaseService* ServiceManager::GetSystemService(ClassID classID)
{
    ServicesMap::iterator it = servicesMap.find(classID);
	
	if (it != servicesMap.end())
		return it->second; 
	else
		return NULL;
}

//--------------------------------------------------------------------------------------------------
void ServiceManager::RegisterServiceAs(IBaseService* newService, ClassID classID)
{
    if (servicesMap.find(classID) == servicesMap.end())
    {
        servicesMap.insert( std::pair<ClassID, IBaseService *>(classID, newService)); 
        servicesList.push_back(newService);
        newService->OnAdded(this); 
    }
};

/*
//--------------------------------------------------------------------------------------------------
template<typename T>
T* ServiceManager::GetSystemServiceAs()
{
    IBaseService *pService = GetSystemService(T::CLASS_ID);
    T *pResult = dynamic_cast<T*>(pService);
    return pResult; 
}

//--------------------------------------------------------------------------------------------------
template<typename T>
void ServiceManager::RegisterServiceAs()
{
    T* pService = new T(); 
    _services.insert( std::pair<ClassID, IBaseService *>(T::CLASS_ID, pService) ); 
    pService->OnAdded(); 
}/**/

//--------------------------------------------------------------------------------------------------
void ServiceManager::OnTick(double delta)
{
    //TODO: prawdopodobnie trzeba to bedzie jakos poprawiæ? zeby bylo asynchronicznie
    ServicesMap::iterator it = servicesMap.begin();  
    while (it != servicesMap.end())
    {
        it->second->OnTick(delta); 
        it++; 
    }
}

//--------------------------------------------------------------------------------------------------
void ServiceManager::SetModel(IDataManager* dataManager )
{
    //TODO: prawdopodobnie trzeba to bedzie jakos poprawiæ? zeby bylo asynchronicznie
    ServicesMap::iterator it = servicesMap.begin();  
    while (it != servicesMap.end())
    {
        it->second->SetModel(dataManager); 
        it++; 
    }
}

int ServiceManager::getNumServices() const
{
    return static_cast<int>(servicesList.size());
}

IBaseService* ServiceManager::getService( int idx )
{
    if ( idx < static_cast<int>(servicesList.size()) ) {
        return servicesList[idx];
    } else {
        return NULL;
    }
}