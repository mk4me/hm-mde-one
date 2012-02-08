/********************************************************************
    created:  2012/01/31
    created:  31:1:2012   9:19
    filename: ISourceManager.h
    author:   Mateusz Janiak
    
    purpose:  Mechanizm ürÛde≥ oparty na zasadach serwisÛw - pluginy dostarczajπ juø instancji ürÛde≥ ktÛre naleøy obs≥uøyÊ.
              Nie sπ one prototypami ani klonowalne. WiÍcej w IService i IServiceManager
*********************************************************************/
#ifndef HEADER_GUARD_CORE__ISOURCEMANAGER_H__
#define HEADER_GUARD_CORE__ISOURCEMANAGER_H__

#include <core/ISource.h>
#include <vector>
#include <utils/Debug.h>

namespace core {

    //! Interfejs zarzπdzajπcy/agregujπcy ürÛd≥a danych aplikacji
    class ISourceManager
    {
    public:
        virtual ~ISourceManager() {};

        //! Rejestruje zadanπ us≥ugÍ.
        //! \param newService
        virtual void registerSource(const ISourcePtr & newSource) = 0;

        //! \return Liczba ürÛde≥.
        virtual int getNumSources() const = 0;
        //! \param idx Indeks ürÛd≥a.
        //! \return èrÛd≥o o zadanym indeksie.
        virtual ISourcePtr getSource(int idx) = 0;
        //! \param id ID ürÛd≥a do wyszukania.
        //! \return Odnalezione ürÛd≥o bπdü NULL.
        virtual ISourcePtr getSource(UniqueID id) = 0;
    };

    typedef shared_ptr<ISourceManager> ISourceManagerPtr;
    typedef shared_ptr<const ISourceManager> ISourceManagerConstPtr;
    typedef weak_ptr<ISourceManager> ISourceManagerWeakPtr;
    typedef weak_ptr<const ISourceManager> ISourceManagerWeakConstPtr;

    //! Metoda wyszukujπca wszystkie ürÛd≥a danego typu (np. implementujπce
    //! dany interfejs).
    template <class T>
    shared_ptr<T> querySource(ISourceManager* manager, T* dummy = nullptr)
    {
        std::vector<shared_ptr<T>> result;
        querySources(manager, result);
        if ( result.empty() ) {
            return shared_ptr<T>();
        } else {
            UTILS_ASSERT(result.size()==1, "Multiple sources found.");
            return result[0];
        }
    }

    //! Metoda wyszukujπca wszystkie ürÛd≥a danego typu (np. implementujπce
    //! dany interfejs).
    template <class T>
    void querySources(ISourceManager* manager, std::vector<shared_ptr<T>>& target)
    {
        for ( int i = 0; i < manager->getNumSources(); ++i ) {
            ISourcePtr service = manager->getSource(i);
            shared_ptr<T> casted = dynamic_pointer_cast<T>(service);
            if ( casted ) {
                target.push_back(casted);
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
} // namespace core