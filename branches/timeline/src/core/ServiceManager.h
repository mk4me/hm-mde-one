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
    //! S�ownik us�ug.
    typedef std::map<ClassID, IBaseService*> ServicesMap;
    //! Sekwencja us�ug.
    typedef std::vector<IBaseService*> ServicesList;

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

public:
    ServiceManager(void);
	virtual ~ServiceManager(void);


    virtual IBaseService* GetSystemService(ClassID classID);
    virtual void RegisterServiceAs(IBaseService* newService, ClassID classID);

    virtual void SetModel(IDataManager* dataManager);

    virtual void updatePass(); 
    virtual void computePass();

public:
    //! \return Liczba us�ug.
    virtual int getNumServices() const;
    //! \param idx Indeks us�ugi.
    //! \return Us�uga o zadanym indeksie.
    virtual IBaseService* getService(int idx);

    //! \return Czas dzia�ania.
    virtual double getTime();
    //! \return Delta od ostatniej ramki.
    virtual double getDeltaTime();
    //! Zeruje czas widziany przez us�ugi.
    void resetTime();
    //! Aktualizuje czas widziany przez us�ugi.
    void updateTime();


private: 



    
};

#endif //SERVICE_MANAGER_H
