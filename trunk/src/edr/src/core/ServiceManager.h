#ifndef SERVICE_MANAGER_H
#define SERVICE_MANAGER_H

#include <map>
#include <vector>

#include <core/IServiceManager.h>
#include <core/IService.h>

#include "ManagerHelper.h"

class ServiceManager: public core::IServiceManager, public ManagerHelper<ServiceManager>
{
public:
    //! S³ownik us³ug.
    typedef std::map<UniqueID, core::IServicePtr> ServicesMap;
    //! Sekwencja us³ug.
    typedef std::vector<core::IServicePtr> ServicesList;

private:
    //! S³ownik us³ug.
    ServicesMap servicesMap; 
    //! Sekwencja us³ug.
    ServicesList servicesList;

public:
    ServiceManager();
	virtual ~ServiceManager(void);

public:

    void update(double deltaTime);

    //! Metoda finalizujaca wszystkie serwisy  wywolywana przed niszczeniem managera
    void finalizeServices();

// IServiceManager
public:
    //! Rejestruje us³ugê. Us³ugi musz¹ mieæ unikatowe ID!
    //! \param service Us³guga do rejestracji.
    virtual void registerService(core::IServicePtr service);
    //! \return Liczba us³ug.
    virtual int getNumServices() const;
    //! \param idx Indeks us³ugi.
    //! \return Us³uga o zadanym indeksie.
    virtual core::IServicePtr getService(int idx);
    //! \param id ID us³ugi do wyszukania.
    //! \return Odnaleziona us³uga b¹dŸ NULL.
    virtual core::IServicePtr getService(UniqueID id);
};

#endif //SERVICE_MANAGER_H
