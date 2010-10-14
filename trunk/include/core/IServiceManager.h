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

    //! \return Liczba us�ug.
    virtual int getNumServices() const = 0;
    //! \param idx Indeks us�ugi.
    //! \return Us�uga o zadanym indeksie.
    virtual IBaseService* getService(int idx) = 0;

    //! \return Czas dzia�ania.
    virtual double getTime() = 0;
    //! \return Delta od ostatniej ramki.
    virtual double getDeltaTime() = 0;

    //! Metoda wyszukuj�ca wszystkie us�ugi danego typu (np. implementuj�ce
    //! dany interfejs).
    template <class T>
    T* queryServices(T* dummy = NULL)
    {
        std::vector<T*> result;
        queryServices(result);
        if ( result.size() ) {
            return result[0];
        } else {
            return NULL;
        }
    }

    //! Metoda wyszukuj�ca wszystkie us�ugi danego typu (np. implementuj�ce
    //! dany interfejs).
    template <class T>
    void queryServices(std::vector<T*>& target)
    {
        for ( int i = 0; i < getNumServices(); ++i ) {
            IBaseService* service = getService(i);
            if ( T* casted = dynamic_cast<T*>(service) ) {
                target.push_back(casted);
            }
        }
    }
};


#endif //I_SERVICE_MANAGER_H
