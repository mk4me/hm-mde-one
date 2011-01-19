#ifndef I_SERVICE_MANAGER_H
#define I_SERVICE_MANAGER_H

#include <core/IService.h>
#include <map>
#include <vector>
#include <utils/Debug.h>

class IModel;
class IDataManager;
class IService;

class IServiceManager
{
public:
    virtual ~IServiceManager(void) {};

    //! Rejestruje zadan¹ us³ugê.
    //! \param newService
    virtual void registerService(IServicePtr newService) = 0;

    //! \return Liczba us³ug.
    virtual int getNumServices() const = 0;
    //! \param idx Indeks us³ugi.
    //! \return Us³uga o zadanym indeksie.
    virtual IServicePtr getService(int idx) = 0;
    //! \param id ID us³ugi do wyszukania.
    //! \return Odnaleziona us³uga b¹dŸ NULL.
    virtual IServicePtr getService(UniqueID id) = 0;
};

typedef CORE_SHARED_PTR(IServiceManager) IServiceManagerPtr;
typedef CORE_CONST_SHARED_PTR(IServiceManager) IServiceManagerConstPtr;
typedef CORE_WEAK_PTR(IServiceManager) IServiceManagerWeakPtr;
typedef CORE_CONST_WEAK_PTR(IServiceManager) IServiceManagerWeakConstPtr;

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

    //! Metoda wyszukuj¹ca wszystkie us³ugi danego typu (np. implementuj¹ce
    //! dany interfejs).
    template <class T>
    CORE_SHARED_PTR(T) queryServices(IServiceManager* manager, T* dummy = NULL)
    {
        std::vector<CORE_SHARED_PTR(T)> result;
        queryServices(manager, result);
        if ( result.empty() ) {
            return CORE_SHARED_PTR(T)();
        } else {
            UTILS_ASSERT(result.size()==1, "Multiple services found.");
            return result[0];
        }
    }

    //! Metoda wyszukuj¹ca wszystkie us³ugi danego typu (np. implementuj¹ce
    //! dany interfejs).
    template <class T>
    void queryServices(IServiceManager* manager, std::vector<CORE_SHARED_PTR(T)>& target)
    {
        for ( int i = 0; i < manager->getNumServices(); ++i ) {
            IServicePtr service = manager->getService(i);
            CORE_SHARED_PTR(T) casted = dynamic_pointer_cast<T>(service);
            if ( casted ) {
                target.push_back(casted);
            }
        }
    }

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////

#endif //I_SERVICE_MANAGER_H
