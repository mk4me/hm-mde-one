#ifndef SERVICE_MANAGER_H
#define SERVICE_MANAGER_H

#include <core/IServiceManager.h>
#include <core/IBaseService.h>
#include <map>
#include <vector>

class IModel;
class IDataManager;

class ServiceManager: public IServiceManager
{
public:
    //! Mapa us�ug.
    typedef std::map<ClassID, IBaseService*> ServicesMap;
    typedef std::vector<IBaseService*> ServicesList;

private:
    ServicesMap servicesMap; 
    ServicesList servicesList;

public:
    ServiceManager(void);
	virtual ~ServiceManager(void);


    virtual IBaseService* GetSystemService(ClassID classID);
    virtual void RegisterServiceAs(IBaseService* newService, ClassID classID);

    virtual void SetModel(IDataManager* dataManager);
    virtual void OnTick(double delta); 

    //! \return Liczba us�ug.
    virtual int getNumServices() const;
    //! \param idx Indeks us�ugi.
    //! \return Us�uga o zadanym indeksie.
    virtual IBaseService* getService(int idx);

private: 



    
};

#endif //SERVICE_MANAGER_H
