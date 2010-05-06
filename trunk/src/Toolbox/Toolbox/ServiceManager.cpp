#include "ServiceManager.h"


// pointer at instance
ServiceManager* ServiceManager::_instance = NULL; 


//--------------------------------------------------------------------------------------------------
void ServiceManager::DestroyInstance()
{
	if (_instance)
		delete _instance;
	_instance = NULL;
}

//--------------------------------------------------------------------------------------------------
ServiceManager::ServiceManager(void)
{
}

//--------------------------------------------------------------------------------------------------
ServiceManager::~ServiceManager(void)
{
	// free allocated memory
	for (std::map<ClassID, IBaseService *>::iterator i = _services.begin(); i != _services.end(); ++i)
		delete i->second;
	// clear container
	_services.clear();
}

//--------------------------------------------------------------------------------------------------
ServiceManager* ServiceManager::GetInstance()
{
    if (_instance)
        return _instance; 
    _instance = new ServiceManager(); 
    return _instance; 
}

//--------------------------------------------------------------------------------------------------
IBaseService* ServiceManager::GetSystemService(ClassID classID)
{
    std::map<ClassID, IBaseService *>::iterator it = _services.find(classID);
	
	if (it != _services.end())
		return it->second; 
	else
		return NULL;
}

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
    std::map<ClassID, IBaseService *>::iterator it = _services.begin();  
    while (it != _services.end())
    {
        it->second->OnTick(delta); 
        it++; 
    }
}

