#ifndef SERVICE_MANAGER_H
#define SERVICE_MANAGER_H

#include <core/IServiceManager.h>
#include <core/IService.h>
#include <map>
#include <vector>
#include <osg/Timer>

class IModel;
class IDataManager;

class ServiceManager: public IServiceManager
{
public:
    //! S�ownik us�ug.
    typedef std::map<UniqueID, IServicePtr> ServicesMap;
    //! Sekwencja us�ug.
    typedef std::vector<IServicePtr> ServicesList;

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

public:
    ServiceManager(void);
	virtual ~ServiceManager(void);

    //! Ustawia dane dla wszystkich us�ug.
    //! \param dataManager
    void loadDataPass(IDataManager* dataManager);
    //! Przebieg update/lateUpdate.
    void updatePass(); 
    //! Przebieg compute.
    void computePass();
    //! Zeruje czas widziany przez us�ugi.
    void resetTime();
    //! \return Czas dzia�ania.
    double getTime();
    //! \return Delta od ostatniej ramki.
    double getDeltaTime();

// IServiceManager
public:
    //! Rejestruje us�ug�. Us�ugi musz� mie� unikatowe ID!
    //! \param service Us�guga do rejestracji.
    virtual void registerService(IServicePtr service);
    //! \return Liczba us�ug.
    virtual int getNumServices() const;
    //! \param idx Indeks us�ugi.
    //! \return Us�uga o zadanym indeksie.
    virtual IServicePtr getService(int idx);
    //! \param id ID us�ugi do wyszukania.
    //! \return Odnaleziona us�uga b�d� NULL.
    virtual IServicePtr getService(UniqueID id);


private: 
    //! Aktualizuje czas widziany przez us�ugi.
    void updateTime();
};

#endif //SERVICE_MANAGER_H