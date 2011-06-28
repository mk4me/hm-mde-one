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
    //! S³ownik us³ug.
    typedef std::map<UniqueID, core::IServicePtr> ServicesMap;
    //! Sekwencja us³ug.
    typedef std::vector<core::IServicePtr> ServicesList;

private:
    //! S³ownik us³ug.
    ServicesMap servicesMap; 
    //! Sekwencja us³ug.
    ServicesList servicesList;
    //! Timer kontroluj¹cy czas widziany przez us³ugi.
    osg::Timer serviceTimer;
    //! Bie¿¹ca delta.
    double serviceDeltaTime;
    //! Bie¿acy czas.
    double serviceTime;
    //! Czas bie¿¹cej ramki.
    osg::Timer_t serviceUpdateTime;
    //! Pomocniczy wskaŸnik
    void* updateMarker;
    //! Chwilowy fix dla timeline - informacja czy pojawi³y siê nowe dane
    bool dataChanged;

public:
    ServiceManager(void);
	virtual ~ServiceManager(void);

public:
    static ServiceManager* getInstance()
    {
        return static_cast<ServiceManager*>(core::getServiceManager());
    }

    //! Chwilowy fix dla timeline - informacja czy pojawi³y siê nowe dane
    bool dataPassRequired() const
    {
        return dataChanged;
    }

    //! Chwilowy fix dla timeline - informacja czy pojawi³y siê nowe dane
    void setDataPassRequired(bool required)
    {
        dataChanged = required;
    }

    //! Ustawia dane dla wszystkich us³ug.
    //! \param dataManager
    void loadDataPass(core::IDataManager* dataManager);
    //! Przebieg update/lateUpdate.
    void updatePass(); 
    //! Zeruje czas widziany przez us³ugi.
    void resetTime();
    //! \return Czas dzia³ania.
    double getTime();
    //! \return Delta od ostatniej ramki.
    double getDeltaTime();
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


private: 
    //! Aktualizuje czas widziany przez us³ugi.
    void updateTime();
};

#endif //SERVICE_MANAGER_H
