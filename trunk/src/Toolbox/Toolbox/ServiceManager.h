#ifndef SERVICE_MANAGER_H
#define SERVICE_MANAGER_H

#include "IBaseService.h"
#include <map>

class ServiceManager
{
	// instance
	static ServiceManager* _instance; 

	// c - tor
    ServiceManager(void);

	// d - tor
	~ServiceManager(void);

public:

	// returns instance
    static ServiceManager* GetInstance(); 

	// instead of d - tor
	static void DestroyInstance();

    template<typename T>
    T* GetSystemServiceAs()
    {
        IBaseService *pService = GetSystemService(T::CLASS_ID);
        T *pResult = dynamic_cast<T*>(pService);
        return pResult; 
    };

    IBaseService* GetSystemService(ClassID classID);

    template<typename T>
    void RegisterServiceAs()
    {
		if (_services.find(T::CLASS_ID) == _services.end())
		{
			T* pService = new T(); 
			_services.insert( std::pair<ClassID, IBaseService *>(T::CLASS_ID, pService)); 
			pService->OnAdded(); 
		}
    };

    void OnTick(double delta); 

private: 

    std::map<ClassID, IBaseService *> _services; 

};

#endif //SERVICE_MANAGER_H
