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
    virtual void registerService(IService* newService) = 0;

    //! \return Liczba us�ug.
    virtual int getNumServices() const = 0;
    //! \param idx Indeks us�ugi.
    //! \return Us�uga o zadanym indeksie.
    virtual IService* getService(int idx) = 0;
    //! \param id ID us�ugi do wyszukania.
    //! \return Odnaleziona us�uga b�d� NULL.
    virtual IService* getService(UniqueID id) = 0;

    //! Metoda wyszukuj�ca wszystkie us�ugi danego typu (np. implementuj�ce
    //! dany interfejs).
    template <class T>
    T* queryServices(T* dummy = NULL)
    {
        std::vector<T*> result;
        queryServices(result);
        if ( result.empty() ) {
            return NULL;
        } else {
            UTILS_ASSERT(result.size()==1, "Multiple services found.");
            return result[0];
        }
    }
    //! Metoda wyszukuj�ca wszystkie us�ugi danego typu (np. implementuj�ce
    //! dany interfejs).
    template <class T>
    void queryServices(std::vector<T*>& target)
    {
        for ( int i = 0; i < getNumServices(); ++i ) {
            IService* service = getService(i);
            if ( T* casted = dynamic_cast<T*>(service) ) {
                target.push_back(casted);
            }
        }
    }
};


#endif //I_SERVICE_MANAGER_H
