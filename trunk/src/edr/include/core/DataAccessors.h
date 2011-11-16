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

namespace core{

    ////! Klasa pomocnicza, nie powinna byæ nigdzie u¿ywana poza metodami poni¿ej
    //class __DataAccessorHelper
    //{
    //    template <class Ptr>
    //    friend std::pair<bool, ObjectWrapperPtr> getOrCreateWrapper(const Ptr & pointer);

    //private:
    //    __DataAccessorHelper() {}

    //    //! \param ptr Surowy wskaŸnik do danych dla którego szukamy ObjectWrappera w DataManager
    //    //! \return ObjectWrapper dla danego surowego wskaŸnika lub Pusty/Niezainicjowany ObjectWrapperPtr (nullptr)
    //    static ObjectWrapperPtr getWrapper(void * ptr);
    //};

    ////! \param collection Kolekcja któr¹ DataManager rozszerzy odpowiednimi danymi wzglêdem jej konfiguracji
    //void queryData(ObjectWrapperCollection & collection);

    //! Przekierowanie z queryData dla poprawnych danych.
    template <class T, class Ptr>
    void __queryDataIsConvertible(IDataManagerBase* manager, std::vector<Ptr> & target, bool exact, boost::true_type)
    {
        //core::ObjectWrapperCollection collection(typeid(T), exact);
        // pobieramy wrappery
        std::vector<ObjectWrapperConstPtr> objects;
        manager->getObjects(objects, typeid(T), exact);

        for(auto it = objects.begin(); it != objects.end(); it++){
            target.push_back((*it)->get(exact));
        }
    }

    //! Przekierowanie z queryData dla niepoprawnych danych.
    template <class T, class Ptr>
    void __queryDataIsConvertible(IDataManagerBase*, std::vector<Ptr>&, bool, boost::false_type)
    {
        UTILS_STATIC_ASSERT( false, "Niewlasciwy typ elementu wektora lub niezdefiniowno wrap. Sprawdz CORE_DEFINE_WRAPPER dla poszukiwanego typu." );
    }

    //! \param manager Data manager.
    //! \param target Wektor wskaŸników na obiekty. WskaŸniki musz¹ byæ konwertowalne z tego
    //!     zdefiniowanego w zasadach wskaŸników dla wrappera.
    //! \param exact Czy maj¹ byæ wyci¹gane obiekty konkretnie tego typu (z pominiêciem polimorfizmu)?
    template <class SmartPtr>
    inline void queryDataPtr(IDataManagerBase* manager, std::vector<SmartPtr>& target, bool exact = false)
    {
        UTILS_STATIC_ASSERT(boost::is_const<typename SmartPtr::element_type>::value, "Interfejs pobierania danych operuje na stalych obiektach.");
        UTILS_STATIC_ASSERT(ObjectWrapperTraits<typename boost::remove_const<typename SmartPtr::element_type>::type>::isDefinitionVisible ||
            ObjectWrapperTraits<typename SmartPtr::element_type>::isDefinitionVisible, "Niewidoczna definicja wrappera.");
        __queryDataIsConvertible<typename SmartPtr::element_type, SmartPtr>(manager, target, exact, boost::true_type());
    }

    //! \param manager Data manager.
    //! \param exact Czy maj¹ byæ wyci¹gane obiekty konkretnie tego typu (z pominiêciem polimorfizmu)?
    //! \return Wektor wskaŸników na obiekty. WskaŸniki musz¹ byæ konwertowalne z tego
    //!     zdefiniowanego w zasadach wskaŸników dla wrappera.
    template <class SmartPtr>
    inline std::vector<SmartPtr> queryDataPtr(IDataManagerBase* manager, bool exact = false, SmartPtr* /*dummy*/ = nullptr)
    {
        std::vector<SmartPtr> target;
        queryDataPtr<SmartPtr>(manager, target, exact);
        return target;
    }

    //! Zastosowanie idiomu "Return Type Resolver" do unikania podwójnego okreœlania typu
    //! elementu kolekcji
    //! \see http://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Return_Type_Resolver
    struct queryDataPtr_t
    {
        //friend class IDataManager;

    private:
        IDataManagerBase* manager;
        bool exact;
        //! \param manager
        //! \param exact
    public:
        inline queryDataPtr_t(IDataManagerBase* manager, bool exact = false) :
        manager(manager), exact(exact)
        {}

        //! \return Kolekcja wskaŸników.
        template <class SmartPtr>
        inline operator std::vector<SmartPtr>() const
        {
            std::vector<SmartPtr> result;
            queryDataPtr<SmartPtr>(manager, result, exact);
            return result;
        }
    };

    //! Wersja funkcji queryData oparta o idiom "Return Type Resolver". Nie trzeba
    //! podawaæ jawnie typu elementu kolekcji jako parametru szablonu.
    inline queryDataPtr_t queryDataPtr(IDataManagerBase* manager, bool exact = false)
    {
        return queryDataPtr_t(manager, exact);
    }


    //! \param manager Data manager.
    //! \param target Wektor wskaŸników na obiekty. WskaŸniki musz¹ byæ konwertowalne z tego
    //!     zdefiniowanego w zasadach wskaŸników dla wrappera.
    //! \param exact Czy maj¹ byæ wyci¹gane obiekty konkretnie tego typu (z pominiêciem polimorfizmu)?
    template <class T, class Ptr>
    inline void queryData(IDataManagerBase* manager, std::vector<Ptr>& target, bool exact = false)
    {
        UTILS_STATIC_ASSERT(ObjectWrapperTraits<T>::isDefinitionVisible, "Niewidoczna definicja wrappera.");
        __queryDataIsConvertible<T, Ptr>(manager, target, exact, boost::is_convertible<Ptr, typename ObjectWrapperT<T>::ConstPtr>());
    }

    //! \param manager Data manager.
    //! \param target Wektor wskaŸników na obiekty. WskaŸniki musz¹ byæ konwertowalne z tego
    //!     zdefiniowanego w zasadach wskaŸników dla wrappera.
    //! \param exact Czy maj¹ byæ wyci¹gane obiekty konkretnie tego typu (z pominiêciem polimorfizmu)?
    template <class T>
    inline std::vector<typename ObjectWrapperT<T>::ConstPtr> queryData(IDataManagerBase* manager, bool exact = false, T* /*dummy*/ = nullptr)
    {
        UTILS_STATIC_ASSERT(ObjectWrapperTraits<T>::isDefinitionVisible, "Niewidoczna definicja wrappera.");
        std::vector<typename ObjectWrapperT<T>::ConstPtr> target;
        queryData<T>(manager, target, exact);
        return target;
    }

    /*bool isTypeRegistered(const TypeInfo & type);
    const TypeInfoSet & getApplicationRegisteredTypes();
    const TypeInfoSet & getTypeBaseTypes(const TypeInfo & type);
    const TypeInfoSet & getTypeDerrivedTypes(const TypeInfo & type);*/

    //template <class Ptr>
    //inline std::pair<bool, ObjectWrapperPtr> getOrCreateWrapper(const Ptr & pointer)
    //{
    //    UTILS_STATIC_ASSERT(ObjectWrapperTraits<typename Ptr::element_type>::isDefinitionVisible, "Dla danego obiektu nie zdefiniowano wrappera - patrz makro CORE_DEFINE_WRAPPER");
    //    UTILS_ASSERT(pointer != nullptr, "Nie mozna utworzyc wrappera dla null pointer");

    //    if(pointer == nullptr){
    //        throw std::runtime_error("Could not create wrapper for null pointer");
    //    }

    //    std::pair<bool, ObjectWrapperPtr> ret(false, __DataAccessorHelper::getWrapper((void*)pointer.get()));

    //    //szukaj w DataManager
    //    if(ret.second == nullptr){
    //        //nie znaleziono - stworz nowy
    //        ret.second = ObjectWrapper::create<typename Ptr::element_type>();
    //        ret.second->set(pointer);
    //    }else{
    //        //zaznacz ze znaleziono
    //        ret.first = true;            
    //    }

    //    return ret;
    //}
}

#endif HEADER_GUARD___DATAACCESSORS_H__