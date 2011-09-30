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
    //! S�ownik us�ug.
    typedef std::map<UniqueID, core::IServicePtr> ServicesMap;
    //! Sekwencja us�ug.
    typedef std::vector<core::IServicePtr> ServicesList;

private:
    //! S�ownik us�ug.
    ServicesMap servicesMap; 
    //! Sekwencja us�ug.
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
    //! Rejestruje us�ug�. Us�ugi musz� mie� unikatowe ID!
    //! \param service Us�guga do rejestracji.
    virtual void registerService(core::IServicePtr service);
    //! \return Liczba us�ug.
    virtual int getNumServices() const;
    //! \param idx Indeks us�ugi.
    //! \return Us�uga o zadanym indeksie.
    virtual core::IServicePtr getService(int idx);
    //! \param id ID us�ugi do wyszukania.
    //! \return Odnaleziona us�uga b�d� NULL.
    virtual core::IServicePtr getService(UniqueID id);
};

#endif //SERVICE_MANAGER_H
