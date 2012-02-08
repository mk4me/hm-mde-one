/********************************************************************
    created:  2012/01/31
    created:  31:1:2012   9:19
    filename: ISourceManager.h
    author:   Mateusz Janiak
    
    purpose:  Mechanizm �r�de� oparty na zasadach serwis�w - pluginy dostarczaj� ju� instancji �r�de� kt�re nale�y obs�u�y�.
              Nie s� one prototypami ani klonowalne. Wi�cej w IService i IServiceManager
*********************************************************************/
#ifndef HEADER_GUARD_CORE__ISOURCEMANAGER_H__
#define HEADER_GUARD_CORE__ISOURCEMANAGER_H__

#include <core/ISource.h>
#include <vector>
#include <utils/Debug.h>

namespace core {

    //! Interfejs zarz�dzaj�cy/agreguj�cy �r�d�a danych aplikacji
    class ISourceManager
    {
    public:
        virtual ~ISourceManager() {};

        //! Rejestruje zadan� us�ug�.
        //! \param newService
        virtual void registerSource(const ISourcePtr & newSource) = 0;

        //! \return Liczba �r�de�.
        virtual int getNumSources() const = 0;
        //! \param idx Indeks �r�d�a.
        //! \return �r�d�o o zadanym indeksie.
        virtual ISourcePtr getSource(int idx) = 0;
        //! \param id ID �r�d�a do wyszukania.
        //! \return Odnalezione �r�d�o b�d� NULL.
        virtual ISourcePtr getSource(UniqueID id) = 0;
    };

    typedef shared_ptr<ISourceManager> ISourceManagerPtr;
    typedef shared_ptr<const ISourceManager> ISourceManagerConstPtr;
    typedef weak_ptr<ISourceManager> ISourceManagerWeakPtr;
    typedef weak_ptr<const ISourceManager> ISourceManagerWeakConstPtr;

    //! Metoda wyszukuj�ca wszystkie �r�d�a danego typu (np. implementuj�ce
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

    //! Metoda wyszukuj�ca wszystkie �r�d�a danego typu (np. implementuj�ce
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