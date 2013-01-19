/********************************************************************
    created:  2011/04/19
    created:  19:4:2011   11:43
    filename: IServiceManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___ISERVICEMANAGER_H__
#define HEADER_GUARD___ISERVICEMANAGER_H__

#include <core/IService.h>
#include <vector>
#include <utils/Debug.h>

namespace core {

    class IServiceManager
    {
    public:
        virtual ~IServiceManager() {}

        //! Rejestruje zadaną usługę.
        //! \param newService
        virtual void registerService(plugin::IServicePtr newService) = 0;

        //! \return Liczba usług.
        virtual int getNumServices() const = 0;
        //! \param idx Indeks usługi.
        //! \return Usługa o zadanym indeksie.
        virtual plugin::IServicePtr getService(int idx) = 0;
        //! \param id ID usługi do wyszukania.
        //! \return Odnaleziona usługa bądź NULL.
        virtual plugin::IServicePtr getService(UniqueID id) = 0;
    };

    typedef shared_ptr<IServiceManager> IServiceManagerPtr;
    typedef shared_ptr<const IServiceManager> IServiceManagerConstPtr;
    typedef weak_ptr<IServiceManager> IServiceManagerWeakPtr;
    typedef weak_ptr<const IServiceManager> IServiceManagerWeakConstPtr;

}

namespace core {

    //! Metoda wyszukująca wszystkie usługi danego typu (np. implementujące
    //! dany interfejs).
    template <class T>
    shared_ptr<T> queryServices(IServiceManager* manager, T* dummy = nullptr)
    {
        std::vector<shared_ptr<T>> result;
        queryServices(manager, result);
        if ( result.empty() ) {
            return shared_ptr<T>();
        } else {
            UTILS_ASSERT(result.size()==1, "Multiple services found.");
            return result[0];
        }
    }

    //! Metoda wyszukująca wszystkie usługi danego typu (np. implementujące
    //! dany interfejs).
    template <class T>
    void queryServices(IServiceManager* manager, std::vector<shared_ptr<T>>& target)
    {
        for ( int i = 0; i < manager->getNumServices(); ++i ) {
            plugin::IServicePtr service = manager->getService(i);
            shared_ptr<T> casted = dynamic_pointer_cast<T>(service);
            if ( casted ) {
                target.push_back(casted);
            }
        }
    }

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////

#endif //HEADER_GUARD___ISERVICEMANAGER_H__
