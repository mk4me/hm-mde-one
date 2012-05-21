/********************************************************************
    created:  2011/10/18
    created:  18:10:2011   11:43
    filename: DataAccessors.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___DATAACCESSORS_H__
#define HEADER_GUARD___DATAACCESSORS_H__

#include <vector>
#include <boost/type_traits.hpp>
#include "utils/Debug.h"
#include "ObjectWrapper.h"
#include "ObjectWrapperCollection.h"
#include <core/IDataManager.h>
#include <core/PluginCommon.h>

namespace core{

    //! Przekierowanie z queryData dla poprawnych danych.
    template <class T, class Ptr>
    void queryDataIsConvertible__(IDataManagerReader* manager, std::vector<Ptr> & target, bool exact, boost::true_type, bool initialize = true)
    {
        // pobieramy wrappery
        std::vector<ObjectWrapperConstPtr> objects;
        manager->getObjects(objects, typeid(T), exact, initialize);

        for(auto it = objects.begin(); it != objects.end(); it++){
            target.push_back((*it)->get(exact));
        }
    }

    //! Przekierowanie z queryData dla niepoprawnych danych.
    template <class T, class Ptr>
    void queryDataIsConvertible__(IDataManagerReader*, std::vector<Ptr>&, bool, boost::false_type, bool initialize = true)
    {
        UTILS_STATIC_ASSERT( false, "Niewlasciwy typ elementu wektora lub niezdefiniowno wrap. Sprawdz CORE_DEFINE_WRAPPER dla poszukiwanego typu." );
    }

    //! \param manager Data manager.
    //! \param target Wektor wskaŸników na obiekty. WskaŸniki musz¹ byæ konwertowalne z tego
    //!     zdefiniowanego w zasadach wskaŸników dla wrappera.
    //! \param exact Czy maj¹ byæ wyci¹gane obiekty konkretnie tego typu (z pominiêciem polimorfizmu)?
    template <class SmartPtr>
    inline void queryDataPtr(IDataManagerReader* manager, std::vector<SmartPtr>& target, bool exact = false, bool initialize = true)
    {
        UTILS_STATIC_ASSERT(boost::is_const<typename SmartPtr::element_type>::value, "Interfejs pobierania danych operuje na stalych obiektach.");
        UTILS_STATIC_ASSERT(ObjectWrapperTraits<typename boost::remove_const<typename SmartPtr::element_type>::type>::isDefinitionVisible ||
            ObjectWrapperTraits<typename SmartPtr::element_type>::isDefinitionVisible, "Niewidoczna definicja wrappera.");
        queryDataIsConvertible__<typename SmartPtr::element_type, SmartPtr>(manager, target, exact, boost::true_type(), initialize);
    }

    //! \param manager Data manager.
    //! \param exact Czy maj¹ byæ wyci¹gane obiekty konkretnie tego typu (z pominiêciem polimorfizmu)?
    //! \return Wektor wskaŸników na obiekty. WskaŸniki musz¹ byæ konwertowalne z tego
    //!     zdefiniowanego w zasadach wskaŸników dla wrappera.
    template <class SmartPtr>
    inline std::vector<SmartPtr> queryDataPtr(IDataManagerReader* manager, bool exact = false, bool initialize = true, SmartPtr* /*dummy*/ = nullptr)
    {
        std::vector<SmartPtr> target;
        queryDataPtr<SmartPtr>(manager, target, exact, initialize);
        return target;
    }

    //! Zastosowanie idiomu "Return Type Resolver" do unikania podwójnego okreœlania typu
    //! elementu kolekcji
    //! \see http://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Return_Type_Resolver
    struct queryDataPtr_t
    {

    private:
        IDataManagerReader* manager;
        bool exact;
		bool initialize;
        //! \param manager
        //! \param exact
    public:
        inline queryDataPtr_t(IDataManagerReader* manager, bool exact = false, bool initialize = true) :
        manager(manager), exact(exact), initialize(initialize)
        {}

        //! \return Kolekcja wskaŸników.
        template <class SmartPtr>
        inline operator std::vector<SmartPtr>() const
        {
            std::vector<SmartPtr> result;
            queryDataPtr<SmartPtr>(manager, result, exact, initialize);
            return result;
        }
    };

    //! Wersja funkcji queryData oparta o idiom "Return Type Resolver". Nie trzeba
    //! podawaæ jawnie typu elementu kolekcji jako parametru szablonu.
    inline queryDataPtr_t queryDataPtr(IDataManagerReader* manager, bool exact = false, bool initialize = true)
    {
        return queryDataPtr_t(manager, exact, initialize);
    }

    //! \param manager Data manager.
    //! \param target Wektor wskaŸników na obiekty. WskaŸniki musz¹ byæ konwertowalne z tego
    //!     zdefiniowanego w zasadach wskaŸników dla wrappera.
    //! \param exact Czy maj¹ byæ wyci¹gane obiekty konkretnie tego typu (z pominiêciem polimorfizmu)?
    template <class T, class Ptr>
    inline void queryData(IDataManagerReader* manager, std::vector<Ptr>& target, bool exact = false, bool initialize = true)
    {
        UTILS_STATIC_ASSERT(ObjectWrapperTraits<T>::isDefinitionVisible, "Niewidoczna definicja wrappera.");
        queryDataIsConvertible__<T, Ptr>(manager, target, exact, boost::is_convertible<Ptr, typename ObjectWrapperT<T>::ConstPtr>(), initialize);
    }

    //! \param manager Data manager.
    //! \param target Wektor wskaŸników na obiekty. WskaŸniki musz¹ byæ konwertowalne z tego
    //!     zdefiniowanego w zasadach wskaŸników dla wrappera.
    //! \param exact Czy maj¹ byæ wyci¹gane obiekty konkretnie tego typu (z pominiêciem polimorfizmu)?
    template <class T>
    inline std::vector<typename ObjectWrapperT<T>::ConstPtr> queryData(IDataManagerReader* manager, bool exact = false, bool initialize = true, T* /*dummy*/ = nullptr)
    {
        UTILS_STATIC_ASSERT(ObjectWrapperTraits<T>::isDefinitionVisible, "Niewidoczna definicja wrappera.");
        std::vector<typename ObjectWrapperT<T>::ConstPtr> target;
        queryData<T>(manager, target, exact, initialize);
        return target;
    }
}

#endif HEADER_GUARD___DATAACCESSORS_H__