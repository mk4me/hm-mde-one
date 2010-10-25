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

    //! Rejestruje zadan� us�ug�.
    //! \param newService
    virtual void registerService(IServicePtr newService) = 0;

    //! \return Liczba us�ug.
    virtual int getNumServices() const = 0;
    //! \param idx Indeks us�ugi.
    //! \return Us�uga o zadanym indeksie.
    virtual IServicePtr getService(int idx) = 0;
    //! \param id ID us�ugi do wyszukania.
    //! \return Odnaleziona us�uga b�d� NULL.
    virtual IServicePtr getService(UniqueID id) = 0;

    //! Metoda wyszukuj�ca wszystkie us�ugi danego typu (np. implementuj�ce
    //! dany interfejs).
    template <class T>
    CORE_SHARED_PTR(T) queryServices(T* dummy = NULL)
    {
        std::vector<CORE_SHARED_PTR(T)> result;
        queryServices(result);
        if ( result.empty() ) {
            return CORE_SHARED_PTR(T)();
        } else {
            UTILS_ASSERT(result.size()==1, "Multiple services found.");
            return result[0];
        }
    }
    //! Metoda wyszukuj�ca wszystkie us�ugi danego typu (np. implementuj�ce
    //! dany interfejs).
    template <class T>
    void queryServices(std::vector<CORE_SHARED_PTR(T)>& target)
    {
        for ( int i = 0; i < getNumServices(); ++i ) {
            IServicePtr service = getService(i);
            CORE_SHARED_PTR(T) casted = boost::dynamic_pointer_cast<T>(service);
            if ( casted ) {
                target.push_back(casted);
            }
        }
    }
};


#endif //I_SERVICE_MANAGER_H
