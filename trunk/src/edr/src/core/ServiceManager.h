#ifndef SERVICE_MANAGER_H
#define SERVICE_MANAGER_H

#include <core/IServiceManager.h>
#include <core/IService.h>
#include <map>
#include <vector>
#include <osg/Timer>
#include <core/PluginCommon.h>

class IModel;
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
    //! Timer kontroluj�cy czas widziany przez us�ugi.
    osg::Timer serviceTimer;
    //! Bie��ca delta.
    double serviceDeltaTime;
    //! Bie�acy czas.
    double serviceTime;
    //! Czas bie��cej ramki.
    osg::Timer_t serviceUpdateTime;
    //! Pomocniczy wska�nik
    void* updateMarker;
    //! Chwilowy fix dla timeline - informacja czy pojawi�y si� nowe dane
    bool dataChanged;

public:
    ServiceManager(void);
	virtual ~ServiceManager(void);

public:
    static ServiceManager* getInstance()
    {
        return static_cast<ServiceManager*>(core::getServiceManager());
    }

    //! Chwilowy fix dla timeline - informacja czy pojawi�y si� nowe dane
    bool dataPassRequired() const
    {
        return dataChanged;
    }

    //! Chwilowy fix dla timeline - informacja czy pojawi�y si� nowe dane
    void setDataPassRequired(bool required)
    {
        dataChanged = required;
    }

    //! Ustawia dane dla wszystkich us�ug.
    //! \param dataManager
    void loadDataPass(core::IDataManager* dataManager);
    //! Przebieg update/lateUpdate.
    void updatePass(); 
    //! Zeruje czas widziany przez us�ugi.
    void resetTime();
    //! \return Czas dzia�ania.
    double getTime();
    //! \return Delta od ostatniej ramki.
    double getDeltaTime();
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


private: 
    //! Aktualizuje czas widziany przez us�ugi.
    void updateTime();
};

#endif //SERVICE_MANAGER_H
