#ifndef SERVICE_MANAGER_H
#define SERVICE_MANAGER_H

#include <core/IServiceManager.h>
#include <core/IService.h>
#include <map>
#include <vector>
#include <osg/Timer>
#include <OpenThreads/Thread>

class IModel;
class IDataManager;

class ServiceManager: public IServiceManager
{
public:
    //! S³ownik us³ug.
    typedef std::map<UniqueID, IService*> ServicesMap;
    //! Sekwencja us³ug.
    typedef std::vector<IService*> ServicesList;

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

public:
    ServiceManager(void);
	virtual ~ServiceManager(void);

    //! Ustawia dane dla wszystkich us³ug.
    //! \param dataManager
    void setData(IDataManager* dataManager);
    //! Przebieg update/lateUpdate.
    void updatePass(); 
    //! Przebieg compute.
    void computePass();
    //! Zeruje czas widziany przez us³ugi.
    void resetTime();

// IServiceManager
public:
    //! Rejestruje us³ugê. Us³ugi musz¹ mieæ unikatowe ID!
    //! \param service Us³guga do rejestracji.
    virtual void registerService(IService* service);
    //! \return Liczba us³ug.
    virtual int getNumServices() const;
    //! \param idx Indeks us³ugi.
    //! \return Us³uga o zadanym indeksie.
    virtual IService* getService(int idx);
    //! \param id ID us³ugi do wyszukania.
    //! \return Odnaleziona us³uga b¹dŸ NULL.
    virtual IService* getService(UniqueID id);

    //! \return Czas dzia³ania.
    virtual double getTime();
    //! \return Delta od ostatniej ramki.
    virtual double getDeltaTime();

private: 
    //! Aktualizuje czas widziany przez us³ugi.
    void updateTime();
};

#endif //SERVICE_MANAGER_H
