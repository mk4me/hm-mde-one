#ifndef SERVICE_MANAGER_H
#define SERVICE_MANAGER_H

#include <core/IServiceManager.h>
#include <core/IService.h>
#include <map>
#include <vector>
#include <osg/Timer>
#include <core/PluginCommon.h>

class core::IDataManager;

class ServiceManager: public core::IServiceManager
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
    ServiceManager(void);
	virtual ~ServiceManager(void);

public:
    static ServiceManager* getInstance()
    {
        return static_cast<ServiceManager*>(core::getServiceManager());
    }

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
