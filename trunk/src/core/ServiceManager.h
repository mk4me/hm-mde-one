#ifndef SERVICE_MANAGER_H
#define SERVICE_MANAGER_H

#include <core/IServiceManager.h>
#include <core/IBaseService.h>
#include <map>
#include <vector>
#include <osg/Timer>

class IModel;
class IDataManager;

class ServiceManager: public IServiceManager
{
public:
    //! S³ownik us³ug.
    typedef std::map<ClassID, IBaseService*> ServicesMap;
    //! Sekwencja us³ug.
    typedef std::vector<IBaseService*> ServicesList;

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

public:
    ServiceManager(void);
	virtual ~ServiceManager(void);


    virtual IBaseService* GetSystemService(ClassID classID);
    virtual void RegisterServiceAs(IBaseService* newService, ClassID classID);

    virtual void SetModel(IDataManager* dataManager);

    virtual void updatePass(); 
    virtual void computePass();

public:
    //! \return Liczba us³ug.
    virtual int getNumServices() const;
    //! \param idx Indeks us³ugi.
    //! \return Us³uga o zadanym indeksie.
    virtual IBaseService* getService(int idx);

    //! \return Czas dzia³ania.
    virtual double getTime();
    //! \return Delta od ostatniej ramki.
    virtual double getDeltaTime();
    //! Zeruje czas widziany przez us³ugi.
    void resetTime();
    //! Aktualizuje czas widziany przez us³ugi.
    void updateTime();


private: 



    
};

#endif //SERVICE_MANAGER_H
