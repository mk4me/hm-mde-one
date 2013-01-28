/********************************************************************
    created:  2012/01/31
    created:  31:1:2012   9:19
    filename: ISourceManager.h
    author:   Mateusz Janiak
    
    purpose:  Mechanizm źródeł oparty na zasadach serwisów - pluginy dostarczają już instancji źródeł które należy obsłużyć.
              Nie są one prototypami ani klonowalne. Więcej w IService i IServiceManager
*********************************************************************/
#ifndef HEADER_GUARD_CORE__ISOURCEMANAGER_H__
#define HEADER_GUARD_CORE__ISOURCEMANAGER_H__

#include <corelib/ISource.h>
#include <vector>
#include <utils/Debug.h>

namespace core {

    //! Interfejs zarządzający/agregujący źródła danych aplikacji
    class ISourceManager
    {
    public:
        //! \return Liczba źródeł.
        virtual int getNumSources() const = 0;
        //! \param idx Indeks źródła.
        //! \return Źródło o zadanym indeksie.
        virtual plugin::ISourcePtr getSource(int idx) = 0;
        //! \param id ID źródła do wyszukania.
        //! \return Odnalezione źródło bądź NULL.
        virtual plugin::ISourcePtr getSource(UniqueID id) = 0;
    };

    typedef shared_ptr<ISourceManager> ISourceManagerPtr;
    typedef shared_ptr<const ISourceManager> ISourceManagerConstPtr;
    typedef weak_ptr<ISourceManager> ISourceManagerWeakPtr;
    typedef weak_ptr<const ISourceManager> ISourceManagerWeakConstPtr;

    //! Metoda wyszukująca wszystkie źródła danego typu (np. implementujące
    //! dany interfejs).
    template <class T>
    core::shared_ptr<T> querySource(ISourceManager* manager, T* dummy = nullptr)
    {
        std::vector<core::shared_ptr<T>> result;
        querySources(manager, result);
        if ( result.empty() ) {
            return core::shared_ptr<T>();
        } else {
            UTILS_ASSERT(result.size()==1, "Multiple sources found.");
            return result[0];
        }
    }

    //! Metoda wyszukująca wszystkie źródła danego typu (np. implementujące
    //! dany interfejs).
    template <class T>
    void querySources(ISourceManager* manager, std::vector<core::shared_ptr<T>>& target)
    {
        for ( int i = 0; i < manager->getNumSources(); ++i ) {
            ISourcePtr service = manager->getSource(i);
            core::shared_ptr<T> casted = core::dynamic_pointer_cast<T>(service);
            if ( casted ) {
                target.push_back(casted);
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
} // namespace core

#endif // HEADER_GUARD_CORE__ISOURCEMANAGER_H__
