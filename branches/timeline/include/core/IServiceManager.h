#ifndef I_SERVICE_MANAGER_H
#define I_SERVICE_MANAGER_H

#include <core/IBaseService.h>
#include <map>
#include <vector>

class IModel;
class IDataManager;
class IBaseService;

class IServiceManager
{
public:
    virtual ~IServiceManager(void) {};
    virtual IBaseService* GetSystemService(ClassID classID) = 0;
    virtual void RegisterServiceAs(IBaseService* newService, ClassID classID) = 0;
    virtual void SetModel(IDataManager* dataManager) = 0;

    //! \return Liczba us³ug.
    virtual int getNumServices() const = 0;
    //! \param idx Indeks us³ugi.
    //! \return Us³uga o zadanym indeksie.
    virtual IBaseService* getService(int idx) = 0;
};


#endif //I_SERVICE_MANAGER_H
